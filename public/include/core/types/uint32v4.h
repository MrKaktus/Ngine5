/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4 component uint32 vector.

*/

#ifndef ENG_CORE_TYPES_UINT32V4
#define ENG_CORE_TYPES_UINT32V4

#include "core/types/basic.h"

namespace en
{
   class uint32v4
      {
      public:
      uint32 x;
      uint32 y;
      union { uint32 z; uint32 width;  };
      union { uint32 w; uint32 height; };
      
      uint32v4(void);
      uint32v4(const uint32 x, const uint32 y, const uint32 z, const uint32 w);
      };
   
   static_assert(sizeof(uint32v4) == 16, "en::uint32v4 size mismatch!");
}

#endif
