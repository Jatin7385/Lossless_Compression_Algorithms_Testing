#ifndef FIXED_HUFFMAN_ENCODING_H
#define FIXED_HUFFMAN_ENCODING_H

#include <cstdint>

/*
    RFC 1951 Fixed Huffman Codes (Section 3.2.6)
    
    Literal/Length codes (0-287):
      0-143:   8 bits, codes 00110000 - 10111111
      144-255: 9 bits, codes 110010000 - 111111111
      256-279: 7 bits, codes 0000000 - 0010111  (256 = END_OF_BLOCK)
      280-287: 8 bits, codes 11000000 - 11000111
    
    Distance codes (0-29):
      All 5 bits, code equals symbol value
*/

struct FixedCode { 
    uint16_t code;    // Huffman code value (MSB-first as per RFC)
    uint8_t length;   // Number of bits
};

// Get Fixed Huffman code for literal (0-255), end-of-block (256), or length (257-287)
inline FixedCode get_fixed_litlen_code(int sym) {
    if (sym <= 143)      return {static_cast<uint16_t>(0b00110000 + sym), 8};
    else if (sym <= 255) return {static_cast<uint16_t>(0b110010000 + (sym - 144)), 9};
    else if (sym <= 279) return {static_cast<uint16_t>(sym - 256), 7};
    else                 return {static_cast<uint16_t>(0b11000000 + (sym - 280)), 8};
}

// Get Fixed Huffman code for distance (0-29)
inline FixedCode get_fixed_distance_code(int sym) {
    return {static_cast<uint16_t>(sym), 5};
}

#endif // FIXED_HUFFMAN_ENCODING_H
