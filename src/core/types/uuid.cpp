/*

 Ngine v5.0

 Module      : Types.
 Requirements: none
 Description : Universal unique identifier.

*/

#include <random>

#include "core/log/log.h"
#include "core/types/uuid.h"
#include "core/utilities/parser.h"

namespace en
{

UUID::UUID() :
    uint128()
{
}

void UUID::init(void)
{
    // Mersenne Twister, pseudo random number generator (64bit version)
    // Random device is initial seed (different for each thread owning its own instance)
    static thread_local std::mt19937_64 randomNumberGenerator{ std::random_device{}() };

    qword[0] = randomNumberGenerator();
    qword[1] = randomNumberGenerator();

    // Set version (4) -> bits 12-15 of time_hi_and_version
    uint8& byte6 = *(((uint8*)&qword[0]) + 6);
    byte6 = (byte6 & 0x0F) | 0x40;

    // Set variant (10xx) -> bits 6-7 of clock_seq_hi_and_reserved
    uint8& byte8 = *((uint8*)&qword[1]); 
    byte8 = (byte8 & 0x3F) | 0x80;
}

bool UUID::init(std::string& description)
{
    if (description.length() != 36)
    {
        logError("Invalid string length to generate UUID:\n%s\n", description.c_str());
        return false;
    }

    // Verifies string format
    // XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
    for(uint32 i=0; i<36; ++i)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            if (description[i] != '-')
            {
                logError("Invalid string format to generate UUID (at %u):\n%s\n", i, description.c_str());
                return false;
            }

            continue;
        }

        if (!isHexCypher(description[i]))
        {
            logError("Invalid string format to generate UUID (at %u):\n%s\n", i, description.c_str());
            return false;
        }
    }

    // Modifies UUID only when whole string is known to be valid
    uint32 offset = 0;
    uint8 valueCharacter = 0;
    for (uint32 i = 0; i < 36; ++i)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            continue;
        }

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

std::string UUID::description(void)
{
    static constexpr char hex[] = "0123456789abcdef";

    std::string out;
    out.reserve(36);
    
    for(uint8 i=0; i<16; ++i)
    {
        if (i == 4 || i == 6 || i == 8 || i == 10)
        {
            out.push_back('-');
        }

        uint8 byte = *(((uint8*)&qword[0]) + i);

        out.push_back(hex[byte >> 4]);
        out.push_back(hex[byte & 0x0F]);
    }

    return out;
}

} // en