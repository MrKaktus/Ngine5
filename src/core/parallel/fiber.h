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

class TaskState;

typedef void(*FiberFunction)(void*);

class Fiber
{
    public:
    FiberFunction function;    // Function this fiber will execute
    void*         state;       // Optional state passed to fiber
    TaskState* waitingForTask; // State of task, that this Fiber is waiting to finish (otherwise nullptr)
    
    virtual ~Fiber() {};
};

/// Converts calling thread to Fiber, from now on it can safely switch to other Fibers
extern Fiber* convertToFiber(const uint32 stackSize,
                             const uint32 maximumStackSize);

extern Fiber* createFiber(const FiberFunction function,
                                          void* fiberState,
                                          const uint32 stackSize,
                                          const uint32 maximumStackSize);

/// Switch fiber running on current thread, to given one.
/// Actual fiber state will be saved in "current" fiber object
extern void switchToFiber(Fiber& current, Fiber& fiber);

} // en

#endif
