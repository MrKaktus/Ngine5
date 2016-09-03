/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 4 component sint32 vector.

*/

#ifndef ENG_CORE_TYPES_SINT32V4
#define ENG_CORE_TYPES_SINT32V4

#include "core/types/basic.h"

namespace en
{
   class sint32v4
      {
      public:
      sint32 x;
      sint32 y;
      union { sint32 z; sint32 width;  };
      union { sint32 w; sint32 height; };
      
      sint32v4(void);
      sint32v4(sint32 _x, sint32 _y, sint32 _z, sint32 _w);
      };
}

#endif
