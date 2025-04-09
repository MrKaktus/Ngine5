/*

 Ngine v5.0
 
 Module      : Fibers support.
 Requirements: none
 Description : Fibers are lightweight threads managed in application address space.

*/

#include "core/parallel/winFiber.h"

#if defined(EN_PLATFORM_WINDOWS)

#include <assert.h>
#include "core/memory/alignedAllocator.h"

namespace en
{

void WINAPI functionExecutingTask(LPVOID lpFiberParameter)
{
    // Reconstruct pointer to Fiber object, on which this function is running,
    // and for which, task should be executed.
    Fiber* fiber = (Fiber*)(lpFiberParameter);

    assert( fiber->function );

    // Executer provided function
    fiber->function(fiber->state);
}

winFiber::winFiber(const uint32 stackSize, const uint32 maximumStackSize) :
    maximumStackSize(0)
{
    function       = nullptr;
    state          = nullptr;
    waitingForTask = nullptr;

    // As thread is converted to fiber and reuses its stack, stackSize and
    // maximumStackSize are ignored.
    handle = ConvertThreadToFiberEx(nullptr, FIBER_FLAG_FLOAT_SWITCH);
    assert( handle != nullptr );
}

winFiber::winFiber(const FiberFunction _function, void* fiberState, const uint32 stackSize, const uint32 _maximumStackSize) :
    maximumStackSize(_maximumStackSize)
{
    function       = _function;
    state          = fiberState;
    waitingForTask = nullptr;

    handle = CreateFiberEx(stackSize,
                           maximumStackSize,
                           FIBER_FLAG_FLOAT_SWITCH,
                           functionExecutingTask,  // (LPFIBER_START_ROUTINE)
                           this);
}

winFiber::~winFiber()
{
    // Don't destroy fiber that represents original thread that converted to it
    if (function)
    {
        DeleteFiber(handle);
    }
}

Fiber* convertToFiber(const uint32 stackSize, const uint32 maximumStackSize)
{
    // Creates Fiber, that will be used to store thread current state
    winFiber* fiber = allocate<winFiber>(1, cacheline);
    new (fiber) winFiber(stackSize, maximumStackSize);
    return fiber;

  //return std::make_unique<winFiber>(stackSize, maximumStackSize);
}

Fiber* createFiber(const FiberFunction function, void* fiberState, const uint32 stackSize, const uint32 maximumStackSize)
{
    winFiber* fiber = allocate<winFiber>(1, cacheline);
    new (fiber) winFiber(function, fiberState, stackSize, maximumStackSize);
    return fiber;
   
  //return std::make_unique<winFiber>(function, fiberState, stackSize, maximumStackSize);
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

} // en

#endif
