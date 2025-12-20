#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "deflate.h"

using namespace std;

/*
CRC (Cyclic Redundancy Check)
- A powerful error detection code that verifies data integrity in digital networks and storage.
- Treats data as a binary number, dividing it by a fixed generator polynomial and appending the remainder (CRC bits) to the data.
- Receiver repeats the process with same polynomial and if the remainder is zero, data is accepted signalling no errors.
- uint32_t -> unsigned 32-bit integer(0 to 2^32 = 4,294,967,295) -> _t represents type, u represents unsigned, int32 is self-explanatory.

Normal poly: x³² + x²⁶ + x²³ + x²² + x¹⁶ + x¹² + x¹¹ + x¹⁰ + x⁸ + x⁷ + x⁵ + x⁴ + x² + x + 1
Hex (MSB):   0x04C11DB7
Hex (LSB):   0xEDB88320  ← reversed bits

- Can detect all odd errors, single bit.
- We append the maximum degree of the polynomial as redundant bits(32 bits here).
- Lets take an example of polynomial : x^4 + x^3 + x^2 + 1
- Coefficients : 1.x^4 + 1.x^3 + 0.x^2 + 0.x^1 + 1.x^0 --> 11001 divide by the binary message.
- Binary division done using XOR operation.

    Take lowest byte of CRC
    XOR it with next input byte
    ↓
    Use result as a table index
    ↓
    Shift CRC by one byte
    ↓
    Mix in precomputed polynomial effect

*/

// CRC-32 polynomial (reversed, standard Ethernet/Zlib/Gzip)
// 0xEDB88320 = hex value = 393,689,524 decimal - reversed (LSB-first)
const uint32_t CRC32_POLY = 0xEDB88320u;

// Fast table-driven CRC32 (Gzip standard)
class CRC32 {
private:
    uint32_t table[256];

    void print_table() {
        for (uint32_t i = 0; i < 256; i++) {
            // cout << "Idx :: " << i << " :: Table[idx] :: " << table[i] << endl;
            uint32_t crc = table[i];
            for (int j = 0 ; j < 8 ; j++) {
                crc = (crc >> 1);
                cout << (crc) << " : ";
            }
            cout << endl;
        }
    }
    
    /*
        Precompute for 256. Why 256? 8 bits. -> 2^8 = 256.
        - Creates a lookup table that pre-computes CRC math for all 256 possible bytes.
        - For each byte i (0 - 255):
        - Process all 8 bits of that byte. - Bit by bit division.
        - crc&1 checks whether this bit needs division. 0 -> No division. 1 - Divide (XOR with polynomial.)
        - crc >> 1 -> Shift right (bring next bit into position.)
        - CRC32_POLY * (crc & 1) - Multiply/ Divide step.
    */
    void generate_table() {
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t crc = i; // Start with byte value
            for (int j = 0; j < 8; j++) { // 8 bits per byte
                crc = (crc >> 1) ^ (CRC32_POLY * (crc & 1));
            }
            table[i] = crc;
        }

        // print_table();
    }
    
public:
    CRC32() { generate_table(); }
    
    uint32_t compute(const uint8_t* data, size_t len) {
        uint32_t crc = 0xFFFFFFFFu;  // Initial value (Gzip standard) - All 32 bits set to 1 (4,294,967,295). u represents unsigned here too.
        // cout << "Length :: " << len << " :: CRC " << crc << endl;
        for (size_t i = 0; i < len; i++) {
            // cout << "Idx :: " << i << " :: Data : ";
            
            /*
                - crc is 32 bits, data[i] is 8 bits. The byte is xored into lowest 8 bits of crc
                - & 0xFF is masking to keep only the lowest 8 bits.
                - idx = lower 8 bits of (crc ^ data[i])
                - idx tells us which precomputed CRC value should be used for this Byte.
            */
            uint32_t idx = (crc ^ data[i]) & 0xFF; // (FF)16 = (255)10
            // cout << data[i] << " :: crc^data[i] :: " << (crc^data[i]) << " & 0xFF :: " << idx << " :: crc >> 8 :: " << (crc >> 8);
            
            // We now shift CRC right by 1 Byte, discarding the byte we just processed.
            // table[idx] - precomputed CRC result for a byte value. Represents 8 polynomial steps at once.
            crc = (crc >> 8) ^ table[idx];

            // cout << " :: Table[Idx] : " << table[idx] << endl;  
        }
        
        return ~crc;  // Finalize (invert bits)
    }
};


int main()
{
    // string input = readFile("./data.txt");
    string input = "Gzip compression is a lossless compression.";
    cout << "Input size: " << input.size() << endl;
    HuffmanResult deflate_compressed = deflate_compress(input);
    cout << "Deflate compressed size: " << deflate_compressed.data.size() << endl;
    cout << "Deflate compressed data: " << deflate_compressed.data << endl;
    cout << "Deflate compressed total bits: " << deflate_compressed.total_bits << endl;

    // Write it to a .gz file.
    ofstream out("gzip_output.gz", ios::binary);

    if (!out){
        cerr << "Failed to create file\n";
        return 1;
    }

    // 1. Header
    out.put(0x1f);
    out.put(0x8b);
    out.put(0x08); // Deflate
    out.put(0x00); // FLG
    out.write("\0\0\0\0", 4); // MTIME
    out.put(0x00); // XFL
    out.put(0x03); // Unix

    // 2. Deflate data
    out.write(deflate_compressed.data.data(), deflate_compressed.data.size());    

    // 3. CRC32
    CRC32 crc;
    uint32_t checksum = crc.compute(reinterpret_cast<const uint8_t*>(input.data()), input.size());
    
    auto write_le32 = [&](uint32_t v) {
        out.put(v & 0xFF);
        out.put((v >> 8) & 0xFF);
        out.put((v >> 16) & 0xFF);
        out.put((v >> 24) & 0xFF);
    };

    write_le32(checksum);
    
    // 4. ISIZE
    write_le32(input.size());

    return 0;
}