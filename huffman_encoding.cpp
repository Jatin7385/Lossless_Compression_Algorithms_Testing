/*
    Implementing Huffman Encoding in C++ for fun.

    Fundamental Idea behind Huffman Encoding is to assign prefix-free shorter codes to more frequent characters.
    Prefix-free means that no code is a prefix of any other code.

    This is also a refresher for me on C++. So, don't mind the very basic comments.

    Steps : 
    1. Count the frequency of each character in the input text.
    2. Build a Huffman Tree(Min Heap) from the frequency of the characters.
    3. Assign codes to the characters in the Huffman Tree. Assign 0 to the left child and 1 to the right child.
    4. Encode the input text using the Huffman Tree.
    5. Decode the encoded text using the Huffman Tree.
*/

#include <iostream> // Provides input and output stream functionality (std:cout, std:cin)
#include <string> // Provides string functionality
#include <unordered_map> // Provides unordered_map(Hash Table) functionality
#include <queue> // Provides priority_queue functionality
#include <algorithm> // Provides sort functionality
#include <bitset> // Provides bitset functionality
# include "huffman_encoding.h"

using namespace std; // Use std namespace to avoid writing std:: prefix

Node* root;

// Quick Notes : 
// int* p -> p is a pointer to an integer
// p = &x -> p stores the address of x
// *p -> value at the address stored in p
// int& r = x -> r is a reference to x.

/** 
    Function to print the heap contents one after the other. 
    @param MinHeap* min_heap : Pointer to the Min Heap
    @return void
*/
void print_heap(MinHeap* min_heap)
{   
    std::cout << "--------------------------------Heap Contents--------------------------------" << std::endl;
    // Printing the Heap to confirm the nodes are being added correctly.
    while (min_heap->pq.size() > 0) 
    {
        // Pop the nodes and print
        Node* top = min_heap->pq.top();
        min_heap->pq.pop();
        std::cout << top->data << " : " << top->freq << std::endl;
    }
}

/** 
    Function to print the Huffman Tree.
    @param Node* root : Pointer to the Root of the Huffman Tree
    @return void
*/
void print_huffman_tree(Node* root)
{
    if (root != nullptr) {
        std::cout << root->data << " : " << root->freq << std::endl;
        if (root->left != nullptr) {
            print_huffman_tree(root->left);
        }
        if (root->right != nullptr) {
            print_huffman_tree(root->right);
        }
    }
}

/** 
    Function to print the frequency map.
    @param unordered_map<char, int>& freq_map : Reference to the frequency map
    @return void
*/
void print_frequency_map(unordered_map<char, int>& freq_map)
{   
    // std::cout is used to print to the console.
    // std::endl does 2 things : 1. Prints a newline character (\n) 2. Flushes the output buffer.(Flushing is expensive)
    std::cout << "--------------------------------Frequency Map--------------------------------" << std::endl; // Print the frequency map if debug mode is enabled.
    // auto& is used to automatically deduce the type of the variable from the reference.
    for (auto& pair : freq_map) {
        std::cout << pair.first << " : " << pair.second << " || "; // Print the character and its frequency.
    }
    std::cout << std::endl; // Print a newline character.
}

