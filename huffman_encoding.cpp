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

using namespace std; // Use std namespace to avoid writing std:: prefix

// Quick Notes : 
// int* p -> p is a pointer to an integer
// p = &x -> p stores the address of x
// *p -> value at the address stored in p
// int& r = x -> r is a reference to x.

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
    priority_queue<Node*, vector<Node*>, compare> pq;
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
Node* build_huffman_tree(unordered_map<char, int>& freq_map, bool debug = false) 
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
void count_frequency(string text, unordered_map<char, int>& freq_map)  //Note : & is used to pass the address of the map to the function.
{
    for (char c : text) {
        freq_map[c]++;
    }
}

/** 
    Function to get the Huffman Codes for each character.
    @param Node* root : Pointer to the Root of the Huffman Tree
    @param string code : The code for the current character
    @param unordered_map<char, string>& huffmanCode : Reference to the Huffman Code map
    @return void
*/
void get_huffman_codes(Node* root, string code, unordered_map<char, string>& huffmanCode, bool debug = false)
{
    if (root != nullptr) {
        if (root->left == nullptr && root->right == nullptr) {
            huffmanCode[root->data] = code;
            std::cout << root->data << " : " << code << std::endl;
        }
        else {
            get_huffman_codes(root->left, code + "0", huffmanCode, debug);
            get_huffman_codes(root->right, code + "1", huffmanCode, debug);
        }
    }
}

/** 
    Function to get the encoded text.
    @param string text : The input text
    @param unordered_map<char, string>& huffmanCode : Reference to the Huffman Code map
    @return string : The encoded text
*/
string get_encoded_text(string text, unordered_map<char, string>& huffmanCode, bool debug = false)
{
    string encoded_text = "";
    for (char c : text) {
        encoded_text += huffmanCode[c];
    }
    return encoded_text;
}

/** 
    Function to decode the encoded text.
    @param Node* root : Pointer to the Root of the Huffman Tree
    @param int &index : Reference to the index of the current character
    @param string str : The encoded text
    @param string& decoded_text : Reference to the decoded text
    @return void
*/
void get_decode_text(Node* root, int &index, string str, string& decoded_text, bool debug = false)
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

	if (str[index] =='0')
		get_decode_text(root->left, index, str, decoded_text, debug);
	else
		get_decode_text(root->right, index, str, decoded_text, debug);
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
    Function to main function.
    @return int : The exit status
*/
int main()
{   
    // Variables
    unordered_map<char, int> freq_map; // Hash Table to store the frequency of each character
    unordered_map<char, string> huffmanCode;
    bool debug = false; // Flag to enable debug mode - Print debug logs if true.
    
    
    // Input text
    string text = "Huffman";

    // Count the frequency of each character in the input text
    count_frequency(text, freq_map);
    if (debug) print_frequency_map(freq_map); // Print the frequency map if debug mode is enabled.

    // Build the Huffman Tree
    Node* root = build_huffman_tree(freq_map, debug);

    // Get the Huffman Codes for each character
    std::cout << "--------------------------------Huffman Codes--------------------------------" << std::endl;
    get_huffman_codes(root, "", huffmanCode, debug);

    // Get Encoded Text
    string encoded_text = get_encoded_text(text, huffmanCode, debug);
    std::cout << "Encoded Text : " << encoded_text << std::endl;

    // Decode the Encoded Text
    // traverse the Huffman Tree again and this time
	// decode the encoded string
	int index = -1;
	string decoded_text = "";
	while (index < (int)encoded_text.size() - 2) {
		get_decode_text(root, index, encoded_text, decoded_text, debug);
	}
	std::cout << "Decoded Text : " << decoded_text << std::endl;

    // Free the memory allocated to the Huffman Tree.
    free_huffman_tree(root);
}

// Execute on MacOS - clang++ huffman_encoding.cpp -o huffman_encoding && ./huffman_encoding
// clang++ is the compiler for C++ made by LLVM and -o is used to specify the output file name.