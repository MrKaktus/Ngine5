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

namespace en
{

Parser::Parser(uint8* _buffer, uint64 _size) :
    buffer(_buffer),
    offset(0),
    size(_size)
{
    assert(buffer);
    assert(size);
}

Parser::~Parser()
{
    delete [] buffer;
}

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
