/*

 Ngine v5.0
 
 Module      : Fibers support.
 Requirements: none
 Description : Fibers are lightweight threads managed in application address space.

*/

#include "core/parallel/psxFiber.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/memory/alignedAllocator.h"
#include "core/memory/pageAllocator.h"

namespace en
{

typedef void (*helperFunction)();

//extern "C"
void functionExecutingTask(int hiAdress, int loAdress)
{
    // Reconstruct pointer to Fiber object, on which this function is running,
    // and for which, task should be executed.
    Fiber* fiber = (Fiber*)( ((uint64)hiAdress << 32) | (uint64)loAdress );
      
    assert( fiber->function );

    // Executer provided function
    fiber->function(fiber->state);
}

psxFiber::psxFiber(const uint32 stackSize, const uint32 _maximumStackSize) :
    stack(nullptr),
    maximumStackSize(_maximumStackSize),
    Fiber()
{
    function       = nullptr;
    state          = nullptr;
    waitingForTask = nullptr;
   
    // Default contructor for thread transitioning to Fiber
    // (will be used to save state fo this thread on first switch, but it already
    // needs to have proper stack assigned)
    
    // Get current context
    int result = getcontext(&context);
    assert( result != -1 );
    assert( context.uc_mcontext );
   
    // Allocate fiber stack
    stack = virtualAllocate(stackSize, maximumStackSize);
    //stack = allocate<uint8>(stackSize, PageSize);
    assert( stack );
   
    // Modify copy of current context, to use dedicated stack
    context.uc_stack.ss_sp   = stack;
    context.uc_stack.ss_size = stackSize;
    context.uc_link          = nullptr;
    
    // From now on, whenever control is returned to original thread (identified
    // by this fiber) thread default stack won't be used anymore, but separately
    // allocated fiber one will be used. On Windows, thread can be properly
    // converted to Fiber, which in turn reuses it's original stack.
}

psxFiber::psxFiber(const FiberFunction _function, void* fiberState, const uint32 stackSize, const uint32 _maximumStackSize) :
    stack(nullptr),
    maximumStackSize(_maximumStackSize),
    Fiber()
{
    function       = _function;
    state          = fiberState;
    waitingForTask = nullptr;

    // Use current context as a base for new one
    int result = getcontext(&context);
    assert( result != -1 );
    assert( context.uc_mcontext );
   
    // Good description of getcontext(), makecontext():
    // https://en.wikipedia.org/wiki/Setcontext
   
    // TODO: VERIFY! Stack allocation on Heap may be not allowed???
   
    // Allocate fiber stack
    stack = virtualAllocate(stackSize, maximumStackSize);
    assert( stack );
   
    // Modify copy of current context, to use dedicated stack
    context.uc_stack.ss_sp   = stack;
    context.uc_stack.ss_size = stackSize;
    context.uc_link          = nullptr;    // TODO: Pointer to context to which control should return on function exit.
                                           //       Otherwise current thread will terminate.

    // Fiber function receives 64bit pointer to state, but makecontext() expects
    // fiber function to receive set of "int" parameters. This means that we need
    // to use proxy function, to be able to properly call desired function, and
    // pass to it 64bit state pointer.
    //
    // There are two ways to accomplish that:
    // 1) Pass both desired function pointer, and its state pointer, both split
    //    to two sint32 variables to proxy function (4x sint32 in total), which
    //    will then recosntruct both, and perform desired function call.
    // 2) Pass pointer to this fiber object, which stores pair of 64bit pointers
    //    (to fiber function and its state), and then call that function indirectly.
    //
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

/*
std::unique_ptr<Fiber> convertToFiber(void)
{
    // Creates Fiber, that will be used to store thread current state
    return std::unique_ptr<psxFiber>(new psxFiber());
  //return std::make_unique<psxFiber>();
}

std::unique_ptr<Fiber> createFiber(const FiberFunction function, void* fiberState, const uint32 stackSize, const uint32 maximumStackSize)
{
    return std::unique_ptr<psxFiber>(new psxFiber(function, fiberState, stackSize, maximumStackSize));
  //return std::make_unique<psxFiber>(function, fiberState, stackSize, maximumStackSize);
}
//*/
Fiber* convertToFiber(const uint32 stackSize, const uint32 maximumStackSize)
{
    // Creates Fiber, that will be used to store thread current state
    psxFiber* fiber = allocate<psxFiber>(1, cacheline);
    new (fiber) psxFiber(stackSize, maximumStackSize);
    return fiber;
   
  //return new psxFiber(stackSize, maximumStackSize);
}

Fiber* createFiber(const FiberFunction function, void* fiberState, const uint32 stackSize, const uint32 maximumStackSize)
{
    psxFiber* fiber = allocate<psxFiber>(1, cacheline);
    new (fiber) psxFiber(function, fiberState, stackSize, maximumStackSize);
    return fiber;
   
  //return new psxFiber(function, fiberState, stackSize, maximumStackSize);
}

void switchToFiber(Fiber& _current, Fiber& _fiber)
{
    psxFiber& current = reinterpret_cast<psxFiber&>(_current);
    psxFiber& fiber   = reinterpret_cast<psxFiber&>(_fiber);
    int result = swapcontext(&current.context, &fiber.context);
}

}

#endif