/*
    Function to build the Huffman Tree using the frequency map.
    @param unordered_map<char, int>& freq_map : Reference to the frequency map
    @return Node* : Pointer to the Root of the Huffman Tree

    Steps : 
    1. Create a min heap of nodes from the frequency map.
    2. While the heap has more than one node, do the following :
        a. Pop the two nodes with the lowest frequency from the heap.
        b. Create a new node with the sum of the frequencies of the two nodes.
        c. Push the new node back into the heap.
    3. The root of the Huffman Tree is the only node left in the heap.
*/
Node* build_huffman_tree(unordered_map<char, int>& freq_map, bool debug) 
{
    // Create a new Min Heap object.
    MinHeap* min_heap = new MinHeap();
    // Create a leaf node for each character and add it
	// to the priority queue.
	for (auto pair: freq_map) {
		// Create a leaf node for the character and add it to the priority queue.
        // pair.first -> character
		// pair.second -> frequency
		// nullptr -> left and right child are nullptr as we are creating leaf nodes.
        min_heap->pq.push(new Node( pair.first, // character, 
                                    pair.second, // frequency, 
                                    nullptr, // left child, 
                                    nullptr)); // right child, 
	}
    
    // If Debug mode is enabled, print the heap contents.
    // if (debug) print_heap(min_heap);
    // print_heap(min_heap);
    std::cout << "Heap Contents : " << min_heap->pq.size() << std::endl;

    // While the heap has more than one node, do the following :
    while (min_heap->pq.size() > 1) {
        // Pop the two nodes with the lowest frequency from the heap.
        Node* left = min_heap->pq.top();
        min_heap->pq.pop();
        Node* right = min_heap->pq.top();
        min_heap->pq.pop();

        // Push the new node back into the heap.
        min_heap->pq.push(new Node('~', left->freq + right->freq, left, right));
    }
    
    if (debug) std::cout << "--------------------------------Huffman Tree Start--------------------------------" << std::endl;
    if (debug) print_huffman_tree(min_heap->pq.top());
    if (debug) std::cout << "--------------------------------Huffman Tree End--------------------------------" << std::endl;
    
    // The root of the Huffman Tree is the only node left in the heap.
    Node* root = min_heap->pq.top();
    // Clear the memory allocated in the Min Heap.
    // Cpp does not have a garbage collector like Java. Makes memory management more deterministic. delete is used to free the memory allocated to the objects.
    delete min_heap;
    return root;
}

/** 
    Function to count the frequency of each character in a string.
    @param string text : The input text
    @param unordered_map<char, int>& freq_map : Reference to the frequency map
    @return void
*/
void count_frequency(string& text, unordered_map<char, int>& freq_map)  //Note : & is used to pass the address of the map to the function.
{
    for (char c : text) {
        freq_map[c]++;
    }
}

/** 
    Function to get the Huffman Code lengths for each character.
    Note :: As per RFC 1951 for Deflate :: Deflate uses Canonical Huffman trees. This adds 2 rules to the original huffman impl.
    - Benefit is that the decoder does not need the entire tree sent over the network, instead it can recreate the tree with
    code lengths and the rules.
    @param Node* root : Pointer to the Root of the Huffman Tree
    @param int depth : The current depth or bit length.
    @param unordered_map<int, int>& codeLen : Reference to the Huffman Code Len map
    @param bool debug : Debug flag
    @return void
*/
void get_code_lengths(Node* root,
                      int depth,
                      unordered_map<int, int>& codeLen,
                      bool debug)
{
    // Root is null
    if (!root) return;
    
    // Leaf node → assign code length
    if (!root->left && !root->right) {
        // DEFLATE requires at least 1 bit.
       int length = (depth == 0) ? 1 : depth;

        codeLen[(unsigned char)root->data] = length;

        if (debug) {
            std::cout << "Symbol "
                      << (int)(unsigned char)root->data
                      << " :: Representing :: " << root->data
                      << " : length = " << length << std::endl;
        }
        return;
    }

    get_code_lengths(root->left,  depth + 1, codeLen, debug);
    get_code_lengths(root->right, depth + 1, codeLen, debug);
}

