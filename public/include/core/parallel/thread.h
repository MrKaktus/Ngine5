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

namespace en
{
   class Thread
      {
      public:
      virtual Thread(void) = 0;
      virtual ~Thread(void) {};

      virtual bool start(void* function, void* params) = 0; // Thread is starting execution
      virtual bool current(void) = 0;    // Bind current thread to instance of this class
      virtual bool sleep(void) = 0;      // Threads goes to slepp mode
      virtual bool wakeUp(void) = 0;     // Thread is waked up by other thread
      virtual bool working(void) = 0;    // Check if thread still exist
      virtual void exit(uint32 ret) = 0; // Thread terminates its execution
      virtual bool stop(void) = 0;       // Thread is terminated by other thread
      };



      private:
#ifdef EN_PLATFORM_WINDOWS
      HANDLE  handle;     // Thread system id
      HANDLE  wakeUp;     // Sleeping semaphore
#endif
      //uint32  m_id;         // Thread id
      bool    m_sleeping;   // Sleep mode flag
      //bool    m_finish;     // Termination indicator
      bool    m_idle;       // Nothing doing


}

#endif