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
      HANDLE  handle;          // Thread handle
      HANDLE  sleepSemapthore; // Sleeping semaphore

      //uint32  m_id;         // Thread id
      bool    m_sleeping;   // Sleep mode flag
      //bool    m_finish;     // Termination indicator
      bool    idle;       // Nothing doing

      virtual winThread();
      virtual ~winThread();

      virtual bool start(void* function, void* params);
      virtual bool current(void);
      virtual bool sleep(void);
      virtual bool wakeUp(void);
      virtual bool working(void);
      virtual void exit(uint32 ret);
      virtual bool stop(void);
      };
}
#endif

#endif