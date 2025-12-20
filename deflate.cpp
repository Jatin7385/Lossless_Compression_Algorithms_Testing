#include <iostream>
#include <fstream>   // For file streams
#include <sstream>
#include <string>
#include <unordered_map>
#include "huffman_encoding.h"
#include "lz77_compression.h"
#include <chrono>
#include "deflate.h"

using namespace std;

string readFile(string fileName, bool debug)
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

// Function to compress the input string using the Deflate algorithm.
HuffmanResult deflate_compress(string& input)
{
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
    HuffmanResult deflate_compressed = huffman_encoding_compress(lz77_compressed_string, true, false); // Takes in (input, bit_packed_flag, debug_flag).

    return deflate_compressed;
}

// int main()
// {
//     return 0;
// }

// int main()
// {
//     // string input = "Deflate compression";
//     string input = readFile("./data.txt");
//     // ----------------------------------- DEFLATE -------------------------------------------------
//     unordered_map<char, int> freq_map; // Hash Table to store the frequency of each character
//     // cout << "Original Text :: " << input << " :: Original Size :: " << input.length() << endl;
    
//     auto start = chrono::high_resolution_clock::now();
//     // Direct Huffman Encoding -- Just for testing - Bit packed - true.
//     HuffmanResult d_huffman_compressed = huffman_encoding_compress(input, true, false); // Takes in (input, bit_packed_flag, debug_flag).

//     // LZ77 Compression
//     Token* compressed_data = lz77_compress(input); // Retrieves the pointer to the token array.
//     string lz77_compressed_string = get_serialized_string_from_token_arr(compressed_data); // Picking up serialized string of the token object array.
//     // string lz77_compressed_string = get_string_from_token_arr(compressed_data);

//     // cout << "LZ77 Compressed Data :: " << lz77_compressed_string << " :: Size :: " << lz77_compressed_string.length() << endl;
    
//     // Huffman Encoding
//     HuffmanResult huffman_compressed = huffman_encoding_compress(lz77_compressed_string, true, false);
//     // cout << "Direct Huffman Compressed :: " << d_huffman_compressed.data << " :: Size :: " << d_huffman_compressed.data.length() << endl;
//     // cout << "Deflate Output :: " << huffman_compressed.data << " :: Size :: " << huffman_compressed.data.length() << endl;
//     auto end = chrono::high_resolution_clock::now();
//     auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
//     cout << "Time taken: " << duration.count()/1000.0 << " seconds" << endl;

//     cout << "Original Size :: " << input.length() << " :: Direct Huffman :: " << d_huffman_compressed.data.length() << " :: LZ77 Compression Size :: " << lz77_compressed_string.length() << " :: Deflate Output Size :: " << huffman_compressed.data.length() << endl;
//     cout << "Original Size :: " << input.size() << " :: Direct Huffman :: " << d_huffman_compressed.data.size() << " :: LZ77 Compression Size :: " << lz77_compressed_string.size() << " :: Deflate Output Size :: " << huffman_compressed.data.size() << endl;

//     cout << "CR : Direct Huffman :: " << (float)(input.size() / (1.0 * d_huffman_compressed.data.size())) << endl;
//     cout << "CR : LZ77 :: " << (float)(input.size() / (1.0 * lz77_compressed_string.size())) << endl;
//     cout << "CR : Deflate :: " << (float)(input.size() / (1.0 * huffman_compressed.data.size())) << endl;


//     cout << "Compression Percentage :: Direct Huffman :: " << (float)(input.size() - d_huffman_compressed.data.size()) / (1.0 * input.size()) * 100 << "%" << endl;
//     cout << "Compression Percentage :: LZ77 :: " << (float)(input.size() - lz77_compressed_string.size()) / (1.0 * input.size()) * 100 << "%" << endl;
//     cout << "Compression Percentage :: Deflate :: " << (float)(input.size() - huffman_compressed.data.size()) / (1.0 * input.size()) * 100 << "%" << endl;

//     // --------------------------------------- INFLATE -------------------------------------------------
//     string huffman_decoded = huffman_encoding_decompress(huffman_compressed.data, true, huffman_compressed.total_bits, false);
//     int tokens_count = 0;

//     // cout << "Huffman Decoded String :: " << huffman_decoded << " :: Size :: " << huffman_decoded.size() << endl;

//     Token* recovered_tokens_arr = unserialize_tokens(huffman_decoded, tokens_count);
//     string decompressed = lz77_decompress(recovered_tokens_arr);
//     // cout << "LZ77 Deserialized String :: " << decompressed << " :: Size :: " << decompressed.size() << endl;
//     // cout << "Input string :: " << input << " :: Size :: " << input.size() << endl;
//     // cout << "Decompressed String :: " << decompressed << " :: Size :: " << decompressed.size() << endl;
//     cout << "Decompression verified :: " << (input == decompressed) << endl;


//     // ------------------------------ Actual ZLIB Deflate Library Results ----------------------------------
//     // z_stream strm;
//     // strm.zalloc = Z_NULL; strm.zfree = Z_NULL; strm.opaque = Z_NULL;
//     // inflateInit2(&strm, -15);  // Raw deflate (no zlib/gzip headers)

//     // strm.avail_in = huffman_compressed.data.size();
//     // strm.next_in = (Bytef*)huffman_compressed.data.data();

//     // char zlib_out[65536];
//     // std::string zlib_decompressed;
//     // do {
//     //     strm.avail_out = sizeof(zlib_out);
//     //     strm.next_out = (Bytef*)zlib_out;
//     //     inflate(&strm, Z_NO_FLUSH);
//     //     zlib_decompressed.append(zlib_out, sizeof(zlib_out) - strm.avail_out);
//     // } while (strm.avail_out == 0);

//     // inflateEnd(&strm);

//     // cout << "zlib raw deflate compatible: " << (input == zlib_decompressed ? "YES" : "NO") << endl;


//     delete[] recovered_tokens_arr;
//     delete[] compressed_data;

//     return 0;
// }