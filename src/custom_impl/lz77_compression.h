#ifndef LZ77_H // If not yet defined/imported then initialize
#define LZ77_H

#include <cstdint>
#include <string>
#include <vector>

// ============================================================================
// RFC 1951 Length/Distance Code Tables (Section 3.2.5)
// ============================================================================

/*
    Length Code Table - Maps raw lengths (3-258) to DEFLATE codes (257-285)
    Each entry: {base_length, code, extra_bits}
    
    The code is what gets Huffman-encoded.
    The extra_bits are written directly after the Huffman code.
    extra_val = length - base_length
*/
struct LengthTableEntry {
    uint16_t base_length;
    uint16_t code;
    uint8_t extra_bits;
};

extern const LengthTableEntry LENGTH_TABLE[];
extern const int LENGTH_TABLE_SIZE;

/*
    Distance Code Table - Maps raw distances (1-32768) to DEFLATE codes (0-29)
    Each entry: {base_distance, code, extra_bits}
*/
struct DistanceTableEntry {
    uint16_t base_distance;
    uint8_t code;
    uint8_t extra_bits;
};

extern const DistanceTableEntry DISTANCE_TABLE[];
extern const int DISTANCE_TABLE_SIZE;

/*
    RFC 1951 DEFLATE Symbol Types
    
    DEFLATE (used in gzip, zlib, PNG) represents compressed data as a stream
    of symbols. Unlike classic LZ77 which uses (offset, length, next_char) triples,
    DEFLATE uses a flat stream where each symbol is ONE of:
    
    1. LITERAL (0-255): A raw uncompressed byte. Used when no match of length ≥3
       is found in the sliding window.
       
    2. BACK_REFERENCE: A (length, distance) pair indicating:
       - length: Copy this many bytes (3-258 per RFC 1951)
       - distance: From this many positions back (1-32768)
       Example: "abcabc" → [lit 'a', lit 'b', lit 'c', ref(3,3)]
                The ref means "go back 3, copy 3 bytes"
    
    3. END_OF_BLOCK (code 256): Marks the end of a compressed block.
    
    Key insight: There is NO trailing character after a back-reference.
    If you need to emit a character after a match, it's a separate LITERAL symbol.
    
    Memory layout uses a union because a symbol is EITHER a literal byte
    OR a length/distance pair, never both simultaneously. This saves memory
    compared to storing all fields for every symbol.
*/
// Replace the Token struct with:
enum class SymbolType : uint8_t {
  LITERAL,         // Raw byte
  BACK_REFERENCE,  // (length, distance) pair
  END_OF_BLOCK     // Marks end of compressed block
};

/*
    DeflateSymbol - A single symbol in the DEFLATE LZ77 output stream.
    
    Uses a tagged union pattern:
    - Check 'type' first to know which union member is valid
    - Access 'literal' only when type == LITERAL
    - Access 'ref.length' and 'ref.distance' only when type == BACK_REFERENCE
    
    Example usage:
        DeflateSymbol sym;
        sym.type = SymbolType::LITERAL;
        sym.literal = 'A';
        
        DeflateSymbol ref;
        ref.type = SymbolType::BACK_REFERENCE;
        ref.ref.length = 5;      // Copy 5 bytes
        ref.ref.distance = 10;   // From 10 positions back
*/
struct DeflateSymbol {
  SymbolType type;
  union {
      uint8_t literal;           // When type == LITERAL
      struct {
          uint16_t length;       // 3-258
          uint16_t distance;     // 1-32768
      } ref;                     // When type == BACK_REFERENCE
  };
};

/*
    RFC 1951 Length/Distance Code Conversion
    
    DEFLATE doesn't directly encode raw length/distance values.
    Instead, it uses base codes + extra bits:
    
    For lengths (3-258):
      - 29 base codes (257-285) that are Huffman-encoded
      - 0-5 extra bits written directly to specify exact length within range
      
    For distances (1-32768):
      - 30 base codes (0-29) that are Huffman-encoded  
      - 0-13 extra bits written directly to specify exact distance within range
    
    This reduces the Huffman alphabet size significantly:
      - Without: 256 length symbols, 32768 distance symbols
      - With: 29 length codes, 30 distance codes
*/

