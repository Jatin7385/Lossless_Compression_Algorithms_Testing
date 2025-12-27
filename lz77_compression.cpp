# include <iostream>
# include <string>
# include <vector>
# include "lz77_compression.h"

using namespace std;

// To this (RFC 1951 standard): https://datatracker.ietf.org/doc/html/rfc1951#section-1
int search_buffer_size = 32768;  // 32KB = 2^15 // Size of the search buffer -- The substring already parsed.
int look_ahead_buffer_size = 258; //  Size of the look ahead buffer -- THe buffer ahead of the 
                                // current character to be searched for maximum possible match

// ============================================================================
// RFC 1951 Length/Distance Code Tables (Section 3.2.5)
// ============================================================================

/*
    Length Code Table
    Each entry: {base_length, code, extra_bits}
    
    The code is what gets Huffman-encoded (257-285).
    The extra_bits are written directly after the Huffman code.
    extra_val = length - base_length
*/
struct LengthTableEntry {
    uint16_t base_length;
    uint16_t code;
    uint8_t extra_bits;
};

static const LengthTableEntry LENGTH_TABLE[] = {
    {3,   257, 0},  {4,   258, 0},  {5,   259, 0},  {6,   260, 0},
    {7,   261, 0},  {8,   262, 0},  {9,   263, 0},  {10,  264, 0},
    {11,  265, 1},  {13,  266, 1},  {15,  267, 1},  {17,  268, 1},
    {19,  269, 2},  {23,  270, 2},  {27,  271, 2},  {31,  272, 2},
    {35,  273, 3},  {43,  274, 3},  {51,  275, 3},  {59,  276, 3},
    {67,  277, 4},  {83,  278, 4},  {99,  279, 4},  {115, 280, 4},
    {131, 281, 5},  {163, 282, 5},  {195, 283, 5},  {227, 284, 5},
    {258, 285, 0}   // Special case: length 258 has code 285, 0 extra bits
};
static const int LENGTH_TABLE_SIZE = sizeof(LENGTH_TABLE) / sizeof(LENGTH_TABLE[0]);

/*
    Distance Code Table
    Each entry: {base_distance, code, extra_bits}
    
    The code is what gets Huffman-encoded (0-29).
    The extra_bits are written directly after the Huffman code.
    extra_val = distance - base_distance
*/
struct DistanceTableEntry {
    uint16_t base_distance;
    uint8_t code;
    uint8_t extra_bits;
};

static const DistanceTableEntry DISTANCE_TABLE[] = {
    {1,     0,  0},  {2,     1,  0},  {3,     2,  0},  {4,     3,  0},
    {5,     4,  1},  {7,     5,  1},  {9,     6,  2},  {13,    7,  2},
    {17,    8,  3},  {25,    9,  3},  {33,   10,  4},  {49,   11,  4},
    {65,   12,  5},  {97,   13,  5},  {129,  14,  6},  {193,  15,  6},
    {257,  16,  7},  {385,  17,  7},  {513,  18,  8},  {769,  19,  8},
    {1025, 20,  9},  {1537, 21,  9},  {2049, 22, 10},  {3073, 23, 10},
    {4097, 24, 11},  {6145, 25, 11},  {8193, 26, 12},  {12289, 27, 12},
    {16385, 28, 13}, {24577, 29, 13}
};
static const int DISTANCE_TABLE_SIZE = sizeof(DISTANCE_TABLE) / sizeof(DISTANCE_TABLE[0]);

/**
    Convert a raw length (3-258) to DEFLATE code format.
    Uses binary search to find the correct table entry.
*/
DeflateCode length_to_deflate_code(uint16_t length) {
    DeflateCode result = {0, 0, 0};
    
    // Find the largest base_length <= length
    int idx = 0;
    for (int i = LENGTH_TABLE_SIZE - 1; i >= 0; i--) {
        if (LENGTH_TABLE[i].base_length <= length) {
            idx = i;
            break;
        }
    }
    
    result.code = LENGTH_TABLE[idx].code;
    result.extra_bits = LENGTH_TABLE[idx].extra_bits;
    result.extra_val = length - LENGTH_TABLE[idx].base_length;
    
    return result;
}

