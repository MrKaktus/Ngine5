/*

 Ngine v5.0
 
 Module      : Fibers support.
 Requirements: none
 Description : Fibers are lightweight threads managed in application address space.

*/

#include "core/parallel/psxFiber.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/memory/pageAllocator.h"

namespace en
{
   typedef void (*helperFunction)();

   extern "C" void functionExecutingTask(int hiAdress, int loAdress)
   {
   // Reconstruct pointer to Fiber object, on which this function is running,
   // and for which, task should be executed.
   Fiber* fiber = (Fiber*)( ((uint64)hiAdress << 32) | (uint64)loAdress );
      
   assert( fiber->function );

   // Executer provided function
   fiber->function(fiber->state);
   }
   

   psxFiber::psxFiber(void) :
      stack(nullptr),
      maximumStackSize(0)
   {
   // Default contructor for thread transitioning to Fiber 
   // (will be used to save state fo this thread on first switch)
   }

   psxFiber::psxFiber(const FiberFunction _function, void* fiberState, const uint32 stackSize, const uint32 _maximumStackSize) :
      function(_function),
      state(fiberState),
      waitingForTask(nullptr),
      stack(nullptr),
      maximumStackSize(_maximumStackSize)
   {
   // Use current context as a base for new one
   int result = getcontext(&context);
   assert( result != -1 );
      
   // Allocate fiber stack
   void* stack = virtualAllocate(stackSize, maximumStackSize);
   assert( stack );
   
   // Modify copy of current context, to use dedicated stack
   context.uc_stack.ss_sp   = stack;
   context.uc_stack.ss_size = stackSize;
   context.uc_link          = nullptr;    // TODO: Pointer to context to which control should return on function exit.
                                          //       Otherwise current thread will terminate.
      
   // Create new context for this fiber.
   // As task to execute is not known yet, helper function will be used.
   // It will pick pointer to task to execute, once this fiber is started.
   sint32 loAdress = (sint32)((uint64)(this) & 0xFFFFFFFF);
   sint32 hiAdress = (sint32)((uint64)(this) >> 32);
      
   // C++ function that requires parameters is cast to C function expected by makecontext.
   // That C++ function when declared as using C convention (through extern "C"), will
   // properly consume arguments from stack (passed below).
   //
   // See more examples at:
   // https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rmctxt.htm
   // https://github.com/stevedekorte/coroutine/blob/master/source/Coro.c
   //
   makecontext(&context, (helperFunction)functionExecutingTask, 2, (int)hiAdress, (int)loAdress);
   }
   
   psxFiber::~psxFiber()
   {
   // Release fiber stack
   if (stack)
      virtualDeallocate(stack, maximumStackSize);
   }
   
   std::unique_ptr<Fiber> convertToFiber(void)
   {
   // Creates Fiber, that will be used to store thread current state 
   return std::make_unique<psxFiber>();
   }

   std::unique_ptr<Fiber> createFiber(const FiberFunction function, void* fiberState, const uint32 stackSize, const uint32 maximumStackSize)
   {
   return std::make_unique<psxFiber>(function, fiberState, stackSize, maximumStackSize);
   }

   void switchToFiber(Fiber& _current, Fiber& _fiber)
   {
   psxFiber& current = reinterpret_cast<psxFiber&>(_current);
   psxFiber& fiber   = reinterpret_cast<psxFiber&>(_fiber);
   int result = swapcontext(&current.context, &fiber.context);
   }
}

#endif
