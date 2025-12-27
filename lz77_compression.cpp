# include <iostream>
# include <string>
# include <vector>
# include "lz77_compression.h"

using namespace std;

// To this (RFC 1951 standard): https://datatracker.ietf.org/doc/html/rfc1951#section-1
int search_buffer_size = 32768;  // 32KB = 2^15 // Size of the search buffer -- The substring already parsed.
int look_ahead_buffer_size = 258; //  Size of the look ahead buffer -- THe buffer ahead of the 
                                // current character to be searched for maximum possible match 

/*
  Function used to compress the input text using LZ77 Compression.
  @param input - string input
  @return vector<DeflateSymbol> - Vector of DeflateSymbols.
*/
vector<DeflateSymbol> lz77_compress(string input, bool debug)
{
    size_t input_length = input.length();
    vector<DeflateSymbol> output;
    size_t index = 0; // Current index variable while parsing the string

    string search_buffer;

    // We run a while loop from from 0 to input_length.
    while (index < input_length)
    {
        // This gives us the corrected start index. 
        // If <=0 then we start from 0, and move index positions, 
        // else start from corrected_index for a length of search_buffer_size
        size_t corrected_start_index = (index < (size_t)search_buffer_size) ? 0 : (index - search_buffer_size);
        size_t corrected_search_buffer_length = (corrected_start_index == 0) ? index : (size_t)search_buffer_size;
        // If index - search_buffer_size <= 0, that means, search_buffer will 
        // start from 0, and be of the index's size. Once it's more than 0, 
        // we start sliding it.
        search_buffer = input.substr(corrected_start_index, corrected_search_buffer_length); 

        // Now we find the longest possible match.
        size_t match_length = 0;
        /*
        Steps : 
            - We loop through the search buffer till input[index] matches.
            - Once we find match_index, we start incrementing match_length and comparing strings.
            - Once the match stops, we emit either a back-reference or a literal.
        */
        size_t best_offset = 0;
        size_t best_length = 0;

        // Looping through search buffer to find matches.
        for (size_t i = 0; i < corrected_search_buffer_length; i++)
        {
            match_length = 0;
            // Finding a match for char at index in search_buffer
            if(input[index] == search_buffer[i])
            {
                // We loop till input and search buffer keep matching from initial match location.
                while (
                    (index + match_length) < input.length() && // Out of bound check
                    (i + match_length) < corrected_search_buffer_length && // Out of bound check
                    match_length < (size_t)look_ahead_buffer_size && // Look Ahead Buffer Size Check
                    input[index + match_length] == search_buffer[i + match_length] // Equality check
                ) {
                    match_length += 1;
                }
                
                // Minimum match length is 3 - prevents small matches from inflating compressed size
                if (match_length >= 3 && best_length < match_length)
                {
                    best_length = match_length;
                    best_offset = index - (corrected_start_index + i);
                }
            }
        }
        
        if (best_length >= 3) {
            // Emit back-reference
            DeflateSymbol sym;
            sym.type = SymbolType::BACK_REFERENCE;
            sym.ref.length = best_length;
            sym.ref.distance = best_offset;
            output.push_back(sym);
            
            if (debug) {
                cout << "Index: " << index 
                     << " :: BACK_REF(len=" << best_length 
                     << ", dist=" << best_offset << ")"
                     << " :: Matched: \"" << input.substr(index, best_length) << "\"" << endl;
            }
            
            index += best_length;  // No +1, no trailing char!
        } else {
            // Emit literal
            DeflateSymbol sym;
            sym.type = SymbolType::LITERAL;
            sym.literal = input[index];
            output.push_back(sym);
            
            if (debug) {
                cout << "Index: " << index 
                     << " :: LITERAL('" << input[index] << "')" << endl;
            }
            
            index++;
        }
    }


    // End of Block// End of block marker
    DeflateSymbol end;
    end.type = SymbolType::END_OF_BLOCK;
    output.push_back(end);

    // Returning the Output Vector
    return output;
}

/*
    Function to decompress DEFLATE LZ77 symbols back to original string.
    @param symbols - Vector of DeflateSymbols
    @param debug - Enable debug output
    @return Decompressed string
*/
string lz77_decompress(const vector<DeflateSymbol>& symbols, bool debug) {
    string output;
    
    for (const auto& sym : symbols) {
        switch (sym.type) {
            case SymbolType::LITERAL:
                output += sym.literal;
                if (debug) cout << "Decompress: LITERAL('" << sym.literal << "')" << endl;
                break;
                
            case SymbolType::BACK_REFERENCE: {
                size_t start = output.length() - sym.ref.distance;
                if (debug) {
                    cout << "Decompress: BACK_REF(len=" << sym.ref.length 
                         << ", dist=" << sym.ref.distance << ") -> \"";
                }
                for (int i = 0; i < sym.ref.length; i++) {
                    char c = output[start + i];
                    output += c;
                    if (debug) cout << c;
                }
                if (debug) cout << "\"" << endl;
                break;
            }
            
            case SymbolType::END_OF_BLOCK:
                if (debug) cout << "Decompress: END_OF_BLOCK" << endl;
                return output;
        }
    }
    
    return output;
}

int main()
{
    string text = "The computerphile channel handles computer topics.";
    bool debug = true;
    
    cout << "Original Text :: \"" << text << "\"" << endl;
    cout << "Original Size :: " << text.length() << " bytes" << endl;
    cout << "----------------------------------------" << endl;
    
    // Compress
    vector<DeflateSymbol> compressed = lz77_compress(text, debug);
    
    cout << "----------------------------------------" << endl;
    cout << "Symbol count :: " << compressed.size() << endl;
    
    // Count literals vs back-references
    int literals = 0, refs = 0;
    for (const auto& sym : compressed) {
        if (sym.type == SymbolType::LITERAL) literals++;
        else if (sym.type == SymbolType::BACK_REFERENCE) refs++;
    }
    cout << "Literals: " << literals << ", Back-references: " << refs << endl;
    
    // Estimate compressed size (simplified: 1 byte per literal, 4 bytes per ref)
    size_t estimated_size = literals * 1 + refs * 4 + 1; // +1 for end marker
    cout << "Estimated compressed size :: " << estimated_size << " bytes" << endl;
    
    cout << "----------------------------------------" << endl;
    
    // Decompress and verify
    string decompressed = lz77_decompress(compressed, false);
    cout << "Decompressed Text :: \"" << decompressed << "\"" << endl;
    cout << "Decompressed Size :: " << decompressed.length() << " bytes" << endl;
    
    // Verify
    bool match = (text == decompressed);
    cout << "----------------------------------------" << endl;
    cout << "Verification :: " << (match ? "SUCCESS ✓" : "FAILED ✗") << endl;
    
    return 0;
}