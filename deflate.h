#ifndef DEFLATE_H
#define DEFLATE_H

#include <string>
#include <unordered_map>
#include "huffman_encoding.h"

// Function to read the file and return the content as a string.
std::string readFile(std::string fileName, bool debug = false);

// Function to compress the input string using the Deflate algorithm.
HuffmanResult deflate_compress(std::string& input);

#endif