# include <iostream>
# include <string>
# include <vector>

using namespace std;

int search_buffer_size = 43; // Size of the search buffer -- The substring already parsed.
int look_ahead_buffer_size = 8; //  Size of the look ahead buffer -- THe buffer ahead of the 
                                // current character to be searched for maximum possible match 
int counter = 0; // Counter to input data into the Tokens Array

/*
  The structure holding the metadata of characters.
  - Offset : How many characters do I need to go back to find the longest match from my current position.
  - Length : Length of the longest match found from the offset position.
  - Next Char : Still not very sure. Holds the next character after the longest match.

  1 Token -> 4B + 4B + 1B = 9 Bytes. This is code/debugging friendly, but instead of compressing total size,
  it expands the size due to the overhead.

  Eg : 
    --> Original Text to be compressed :: The computerphile channel handles computer topics. :: Size :: 50
    --> Compressed Data :: The computrhilhane dsrtpc. :: Size : 234 -> 26*9 ==> That's expansion instead of compression.
    --> Decompressed Data :: The computerphile channel handles computer topics. :: Size : 50

    Even with uint8_t -> 3Bytes per token. 3*26 = 78. Original Size was 50. Helps, but still no good.

  Ideally the solution is to use a BitWriter and BitReader instead. (Writing bits to stream).
*/
struct Token
{
    // int offset; // The offset of the longest match. ---> int uses 32 bits - 4 Bytes
    // int length_of_match; // The length of the longest match.
    uint8_t offset; // uint8_t -> 8 bits - 1 Byte
    uint8_t length_of_match;
    char next_char; // The next char after the longest match. --> char uses 8bits - 1 Byte.
};

/*
  Function used to compress the input text using LZ77 Compression.
  @param input - string input
  @return Token* - Pointer of the first token of the Token array.
*/
Token* compress(string input)
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
        cout << "Index : " << index << " :: Current Char : " << input[index] << " :: search_buffer : " << search_buffer << " :: Offset : " << token.offset << " :: Length : " << token.length_of_match << " :: Next Char : " << token.next_char << " :: Matched String : " << input.substr(index, token.length_of_match) << " :: " << input.substr(index, token.length_of_match).length() << endl;

        // Appending it to the Tokens array
        output[counter] = token;
        counter++;
        // cout << "Updated Index :: " << index << " : " << (length+1) << " :: " << (index + length + 1)  << endl;
        index = index + (token.length_of_match+1); // Increment index/current position by maximum length of match.
    }

    return output;
}

/*
    Function to decompress data using LZ77 Decompression Algorithm.
    @param Token* compressed : Pointer for Compressed Token Array.
    @return string - decompressed data
*/
string decompress(Token* compressed)
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

int main()
{
    string text = "The computerphile channel handles computer topics.";
    cout << "Sizeof Char :: " << sizeof(char) << " :: Sizeof Int :: " << sizeof(uint8_t) << endl;
    cout << "Original Text to be compressed :: " << text << " :: Size :: " << (sizeof(char) * text.length()) << endl;
    Token* compressed_data = compress(text);

    cout << "Compressed Data :: ";
    int compressed_length = 0;
    for (int i = 0; i < counter; i++)
    {
        cout << compressed_data[i].next_char;
        compressed_length++;
    }

    cout << " :: Size : " << (counter * (sizeof(uint8_t) * 2 + sizeof(char))) << endl;

    string decompressed_data = decompress(compressed_data);
    cout << "Decompressed Data :: " << decompressed_data << " :: Size : " << (sizeof(char) * decompressed_data.length()) << endl;

    // Memory leaks prevention
    delete[] compressed_data;
    return 0;
}