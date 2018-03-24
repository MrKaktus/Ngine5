/*

 Ngine v5.0
 
 Module      : Hash
 Requirements: none
 Description : Simple hash container and generation.

*/

#include "core/algorithm/hash.h"
#include "core/algorithm/MurmurHash2.h"
#include "core/algorithm/MurmurHash3.h"

namespace en
{
   hash hashData(const void* data, const uint32 size, const uint32 seed)
   {
   hash result = 0;

#if UseBigHash
   MurmurHash3_x64_128(data, size, seed, &result);
#else
   result = MurmurHash64A(data, size, seed);
#endif

   return result;
   }

   hash hashString(const std::string name, const uint32 seed)
   {
   hash result = 0;

#if UseBigHash
   MurmurHash3_x64_128((const void *)name.c_str(), static_cast<sint32>(name.length()), seed, &result);
#else
   result = MurmurHash64A((const void *)name.c_str(), static_cast<sint32>(name.length()), seed);
#endif

   return result;
   }
}
