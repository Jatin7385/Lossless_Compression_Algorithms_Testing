#ifndef DEFLATE_H
#define DEFLATE_H

#include <string>
#include <vector>
#include <cstdint>

/*
    RFC 1951 DEFLATE Compression
    
    Uses:
    - LZ77 from lz77_compression.cpp
    - Fixed Huffman codes (BTYPE=01) for standard decompressor compatibility
    
    Output can be decompressed with: gzip -d, Python zlib, etc.
*/

struct DeflateResult {
    std::vector<uint8_t> data;
    size_t total_bits;
    size_t original_size;
};

// File I/O
std::string readFile(std::string fileName, bool debug = false);

// DEFLATE compression/decompression
DeflateResult deflate_compress(const std::string& input, bool debug = false);
std::string deflate_decompress(const std::vector<uint8_t>& data, bool debug = false);

// Gzip wrapper (RFC 1952)
std::vector<uint8_t> wrap_gzip(const std::vector<uint8_t>& deflate_data, size_t original_size);
std::vector<uint8_t> gzip_compress(const std::string& input, bool debug = false);

#endif
