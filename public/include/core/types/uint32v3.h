/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 3 component uint32 vector.

*/

#ifndef ENG_CORE_TYPES_UINT32V3
#define ENG_CORE_TYPES_UINT32V3

#include "core/types/basic.h"

namespace en
{
   class uint32v3
      {
      public:
      union { uint32 x; uint32 width;  };
      union { uint32 y; uint32 height; };
      union { uint32 z; uint32 depth;  };
      
      uint32v3(void);
      uint32v3(uint32 a, uint32 b, uint32 c);
      };
}

#endif