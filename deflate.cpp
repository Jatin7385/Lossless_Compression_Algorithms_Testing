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

    if(debug) cout << "File content:\n" << content << endl;

    file.close();

    return content;
}

int main()
{
    // string input = "Deflate compression";
    string input = readFile("./data.txt");
    unordered_map<char, int> freq_map; // Hash Table to store the frequency of each character
    cout << "Original Text :: " << input << " :: Original Size :: " << input.length() << endl;
    
    // Direct Huffman Encoding -- Just for testing
    string d_huffman_compressed = huffman_encoding_compress(input, false);

    // LZ77 Compression
    Token* compressed_data = lz77_compress(input);
    string lz77_compressed_string = get_string_from_token_arr(compressed_data);

    cout << "LZ77 Compressed Data :: " << lz77_compressed_string << " :: Size :: " << lz77_compressed_string.length() << endl;
    
    // Huffman Encoding
    string huffman_compressed = huffman_encoding_compress(lz77_compressed_string, false);

    cout << "Direct Huffman Compressed :: " << d_huffman_compressed << " :: Size :: " << d_huffman_compressed.length() << endl;
    cout << "Deflate Output :: " << huffman_compressed << " :: Size :: " << huffman_compressed.length() << endl;
    
    return 0;
}