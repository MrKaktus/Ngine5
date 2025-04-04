/*

 Ngine v5.0
 
 Module      : Threads support.
 Requirements: none
 Description : Allows easy creation and management of threads.

*/

#ifndef ENG_CORE_PARALLEL_THREAD
#define ENG_CORE_PARALLEL_THREAD

#include "core/defines.h"
#include "core/types.h"
#include "utilities/timer.h"

#include <memory>

namespace en
{

constexpr uint32 InvalidThreadID = 0xFFFFFFFF;

class Thread;

typedef void*(*ThreadFunction)(Thread* thread);

class Thread
{
    public:
    virtual void*  state(void) = 0;                       ///< State passed on thread start
    virtual void   name(std::string threadName) = 0;      ///< Sets thread name (need to be called by thread itself)
    virtual uint32 id(void) = 0;                          ///< Thread unique index
    virtual uint64 coresExecutionMask(void) = 0;          ///< Mask of CPU cores on which thread can be executed
    virtual void   executeOn(const uint64 coresMask) = 0; ///< Specifies CPU cores on which this thread can execute
    virtual void   sleep(void) = 0;                       ///< Thread puts itself to sleep mode (need to be called by thread itself)
    virtual void   sleepFor(const Time time) = 0;         ///< Thread puts itself to sleep for given period of time
    virtual void   sleepUntil(const Time time) = 0;       ///< Thread puts itself to sleep until absolute moment in time occurs
    virtual void   wakeUp(void) = 0;                      ///< Thread is waken up by other thread
    virtual bool   working(void) = 0;                     ///< Check if thread is executing
    virtual void   exit(uint32 ret) = 0;                  ///< Thread terminates its execution with return code (need to be called by thread itself)
    virtual void   waitUntilCompleted(void) = 0;          ///< Calling thread sleeps until thread is not finished
    
    virtual ~Thread(void) {};                  
};

// Info: There is no method to check if thread is sleeping on purpose, as it can
//       be woken up by other thread at any time, while current thread would make
//       such query. Application needs to track threads sleeping state on it's own,
//       by guarding places where such threads go to sleep and can be woken up,
//       with mutexes or other synchronization mechanism.

std::unique_ptr<Thread> startThread(ThreadFunction function, void* threadState);

extern void   setThreadName(std::string threadName);
extern uint32 currentThreadId(void);  ///< Index of current thread (calling this function)
extern uint32 spawnedThreads(void);   ///< Threads created since application start
extern uint32 runningThreads(void);   ///< Count of threads that are still running
extern void   wakeUpMainThread(void); ///< Wakes up main thread to process incoming events
extern uint32 currentCoreId(void);    ///< Index of CPU core on which this thread is currently executing

} // en

#endif