/**
    Convert a raw distance (1-32768) to DEFLATE code format.
    Uses binary search to find the correct table entry.
*/
DeflateCode distance_to_deflate_code(uint16_t distance) {
    DeflateCode result = {0, 0, 0};
    
    // Find the largest base_distance <= distance
    int idx = 0;
    for (int i = DISTANCE_TABLE_SIZE - 1; i >= 0; i--) {
        if (DISTANCE_TABLE[i].base_distance <= distance) {
            idx = i;
            break;
        }
    }
    
    result.code = DISTANCE_TABLE[idx].code;
    result.extra_bits = DISTANCE_TABLE[idx].extra_bits;
    result.extra_val = distance - DISTANCE_TABLE[idx].base_distance;
    
    return result;
}

/**
    Convert LZ77 symbols to fully encoded DEFLATE symbols.
*/
vector<EncodedDeflateSymbol> convert_to_deflate_codes(
    const vector<DeflateSymbol>& symbols,
    bool debug
) {
    vector<EncodedDeflateSymbol> encoded;
    
    for (const auto& sym : symbols) {
        EncodedDeflateSymbol enc;
        
        switch (sym.type) {
            case SymbolType::LITERAL:
                enc.type = EncodedSymbolType::LITERAL;
                enc.literal = sym.literal;
                if (debug) {
                    cout << "Encode: LITERAL(" << (int)sym.literal << ") -> code " 
                         << (int)sym.literal << endl;
                }
                break;
                
            case SymbolType::BACK_REFERENCE: {
                enc.type = EncodedSymbolType::LENGTH_DISTANCE;
                enc.ref.length = length_to_deflate_code(sym.ref.length);
                enc.ref.distance = distance_to_deflate_code(sym.ref.distance);
                
                if (debug) {
                    cout << "Encode: BACK_REF(len=" << sym.ref.length 
                         << ", dist=" << sym.ref.distance << ") -> "
                         << "length_code=" << enc.ref.length.code 
                         << "+" << (int)enc.ref.length.extra_bits << "bits(" 
                         << enc.ref.length.extra_val << "), "
                         << "dist_code=" << enc.ref.distance.code 
                         << "+" << (int)enc.ref.distance.extra_bits << "bits(" 
                         << enc.ref.distance.extra_val << ")" << endl;
                }
                break;
            }
            
            case SymbolType::END_OF_BLOCK:
                enc.type = EncodedSymbolType::END_OF_BLOCK;
                if (debug) {
                    cout << "Encode: END_OF_BLOCK -> code 256" << endl;
                }
                break;
        }
        
        encoded.push_back(enc);
    }
    
    return encoded;
}

// ============================================================================
// Reverse Lookup: DEFLATE Codes → Raw Values (for Decompression)
// ============================================================================

/**
    Convert a DEFLATE length code (257-285) + extra bits back to raw length (3-258).
*/
uint16_t deflate_code_to_length(uint16_t code, uint16_t extra_val) {
    // Find the entry with matching code
    for (int i = 0; i < LENGTH_TABLE_SIZE; i++) {
        if (LENGTH_TABLE[i].code == code) {
            return LENGTH_TABLE[i].base_length + extra_val;
        }
    }
    // Should never reach here with valid input
    return 0;
}

/**
    Convert a DEFLATE distance code (0-29) + extra bits back to raw distance (1-32768).
*/
uint16_t deflate_code_to_distance(uint16_t code, uint16_t extra_val) {
    // Find the entry with matching code
    for (int i = 0; i < DISTANCE_TABLE_SIZE; i++) {
        if (DISTANCE_TABLE[i].code == code) {
            return DISTANCE_TABLE[i].base_distance + extra_val;
        }
    }
    // Should never reach here with valid input
    return 0;
}

