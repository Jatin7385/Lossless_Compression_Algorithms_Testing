#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "deflate.h"

using namespace std;

int main()
{
    string input = readFile("./data.txt");
    cout << "Input size: " << input.size() << endl;
    HuffmanResult deflate_compressed = deflate_compress(input);
    cout << "Deflate compressed size: " << deflate_compressed.data.size() << endl;
    cout << "Deflate compressed data: " << deflate_compressed.data << endl;
    cout << "Deflate compressed total bits: " << deflate_compressed.total_bits << endl;
    return 0;
}