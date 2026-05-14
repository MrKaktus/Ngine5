/*

 Ngine v5.0

 Module      : JSON parser.
 Requirements: none
 Description : Set of helper functions for parsing JSON files.

*/

#include "core/config/config.h"
#include "core/utilities/parserJSON.h"

#include <cassert>
#include <stdlib.h> // atoi(), atof(), strtoul()
#include <charconv> // std::from_chars()

namespace en
{

static bool isCommaOrTerminator(const uint8 byte)
{
    if (byte == ',' ||
        byte == ']' ||
        byte == '}')
    {
        return true;
    }

    return false;
}

bool isWhitespaceJSON(const uint8 input)
{
    if (input == ' '  || // Space
        input == '\t' || // Horizontal tab
        input == '\r' || // Carriage return
        input == '\n')   // Line feed
    {
        return true;
    }

    return false;
}

// JSON string is always wrapped into "", and can contain unicode representation in form \uHHHH where H is cypher
ParsingResult stringLengthJSON(const uint8* buffer, const uint64 size, uint64& length)
{
    length = 0;

    while (length < size)
    {
        uint8 byte = buffer[length++];

        // Enter quotation mark section (unicode character)
        if (byte == '\\')
        {
            if (length >= size)
            {
                return ParsingResult::IncompleteData;
            }

            byte = buffer[length++];
            if (byte == 'u')
            {
                // Skip next 4 hex chars
                for (uint8 i = 0; i < 4; ++i)
                {
                    if (length >= size)
                    {
                        return ParsingResult::IncompleteData;
                    }

                    if (!isHexCypher(buffer[length++]))
                    {
                        // Parsed string is invalid!
                        length--;
                        return ParsingResult::InvalidFormat;
                    }
                }
            }
        }
        else
        {
            if (byte == '"')
            {
                break;
            }
        }
    }

    // Returned length is expected to describe length of string
    // between two quotation signs "". Thus it is decreased by
    // one when closing qoute is being detected.
    length--;
    return ParsingResult::Success;
}

ParserJSON::ParserJSON(const uint8* _buffer, const uint64 _size, const bool takeOwnership) :
    ParserState(_buffer, _size, takeOwnership),
    type(JSONType::None),
    parseDepth(0)
{
}

ParserJSON::~ParserJSON()
{
}

// Integer notation:
// 
// [-]cccc
//
// - negative sign is optional
// - integer part is required
//
// Minimum case: c
//
// Floating point notation:
// 
// [-][cccc].cccc[e/E][-/+][cccc]
//
// - negative sign is optional
// - integer part or fractional part is optional
// - dot is required
// - e notation is optional
//   - exponent sign is optional
//
// Minimum case: .x
//
// _type is only set when its properly determined (on success).
// offset is placed at next character after detected number.
ParsingResult ParserJSON::detectNumberType(JSONType& _type)
{
    // Skip optional negative sign
    bool isSigned = false;
    if (buffer[offset] == '-')
    {
        isSigned = true;

        ++offset;
        if (offset >= size)
        {
            foundNumberLength = 0;
            return ParsingResult::IncompleteData;
        }
    }

    // Integer part detection
    bool integerPart = false;
    if (isCypher(buffer[offset]))
    {
        integerPart = true;

        // 1..9 can be followed by more digits
        // 0 is expected to be immediately followed by . or terminator.
        if (buffer[offset++] != '0')
        {
            // It may be integer or float
            for (; offset<size; ++offset)
            {
                if (isCypher(buffer[offset]))
                {
                    continue;
                }

                if (buffer[offset] == '.')
                {
                    // Its float at this point
                    break;
                }

                // JSON allowed characters following numerical value
                if (isWhitespaceJSON(buffer[offset]) ||
                    isCommaOrTerminator(buffer[offset]))
                {
                    foundNumberLength = offset - foundNumberOffset;

                    _type = JSONType::UnsignedInteger;
                    if (isSigned)
                    {
                        _type = JSONType::Integer;
                    }

                    return ParsingResult::Success;
                }

                foundNumberLength = 0;
                return ParsingResult::InvalidFormat;
            }

            // All decoded till now are valid values:
            // "-1..9" and "1..9" 
            // [-]cccc
            // [-]cccc.
            // but we follow JSON rules, and we're reached
            // end of buffer before encountering end of JSON
            // thus we return IncompleteData. We don't know
            // if its partial value representation either.
            if (offset >= size)
            {
                foundNumberLength = 0;
                return ParsingResult::IncompleteData;
            }
        }
    }

    // At this point its either:
    //    [-]0
    // [-]cccc.[cccc][e/E][-/+][cccc]
    //    [-]0.[cccc][e/E][-/+][cccc]
    //     [-].cccc[e/E][-/+][cccc]
    //        ^
    //        |
    //        offset
    //
    if (integerPart &&
        (isWhitespaceJSON(buffer[offset]) || isCommaOrTerminator(buffer[offset])))
    {
         // [-]0
         foundNumberLength = offset - foundNumberOffset;

         _type = JSONType::UnsignedInteger;
         if (isSigned)
         {
             _type = JSONType::Integer;
         }

         return ParsingResult::Success;
    }
    else
    if (buffer[offset] != '.')
    {
        // Required dot was not found
        foundNumberLength = 0;
        return ParsingResult::InvalidFormat;
    }

    // Move past dot
    ++offset;

    // Parses fractional part
    bool fractionalPart = false;
    for(; offset<size; ++offset)
    {
        if (isCypher(buffer[offset]))
        {
            fractionalPart = true;
            continue;
        }

        if (buffer[offset] == 'e' ||
            buffer[offset] == 'E')
        {
            break;
        }

        // JSON allowed characters following numerical value
        if (isWhitespaceJSON(buffer[offset]) ||
            isCommaOrTerminator(buffer[offset]))
        {
            // Fractional part is mandatory if there is no integer part
            if (!integerPart && !fractionalPart)
            {
                foundNumberLength = 0;
                return ParsingResult::InvalidFormat;
            }

            // [-]cccc.[cccc]
            //    [-]0.[cccc]
            //     [-].cccc
            foundNumberLength = offset - foundNumberOffset;
            _type = JSONType::Float;
            return ParsingResult::Success;
        }

        foundNumberLength = 0;
        return ParsingResult::InvalidFormat;
    }

    if (offset >= size)
    {
        foundNumberLength = 0;
        return ParsingResult::IncompleteData;
    }

    // Fractional part is mandatory if there is no integer part
    if (!integerPart && !fractionalPart)
    {
        foundNumberLength = 0;
        return ParsingResult::InvalidFormat;
    }

    // At this point its either:
    // [-]cccc.[cccc][e/E][-/+][cccc][f/F]
    //    [-]0.[cccc][e/E][-/+][cccc][f/F]
    //       [-].cccc[e/E][-/+][cccc][f/F]
    //                ^
    //                |
    //                offset
    //
    // [e/E] needed to be detected to reach this place
    // so this is just sanity check.
    if (buffer[offset] != 'e' &&
        buffer[offset] != 'E') // unlikely
    {
        foundNumberLength = 0;
        return ParsingResult::InvalidFormat;
    }

    // Skip required E
    ++offset;
    if (offset >= size)
    {
        foundNumberLength = 0;
        return ParsingResult::IncompleteData;
    }

    // Skip optional sign
    if (buffer[offset] == '-' ||
        buffer[offset] == '+')
    {
        ++offset;
        if (offset >= size)
        {
            foundNumberLength = 0;
            return ParsingResult::IncompleteData;
        }
    }

    // Required exponent value
    for(; offset<size; ++offset)
    {
        if (isCypher(buffer[offset]))
        {
            continue;
        }

        // JSON allowed characters following numerical value
        if (isWhitespaceJSON(buffer[offset]) ||
            isCommaOrTerminator(buffer[offset]))
        {
            foundNumberLength = offset - foundNumberOffset;
            _type = JSONType::Float;
            return ParsingResult::Success;
        }

        foundNumberLength = 0;
        return ParsingResult::InvalidFormat;
    }

    if (offset >= size)
    {
        foundNumberLength = 0;
        return ParsingResult::IncompleteData;
    }

    // Should never reach this place
    foundNumberLength = 0;
    return ParsingResult::InvalidFormat;
}

ParsingResult ParserJSON::findNextElement(JSONType& _type)
{
    while(offset < size)
    {
        uint8 byte = buffer[offset++];

        // Between separators, there should be only whitespaces
        if (isWhitespaceJSON(byte))
        {
            continue;
        }

        // By default assume it will fail to parse
        type = JSONType::None;

        if (byte == '{')
        {
            parseDepth++;
            type = _type = JSONType::Object;
            return ParsingResult::Success;
        }
        else
        if (byte == '"')
        {
            foundStringOffset = offset;
            foundStringLength = -1;
            uint64 temp = 0;
            ParsingResult result = stringLengthJSON(buffer + offset, size - offset, temp);
            if (result == ParsingResult::Success)
            {
                foundStringLength = (sint32)temp;
                type = _type = JSONType::String;
            }

            return result;          
        }
        else
        if (byte == ':')
        {
            type = _type = JSONType::Separator;
            return ParsingResult::Success;
        }
        else
        if (byte == '[')
        {
            parseDepth++;
            type = _type = JSONType::Array;
            return ParsingResult::Success;
        }
        else
        if (byte == '-' || isCypher(byte))
        {
            // Restores offset to point at first character of number representation
            offset--;
            foundNumberOffset = offset;
 
            JSONType temp = JSONType::None;
            ParsingResult result = detectNumberType(temp);
            if (result == ParsingResult::Success)
            {
                type = _type = temp;
            }

            return result;
        }
        else
        if (byte == ']')
        {
            parseDepth--;
            type = _type = JSONType::ArrayTerminator;
            return ParsingResult::Success;
        }
        else
        if (byte == ',')
        {
            type = _type = JSONType::Comma;
            return ParsingResult::Success;
        }
        else
        if (byte == '}')
        {
            parseDepth--;
            type = _type = JSONType::ObjectTerminator;
            return ParsingResult::Success;
        }
        else
        if (byte == 't')
        {
            // Expect "true" keyword:

            if (offset >= size)
            {
                return ParsingResult::IncompleteData;
            }
            if (buffer[offset++] != 'r')
            {
                return ParsingResult::InvalidFormat;
            }

            if (offset >= size)
            {
                return ParsingResult::IncompleteData;
            }
            if (buffer[offset++] != 'u')
            {
                return ParsingResult::InvalidFormat;
            }

            if (offset >= size)
            {
                return ParsingResult::IncompleteData;
            }
            if (buffer[offset++] != 'e')
            {
                return ParsingResult::InvalidFormat;
            }

            type = _type = JSONType::True;
            return ParsingResult::Success;
        }
        else
        if (byte == 'f')
        {
            // Expect "false" keyword:

            if (offset >= size)
            {
                return ParsingResult::IncompleteData;
            }
            if (buffer[offset++] != 'a')
            {
                return ParsingResult::InvalidFormat;
            }

            if (offset >= size)
            {
                return ParsingResult::IncompleteData;
            }
            if (buffer[offset++] != 'l')
            {
                return ParsingResult::InvalidFormat;
            }

            if (offset >= size)
            {
                return ParsingResult::IncompleteData;
            }
            if (buffer[offset++] != 's')
            {
                return ParsingResult::InvalidFormat;
            }

            if (offset >= size)
            {
                return ParsingResult::IncompleteData;
            }
            if (buffer[offset++] != 'e')
            {
                return ParsingResult::InvalidFormat;
            }

            type = _type = JSONType::False;
            return ParsingResult::Success;
        }
        else
        if (byte == 'n')
        {
            // Expect "null" keyword:

            if (offset >= size)
            {
                return ParsingResult::IncompleteData;
            }
            if (buffer[offset++] != 'u')
            {
                return ParsingResult::InvalidFormat;
            }

            if (offset >= size)
            {
                return ParsingResult::IncompleteData;
            }
            if (buffer[offset++] != 'l')
            {
                return ParsingResult::InvalidFormat;
            }

            if (offset >= size)
            {
                return ParsingResult::IncompleteData;
            }
            if (buffer[offset++] != 'l')
            {
                return ParsingResult::InvalidFormat;
            }

            type = _type = JSONType::Null;
            return ParsingResult::Success;
        }

        return ParsingResult::InvalidFormat;
    }

    return ParsingResult::IncompleteData;
}

JSONType ParserJSON::currentElement(void) const
{
    return type;
}

bool ParserJSON::readU64(uint64& value) const
{
    // Last found type is not a unsigned integer number
    // (cannot read negative value to unsigned integer)
    if (foundNumberLength <= 0 ||
        type != JSONType::UnsignedInteger)
    {
        return false;
    }

    std::from_chars_result result = std::from_chars((const char*)&buffer[foundNumberOffset], 
                                                    (const char*)&buffer[foundNumberOffset + foundNumberLength], 
                                                    value, 10);

    if (result.ec == std::errc())
    {
        // success
        // result.ptr tells where parsing stopped
        return true;
    }

    return false;
}

bool ParserJSON::readS64(sint64& value) const
{
    // Last found type is not a integer number
    if (foundNumberLength <= 0 ||
        (type != JSONType::UnsignedInteger && type != JSONType::Integer) )
    {
        return false;
    }

    std::from_chars_result result = std::from_chars((const char*)&buffer[foundNumberOffset],
                                                    (const char*)&buffer[foundNumberOffset + foundNumberLength],
                                                    value, 10);

    if (result.ec == std::errc())
    {
        // success
        // result.ptr tells where parsing stopped
        return true;
    }

    return false;
}

bool ParserJSON::readFloat(float& value) const
{
    // Last found type is not a float nor integer number
    if (foundNumberLength <= 0 ||
        (type != JSONType::Float &&
         type != JSONType::UnsignedInteger && 
         type != JSONType::Integer))
    {
        return false;
    }

    std::from_chars_result result = std::from_chars((const char*)&buffer[foundNumberOffset],
                                                    (const char*)&buffer[foundNumberOffset + foundNumberLength],
                                                    value);

    if (result.ec == std::errc())
    {
        // success
        // result.ptr tells where parsing stopped
        return true;
    }

    return false;
}

bool ParserJSON::readDouble(double& value) const
{
    // Last found type is not a float nor integer number
    if (foundNumberLength <= 0 ||
        (type != JSONType::Float &&
         type != JSONType::UnsignedInteger && 
         type != JSONType::Integer))
    {
        return false;
    }

    std::from_chars_result result = std::from_chars((const char*)&buffer[foundNumberOffset],
                                                    (const char*)&buffer[foundNumberOffset + foundNumberLength],
                                                    value);

    if (result.ec == std::errc())
    {
        // success
        // result.ptr tells where parsing stopped
        return true;
    }

    return false;
}

}