/**
    Decompress EncodedDeflateSymbols back to original string.
    This is the counterpart to convert_to_deflate_codes() + lz77_compress().
*/
string lz77_decompress_encoded(
    const vector<EncodedDeflateSymbol>& symbols,
    bool debug
) {
    string output;
    
    for (const auto& sym : symbols) {
        switch (sym.type) {
            case EncodedSymbolType::LITERAL:
                output += sym.literal;
                if (debug) {
                    cout << "Decode: LITERAL(" << (int)sym.literal 
                         << ") -> '" << sym.literal << "'" << endl;
                }
                break;
                
            case EncodedSymbolType::LENGTH_DISTANCE: {
                // Convert codes back to raw values
                uint16_t length = deflate_code_to_length(
                    sym.ref.length.code, 
                    sym.ref.length.extra_val
                );
                uint16_t distance = deflate_code_to_distance(
                    sym.ref.distance.code, 
                    sym.ref.distance.extra_val
                );
                
                if (debug) {
                    cout << "Decode: LENGTH_DISTANCE(code=" << sym.ref.length.code 
                         << "+" << sym.ref.length.extra_val 
                         << ", dist_code=" << sym.ref.distance.code 
                         << "+" << sym.ref.distance.extra_val << ") -> "
                         << "len=" << length << ", dist=" << distance << " -> \"";
                }
                
                // Copy bytes from back-reference
                size_t start = output.length() - distance;
                for (uint16_t i = 0; i < length; i++) {
                    char c = output[start + i];
                    output += c;
                    if (debug) cout << c;
                }
                if (debug) cout << "\"" << endl;
                break;
            }
            
            case EncodedSymbolType::END_OF_BLOCK:
                if (debug) cout << "Decode: END_OF_BLOCK" << endl;
                return output;
        }
    }
    
    return output;
}

// ============================================================================
// LZ77 Compression/Decompression
// ============================================================================ 

/*
  Function used to compress the input text using LZ77 Compression.
  @param input - string input
  @return vector<DeflateSymbol> - Vector of DeflateSymbols.
*/
vector<DeflateSymbol> lz77_compress(string input, bool debug)
{
    size_t input_length = input.length();
    vector<DeflateSymbol> output;
    size_t index = 0; // Current index variable while parsing the string

    string search_buffer;

    // We run a while loop from from 0 to input_length.
    while (index < input_length)
    {
        // This gives us the corrected start index. 
        // If <=0 then we start from 0, and move index positions, 
        // else start from corrected_index for a length of search_buffer_size
        size_t corrected_start_index = (index < (size_t)search_buffer_size) ? 0 : (index - search_buffer_size);
        size_t corrected_search_buffer_length = (corrected_start_index == 0) ? index : (size_t)search_buffer_size;
        // If index - search_buffer_size <= 0, that means, search_buffer will 
        // start from 0, and be of the index's size. Once it's more than 0, 
        // we start sliding it.
        search_buffer = input.substr(corrected_start_index, corrected_search_buffer_length); 

        // Now we find the longest possible match.
        size_t match_length = 0;
        /*
        Steps : 
            - We loop through the search buffer till input[index] matches.
            - Once we find match_index, we start incrementing match_length and comparing strings.
            - Once the match stops, we emit either a back-reference or a literal.
        */
        size_t best_offset = 0;
        size_t best_length = 0;

        // Looping through search buffer to find matches.
        for (size_t i = 0; i < corrected_search_buffer_length; i++)
        {
            match_length = 0;
            // Finding a match for char at index in search_buffer
            if(input[index] == search_buffer[i])
            {
                // We loop till input and search buffer keep matching from initial match location.
                while (
                    (index + match_length) < input.length() && // Out of bound check
                    (i + match_length) < corrected_search_buffer_length && // Out of bound check
                    match_length < (size_t)look_ahead_buffer_size && // Look Ahead Buffer Size Check
                    input[index + match_length] == search_buffer[i + match_length] // Equality check
                ) {
                    match_length += 1;
                }
                
                // Minimum match length is 3 - prevents small matches from inflating compressed size
                if (match_length >= 3 && best_length < match_length)
                {
                    best_length = match_length;
                    best_offset = index - (corrected_start_index + i);
                }
            }
        }
        
        if (best_length >= 3) {
            // Emit back-reference
            DeflateSymbol sym;
            sym.type = SymbolType::BACK_REFERENCE;
            sym.ref.length = best_length;
            sym.ref.distance = best_offset;
            output.push_back(sym);
            
            if (debug) {
                cout << "Index: " << index 
                     << " :: BACK_REF(len=" << best_length 
                     << ", dist=" << best_offset << ")"
                     << " :: Matched: \"" << input.substr(index, best_length) << "\"" << endl;
            }
            
            index += best_length;  // No +1, no trailing char!
        } else {
            // Emit literal
            DeflateSymbol sym;
            sym.type = SymbolType::LITERAL;
            sym.literal = input[index];
            output.push_back(sym);
            
            if (debug) {
                cout << "Index: " << index 
                     << " :: LITERAL('" << input[index] << "')" << endl;
            }
            
            index++;
        }
    }


    // End of Block// End of block marker
    DeflateSymbol end;
    end.type = SymbolType::END_OF_BLOCK;
    output.push_back(end);

    // Returning the Output Vector
    return output;
}

