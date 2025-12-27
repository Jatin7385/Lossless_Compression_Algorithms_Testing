/*
    RFC 1951 DEFLATE Compression Implementation
    
    This file orchestrates:
    - LZ77 compression (from lz77_compression.cpp)
    - Huffman encoding (Fixed codes for now, see note below)
    
    Note on Dynamic Huffman:
    Your huffman_encoding.cpp works on char frequencies (0-255).
    DEFLATE Dynamic Huffman (BTYPE=10) requires:
    - Literal/Length alphabet: 0-285 (286 symbols including lengths & end-of-block)
    - Distance alphabet: 0-29 (30 symbols)
    - A third Huffman tree to encode code lengths themselves
    - Run-length encoding for repeated code lengths
    
    For standard decompressor compatibility (gzip, zlib), we use Fixed Huffman (BTYPE=01)
    which uses predefined tables from RFC 1951 Section 3.2.6.
    
    To enable Dynamic Huffman, huffman_encoding.cpp would need modification to:
    - Work on integer symbols (0-285) instead of just chars
    - Output the encoded code length tables in DEFLATE format
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>
#include "deflate.h"
#include "lz77_compression.h"
#include "bit_utils.h"
#include "fixed_huffman_encoding.h"

using namespace std;

// ============================================================================
// File I/O
// ============================================================================

string readFile(string fileName, bool debug) {
    ifstream file(fileName);
    if (!file.is_open()) { cerr << "Could not open: " << fileName << endl; return ""; }
    stringstream buffer;
    buffer << file.rdbuf();
    if (debug) cout << "Read " << buffer.str().length() << " bytes" << endl;
    return buffer.str();
}

// ============================================================================
// DEFLATE Compression (Fixed Huffman, BTYPE=01)
// ============================================================================

DeflateResult deflate_compress(const string& input, bool debug) {
    BitWriter writer;
    
    // LZ77 compression (from lz77_compression.cpp)
    vector<DeflateSymbol> symbols = lz77_compress(const_cast<string&>(input), debug);
    
    // Block header: BFINAL=1, BTYPE=01 (fixed Huffman)
    writer.write_bits(0b011, 3); // BFINAL=1, BTYPE=01 (fixed Huffman). As per Deflate RFC 1951.
    
    // Encode each symbol
    for (const auto& sym : symbols) {
        if (sym.type == SymbolType::LITERAL) {
            FixedCode fc = get_fixed_litlen_code(sym.literal);
            writer.write_code_reversed(fc.code, fc.length); // LSB to MSB. As per Deflate RFC 1951.
        }
        else if (sym.type == SymbolType::BACK_REFERENCE) {
            // Length code (uses tables from lz77_compression.h)
            DeflateCode len = length_to_deflate_code(sym.ref.length);
            FixedCode fc = get_fixed_litlen_code(len.code);
            writer.write_code_reversed(fc.code, fc.length); // LSB to MSB. As per Deflate RFC 1951.
            if (len.extra_bits > 0) writer.write_bits(len.extra_val, len.extra_bits);
            
            // Distance code
            DeflateCode dist = distance_to_deflate_code(sym.ref.distance);
            FixedCode fcd = get_fixed_distance_code(dist.code);
            writer.write_code_reversed(fcd.code, fcd.length); // LSB to MSB. As per Deflate RFC 1951.
            if (dist.extra_bits > 0) writer.write_bits(dist.extra_val, dist.extra_bits);
        }
        else if (sym.type == SymbolType::END_OF_BLOCK) {
            FixedCode fc = get_fixed_litlen_code(256); // End of block code value is 256.
            writer.write_code_reversed(fc.code, fc.length); // LSB to MSB. As per Deflate RFC 1951.
        }
    }
    
    return {writer.data, writer.bit_pos, input.size()};
}

// ============================================================================
// DEFLATE Decompression (for verification)
// ============================================================================

string deflate_decompress(const vector<uint8_t>& data, bool debug) {
    BitReader reader(data);
    string output;
    
    uint32_t bfinal = reader.read_bits(1);
    uint32_t btype = reader.read_bits(2);
    if (debug) cout << "BFINAL=" << bfinal << ", BTYPE=" << btype << endl;
    if (btype != 1) { cerr << "Only BTYPE=01 supported" << endl; return ""; }
    
    while (reader.has_bits()) {
        int sym = reader.read_fixed_litlen_code();
        if (sym < 0) break;
        
        if (sym < 256) { // Value less than 256 is a literal.
            output += static_cast<char>(sym);
        } else if (sym == 256) { // Value is 256 is end of block.
            break;  // End of block
        } else { // Value is greater than 256 is a back reference. Length and Distance Codes are read next.
            // Length: use tables from lz77_compression.h
            uint16_t length = deflate_code_to_length(sym, 0);
            for (int i = 0; i < LENGTH_TABLE_SIZE; i++) {
                if (LENGTH_TABLE[i].code == sym && LENGTH_TABLE[i].extra_bits > 0) {
                    length = LENGTH_TABLE[i].base_length + reader.read_bits(LENGTH_TABLE[i].extra_bits);
                    break; // Break out of the loop if the length code is found.
                }
            }
            
            int dist_code = reader.read_fixed_distance_code(); // Read the distance code.
            uint16_t distance = deflate_code_to_distance(dist_code, 0);
            for (int i = 0; i < DISTANCE_TABLE_SIZE; i++) {
                if (DISTANCE_TABLE[i].code == dist_code && DISTANCE_TABLE[i].extra_bits > 0) {
                    distance = DISTANCE_TABLE[i].base_distance + reader.read_bits(DISTANCE_TABLE[i].extra_bits);
                    break; // Break out of the loop if the distance code is found.
                }
            }
            
            size_t start = output.length() - distance;
            for (uint16_t i = 0; i < length; i++) output += output[start + i];
        }
    }
    return output;
}

// ============================================================================
// Main
// ============================================================================

int main() {
    cout << "================== RFC 1951 DEFLATE ==================" << endl;
    
    string input = "The quick brown fox jumps over the lazy dog. The lazy dog sleeps.";
    cout << "Original: \"" << input << "\" (" << input.size() << " bytes)" << endl;
    
    // Compress
    DeflateResult compressed = deflate_compress(input, false);
    cout << "Compressed: " << compressed.data.size() << " bytes" << endl;
    
    // Verify with internal decompressor
    string decompressed = deflate_decompress(compressed.data, false);
    cout << "Verification: " << (input == decompressed ? "SUCCESS ✓" : "FAILED ✗") << endl;
    
    // Write files
    ofstream(("output.deflate"), ios::binary).write(
        reinterpret_cast<const char*>(compressed.data.data()), compressed.data.size());
    
    return 0;
}
