/*

 Ngine v5.0

 Module      : Types.
 Requirements: none
 Description : Universal unique identifier.

*/

#include <random>

#include "core/types/uuid.h"

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