/*

 Ngine v5.0
 
 Module      : Fibers support.
 Requirements: none
 Description : Allows easy creation and management of fibers.

*/

#ifndef ENG_CORE_PARALLEL_WINDOWS_FIBER
#define ENG_CORE_PARALLEL_WINDOWS_FIBER

#include "core/parallel/fiber.h"

#if defined(EN_PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace en
{
   class winFiber : public Fiber
      {
      public:
      LPVOID handle;
      uint32 maximumStackSize;
         
      winFiber(const uint32 stackSize,
               const uint32 maximumStackSize);
     ~winFiber();
      };
}
#endif

#endif
