/*

 Ngine v5.0
 
 Module      : Fibers support.
 Requirements: none
 Description : Fibers are lightweight threads managed in application address space.

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
      LPVOID handle;           // Pointer to fiber object
      uint32 maximumStackSize;
         
      winFiber(void);
      winFiber(const FiberFunction function, 
               void* fiberState,
               const uint32 stackSize,
               const uint32 maximumStackSize);
     ~winFiber();
      };
}
#endif

#endif
