/*

 Ngine v5.0
 
 Module      : Hash
 Requirements: none
 Description : Simple hash container and generation.

*/

#ifndef ENG_CORE_ALGORITHMS_HASH
#define ENG_CORE_ALGORITHMS_HASH

#include <string>

#include "core/types.h"

#define UseBigHash 0

// Tweak hash seed until finding perfect hash for given set of assets. Hash seed
// should be stored together with assets in the package, so patching asset set 
// can also update used seed to maintain perfect hash.
#define PerfectHashSeed 0xC438F289 

namespace en
{

#if UseBigHash
// Hash is defined as separate type to allow easy change of underlying type for fine tuning (64 vs 128 bit)
typedef uint128 hash; 
#else
typedef uint64 hash;  
#endif

// Generates hash from any data
hash hashData(const void* data, const uint32 size, const uint32 seed = PerfectHashSeed);

// Generates hash from string
hash hashString(const std::string& name, const uint32 seed = PerfectHashSeed);

} // en

#endif
