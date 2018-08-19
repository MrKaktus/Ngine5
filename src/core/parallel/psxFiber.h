/*

 Ngine v5.0
 
 Module      : Fibers support.
 Requirements: none
 Description : Allows easy creation and management of fibers.

*/

#ifndef ENG_CORE_PARALLEL_POSIX_FIBER
#define ENG_CORE_PARALLEL_POSIX_FIBER

#include "core/parallel/fiber.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#define _XOPEN_SOURCE
#include <ucontext.h>

namespace en
{
   class psxFiber : public Fiber
      {
      public:
      ucontext_t context;
      void*      stack;
      uint32     maximumStackSize;
          
      psxFiber(const uint32 stackSize,
               const uint32 maximumStackSize);
     ~psxFiber();
      };
}
#endif

#endif
