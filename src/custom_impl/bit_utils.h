#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#include <vector>
#include <cstdint>

/*
    Bit utilities for RFC 1951 DEFLATE
    
    BitReader: Reads bits LSB-first from a byte stream
    BitWriter: Writes bits LSB-first to a byte stream
    
    DEFLATE uses LSB-first bit packing within bytes.
*/

class BitReader {
    const std::vector<uint8_t>& data;
    size_t bit_pos = 0;
    
public:
    BitReader(const std::vector<uint8_t>& d) : data(d) {}
    
    // Read 'count' bits in LSB-first order
    uint32_t read_bits(int count) {
        uint32_t result = 0;
        for (int i = 0; i < count; i++) {
            if (bit_pos / 8 < data.size()) {
                if (data[bit_pos / 8] & (1 << (bit_pos % 8))) {
                    result |= (1 << i);
                }
            }
            bit_pos++;
        }
        return result;
    }
    
    // Read one bit for MSB-first Huffman code accumulation
    int read_bit_msb() {
        if (bit_pos / 8 >= data.size()) return 0;
        int bit = (data[bit_pos / 8] >> (bit_pos % 8)) & 1;
        bit_pos++;
        return bit;
    }
    
    // Read a Fixed Huffman literal/length code (RFC 1951 Section 3.2.6)
    int read_fixed_litlen_code() {
        uint32_t code = 0;
        
        // Try 7 bits first (codes 256-279)
        for (int i = 0; i < 7; i++) code = (code << 1) | read_bit_msb();
        if (code <= 23) return 256 + code;
        
        // Try 8 bits (codes 0-143 and 280-287)
        code = (code << 1) | read_bit_msb();
        if (code >= 0b00110000 && code <= 0b10111111) return code - 0b00110000;
        if (code >= 0b11000000 && code <= 0b11000111) return 280 + (code - 0b11000000);
        
        // Try 9 bits (codes 144-255)
        code = (code << 1) | read_bit_msb();
        if (code >= 0b110010000 && code <= 0b111111111) return 144 + (code - 0b110010000);
        
        return -1;  // Invalid code
    }
    
    // Read a Fixed Huffman distance code (always 5 bits)
    int read_fixed_distance_code() {
        uint32_t code = 0;
        for (int i = 0; i < 5; i++) code = (code << 1) | read_bit_msb();
        return code;
    }
    
    bool has_bits() const { return bit_pos / 8 < data.size(); }
    size_t position() const { return bit_pos; }
};


class BitWriter {
public:
    std::vector<uint8_t> data;
    size_t bit_pos = 0;
    
    // Write 'count' bits in LSB-first order (for extra bits, block header)
    void write_bits(uint32_t value, int count) {
        for (int i = 0; i < count; i++) {
            if (bit_pos / 8 >= data.size()) data.push_back(0);
            if (value & (1 << i)) data[bit_pos / 8] |= (1 << (bit_pos % 8));
            bit_pos++;
        }
    }
    
    // Write Huffman code (MSB-first code, packed LSB-first into bytes)
    void write_code_reversed(uint32_t code, int length) {
        for (int i = length - 1; i >= 0; i--) {
            if (bit_pos / 8 >= data.size()) data.push_back(0);
            if (code & (1 << i)) data[bit_pos / 8] |= (1 << (bit_pos % 8));
            bit_pos++;
        }
    }
    
    size_t total_bits() const { return bit_pos; }
};

#endif // BIT_UTILS_H