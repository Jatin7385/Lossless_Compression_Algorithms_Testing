#ifndef LZ77_H // If not yet defined/imported then initialize
#define LZ77_H

/*
  The structure holding the metadata of characters.
  - Offset : How many characters do I need to go back to find the longest match from my current position.
  - Length : Length of the longest match found from the offset position.
  - Next Char : Still not very sure. Holds the next character after the longest match.

  1 Token -> 4B + 4B + 1B = 9 Bytes. This is code/debugging friendly, but instead of compressing total size,
  it expands the size due to the overhead.

  Eg : 
    --> Original Text to be compressed :: The computerphile channel handles computer topics. :: Size :: 50
    --> Compressed Data :: The computrhilhane dsrtpc. :: Size : 234 -> 26*9 ==> That's expansion instead of compression.
    --> Decompressed Data :: The computerphile channel handles computer topics. :: Size : 50

    Even with uint8_t -> 3Bytes per token. 3*26 = 78. Original Size was 50. Helps, but still no good.

  Ideally the solution is to use a BitWriter and BitReader instead. (Writing bits to stream).
*/
struct Token
{
    int offset; // The offset of the longest match. ---> int uses 32 bits - 4 Bytes
    int length_of_match; // The length of the longest match.
    // uint8_t offset; // uint8_t -> 8 bits - 1 Byte
    // uint8_t length_of_match;
    char next_char; // The next char after the longest match. --> char uses 8bits - 1 Byte.
};

/*
  Function used to compress the input text using LZ77 Compression.
  @param input - string input
  @return Token* - Pointer of the first token of the Token array.
*/
Token* lz77_compress(std::string input, bool debug = false);

/*
    Function to decompress data using LZ77 Decompression Algorithm.
    @param Token* compressed : Pointer for Compressed Token Array.
    @return string - decompressed data
*/
std::string lz77_decompress(Token* compressed);

Token* unserialize_tokens(const std::string& binary, int& out_token_count);

/*
    Function to return serialized string from Token array
    @param Token* compressed : Pointer for Compressed Token Array.
    @return string - compressed string
*/
std::string get_serialized_string_from_token_arr(Token* data);

/*
      Function to return string from Token array
    @param Token* compressed : Pointer for Compressed Token Array.
    @return string - compressed string
*/
std::string get_string_from_token_arr(Token* compressed_data);

#endif