// Result of converting a length or distance to DEFLATE code format
struct DeflateCode {
    uint16_t code;        // Base code (257-285 for length, 0-29 for distance)
    uint8_t extra_bits;   // Number of extra bits to write (0-13)
    uint16_t extra_val;   // Value of the extra bits
};

// A fully encoded DEFLATE symbol ready for Huffman encoding
enum class EncodedSymbolType : uint8_t {
    LITERAL,          // Code 0-255: raw byte
    END_OF_BLOCK,     // Code 256: end marker
    LENGTH_DISTANCE   // Code 257-285 + distance code 0-29
};

struct EncodedDeflateSymbol {
    EncodedSymbolType type;
    union {
        uint8_t literal;  // For LITERAL (code = literal value)
        struct {
            DeflateCode length;    // Length code (257-285) + extra bits
            DeflateCode distance;  // Distance code (0-29) + extra bits
        } ref;
    };
};

// ==================== Encoding (Compression) ====================

/**
    Convert a raw length (3-258) to DEFLATE code format.
    @param length - Match length from LZ77 (must be 3-258)
    @return DeflateCode with base code (257-285), extra_bits count, and extra_val
*/
DeflateCode length_to_deflate_code(uint16_t length);

/**
    Convert a raw distance (1-32768) to DEFLATE code format.
    @param distance - Back-reference distance from LZ77 (must be 1-32768)
    @return DeflateCode with base code (0-29), extra_bits count, and extra_val
*/
DeflateCode distance_to_deflate_code(uint16_t distance);

/**
    Convert LZ77 symbols to fully encoded DEFLATE symbols.
    This converts raw length/distance values to base codes + extra bits.
    @param symbols - Vector of LZ77 DeflateSymbols
    @param debug - Enable debug output
    @return Vector of EncodedDeflateSymbols ready for Huffman encoding
*/
std::vector<EncodedDeflateSymbol> convert_to_deflate_codes(
    const std::vector<DeflateSymbol>& symbols, 
    bool debug = false
);

// ==================== Decoding (Decompression) ====================

/**
    Convert a DEFLATE length code (257-285) + extra bits back to raw length (3-258).
    @param code - Length code from Huffman decoding (257-285)
    @param extra_val - Extra bits value
    @return Raw length value (3-258)
*/
uint16_t deflate_code_to_length(uint16_t code, uint16_t extra_val);

/**
    Convert a DEFLATE distance code (0-29) + extra bits back to raw distance (1-32768).
    @param code - Distance code from Huffman decoding (0-29)
    @param extra_val - Extra bits value
    @return Raw distance value (1-32768)
*/
uint16_t deflate_code_to_distance(uint16_t code, uint16_t extra_val);

/**
    Decompress EncodedDeflateSymbols back to original string.
    This is used after Huffman decoding to reconstruct the original data.
    @param symbols - Vector of EncodedDeflateSymbols (from Huffman decoder)
    @param debug - Enable debug output
    @return Decompressed string
*/
std::string lz77_decompress_encoded(
    const std::vector<EncodedDeflateSymbol>& symbols,
    bool debug = false
);

/*
  Function used to compress the input text using LZ77 Compression.
  @param input - string input
  @return vector<DeflateSymbol> - Vector of DeflateSymbols.
*/
std::vector<DeflateSymbol> lz77_compress(std::string input, bool debug = false);

/*
    Function to decompress data using LZ77 Decompression Algorithm.
    @param vector<DeflateSymbol> compressed : Vector of DeflateSymbols.
    @param debug : Enable debug output
    @return string - decompressed data
*/
std::string lz77_decompress(const std::vector<DeflateSymbol>& compressed, bool debug = false);

// Token* unserialize_tokens(const std::string& binary, int& out_token_count);

// /*
//     Function to return serialized string from Token array
//     @param Token* compressed : Pointer for Compressed Token Array.
//     @return string - compressed string
// */
// std::string get_serialized_string_from_token_arr(Token* data);

// /*
//       Function to return string from Token array
//     @param Token* compressed : Pointer for Compressed Token Array.
//     @return string - compressed string
// */
// std::string get_string_from_token_arr(Token* compressed_data);

#endif