/** 
    Function to get Bit Packed encoded text.
    @param string text : The input text
    @param unordered_map<char, string>& huffmanCode : Reference to the Huffman Code map
    @param bool debug : debug flag
    @return HuffmanResult : The HuffmanResult structure holding the encoded text and the total bits used.

    Quick Note :: A string has characters with each char(1 or 0) holding 1 Byte/8 bits. Bit packing helps pack these into actual bits.
    This compresses the size.
*/
BitPackedResult get_encoded_bitpacked_text(
    string& text,
    unordered_map<int, HuffmanResult>& canonical_codes,
    bool debug = false
) {
    size_t total_bits = 0;
    for (char c : text) total_bits += canonical_codes.at(c).total_bits;

    std::vector<uint8_t> packed((total_bits + 7) / 8, 0); // Add 7 before dividing by 8 to round up to the nearest byte,
// ensuring all bits fit without truncation.

    size_t bit_pos = 0;

    for (char c : text) {
        HuffmanResult hr = canonical_codes.at(c);
        for (int i = 0; i < hr.total_bits; i++) {
            size_t byte_idx = bit_pos / 8;
            size_t bit_idx = bit_pos % 8;  // LSB first
            if (hr.bytes & (1 << i)) {
                packed[byte_idx] |= (1 << bit_idx);
            }
            bit_pos++;
        }
        if (debug) {
            std::cout << "Char: " << c 
                      << " Code: " << std::bitset<32>(hr.bytes).to_string().substr(32 - hr.total_bits) 
                      << std::endl;
        }
    }

    if (debug) {
        cout << "Packed bits: ";
        for (size_t i = 0; i < packed.size(); i++) {
            cout << bitset<8>(packed[i]) << " ";
        }
        cout << "\nTotal bits used: " << total_bits << endl;
    }

    return {packed, total_bits};
}

/** 
    Function to decode the encoded text.
    @param Node* root : Pointer to the Root of the Huffman Tree
    @param int &index : Reference to the index of the current character
    @param string str : The encoded text
    @param string& decoded_text : Reference to the decoded text
    @return void
*/
void get_decode_text(Node* root, int &index, string& encoded_text, string& decoded_text, bool debug)
{
	if (root == nullptr) {
		return;
	}

	// found a leaf node
	if (!root->left && !root->right)
	{
		decoded_text += root->data;
		return;
	}

	index++;

	if (encoded_text[index] =='0')
		get_decode_text(root->left, index, encoded_text, decoded_text, debug);
	else
		get_decode_text(root->right, index, encoded_text, decoded_text, debug);
}

/** 
    Function to decode the bit packed encoded text.
    @param vector<uint8_t>& packed_data : Referemce to the packed data
    @param string &canonical_codes : Reference to the Canonical Codes.
    @param bool debug : debug flag
    @return std::string decoded_text
*/
string get_bit_packed_decoded_text(vector<uint8_t>& packed_data,
                             unordered_map<int, HuffmanResult>& canonical_codes,
                             bool debug) {

    // Reverse map: code -> symbol (code stored LSB-first)
    unordered_map<uint32_t, pair<int,uint8_t>> code_to_symbol;
    for (auto& [sym, hr] : canonical_codes) {
        code_to_symbol[hr.bytes] = {sym, hr.total_bits};
    }

    string decoded;
    uint32_t buffer = 0;
    int bits_in_buffer = 0;

    for (uint8_t byte : packed_data) {
        buffer |= (byte << bits_in_buffer); // append byte LSB-first
        bits_in_buffer += 8;

        // Try to match codes in buffer
        bool matched_any = true;
        while (bits_in_buffer > 0 && matched_any) {
            matched_any = false;
            for (auto& [code, sym_len] : code_to_symbol) {
                int len = sym_len.second;
                if (len > bits_in_buffer) continue;

                uint32_t mask = (1U << len) - 1;
                if ((buffer & mask) == code) {
                    decoded += static_cast<char>(sym_len.first);
                    buffer >>= len;
                    bits_in_buffer -= len;
                    matched_any = true;
                    break;
                }
            }
        }
    }

    return decoded;
}


/*
    Function to free the memory allocated recursively to the Huffman Tree.
    @param Node* root : Pointer to the Root of the Huffman Tree
    @return void
*/
void free_huffman_tree(Node* root) 
{
    if (root != nullptr) 
    {
        free_huffman_tree(root->left);
        free_huffman_tree(root->right);
        delete root;
    }
}

