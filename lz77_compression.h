#ifndef LZ77_H // If not yet defined/imported then initialize
#define LZ77_H

#include <cstdint>
#include <string>
#include <vector>

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