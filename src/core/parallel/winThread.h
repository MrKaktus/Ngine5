/*

 Ngine v5.0
 
 Module      : Threads support.
 Requirements: none
 Description : Allows easy creation and management of threads.

*/

#ifndef ENG_CORE_PARALLEL_WINDOWS_THREAD
#define ENG_CORE_PARALLEL_WINDOWS_THREAD

#include "core/parallel/thread.h"

#if defined(EN_PLATFORM_WINDOWS)

namespace en
{
   class winThread : public Thread
      {
      public:
      HANDLE handle;            // Thread handle
      HANDLE sleepSemapthore;   // Sleeping semaphore
      void*  localState;        // State passed on thread creation
      volatile bool isSleeping; // Thread sleeps
      volatile bool valid;      // Thread is executing (may sleep)

      winThread(); // Wraps around current thread
      winThread(ThreadFunction function, void* threadState);
      virtual ~winThread();

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
