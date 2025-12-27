# include <iostream>
# include <string>
# include <vector>
# include "lz77_compression.h"

using namespace std;

// To this (RFC 1951 standard): https://datatracker.ietf.org/doc/html/rfc1951#section-1
int search_buffer_size = 32768;  // 32KB = 2^15 // Size of the search buffer -- The substring already parsed.
int look_ahead_buffer_size = 258; //  Size of the look ahead buffer -- THe buffer ahead of the 
                                // current character to be searched for maximum possible match 
int counter = 0; // Counter to input data into the Tokens Array

/*
  Function used to compress the input text using LZ77 Compression.
  @param input - string input
  @return Token* - Pointer of the first token of the Token array.
*/
Token* lz77_compress(string input, bool debug)
{
    int input_length = input.length();
    /* Token Array in the worst case can be the length of input 
       Imagine input length to be 26 and input to be 'abcdefghi....z'
       Here, our final output will be an uncompressed Token array of size 26 
       with each Token having an offset and length of 0.
    */
    Token* output = new Token[input_length];
    int index = 0; // Current index variable while parsing the string

    string search_buffer;

    // We run a while loop from from 0 to input_length.
    while (index < input_length)
    {
        // This gives us the corrected start index. 
        // If <=0 then we start from 0, and move index positions, 
        // else start from corrected_index for a length of search_buffer_size
        int corrected_start_index = (index - search_buffer_size) <= 0 ? 0 : (index - search_buffer_size);
        int corrected_search_buffer_length = (corrected_start_index == 0) ? index : search_buffer_size;
        // If index - search_buffer_size <= 0, that means, search_buffer will 
        // start from 0, and be of the index's size. Once it's more than 0, 
        // we start sliding it.
        search_buffer = input.substr(corrected_start_index, corrected_search_buffer_length); 

        // Now we find the longest possible match.
        int match_length = 0;
        /*
        Steps : 
            - We loop through the search buffer till input[index] matches.
            - Once we find match_index, we start incrementing match_length and comparing strings. - Another way is compare chars.
            - Once the match stops, index - match_index = offset ||| match_length is the length. ||| input[index + 1] = next_char.
        */
        // Initializing Token with init values
        Token token;
        token.offset = 0;
        token.length_of_match = 0;

        // Looping through search buffer to find matches.
        for (int i = 0; i < corrected_search_buffer_length; i++)
        {
            match_length = 0;
            // Finding a match for char at index in search_buffer
            if(input[index] == search_buffer[i])
            {
                // We loop till input and search buffer keep matching from initial match location.
                while (
                    (index + match_length) < input.length() && // Out of bound check
                    (i + match_length) < corrected_search_buffer_length && // Out of bound check
                    match_length < look_ahead_buffer_size && // Look Ahead Buffer Size Check
                    input[index + match_length] == search_buffer[i + match_length] // Equality check
                ) {
                    match_length += 1;
                    // cout << "Matching :: " << input[index + match_length+1] << " == " << search_buffer[i + match_length+1] << " :: match_length : " << match_length << " :: i : " << i << endl;
                }
                
                if (token.length_of_match < match_length) // Max match check
                {
                    token.length_of_match = match_length;
                    token.offset = index - (corrected_start_index + i);
                }
            }
        }
        token.next_char = (index + token.length_of_match) < input.length() ? input[index + token.length_of_match] : '\0';
        if(debug) cout << "Index : " << index << " :: Current Char : " << input[index] << " :: search_buffer : " << search_buffer << " :: Offset : " << token.offset << " :: Length : " << token.length_of_match << " :: Next Char : " << token.next_char << " :: Matched String : " << input.substr(index, token.length_of_match) << " :: " << input.substr(index, token.length_of_match).length() << endl;

        // Appending it to the Tokens array
        output[counter] = token;
        counter++;
        // cout << "Updated Index :: " << index << " : " << (length+1) << " :: " << (index + length + 1)  << endl;
        index = index + (token.length_of_match+1); // Increment index/current position by maximum length of match.
    }

    return output;
}

