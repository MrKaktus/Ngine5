/*

 Ngine v5.0
 
 Module      : Fibers support.
 Requirements: none
 Description : Allows easy creation and management of fibers.

*/

#ifndef ENG_CORE_PARALLEL_FIBER
#define ENG_CORE_PARALLEL_FIBER

#include "core/defines.h"
#include "core/types.h"

namespace en
{
   class Fiber
      {
      public:
      virtual ~Fiber() {};
      };

   // Switch current thread, running current fiber, to given one.
   extern void switchToFiber(Fiber& fiber);
}

#endif
