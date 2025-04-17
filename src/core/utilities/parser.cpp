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

Parser::Parser(const uint8* _buffer, const uint64 _size) :
    buffer(_buffer),
    offset(0),
    size(_size),
    type(ParserType::None),
    foundStringOffset(0),
    foundStringLength(-1),
    numberOffset(0),
    numberLength(-1)
{
    assert(buffer);
    assert(size);
}

Parser::~Parser()
{
    delete [] buffer;
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

            if (isFloat(offset, numberLength))
            {                
                numberOffset = offset;
                offset += numberLength;
                type = ParserType::Float;
                return type;
            }
            else
            if (isInteger(offset, numberLength))
            {
                numberOffset = offset;
                offset += numberLength;
                type = ParserType::Integer;
                return type;
            }
            else // Some kind of string
            {
                isString(offset, foundStringLength);

                foundStringOffset = offset;
                type = ParserType::String;
                offset += foundWordLength;
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

bool Parser::isFloat(const uint64 startOffset, sint32& length)
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
            if (isCypher(buffer[startOffset]))
            {
                continue;
            }

            if (buffer[startOffset] == '.')
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
        length = sint32(currentOffset - startOffset);
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
            length = sint32(currentOffset - startOffset);
            return true;
        }
    }

    if (buffer[currentOffset] == 'f' ||
        buffer[currentOffset] == 'F')
    {
        ++currentOffset;
    }

    length = sint32(currentOffset - startOffset);
    return true;
}

bool Parser::isInteger(const uint64 startOffset, sint32& length)
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
        length = sint32(currentOffset - startOffset);
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
    if (numberLength <= 0 ||
        type != ParserType::Integer)
    {
        return false;
    }

    // Cannot read negative value to unsigned integer
    if (buffer[numberOffset] == '-')
    {
        return false;
    }

    char* ending = nullptr;
    value = strtoul((const char*)&buffer[numberOffset], &ending, 10);
    return true;
}

bool Parser::readS64(sint64& value)
{
    // Last found type is not a integer number
    if (numberLength <= 0 ||
        type != ParserType::Integer)
    {
        return false;
    }

    char* ending = nullptr;
    value = strtol((const char*)&buffer[numberOffset], &ending, 10);
    return true;
}

bool Parser::readF32(float& value)
{
    // Last found type is not floating point number
    if (numberLength <= 0 ||
        type != ParserType::Float)
    {
        return false;
    }

    char* ending = nullptr;
    value = strtof((const char*)&buffer[numberOffset], &ending);
    return true;
}

bool Parser::readF64(double& value)
{
    // Last found type is not floating point number
    if (numberLength <= 0 ||
        type != ParserType::Float)
    {
        return false;
    }

    char* ending = nullptr;
    value = strtod((const char*)&buffer[numberOffset], &ending);
    return true;
}

const char* Parser::string(void)
{
    if (foundWordOffset < 0)
    {
        return nullptr;
    }

    return (const char*)&buffer[foundWordOffset];
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

bool isCypher(uint8 input)
{
    return ((input > 47) && (input < 58));
}

bool isUpperCaseLetter(uint8 input)
{
    return ((input > 64) && (input < 91));
}

bool isLowerCaseLetter(uint8 input)
{
    return ((input > 96) && (input < 123));
}

bool isLetter(uint8 input)
{
    return ((input > 64) && (input < 91)) ||
           ((input > 96) && (input < 123));
}

bool isCharacter(uint8 input)
{
    return ((input > 32) && (input < 127));
}

bool isWhitespace(uint8 input)
{
    if ( input == ' '  || 
         input == '\t' || 
         input == '\r' || 
         input == '\n' || 
         input == '\v' || 
         input == '\f' ) 
    {
        return true;
    }

    return false;
}

bool isEol(uint8 input)
{
    if ( input == '\r' || 
         input == '\n' || 
         input == '\v' || 
         input == '\f' )
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

} // en