/*
    Function to decompress DEFLATE LZ77 symbols back to original string.
    @param symbols - Vector of DeflateSymbols
    @param debug - Enable debug output
    @return Decompressed string
*/
string lz77_decompress(const vector<DeflateSymbol>& symbols, bool debug) {
    string output;
    
    for (const auto& sym : symbols) {
        switch (sym.type) {
            case SymbolType::LITERAL:
                output += sym.literal;
                if (debug) cout << "Decompress: LITERAL('" << sym.literal << "')" << endl;
                break;
                
            case SymbolType::BACK_REFERENCE: {
                size_t start = output.length() - sym.ref.distance;
                if (debug) {
                    cout << "Decompress: BACK_REF(len=" << sym.ref.length 
                         << ", dist=" << sym.ref.distance << ") -> \"";
                }
                for (int i = 0; i < sym.ref.length; i++) {
                    char c = output[start + i];
                    output += c;
                    if (debug) cout << c;
                }
                if (debug) cout << "\"" << endl;
                break;
            }
            
            case SymbolType::END_OF_BLOCK:
                if (debug) cout << "Decompress: END_OF_BLOCK" << endl;
                return output;
        }
    }
    
    return output;
}

int main()
{
    string text = "The computerphile channel handles computer topics.";
    bool debug = false;
    
    cout << "================== DEFLATE LZ77 FULL PIPELINE ==================" << endl;
    cout << "Original Text :: \"" << text << "\"" << endl;
    cout << "Original Size :: " << text.length() << " bytes" << endl;
    cout << "----------------------------------------------------------------" << endl;
    
    // Step 1: LZ77 Compress → DeflateSymbol
    cout << "\n[Step 1] LZ77 Compression (raw symbols):" << endl;
    vector<DeflateSymbol> lz77_output = lz77_compress(text, debug);
    
    int literals = 0, refs = 0;
    for (const auto& sym : lz77_output) {
        if (sym.type == SymbolType::LITERAL) literals++;
        else if (sym.type == SymbolType::BACK_REFERENCE) refs++;
    }
    cout << "  Output: " << lz77_output.size() << " symbols "
         << "(Literals: " << literals << ", Back-refs: " << refs << ", End: 1)" << endl;
    
    // Step 2: Convert to DEFLATE codes → EncodedDeflateSymbol
    cout << "\n[Step 2] Convert to DEFLATE Codes:" << endl;
    vector<EncodedDeflateSymbol> encoded = convert_to_deflate_codes(lz77_output, true);
    
    cout << "\n  Code Distribution:" << endl;
    cout << "    Literal codes (0-255): " << literals << endl;
    cout << "    Length codes (257-285): " << refs << endl;
    cout << "    Distance codes (0-29): " << refs << endl;
    cout << "    End-of-block code (256): 1" << endl;
    
    int total_extra_bits = 0;
    for (const auto& sym : encoded) {
        if (sym.type == EncodedSymbolType::LENGTH_DISTANCE) {
            total_extra_bits += sym.ref.length.extra_bits + sym.ref.distance.extra_bits;
        }
    }
    cout << "    Total extra bits: " << total_extra_bits << endl;
    
    cout << "\n-------------------------------------------------------" << endl;
    
    cout << "\n[Step 4] Decompression from EncodedDeflateSymbol:" << endl;
    string decompressed = lz77_decompress_encoded(encoded, debug);
    cout << "  Decompressed: \"" << decompressed << "\"" << endl;
    
    bool match = (text == decompressed);
    cout << "  Status: " << (match ? "SUCCESS ✓" : "FAILED ✗") << endl;
    
    return 0;
}