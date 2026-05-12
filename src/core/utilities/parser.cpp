/*

 Ngine v4.0
 
 Module      : Text parser.
 Requirements: none
 Description : Set of helper functions for text parsing.
               It contains basic functions required to
               parse ASCII based file format or script. 

*/

#include "core/config/config.h"
#include "core/utilities/parser.h"

#include <cassert>
#include <stdlib.h> // atoi(), atof(), strtoul()

namespace en
{
ParserState::ParserState(const uint8* _buffer, const uint64 _size) :
    buffer(_buffer),
    offset(0),
    size(_size),
    foundStringOffset(0),
    foundStringLength(-1),
    foundNumberOffset(0),
    foundNumberLength(-1)
{
    assert(buffer);
    assert(size);
}

ParserState::~ParserState()
{
    delete [] buffer;
}

Parser::Parser(const uint8* _buffer, const uint64 _size) :
    ParserState(_buffer, _size),
    type(ParserType::None)
{
}

// TODO: Deprecate
bool Parser::position(uint64 newOffset)
{
    // Check if offset is in range
    if (newOffset >= size)
    {
        return false;
    }

    offset = newOffset;
    return true;
}

ParserType Parser::findNextElement(void)
{
    uint8 byte = 0;
    while(offset < size)
    {
        byte = buffer[offset++];

        // Skips whitespaces until parseable element is found
        if (isWhitespace(byte))
        {
            continue;
        }

        if (isCharacter(byte))
        {
            // Restores offset to point at first character of string representation
            offset--;

            if (isFloat(offset, foundNumberLength))
            {                
                foundNumberOffset = offset;
                offset += foundNumberLength;
                type = ParserType::Float;
                return type;
            }
            else
            if (isInteger(offset, foundNumberLength))
            {
                foundNumberOffset = offset;
                offset += foundNumberLength;
                type = ParserType::Integer;
                return type;
            }
            else // Some kind of string
            {
                if (!isString(offset, foundStringLength))
                {
                    assert( 0 );
                }

                foundStringOffset = offset;
                type = ParserType::String;
                offset += foundStringLength;
                return type;
            }
        }
        else
        if (isEol(byte))
        {
            // EOL is not implicitly skipped, but explicitly reported back
            // to allow application parsing logic taking it into notice.
            type = ParserType::EndOfLine;
            return type;
        }

        // How did we got here?
        assert(0);
    }

    return ParserType::None;
}

ParserType Parser::currentElement(void) const
{
    return type;
}

bool Parser::isFloat(const uint64 startOffset, uint64& length)
{
    // Floating point notation:
    // 
    // [-/+][cccc].cccc[e/E][-][cccc][f/F]
    //
    // - negative sign is optional
    // - integer part or fractional part is optional
    // - dot is required
    // - e notation is optional
    // - f/d postfix is optional
    // - value may be immediately followed by other string (like arithmetic opoeration sign, etc.)
    //
    // Minimum case: .x
    if (startOffset + 2 > size)
    {
        return false;
    }

    uint64 currentOffset = startOffset;

    // Verify optional sign
    bool sign = false;
    if (buffer[currentOffset] == '-' ||
        buffer[currentOffset] == '+')
    {
        sign = true;
        ++currentOffset;
    }

    // Verify optional integer part
    bool integerPart = false;
    if (isCypher(buffer[currentOffset]))
    {
        integerPart = true;
        for(++currentOffset; currentOffset<size; ++currentOffset)
        {
            if (isCypher(buffer[currentOffset]))
            {
                continue;
            }

            if (buffer[currentOffset] == '.')
            {
                break;
            }
            else
            {
                return false;
            }
        }

        // Case parsed:
        // [-/+]cccc[EOF]
        if (currentOffset == size)
        {
            return false;
        }
    }

    // At this point possible cases were parsed:
    // [-/+][cccc.]
    // empty

    // Parser should point now at required dot
    if (buffer[currentOffset] != '.')
    {
        return false;
    }

    // Skip required dot
    ++currentOffset;

    if (currentOffset == size)
    {
        // '-.' / '+.' strings are not a number
        if (sign && !integerPart)
        {
            return false;
        }
    }

    // If there is no integer part, fractional part is required
    if (!integerPart &&
        !isCypher(buffer[currentOffset]))
    {
        return false;
    }

    // Skip fractional part
    for (; currentOffset<size; ++currentOffset)
    {
        if (!isCypher(buffer[currentOffset]))
        {
            break;
        }
    }

    // At this point it is known its string representation of float value.
    // Its full length still remains to be determined.

    if (currentOffset == size)
    {
        length = currentOffset - startOffset;
        return true;
    }

    // Verify optional e/E notation
    if (buffer[currentOffset] == 'e' ||
        buffer[currentOffset] == 'E')
    {
        ++currentOffset;

        if (currentOffset == size)
        {
            // 'E' should be followed by cypher
            return false;
        }

        // Verify optional negative sign
        bool sign2 = false;
        if (buffer[currentOffset] == '-')
        {
            sign2 = true;
            ++currentOffset;

            if (currentOffset == size)
            {
                // 'E-' should be followed by cypher
                return false;
            }
        }

        // Iterate over cyphers of E notation
        bool Ecyphers = false;
        for(; currentOffset<size; ++currentOffset)
        {
            if (isCypher(buffer[currentOffset]))
            {
                Ecyphers = true;
            }
            else
            {
                break;
            }
        }

        if (!Ecyphers)
        {
            // E[-] should be followed by cypher
            return false;
        }

        if (currentOffset == size)
        {
            length = currentOffset - startOffset;
            return true;
        }
    }

    if (buffer[currentOffset] == 'f' ||
        buffer[currentOffset] == 'F')
    {
        ++currentOffset;
    }

    length = currentOffset - startOffset;
    return true;
}

bool Parser::isInteger(const uint64 startOffset, uint64& length)
{
    // Integer notation:
    // 
    // [-/+][cccc]
    //
    // - negative sign is optional
    // - integer part is required
    //
    // Minimum case: c
    if (startOffset >= size)
    {
        return false;
    }

    uint64 currentOffset = startOffset;

    // Verify optional sign
    if (buffer[currentOffset] == '-' ||
        buffer[currentOffset] == '+')
    {
        ++currentOffset;
    }

    // '-' string is not a number
    if (currentOffset == size)
    {
        return false;
    }

    // Verify integer part
    bool integerPart = false;
    for(; currentOffset<size; ++currentOffset)
    {
        if (isCypher(buffer[currentOffset]))
        {
            integerPart = true;
        }
        else
        {
            break;
        }
    }

    if (integerPart)
    {
        length = currentOffset - startOffset;
        return true;
    }

    return false;
}

bool Parser::isString(const uint64 startOffset, sint32& length)
{
    // Minimum case: c
    if (startOffset >= size)
    {
        return false;
    }

    // String needs to start at startOffset 
    // (there can be no whitespaces, EOL, EOF, etc.)
    if (!isCharacter(buffer[startOffset]))
    {
        return false;
    }

    uint64 currentOffset = startOffset;

    for(; currentOffset<size; ++currentOffset)
    {
        if (!isCharacter(buffer[currentOffset]))
        {
            break;
        }
    }

    length = sint32(currentOffset - startOffset);
    return true;
}

bool Parser::readU64(uint64& value)
{
    // Last found type is not a integer number
    if (foundNumberLength == 0 ||
        type != ParserType::Integer)
    {
        return false;
    }

    // Cannot read negative value to unsigned integer
    if (buffer[foundNumberOffset] == '-')
    {
        return false;
    }

    char* ending = nullptr;
    value = strtoul((const char*)&buffer[foundNumberOffset], &ending, 10);
    return true;
}

bool Parser::readS64(sint64& value)
{
    // Last found type is not a integer number
    if (foundNumberLength == 0 ||
        type != ParserType::Integer)
    {
        return false;
    }

    char* ending = nullptr;
    value = strtol((const char*)&buffer[foundNumberOffset], &ending, 10);
    return true;
}

bool Parser::readF32(float& value)
{
    // Last found type is not floating point number
    if (foundNumberLength == 0 ||
        type != ParserType::Float)
    {
        return false;
    }

    char* ending = nullptr;
    value = strtof((const char*)&buffer[foundNumberOffset], &ending);
    return true;
}

bool Parser::readF64(double& value)
{
    // Last found type is not floating point number
    if (foundNumberLength == 0 ||
        type != ParserType::Float)
    {
        return false;
    }

    char* ending = nullptr;
    value = strtod((const char*)&buffer[foundNumberOffset], &ending);
    return true;
}

const char* Parser::string(void)
{
    if (foundStringOffset < 0)
    {
        return nullptr;
    }

    return (const char*)&buffer[foundStringOffset];
}

uint32 Parser::stringLength(void)
{
    if (type != ParserType::String)
    {
        return 0;
    }

    return foundStringLength;
}


bool Parser::isStringMatching(const char* string)
{
    // Last found type is not string
    if (foundStringLength <= 0 ||
        type != ParserType::String)
    {
        return false;
    }

    // Verifies that both strings have the same length
    const size_t inLength = strlen(string);
    if (foundStringLength != inLength)
    {
        return false;
    }

    if (strncmp((const char *)&buffer[foundStringOffset], string, foundStringLength) == 0)
    {
        return true;
    }

    return false;
}




bool Parser::read(std::string& word, bool& eol)
{
    // Check if offset is in range
    if (offset >= size)
    {
        return false;
    }

    // Clean passed parameters
    word.erase();
    eol = false;

    // Try to extract first word from given location
    uint8 byte = 0;
    bool found = false;
    for(;;)
    {
        byte = buffer[offset++];

        if (isCharacter(byte))
        {
            word.push_back(byte);
            found = true;
        }
        else
        if (isEol(byte))
        {
            eol = true;
            break;
        }
        else
        if (found)
        {
            // If current byte is neither character nor EOL, but word 
            // parsing already started, this indicates end of word.
            break;
        }

        if (offset >= size)
        {
            break;
        }
    }

    return found;
}

bool Parser::readLine(std::string& line)
{
    // Check if offset is in range
    if (offset >= size)
    {
        return false;
    }

    // Clean passed parameters
    line.erase();

    // Try to extract first line from given location
    uint8 byte = 0;
    bool found = false;
    for(;;)
    {
        byte = buffer[offset++];
        if (isEol(byte))
        {
            break;
        }

        if (isCharacter(byte))
        {
            found = true;
        }

        line.push_back(byte);
        if (offset >= size)
        {
            break;
        }
    }

    return found;
}

bool Parser::skipToNextLine(void)
{
    // Check if offset is in range
    if (offset >= size)
    {
        return false;
    }

    bool found = false;
    uint8 byte = 0;
    for(;;)
    {
        byte = buffer[offset];

        if (!found)
        {
            if (isEol(byte))
            {
                // Marks moment when first byte in EOL sequence is found.
                found = true;
            }
        }
        else // Multiple EOL sequences following each other will be skipped at once.
        {
            if (!isEol(byte))
            {
                // During parsing of EOL bytes sequence, first not-EOL byte
                // means that offset was moved to beginning of next line.
                break;
            }
        }

        offset++;
    }

    return true;
}

bool Parser::end(void)
{
    return (offset >= size);
}

bool isCypher(const uint8 input)
{
    return ((input > 47) && (input < 58));
}

bool isHexCypher(const uint8 input)
{
    return (((input > 47) && (input < 58)) ||  // [0,9]
            ((input > 64) && (input < 71)) ||  // [A,F]
            ((input > 96) && (input < 103)));  // [a,f]
}

bool isUpperCaseLetter(const uint8 input)
{
    return ((input > 64) && (input < 91));
}

bool isLowerCaseLetter(const uint8 input)
{
    return ((input > 96) && (input < 123));
}

bool isLetter(const uint8 input)
{
    return ((input > 64) && (input < 91)) ||
           ((input > 96) && (input < 123));
}

bool isCharacter(const uint8 input)
{
    return ((input > 32) && (input < 127));
}

bool isWhitespace(const uint8 input)
{
    if ( input == ' '  || 
         input == '\t' ) 
    {
        return true;
    }

    return false;
}

bool isEol(const uint8 input)
{
    if ( input == '\r' || // Carriage return
         input == '\n' || // Line feed
         input == '\v' || // Vertical Tab
         input == '\f' )  // Form Feed
    {
        return true;
    }

    return false;
}

bool isInteger(const char* text, const uint32 length)
{
    // Integer notation:
    // 
    // [-][cccc]
    //
    // - negative sign is optional
    // - integer part is required
    //
    // Minimum case: c
    assert( length > 0 );

    uint32 offset = 0;

    // Verify optional negative sign
    if (text[0] == '-')
    {
        ++offset;
    }

    // Empty string or '-' string is not a number
    if (offset == length)
    {
        return false;
    }

    // Verify integer part
    for(; offset<length; ++offset)
    {
        if (!isCypher(text[offset]))
        {
            break;
        }
    }

    if (offset == length)
    {
        return true;
    }

    return false;
}

bool isFloat(const char* text, const uint32 length)
{
    // Floating point notation:
    // 
    // [-][cccc].cccc[e/E][cccc][f/F]
    //
    // - negative sign is optional
    // - integer part or fractional part is optional
    // - dot is required
    // - e notation is optional
    // - f/d postfix is optional
    //
    // Minimum case: .x
    if (length < 2)
    {
        return false;
    }

    uint32 offset = 0;

    // Verify optional negative sign
    if (text[0] == '-')
    {
        ++offset;
    }

    // Verify optional integer part
    bool integerPart = false;
    if (isCypher(text[offset]))
    {
        integerPart = true;
        for(++offset; offset<length; ++offset)
        {
            if (isCypher(text[offset]))
            {
                continue;  
            }

            if (text[offset] == '.')
            {
                break;
            }
            else
            {
                return false;
            }
        }
    }

    // TODO: What about [-][cccc]. case?
    // TODO: Which case does it cover?
    if (offset == length)
    {
        return false;
    }

    // Skip required dot
    if (text[offset] == '.')
    {
        ++offset;

/* TODO: Missing check?
        // '-.' string is not a number
        if (!integerPart && offset == length)
        {
            return false;
        }
//*/
    }

    // If there is no integer part, fractional part is required
    if (!integerPart &&
        !isCypher(text[offset]))
    {
        return false;
    }

    // Skip fractional part
    for(; offset<length; ++offset)
    {
        if (!isCypher(text[offset]))
        {
            break;
        }
    }

    if (offset == length)
    {
        return true;
    }

    // Verify optional e/E notation
    if (text[offset] == 'e' ||
        text[offset] == 'E')
    {
        for(++offset; offset<length; ++offset)
        {
            if (!isCypher(text[offset]))
            {
                break;
            }
        }
    }

    if (offset == length)
    {
        return true;
    }

    if (text[offset] == 'f' ||
        text[offset] == 'F' )
    {
        if (offset == length - 1)
        {
            return true;
        }
    }

    return false;
}

ParsingResult parseString(const uint8* buffer, const uint64 size, uint64& length)
{
    if (!buffer || size == 0)
    {
        return ParsingResult::IncompleteData;
    }

    uint64 offset = 0;

    // Check for empty string
    if (buffer[offset] == 0)
    {
        length = 1;
        ParsingResult::Success;
    }

    // String needs to start at startOffset 
    // (there can be no whitespaces, EOL, EOF, etc.)
    if (!isCharacter(buffer[offset]))
    {
        return ParsingResult::InvalidFormat;
    }

    for (; offset < size; ++offset)
    {
        if (!isCharacter(buffer[offset]))
        {
            if (buffer[offset] != 0)
            {
                // String should be null terminated
                return ParsingResult::InvalidFormat;
            }

            // Detected end of null terminated string.
            // Returns string length including terminating zero.
            length = offset + 1;
            return ParsingResult::Success;
        }
    }

    return ParsingResult::IncompleteData;
}

// Returns success if buffer content is byte sequence ending with EOL (LF or CRLF).
// In such case returned length is length of line without EOL bytes, while offset
// points to what would be next byte after EOL signature (so line length plus size 
// of EOL bytes).
ParsingResult isLine(const uint8* buffer, const uint64 size, uint64& length, uint64& offset)
{
    if (!buffer || size == 0)
    {
        return ParsingResult::IncompleteData;
    }

    // Finds length of the line
    uint32 i = 0;
    while(i < size && !isEol(buffer[i]))
    {
        i++;
    }

    // EOL terminator was not found
    if (i == size)
    {
        return ParsingResult::InvalidFormat;
    }

    length = i;

    // CRLF case
    if (buffer[i] == '\r') // [[ unlikely ]]
    {
        i++;
        if (i >= size) // [[ unlikely ]]
        {
            return ParsingResult::IncompleteData;
        }

        if (buffer[i] != '\n')
        {
            return ParsingResult::InvalidFormat;
        }
    }
    i++;
    
    offset = i;
    return ParsingResult::Success;
}

// Returns success if buffer content is string followed by EOL (LF or CRLF) 
// and its matching input string (which is null terminated but that null is 
// not taken into notice during comparison). In such case returned length is 
// offset to what would be next byte after EOL signature (so string length 
// plus size of EOL bytes).
ParsingResult isLineMatching(const uint8* buffer, const uint64 size, const char* string, uint64& length)
{
    if (!buffer || size == 0 || !string || strlen(string) > size)
    {
        return ParsingResult::IncompleteData;
    }

    // Compares buffer content to input string (ignoring terminating bytes for both)
    if (strncmp((const char*)buffer, string, strlen(string)) != 0)
    {
        return ParsingResult::InvalidFormat;
    }

    uint64 offset = strlen(string);
    if (offset == size)
    {
        ParsingResult::IncompleteData;
    }

    // String is followed by EOL (LF or CRLF)
    if (!isEol(buffer[offset]))
    {
        return ParsingResult::InvalidFormat;
    }
    // CRLF case
    if (buffer[offset] == '\r') // [[ unlikely ]]
    {
        offset++;
        if (offset >= size) // [[ unlikely ]]
        {
            return ParsingResult::IncompleteData;
        }

        if (buffer[offset] != '\n')
        {
            return ParsingResult::InvalidFormat;
        }
    }
    offset++;

    length = offset;
    return ParsingResult::Success;
}

bool nextWord(const std::string_view& line, uint64& lineOffset, std::string_view& word)
{
    if (lineOffset > line.size())
    {
        return false;
    }

    size_t spaceOffset = line.find(' ', lineOffset);
    if (spaceOffset == std::string::npos)
    {
        // See if its not last word in line
        word = line.substr(lineOffset);
        if (!word.size())
        {
            return false;
        }

        return true;
    }

    word = line.substr(lineOffset, spaceOffset - lineOffset);
    lineOffset = spaceOffset + 1;
    return true;
}

} // en
