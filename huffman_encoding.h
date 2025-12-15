# ifndef HUFFMAN_ENCODING_H
# define HUFFMAN_ENCODING_H

#include <string> // Provides string functionality
#include <unordered_map> // Provides unordered_map(Hash Table) functionality
#include <queue> // Provides priority_queue functionality
#include <vector>

// Huffman Result structure to store the string result and the total bits used.
struct HuffmanResult {
    std::string data;
    size_t total_bits;
};

struct BitReader {
    const std::string& data;
    size_t bit_pos = 0;
    size_t total_bits;

    BitReader(const std::string& d, size_t bits)
        : data(d), total_bits(bits) {}

    bool has_bits() const {
        return bit_pos < total_bits;
    }

    int read_bit() {
        size_t byte_idx = bit_pos / 8;
        size_t bit_idx  = 7 - (bit_pos % 8);
        bit_pos++;
        return (data[byte_idx] >> bit_idx) & 1;
    }
};

// Node structure for the Huffman Tree
struct Node { // Structs and Classes are the same in C++. Default member access and Inheritance in Structs are public by default and private in classes by default.
    char data; // Character data
    int freq; // Frequency of the character
    Node* left; // Left Child Pointer --> * is used to declare a pointer to a Node
    Node* right; // Right Child Pointer

    // Constructor to create a new node
    Node(char c, int freq, Node* left = nullptr, Node* right = nullptr) {
        this->data = c;
        this->freq = freq;
        this->left = left;
        this->right = right;
    }
};

// Min Heap structure to store the nodes of the Huffman Tree.
struct MinHeap {
    // Comparator function to compare the objects. (Used to convert the priority queue to a Min heap.)
    struct compare {
        bool operator()(Node* a, Node* b) {
            return a->freq > b->freq; // min-heap based on Node::freq
        }
    };

    // Priority Queue to store the nodes of the Huffman Tree.
    // So the priority queue/Min Heap is supposed to store Node type objects.
    // Priority Queue is a Max heap by default. Compare is used to convert it to a Min heap.
    // Parameters of the priority_queue constructor : 
        // 1. Type of the objects to be stored in the priority queue.
        // 2. Type of the container to be used to store the objects. (vector is used here.)
        // 3. Comparator function to compare the objects.
    std::priority_queue<Node*, std::vector<Node*>, compare> pq;
    // pq.top() -> returns the top element of the priority queue.
    // pq.push(node) -> pushes the node into the priority queue.
    // pq.pop() -> pops the top element of the priority queue.
    // pq.size() -> returns the size of the priority queue.
    // pq.empty() -> returns true if the priority queue is empty, false otherwise.
};

/** 
    Function to print the heap contents one after the other. 
    @param MinHeap* min_heap : Pointer to the Min Heap
    @return void
*/
void print_heap(MinHeap* min_heap);

/** 
    Function to print the Huffman Tree.
    @param Node* root : Pointer to the Root of the Huffman Tree
    @return void
*/
void print_huffman_tree(Node* root);

/** 
    Function to print the frequency map.
    @param std::unordered_map<char, int>& freq_map : Reference to the frequency map
    @return void
*/
void print_frequency_map(std::unordered_map<char, int>& freq_map);

/**
    Function to build the Huffman Tree using the frequency map.
    @param std::unordered_map<char, int>& freq_map : Reference to the frequency map
    @return Node* : Pointer to the Root of the Huffman Tree

    Steps : 
    1. Create a min heap of nodes from the frequency map.
    2. While the heap has more than one node, do the following :
        a. Pop the two nodes with the lowest frequency from the heap.
        b. Create a new node with the sum of the frequencies of the two nodes.
        c. Push the new node back into the heap.
    3. The root of the Huffman Tree is the only node left in the heap.
*/
Node* build_huffman_tree(std::unordered_map<char, int>& freq_map, bool debug = false);

/** 
    Function to count the frequency of each character in astd::string.
    @param std::stringtext : The input text
    @param std::unordered_map<char, int>& freq_map : Reference to the frequency map
    @return void
*/
void count_frequency(std::string& text, std::unordered_map<char, int>& freq_map);  //Note : & is used to pass the address of the map to the function.

/** 
    Function to get the Huffman Codes for each character.
    @param Node* root : Pointer to the Root of the Huffman Tree
    @param std::string code : The code for the current character
    @param std::unordered_map<char,std::string>& huffmanCode : Reference to the Huffman Code map
    @return void
*/
void get_huffman_codes(Node* root, std::string code, std::unordered_map<char,std::string>& huffmanCode, bool debug = false);

/** 
    Function to get the encoded text.
    @param std::stringtext : The input text
    @param std::unordered_map<char,std::string>& huffmanCode : Reference to the Huffman Code map
    @return HuffmanResult : The HuffmanResult structure holding the encoded text and the total bits used.
*/
HuffmanResult get_encoded_text(std::string& text, std::unordered_map<char,std::string>& huffmanCode, bool debug = false);

/** 
    Function to get Bit Packed encoded text.
    @param string text : The input text
    @param unordered_map<char, string>& huffmanCode : Reference to the Huffman Code map
    @return HuffmanResult : The HuffmanResult structure holding the encoded text and the total bits used.
*/
HuffmanResult get_encoded_bitpacked_text(std::string& text, std::unordered_map<char, std::string>& huffmanCode);

/** 
    Function to decode the encoded text.
    @param Node* root : Pointer to the Root of the Huffman Tree
    @param int &index : Reference to the index of the current character
    @param std::stringstr : The encoded text
    @paramstd::string& decoded_text : Reference to the decoded text
    @return void
*/
void get_decode_text(Node* root, int &index,std::string& encoded_text,std::string& decoded_text, bool debug = false);

/** 
    Function to decode the bit packed encoded text.
    @param Node* root : Pointer to the Root of the Huffman Tree
    @param string &packed : Reference to the packed string.
    @param size_t total_bits : Total Bits
    @param bool debug : debug flag
    @return std::string decoded_text
*/
std::string get_bit_packed_decoded_text(Node* root, const std::string& packed, size_t total_bits, bool debug = false);

/**
    All encompassing function to take in the input and return compressed output.
    @param string input
    @param bool debug
    @returns HuffmanResult : The HuffmanResult structure holding the encoded text and the total bits used.
*/
HuffmanResult huffman_encoding_compress(std::string& input, bool bit_packed = false, bool debug = false);

/**
    All encompassing function to take in the compressed input and return decompressed output.
    @param string compressed_input
    @param bool debug
    @returns string decompressed output
*/
std::string huffman_encoding_decompress(std::string& compressed_input, bool bit_packed, int total_bits, bool debug = false);

/**
    Function to free the memory allocated recursively to the Huffman Tree.
    @param Node* root : Pointer to the Root of the Huffman Tree
    @return void
*/
void free_huffman_tree(Node* root);

#endif