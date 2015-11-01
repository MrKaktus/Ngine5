

#ifndef ENG_THREADING_SHEDULER
#define ENG_THREADING_SHEDULER

#include "threading/task.h"
#include "threading/maintask.h"
#include "threading/thread.h"
#include "threading/worker.h"
#include "threading.h"
#include "core/utilities/Tsingleton.h"

namespace en
{
   namespace threads
   {
   class Interface
         {
         public:
         TaskState* run(Task* task, bool wait = false);             // Adds task for execution 
         TaskState* run(Task* task, uint8 core, bool wait = false); // Adds task for execution on specified core 
         //TaskState* parallel(Task* task, Range range, bool wait = false); // Adds splittable task for execution 
         void       wait(TaskState* state);              // Wait for specified task to be done
         uint32     core(void) const;                    // Core on which current code is executed
         uint32     workers(void) const;                 // Worker Threads count
         void       shutdown(void);                      // Send signal to terminate all workers and finish application
         bool       closing(void) const;                 // Returns true if Thread-Pool is shutting down
         };
   }

extern threads::Interface Scheduler;  // Default instance of Thread-Pool Scheduler
}

#include "threading/threading.inl"












//
//
//// Describes sheduler working on pool of tasks
//class NSchedulerContext : public Tsingleton<NSchedulerContext>
//      {
//      private:
//      NSchedulerContext();
//      friend class Tsingleton<NSchedulerContext>;
//      friend class WorkerThread;
//
//#ifdef EN_PLATFORM_WINDOWS
//      HANDLE         m_sleepingSignal; // Counts sleeping threads
//      HANDLE         m_wakeUpSignal;   // Signals that workers should react to new event
//#endif
//      WorkerThread** m_thread;         // Table of pointers to worker threads
//      uint32         m_workers;        // Threads count
//      static Ntls    m_currentState;   // Pointer to current worker state (Thread-Dependent)
//      volatile
//      NTaskState*    m_mainState;      // State of main task
//      static bool    m_created;        // Flag indicates state of Task Pool
//      bool           m_started;        // Flag indicates if main thread started execution
//      bool           m_shutdown;       // Flag terminating execution
//
//      void init(void);                 // Init Task Pool
//      void destroy(void);              // Shutdown Task Pool
//      void wakeUpSignal(void);         // Resumes sleeping workers
//
//      public:
//     ~NSchedulerContext();
//
//      bool        start(NTask* task);                                     // Main thread starts execution of first task
//      NTaskState* run(NTask* task, bool wait = false);                    // Adds task for execution 
//      NTaskState* run(NTask* task, uint8 core, bool wait = false);        // Adds task for execution on specified core 
//      //NTaskState* parallel(NTask* task, NRange range, bool wait = false); // Adds splittable task for execution 
//      void        wait(NTaskState* state);             // Wait for specified task to be done
//      uint8       current(void);                       // Core on which current code is executed
//      uint8       cores(void);                         // Count of all working cores
//      void        shutdown(void);                      // Send signal to terminate all workers and finish application
//      bool        closing(void);                       // Returns true if Task Pool is shutting down
//      };
//
//forceinline NTaskState* NTask::run(void)
//{
// return NSchedulerContext::getInstance().run(this);
//}

#endif