// Unserialize and return Token Array
Token* unserialize_tokens(const string& binary, int& out_token_count)
{
    const int BYTES_PER_TOKEN = 5;

    if (binary.size() % BYTES_PER_TOKEN != 0) {
        cerr << "Corrupt LZ77 stream\n";
        return nullptr;
    }

    out_token_count = binary.size() / BYTES_PER_TOKEN;
    Token* tokens = new Token[out_token_count];

    int idx = 0;
    for (int i = 0; i < out_token_count; i++) {
        uint8_t b0 = (uint8_t)binary[idx++];
        uint8_t b1 = (uint8_t)binary[idx++];
        uint8_t b2 = (uint8_t)binary[idx++];
        uint8_t b3 = (uint8_t)binary[idx++];

        tokens[i].offset =
            (b0 << 8) | b1;

        tokens[i].length_of_match =
            (b2 << 8) | b3;

        tokens[i].next_char = binary[idx++];
    }

    return tokens;
}


string get_serialized_string_from_token_arr(Token* compressed_data)
{
    string binary = "";
    for (int i = 0; i < counter; i++) {
        // store offset and length as 2 bytes each
        binary += (char)((compressed_data[i].offset >> 8) & 0xFF);
        binary += (char)(compressed_data[i].offset & 0xFF);

        binary += (char)((compressed_data[i].length_of_match >> 8) & 0xFF);
        binary += (char)(compressed_data[i].length_of_match & 0xFF);

        // store next_char
        binary += compressed_data[i].next_char;
    }
    // cout << "LZ77 Serialized String :: " << binary << " :: Size :: " << binary.size() << endl;
    // cout << "Done :: " << endl;
    return binary;
}

string get_string_from_token_arr(Token* compressed_data)
{
    string lz77_compressed_string = "";
    int compressed_length = 0;
    for (int i = 0; i < counter; i++)
    {
        lz77_compressed_string += compressed_data[i].next_char;
        cout << compressed_data[i].next_char;
        compressed_length++;
    }
    return lz77_compressed_string;
}

/*
    Function to decompress data using LZ77 Decompression Algorithm.
    @param Token* compressed : Pointer for Compressed Token Array.
    @return string - decompressed data
*/
string lz77_decompress(Token* compressed)
{
    string decompressed_text = "";
    int pos = 0;
    // Loop through the Token array
    for(int i = 0; i < counter; i++)
    {
        // If offset is greater than 0, then we go back and reconstruct.
        if(compressed[i].offset > 0)
        {
            int start = (pos - compressed[i].offset);
            int len = compressed[i].length_of_match;
            while (len > 0) 
            {
                decompressed_text += decompressed_text[start]; // Decompression happens with decompressed data.
                start++;
                len--;
                pos++;
            }
        } 
        // '\0' is for checking char Null
        if (compressed[i].next_char != '\0') decompressed_text += compressed[i].next_char;
        pos++;
    }

    return decompressed_text;
}

// int main()
// {
//     // string text = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
//     string text = "The computerphile channel handles computer topics.";
//     cout << "Sizeof Char :: " << sizeof(char) << " :: Sizeof Int :: " << sizeof(uint8_t) << endl;
//     cout << "Original Text to be compressed :: " << text << " :: Size :: " << (sizeof(char) * text.length()) << endl;
//     Token* compressed_data = lz77_compress(text);

//     cout << "Compressed Data :: ";
//     int compressed_length = 0;
//     for (int i = 0; i < counter; i++)
//     {
//         cout << compressed_data[i].next_char;
//         compressed_length++;
//     }

//     cout << " :: Size : " << (counter * (sizeof(uint8_t) * 2 + sizeof(char))) << endl;

//     string decompressed_data = lz77_decompress(compressed_data);
//     cout << "Decompressed Data :: " << decompressed_data << " :: Size : " << (sizeof(char) * decompressed_data.length()) << endl;

//     // Memory leaks prevention
//     delete[] compressed_data;
//     return 0;
// }