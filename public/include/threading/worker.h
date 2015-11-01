/*

 Ngine v5.0
 
 Module      : Worker thread.
 Visibility  : Engine only.
 Requirements: none
 Description : Manages worker thread during 
               execution of sheduled tasks.

*/

#ifndef ENG_THREADING_WORKER
#define ENG_THREADING_WORKER

#include "core/defines.h"
#include "core/types.h"
#include "threading/mutex.h"
#include "threading/Nqueue.h"
#include "threading/task.h"

namespace en
{
   namespace threads
   {
   class cachealign WorkerThread
         {
         private:
         friend class  Interface;
         friend struct Context;
   
         Nqueue<Task*>* queue[2];    // Queues of tasks to execute (normal and dedicated) 
         Nmutex         mutex;       // For non-blocking tasks stealing protection
         TaskState*     state;       // Status of currently executed task
         uint8          id;          // Current worker id
         bool           done;        // Indicates if thread finished execution
         #ifdef EN_PLATFORM_WINDOWS
         HANDLE         handle;      // Thread system id
         #endif
   
         TaskState* push(Task* task, bool dedicated = false); // Push task to specified queue for execution
         bool       pop(Task** task, bool dedicated = false); // Take one task from the specified queue for execution
         bool       steal(void);                              // Try to get some work from other threads 
         //bool        give(WorkerThread* dst);                // Try to give up some work to idle thread
   
         public:
         WorkerThread(uint8 id);          // Thread is sleeping until first tasks will arrive for execution. 
        ~WorkerThread();
   
         uint32 start(void);              // Workers entry point 
         void   schedule(TaskState* in);  // Main loop scheduling tasks
         bool   finished(void);           // Worker state
         void   sleep(void);              // Worker Thread goes to sleep mode until some work will be signaled
         };
   }
}

//class cachealign WorkerThread
//      {
//      private:
//      friend class NSchedulerContext;
//
//      Nqueue<NTask*>* m_queue[2];    // Queues of tasks to execute (normal and dedicated) 
//      Nmutex          m_mutex;       // For non-blocking tasks stealing protection
//      NTaskState*     m_state;       // Status of currently executed task
//      uint8           m_id;          // Current worker id
//      bool            m_done;        // Indicates if thread finished execution
//#ifdef EN_PLATFORM_WINDOWS
//      HANDLE          m_handle;      // Thread system id
//#endif
//
//      NTaskState* push(NTask* task, bool dedicated = false); // Push task to specified queue for execution
//      bool        pop(NTask** task, bool dedicated = false); // Take one task from the specified queue for execution
//      bool        steal(void);                               // Try to get some work from other threads 
//      //bool        give(WorkerThread* dst);                   // Try to give up some work to idle thread
//
//      public:
//      WorkerThread(uint8 id);
//     ~WorkerThread();
//
//      uint32 start(void);              // Workers entry point 
//      void   schedule(NTaskState* in); // Main loop scheduling tasks
//      bool   finished(void);           // Worker state
//      void   sleep(void);              // Worker Thread goes to sleep mode until some work will be signaled
//      };

#endif