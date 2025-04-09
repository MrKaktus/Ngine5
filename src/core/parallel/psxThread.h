/*

 Ngine v5.0
 
 Module      : Threads support.
 Requirements: none
 Description : Allows easy creation and management of threads.

*/

#ifndef ENG_CORE_PARALLEL_POSIX_THREAD
#define ENG_CORE_PARALLEL_POSIX_THREAD

#include "core/parallel/thread.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include <pthread.h>
#include <unistd.h>

namespace en
{

class psxThread : public Thread
{
    public:
    pthread_t handle;          // Thread handle
    pthread_attr_t attr;       // Thread state
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    void* localState;          // State passed on thread creation
    uint32 index;              // Thread unique ID
    volatile bool valid;       // Thread is executing (may sleep)

    psxThread(ThreadFunction function, void* threadState);
    virtual ~psxThread();

    virtual void* state(void);
    virtual void name(std::string threadName);
    virtual uint32 id(void);
    virtual uint64 coresExecutionMask(void);
    virtual void   executeOn(const uint64 coresMask);
    virtual void sleep(void);
    virtual void sleepFor(const Time time);
    virtual void sleepUntil(const Time time);
    virtual void wakeUp(void);
  //virtual bool sleeping(void);
    virtual bool working(void);
    virtual void exit(uint32 ret);
    virtual void waitUntilCompleted(void);
};
   
extern uint64 currentThreadSystemId(void);

extern uint32 registerThread(uint64 threadSystemId); ///< Registers thread with given systemId, and returns assigned localId
extern void releaseThread(uint32 threadId);          ///< Releases thread pointed by local threadId

} // en

#endif
#endif
