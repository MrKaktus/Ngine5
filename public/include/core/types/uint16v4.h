/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4 component uint16 vector.

*/

#ifndef ENG_CORE_TYPES_UINT16V4
#define ENG_CORE_TYPES_UINT16V4

#include "core/types/basic.h"

namespace en
{
   class uint16v4
      {
      public:
      union { uint16 x; uint16 width;  };
      union { uint16 y; uint16 height; };
      union { uint16 z; uint16 depth;  };
      union { uint16 w; uint16 layer;  };
      
      uint16v4(void);
      uint16v4(const uint16 x, const uint16 y, const uint16 z, const uint16 w);

      bool operator== (const uint16v4 b) const;
      };
   
   static_assert(sizeof(uint16v4) == 8, "en::uint16v4 size mismatch!");
}

#endif
