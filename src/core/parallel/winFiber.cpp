/*

 Ngine v5.0
 
 Module      : Fibers support.
 Requirements: none
 Description : Fibers are lightweight threads managed in application address space.

*/

#include "core/parallel/winFiber.h"

#if defined(EN_PLATFORM_WINDOWS)

#include <assert.h>

namespace en
{
   void functionExecutingTask(LPVOID lpFiberParameter)
   {
   // Reconstruct pointer to Fiber object, on which this function is running,
   // and for which, task should be executed.
   Fiber* fiber = (Fiber*)(lpFiberParameter);

   }

   winFiber::winFiber(void) :
      maximumStackSize(0)
   {
   handle = ConvertThreadToFiberEx(nullptr, FIBER_FLAG_FLOAT_SWITCH);
   assert( handle != nullptr );
   }

   winFiber::winFiber(const uint32 stackSize, const uint32 _maximumStackSize) :
      maximumStackSize(_maximumStackSize)
   {
   handle = CreateFiberEx(stackSize,
                          maximumStackSize,
                          FIBER_FLAG_FLOAT_SWITCH,
                          functionExecutingTask,  // (LPFIBER_START_ROUTINE)
                          this);
   }

   winFiber::~winFiber()
   {
   DeleteFiber(handle);
   }

   std::unique_ptr<Fiber> convertToFiber(void)
   {
   // Creates Fiber, that will be used to store thread current state 
   return std::make_unique<winFiber>();
   }

   std::unique_ptr<Fiber> createFiber(const uint32 stackSize, const uint32 maximumStackSize)
   {
   return std::make_unique<winFiber>(stackSize, maximumStackSize);
   }

   void switchToFiber(Fiber& _current, Fiber& _fiber)
   {
   // Current state is tracked by Windows in automatic way
   winFiber& fiber = reinterpret_cast<winFiber&>(_fiber);
   SwitchToFiber(fiber.handle);
   }

/*
   uint32 winFiber::id(void)
   {
   // TODO: Find handle in global table and convert to index!
   PVOID handle = GetCurrentFiber();
   }
*/
}
#endif
