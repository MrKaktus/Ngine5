/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 2 component uint32 vector.

*/

#ifndef ENG_CORE_TYPES_UINT32V2
#define ENG_CORE_TYPES_UINT32V2

#include "core/types/basic.h"

namespace en
{
   class uint32v2
      {
      public:
      union { uint32 x; uint32 width;  uint32 base;  };
      union { uint32 y; uint32 height; uint32 count; };
      
      uint32v2(void);
      uint32v2(const uint32 x, const uint32 y);

      bool operator== (const uint32v2 b) const;
      };
   
   static_assert(sizeof(uint32v2) == 8, "en::uint32v2 size mismatch!");
}

#endif
