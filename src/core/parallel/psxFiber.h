/*

 Ngine v5.0
 
 Module      : Fibers support.
 Requirements: none
 Description : Fibers are lightweight threads managed in application address space.

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
             const uint32 _maximumStackSize);
    psxFiber(const FiberFunction function,
             void* fiberState,
             const uint32 stackSize,
             const uint32 maximumStackSize);
   ~psxFiber();
};

} // en

#endif
#endif
