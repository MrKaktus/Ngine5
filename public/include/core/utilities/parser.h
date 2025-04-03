/*

 Ngine v5.0
 
 Module      : Text parser.
 Requirements: none
 Description : Set of helper functions for text parsing.
               It contains basic functions required to
               parse ASCII based file format or script. 

*/

#ifndef ENG_CORE_UTILITIES_PARSER
#define ENG_CORE_UTILITIES_PARSER

#include "core/defines.h"

namespace en
{

class Parser
{
    private:
    uint8* buffer; // Pointer to text that need to be parsed
    uint64 offset; // Current offset in text
    uint64 size;   // Text size
    
    public:
    Parser(uint8* buffer, uint64 size); 
   ~Parser();

    bool position(uint64 offset);            // Sets current position in buffer
    bool read(std::string& word, bool& eol); // Reads single word, character sequence ended with whitespace. Sets EOL to true if reached EOL.
    bool readLine(std::string& line);        // Reads whole line
    bool skipToNextLine(void);               // Updates offset to position on the beginning of next line
    bool end(void);                          // Returns true if offset reached end of buffer
};

bool isCypher(uint8 input);
bool isCharacter(uint8 input);
bool isWhitespace(uint8 input);
bool isEol(uint8 input);
// Length is expected length of provided integer string representation
bool isInteger(const char* text, const uint32 length);
// Length is expected length of provided float string representation
bool isFloat(const char* text, const uint32 length);

} // en
#endif