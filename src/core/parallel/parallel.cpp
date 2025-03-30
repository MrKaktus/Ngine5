/*

 Ngine v5.0
 
 Module      : Threads support.
 Requirements: none
 Description : Allows easy creation and management of threads.

*/

#include "core/defines.h"
#include "core/parallel/mutex.h"

#if defined(EN_PLATFORM_OSX)
#include "core/parallel/psxThread.h"
#endif
#if defined(EN_PLATFORM_WINDOWS)
#include "core/parallel/winThread.h"
#endif

#include <assert.h>

constexpr uint32 MaxThreads      = 256;

namespace en
{

// Internal

static Mutex  lockThreadID;                    // Used when modifying thread ID translation table
static std::atomic<uint32> threadsSpawned(0);  // Amount of threads spawned since start of application
static uint64 registeredThread[MaxThreads];    // Thread global to local ID translation table

namespace parallel
{

// Initialize thread global ID to local ID translation table. This also means
// that this code is executing on main thread, and thus we can query it's
// global ID and init table with it.
void init(void)
{
    uint64 systemId = currentThreadSystemId();

    memset(&registeredThread[0], 0, sizeof(uint64) * MaxThreads);
    registeredThread[0] = systemId;
    threadsSpawned.store(1, std::memory_order_release);
}

} // en::parallel

uint32 registerThread(uint64 threadSystemId)
{
    uint32 index = threadsSpawned.fetch_add(1, std::memory_order_release);
    assert( (index + 1) < MaxThreads );

    registeredThread[index] = threadSystemId;
   
    return index;
}

void releaseThread(uint32 threadId)
{
    registeredThread[threadId] = InvalidThreadID;
}


// Public


uint32 currentThreadId(void)
{
    uint64 threadId = currentThreadSystemId();

    // This thread is already created, so is already on the list. As list can only
    // grow, there is no risk of colliding with other thread being created during
    // this thread iterating over list.
    uint32 threads = threadsSpawned.load(std::memory_order_acquire);
   
    for(uint32 index=0; index<threads; ++index)
    {
        if (registeredThread[index] == threadId)
        {
            return index;
        }
    }

    // Code execution should never reach this place
    assert( 0 );
    return InvalidThreadID;

    // Alternative:
    //
    // #include <thread>
    //
    // std::thread::id id = std::this_thread::get_id();
    //
    // id still needs to be translated to local index that can be used to access arrays.
}

uint32 spawnedThreads(void)
{
    return threadsSpawned.load(std::memory_order_relaxed);
}

uint32 runningThreads(void)
{
    uint32 threads = threadsSpawned.load(std::memory_order_relaxed);
   
    // Check how many threads are still running
    uint32 running = 0;
    for(uint32 i=0; i<threads; ++i)
    {
        if (registeredThread[i] != InvalidThreadID)
        {
            running++;
        }
    }

    return running;
}

} // en
