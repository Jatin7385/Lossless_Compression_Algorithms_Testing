#include <iostream>
#include <fstream>   // For file streams
#include <sstream>
#include <string>
#include <unordered_map>
#include "huffman_encoding.h"
#include "lz77_compression.h"

using namespace std;

string readFile(string fileName, bool debug = false)
{
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Could not open file.\n";
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();  // Read the entire file into the buffer
    string content = buffer.str();

    // if(debug) cout << "File content:\n" << content << endl;

    file.close();

    return content;
}

int main()
{
    // string input = "Deflate compression";
    string input = readFile("./data.txt");
    // ----------------------------------- DEFLATE -------------------------------------------------
    unordered_map<char, int> freq_map; // Hash Table to store the frequency of each character
    // cout << "Original Text :: " << input << " :: Original Size :: " << input.length() << endl;
    
    // Direct Huffman Encoding -- Just for testing - Bit packed - true.
    HuffmanResult d_huffman_compressed = huffman_encoding_compress(input, true, false); // Takes in (input, bit_packed_flag, debug_flag).

    // LZ77 Compression
    Token* compressed_data = lz77_compress(input); // Retrieves the pointer to the token array.
    string lz77_compressed_string = get_serialized_string_from_token_arr(compressed_data); // Picking up serialized string of the token object array.
    // string lz77_compressed_string = get_string_from_token_arr(compressed_data);

    // cout << "LZ77 Compressed Data :: " << lz77_compressed_string << " :: Size :: " << lz77_compressed_string.length() << endl;
    
    // Huffman Encoding
    HuffmanResult huffman_compressed = huffman_encoding_compress(lz77_compressed_string, true, false);
    // cout << "Direct Huffman Compressed :: " << d_huffman_compressed.data << " :: Size :: " << d_huffman_compressed.data.length() << endl;
    // cout << "Deflate Output :: " << huffman_compressed.data << " :: Size :: " << huffman_compressed.data.length() << endl;
    
    cout << "Original Size :: " << input.length() << " :: Direct Huffman :: " << d_huffman_compressed.data.length() << " :: LZ77 Compression Size :: " << lz77_compressed_string.length() << " :: Deflate Output Size :: " << huffman_compressed.data.length() << endl;
    cout << "Original Size :: " << input.size() << " :: Direct Huffman :: " << d_huffman_compressed.data.size() << " :: LZ77 Compression Size :: " << lz77_compressed_string.size() << " :: Deflate Output Size :: " << huffman_compressed.data.size() << endl;

    cout << "CR : Direct Huffman :: " << (float)(input.size() / (1.0 * d_huffman_compressed.data.size())) << endl;
    cout << "CR : LZ77 :: " << (float)(input.size() / (1.0 * lz77_compressed_string.size())) << endl;
    cout << "CR : Deflate :: " << (float)(input.size() / (1.0 * huffman_compressed.data.size())) << endl;

    // --------------------------------------- INFLATE -------------------------------------------------
    string huffman_decoded = huffman_encoding_decompress(huffman_compressed.data, true, huffman_compressed.total_bits, false);
    int tokens_count = 0;

    // cout << "Huffman Decoded String :: " << huffman_decoded << " :: Size :: " << huffman_decoded.size() << endl;

    Token* recovered_tokens_arr = unserialize_tokens(huffman_decoded, tokens_count);
    string decompressed = lz77_decompress(recovered_tokens_arr);
    // cout << "LZ77 Deserialized String :: " << decompressed << " :: Size :: " << decompressed.size() << endl;
    // cout << "Input string :: " << input << " :: Size :: " << input.size() << endl;
    // cout << "Decompressed String :: " << decompressed << " :: Size :: " << decompressed.size() << endl;
    cout << "Decompression verified :: " << (input == decompressed) << endl;

    delete[] recovered_tokens_arr;
    delete[] compressed_data;

    return 0;
}