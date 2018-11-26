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

#include <memory>

namespace en
{
   class Fiber
      {
      public:
      virtual ~Fiber() {};
      };

   // Converts calling thread to Fiber, from now on it can safely switch to other Fibers
   extern std::unique_ptr<Fiber> convertToFiber(void);

   extern std::unique_ptr<Fiber> createFiber(const uint32 stackSize, const uint32 maximumStackSize);

   // Switch fiber running on current thread, to given one.
   // Actual fiber state will be saved in "current" fiber object
   extern void switchToFiber(Fiber& current, Fiber& fiber);
}

#endif
