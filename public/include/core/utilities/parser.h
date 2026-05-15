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

enum class ParsingResult : uint8
{
    Success        = 0,
    IncompleteData = 1, // Provided buffer was not big enough to finish parsing
    InvalidFormat  = 2, // Buffer contents are not following expected parsing rules
    Unsupported    = 3, // Result of parsing points at unsupported configuration
};

enum ParserType
{
    None     = 0,
    String      ,
    Integer     ,
    Float       ,
    EndOfLine   ,
  //EndOfFile   ,          
};

class ParserState
{
protected:
    const uint8* buffer; // Pointer to text that need to be parsed
    const uint64 size;   // Text size
    uint64 offset;       // Current offset in text
    bool ownsBuffer;     // True of parser owns passed in buffer and should release it when done

    // When String element is detected, it's length is calculated,
    // and offset is moved to first character after that string.
    // This allows easy operations like comparison on detected string.
    uint64 foundStringOffset;
    sint32 foundStringLength;

    // When Number is detected, its parsed in advance, calculating 
    // it's length in characters, and detecting it's type. Offset is 
    // moved to first character after that number. Then based on 
    // it's type, final value can be extracted (if needed).
    uint64 foundNumberOffset;
    uint64 foundNumberLength;

public:
    // Passes ownership of buffer to parser
    ParserState(const uint8* buffer, const uint64 size, const bool takeOwnership);
    virtual ~ParserState();
};

class Parser : public ParserState
{
protected:
    ParserType type;     // Last found type

    // If characters sequence at specified offset location 
    // is representing float value, returns length of that
    // string representation.
    bool isFloat(const uint64 startOffset, uint64& length);

    // If characters sequence at specified offset location 
    // is representing integer value, returns length of that
    // string representation. This check should be done
    // after isFloat one.
    bool isInteger(const uint64 startOffset, uint64& length);

    // Returns true if there is characters sequence at
    // specified offset location.
    bool isString(const uint64 startOffset, sint32& length);

public:
    // Passes ownership of buffer to parser
    Parser(const uint8* buffer, const uint64 size, const bool takeOwnership = true); 
   ~Parser();

    // Progresses in parsed buffer to next parsable element
    // (it does not follow any specific format rules, as 
    // opposed to well defined formats like JSON).
    virtual ParserType findNextElement(void);

    // Returns type of currently detected element
    ParserType currentElement(void) const;

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

// [0..9]
bool isCypher(const uint8 input);

// [0..9][A..F][a..f]
bool isHexCypher(const uint8 input);

// [A..Z]
bool isUpperCaseLetter(const uint8 input);

// [a..z]
bool isLowerCaseLetter(const uint8 input);

// [A..Z][a..z]
bool isLetter(const uint8 input);

// All printable characters [!..~]
bool isCharacter(const uint8 input);

// Space & horizontal tabulator
bool isWhitespace(const uint8 input);

// Carriage return (CR), line feed (LF), vertical tab, form feed
bool isEol(const uint8 input);

// TODO: Below should all return ParsingResult if they parse provided buffer

// Length is expected length of provided integer string representation
bool isInteger(const char* text, const uint32 length);
// Length is expected length of provided float string representation
bool isFloat(const char* text, const uint32 length);

// Converts hex cypher [0..9][A..F][a..f] into corresponding value
bool convertHex(const char character, uint8& value);

// Returns length of detected string with terminating zero.
// For empty string will return Success and length of 1.
ParsingResult parseString(const uint8* buffer, const uint64 size, uint64& length);

// Returns success if buffer content is byte sequence ending with EOL (LF or CRLF).
// In such case returned length is length of line without EOL bytes, while offset
// points to what would be next byte after EOL signature (so line length plus size 
// of EOL bytes).
ParsingResult isLine(const uint8* buffer, const uint64 size, uint64& length, uint64& offset);

// Returns success if buffer content is string followed by EOL (LF or CRLF) 
// and its matching input string (which is null terminated but that null is 
// not taken into notice during comparison). In such case returned length is 
// offset to what would be next byte after EOL signature (so string length 
// plus size of EOL bytes).
ParsingResult isLineMatching(const uint8* buffer, const uint64 size, const char* string, uint64& length);

bool nextWord(const std::string_view& line, uint64& lineOffset, std::string_view& word);

} // en

#endif