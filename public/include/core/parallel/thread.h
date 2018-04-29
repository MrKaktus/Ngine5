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

#include <memory>

namespace en
{
   class Thread;
   
   typedef void*(*ThreadFunction)(Thread* thread);

   class Thread
      {
      public:
      virtual void* state(void) = 0; // State passed on thread start
      virtual void name(std::string threadName) = 0; // Thread sets its name 
      virtual void sleep(void) = 0;      // Thread puts itself to sleep mode
      virtual void wakeUp(void) = 0;     // Thread is waken up by other thread
      virtual bool sleeping(void) = 0;   // Check if thread is sleeping
      virtual bool working(void) = 0;    // Check if thread is executing
      virtual void exit(uint32 ret) = 0; // Thread terminates its execution with return code
      virtual void waitUntilCompleted(void) = 0; // Calling thread sleeps until thread is not finished
      
      virtual ~Thread(void) {};                  // Terminate thread execution
      };
   
    std::unique_ptr<Thread> startThread(ThreadFunction function, void* threadState);
   
    // Returns pointer to instance describing current thread (or nullptr for main thread)
    Thread* currentThread(void);
}

#endif
