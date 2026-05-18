/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 128 bit unsigned integer.

*/

#include "core/log/log.h"
#include "core/types/uint128.h"
#include "core/utilities/parser.h"

namespace en
{

uint128::uint128(const void* src)
{
    if (src)
    {
        qword[0] = *(const uint64*)src;
        qword[1] = *((const uint64*)src + 1);
    }
}

uint128& uint128::operator=(const uint128& b)
{
    if (this != &b) 
    { 
        qword[0] = b.qword[0];
        qword[1] = b.qword[1];
    }

    return *this;
}

// Initializes uint128 from its text description in format:
// 0xXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Both lowercase and uppercase letters are allowed in any combination.
// Returns false if provided string is incorrectly formatted.
bool uint128::init(std::string& description)
{
    if (description.length() != 34)
    {
        logError("Invalid string length to generate uint128:\n%s\n", description.c_str());
        return false;
    }

    // Verifies string format
    // 0xXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    if (description[0] != '0' || description[1] != 'x')
    {
        logError("Invalid string format to generate uint128:\n%s\n", description.c_str());
        return false;
    }
    for (uint32 i = 2; i < 34; ++i)
    {
        if (!isHexCypher(description[i]))
        {
            logError("Invalid string format to generate uint128 (at %u):\n%s\n", i, description.c_str());
            return false;
        }
    }

    // Modifies uint128 only when whole string is known to be valid
    uint32 offset = 0;
    uint8 valueCharacter = 0;
    for (uint32 i = 2; i < 34; ++i)
    {
        uint8 value = 0;
        convertHex(description[i], value);

        // High nibble
        if (valueCharacter % 2 == 0)
        {
            *((uint8*)(&qword[0]) + offset) = value << 4;
        }
        else // Low nibble
        {
            *((uint8*)(&qword[0]) + offset) += value;
        }

        ++valueCharacter;
        if (valueCharacter % 2 == 0)
        {
            ++offset;
        }
    }

    return true;
}

std::string uint128::description(void)
{
    static constexpr char hex[] = "0123456789abcdef";

    std::string out;
    out.reserve(34);
    out.push_back('0');
    out.push_back('x');

    for (uint8 i = 0; i < 16; ++i)
    {
        uint8 byte = *(((uint8*)&qword[0]) + i);

        out.push_back(hex[byte >> 4]);
        out.push_back(hex[byte & 0x0F]);
    }

    return out;
}

} // en
