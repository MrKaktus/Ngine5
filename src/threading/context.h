/*

 Ngine v5.0
 
 Module      : Threads
 Requirements: none
 Description : Manages threads, thread-pool and tasks
               execution through simple interface.

*/

#ifndef ENG_THREADS_CONTEXT
#define ENG_THREADS_CONTEXT

#include "threading/Nqueue.h"
#include "threading/scheduler.h"

namespace en
{
   namespace threads
   {
   // It should always be declared as static
   class TLS
         {
         private:
         bool  created;               // Indicates if TLS is initialized
         #ifdef EN_PLATFORM_WINDOWS
         DWORD index;                 // Index in TLS pointers table
         #endif
   
         public:
         TLS();
        ~TLS();
   
         bool  create(void);
         void  destroy(void);
         void  set(void* ptr);           // Sets pointer to Thread-dependent data
         void* get(void);                // Gets pointer to Thread-dependent data
         };

   struct Context
          {
#ifdef EN_PLATFORM_WINDOWS
          HANDLE         sleepingSignal; // Counts sleeping threads
          HANDLE         wakeUpSignal;   // Signals that workers should react to new event
#endif
          WorkerThread** thread;         // Table of pointers to worker threads
          uint32         workers;        // Threads count
   static TLS            currentState;   // Pointer to current worker state (Thread-Dependent)
 volatile TaskState*     mainState;      // State of main task
   static bool           created;        // Flag indicates state of Task Pool
          bool           started;        // Flag indicates if main thread started execution
          bool           shutdown;       // Flag terminating execution

          Context();
         ~Context();

          bool create(void);
          void destroy(void);
          bool start(Task* task);        // Main thread starts execution of first task
          void wakeUpAllThreads(void);   // Resumes all sleeping workers
          };
   }

extern threads::Context SchedulerContext;
}

#endif
