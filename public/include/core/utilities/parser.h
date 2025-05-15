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

enum ParserType
{
    None     = 0,
    String      ,
    Integer     ,
    Float       ,
    EndOfLine   ,
  //EndOfFile   ,          
};

class Parser
{
    protected:
    const uint8* buffer; // Pointer to text that need to be parsed
    uint64 offset;       // Current offset in text
    const uint64 size;   // Text size
    
    ParserType type;     // Last found type

    uint64 foundStringOffset;
    sint32 foundStringLength;

    uint64 numberOffset;
    sint32 numberLength;

    public:
    // Passes ownership of buffer to parser
    Parser(const uint8* buffer, const uint64 size); 
   ~Parser();

    // Progresses in parsed buffer to next parsable element
    // (it does not follow any specific format rules, as 
    // opposed to well defined formats like JSON).
    virtual ParserType findNextElement(void);

    // Returns type of currently detected element
    ParserType currentElement(void) const;

    // If characters sequence at specified offset location 
    // is representing float value, returns length of that
    // string representation.
    bool isFloat(const uint64 startOffset, sint32& length);

    // If characters sequence at specified offset location 
    // is representing integer value, returns length of that
    // string representation. This check should be done
    // after isFloat one.
    bool isInteger(const uint64 startOffset, sint32& length);

    // Returns true if there is characters sequence at
    // specified offset location.
    bool isString(const uint64 startOffset, sint32& length);

    // If isInteger() returned true, below methods convert
    // string representation into integer value. False is
    // returned if value is negative but U64 is expected.
    // False is also returned if parsed value doesn't fit
    // into S64 range.
    bool readU64(uint64& value);
    bool readS64(sint64& value);

    bool readF32(float& value);
    bool readF64(double& value);

    bool isStringMatching(const char* string);

    const char* string(void);
    uint32 stringLength(void);



    bool position(uint64 offset);            // Sets current position in buffer
    bool read(std::string& word, bool& eol); // Reads single word, character sequence ended with whitespace. Sets EOL to true if reached EOL.
    bool readLine(std::string& line);        // Reads whole line
    bool skipToNextLine(void);               // Updates offset to position on the beginning of next line
    bool end(void);                          // Returns true if offset reached end of buffer
};

bool isCypher(uint8 input);
bool isUpperCaseLetter(uint8 input);
bool isLowerCaseLetter(uint8 input);
bool isLetter(uint8 input);
bool isCharacter(uint8 input);
bool isWhitespace(uint8 input);
bool isEol(uint8 input);
// Length is expected length of provided integer string representation
bool isInteger(const char* text, const uint32 length);
// Length is expected length of provided float string representation
bool isFloat(const char* text, const uint32 length);

} // en

#endif