/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : 2 component sint16 vector.

*/

#ifndef ENG_CORE_TYPES_SINT16V2
#define ENG_CORE_TYPES_SINT16V2

#include "core/types/basic.h"

namespace en
{
   class sint16v2
      {
      public:
      sint16 x;
      sint16 y;
      
      sint16v2(void);
      sint16v2(const sint16 x, const sint16 y);

      bool operator== (const sint16v2 b) const;
      };
   
   static_assert(sizeof(sint16v2) == 4, "en::sint16v2 size mismatch!");
}

#endif
