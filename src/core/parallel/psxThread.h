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
      void* localState;          // State passed on thread creation
      volatile bool isSleeping;  // Thread sleeps
      volatile bool valid;       // Thread is executing (may sleep)

      psxThread(ThreadFunction function, void* threadState);
      virtual ~psxThread();

      virtual void* state(void);
      virtual void name(std::string threadName);
      virtual void sleep(void);
      virtual void wakeUp(void);
      virtual bool sleeping(void);
      virtual bool working(void);
      virtual void exit(uint32 ret);
      virtual void waitUntilCompleted(void);
      };
}
#endif

#endif