/** 
  build_canonical_codes function generates canonical huffman codes.
  @param unordered_map<int, int> huffman_code_lengths - Extracted code lengths
  @param unordered_map<int, HuffmanResult> huffman_out_codes - Output canonical codes.
  @param bool debug - Debug Flag
  @return void - Output codes are stored in huffman_out_codes object.
*/
void build_canonical_codes(
    unordered_map<int, int>& huffman_code_lengths,
    unordered_map<int, HuffmanResult>& huffman_out_codes,
    bool debug
) {
    struct SymLen {
        int symbol;
        int length;
    };

    vector<SymLen> symbols;
    for (auto& [sym, len] : huffman_code_lengths) symbols.push_back({sym, len});

    // Canonical Rules based Sorting :: - Code length ascending && then Numerical Value Ascending for Length conflicts.
    // sort is an inbuilt function. (begin, end, comparator)
    // symbols.begin()/.end() → iterator to first/last element
    sort(symbols.begin(), symbols.end(),
         [](const SymLen& a, const SymLen& b) {
             if (a.length != b.length)
                 return a.length < b.length;
             return a.symbol < b.symbol;
    });

    if (debug) {
        cout << endl;

        cout << " ------------------ AFTER SORTING -------------" << endl;
        for (auto& s : symbols) {
            cout << "Symbol :: "
            << s.symbol
            << " :: Representing :: "
            << (char) s.symbol
            << " :: Length :: "
            << s.length
            <<endl;
        }
        

        cout << " ------------------ END SORTING -------------" << endl;
    }

    uint32_t code = 0;
    int prevLen = 0;

    // This loop is a bit difficult to understand at first, because there are a few important things to understand.
    /*
        - This loop is basically extracting huffman codes without creating a tree and parsing it.
        - It's maintaining the prefix code property.
        - It's maintaining the shortest symbols get shortest codes.
        - Decoder can follow the same steps without the tree.


        Example :: 
        Start: code = 0, prevLen = 0

        Symbol ' ' (length 3):
        code <<= 3-0 = 3 -> 000
        Assign code = 000 (binary) = 0
        Increment code: code = 1

        Symbol 'o' (length 3):
        code <<= 3-3 = 0 -> 001
        Assign code = 001
        Increment code: code = 2

        Symbol 's' (length 3):
        code <<= 0 -> 010
        Assign code = 010
        Increment code: code = 3

        Symbol 'a' (length 4):
        code <<= 4-3 = 1 -> 0110
        Assign code = 0110
        Increment code: code = 0111

        Symbol 'e' (length 4):
        code <<= 0 -> 0111
        Assign code = 0111
        Increment code: code = 1000

        Symbol 'i' (length 4):
        code <<= 0 -> 1000
        Assign code = 1000
        Increment code: code = 1001

        Tree Equivalence :: 
        Depth 3: ' ' = 000, 'o' = 001, 's' = 010
       0
     /   \
   0       1
  / \     / \
 ' ' 'o' 's' ...

        When moving to Depth 4, code << 1 ensures the new codes start after all depth-3 codes.
        Depth 4: 'a' = 0110, 'e' = 0111, 'i' = 1000 ...

        (root)
       /      \
     0          1
    / \        /  \
  0    1     0     1
 ' '   'o'  's'   ?
               \
               (Depth 4)
                /  \
              0      1
             / \    / \
           'a' 'e''i' 'l'
                / \
              'm' 'n'


    */
    for (auto& s : symbols) {

        if (debug) {
            cout << "s.length :: " 
                << (s.length) 
                << " :: prevLen :: "
                << prevLen
                << " :: (s.length - prevLen) :: "
                << (s.length - prevLen)
                << endl;
        }

        // If we go deeper in the tree, descend left until we reach the required depth.
        code <<= (s.length-prevLen);
        
        huffman_out_codes[s.symbol] = {
            code,
            (uint8_t)s.length
        };

        if (debug) {
            std::cout << "Symbol " << s.symbol
                      << " : code = "
                      << std::bitset<32>(code).to_string().substr(32 - s.length)
                      << " (" << s.length << ")\n";
        }

        // Move to the next leaf at this depth (or subtree)
        code++;
        // All codes of the same length are contiguous
        prevLen = s.length;
    }

    if (debug) {
        for (auto& o : huffman_out_codes) {
            cout << "Symbol :: " << o.first << " :: Code :: " << o.second.bytes << " : code = "
                      << std::bitset<32>(o.second.bytes).to_string().substr(32 - o.second.total_bits) <<  " :: length :: " << o.second.total_bits << endl;
        }
    }
}


