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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <atomic>

// Global storing Windows ID of main thread
extern DWORD mainThreadId;

namespace en
{
   // Windows has different entry point for threads, thus passed function and
   // it's parameters need to be wrapped into container and passed in such way
   // to interface function.
   struct winThreadContainer
      {
      ThreadFunction function;
      Thread* threadClass;

      winThreadContainer(ThreadFunction function, Thread* threadClass);
      };

   class winThread : public Thread
      {
      public:
      HANDLE handle;                // Thread handle
      HANDLE sleepSemaphore;        // Sleeping semaphore
      winThreadContainer package;   // Packaged app thread and this class instance pointer
      void*  localState;            // State passed on thread creation
      uint32 index;                 // Thread unique ID
      std::atomic<bool> isSleeping; // Thread sleeps
      std::atomic<bool> valid;      // Thread is executing (may sleep)

      winThread(ThreadFunction function, void* threadState);
      virtual ~winThread();

      virtual void* state(void);
      virtual void name(std::string threadName);
      virtual uint32 id(void);
      virtual uint64 coresExecutionMask(void);
      virtual void   executeOn(const uint64 coresMask);
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