BitPackedResult huffman_encoding_compress(string& input, bool bit_packed, bool debug)
{
    // Variables
    unordered_map<char, int> freq_map; // Hash Table to store the frequency of each character
    unordered_map<int, int> huffman_code_lengths;
    unordered_map<int, HuffmanResult> huffman_out_codes;

    if (debug) cout << "Original Text : " << input << endl;

    // Count the frequency of each character in the input text
    count_frequency(input, freq_map);
    if (debug) print_frequency_map(freq_map); // Print the frequency map if debug mode is enabled.

    // Build the Huffman Tree
    Node* root = build_huffman_tree(freq_map, debug);

    // Get the Huffman Codes for each character
    std::cout << "--------------------------------Huffman Codes--------------------------------" << std::endl;
    get_code_lengths(root, 0, huffman_code_lengths, debug);

    build_canonical_codes(huffman_code_lengths, huffman_out_codes, debug);

    // Get Encoded Text
    BitPackedResult encoded_text = get_encoded_bitpacked_text(input, huffman_out_codes, debug);

    return encoded_text;
}



// string huffman_encoding_decompress(string& compressed_input, bool bit_packed_flag, int total_bits, bool debug)
// {

//     // cout << "Bit packed << " << bit_packed_flag :: endl;
//     // Decode the Encoded Text
//     // traverse the Huffman Tree again and this time
// 	// decode the encoded string
//     string decoded_text = "";
//     if (!bit_packed_flag)
//     {
//         int index = -1;
//         while ((index + 1) < (int)compressed_input.size()) {
//             get_decode_text(root, index, compressed_input, decoded_text, debug);
//         }
//     } else
//     {
//         decoded_text = get_bit_packed_decoded_text(root, compressed_input, total_bits, debug);
//     }
// 	if (debug) std::cout << "Decoded Text : " << decoded_text << std::endl;

//     // Free the memory allocated to the Huffman Tree.
//     free_huffman_tree(root);

//     return decoded_text;
// }


// /** 
//     Function to main function.
//     @return int : The exit status
// */
int main()
{   
    // Variables
    unordered_map<char, int> freq_map; // Hash Table to store the frequency of each character
    unordered_map<int, int> huffman_code_lengths;
    unordered_map<int, HuffmanResult> huffman_out_codes;
    bool debug = true; // Flag to enable debug mode - Print debug logs if true.
    
    
    // Input text
    string text = "Huffman Encoding is a lossless compression algorithm.";

    cout << "Original Text : " << text << endl;

    // Count the frequency of each character in the input text
    count_frequency(text, freq_map);
    if (debug) print_frequency_map(freq_map); // Print the frequency map if debug mode is enabled.

    // Build the Huffman Tree
    Node* root = build_huffman_tree(freq_map, debug);

    // Get the Huffman Codes for each character
    std::cout << "--------------------------------Huffman Codes--------------------------------" << std::endl;
    get_code_lengths(root, 0, huffman_code_lengths, debug);

    build_canonical_codes(huffman_code_lengths, huffman_out_codes, debug);

    // Get Encoded Text
    BitPackedResult encoded_text = get_encoded_bitpacked_text(text, huffman_out_codes, debug);

    // Now that we have our packed bits ready. huffman_out_codes - which is the table used for decoding refernce is sent in a header.

    string decoded = get_bit_packed_decoded_text(encoded_text.data, huffman_out_codes, debug);
    cout << "Decoded string :: " << decoded << endl;

    // Free the memory allocated to the Huffman Tree.
    free_huffman_tree(root);
}

// Execute on MacOS - clang++ huffman_encoding.cpp -o huffman_encoding && ./huffman_encoding
// clang++ is the compiler for C++ made by LLVM and -o is used to specify the output file name.