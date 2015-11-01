


#ifndef ENG_THREADING_TASK
#define ENG_THREADING_TASK

#include "core/defines.h"
#include "core/types.h"

namespace en
{
   namespace threads
   {
   class Interface;
   class WorkerThread;

   // Status of spawned task
   // ( Task can be splitted to several sub-tasks that will be 
   //   processed simultaneously. In such situation they all 
   //   share one status structure that describes status of 
   //   logical task spawned by program. This also require that 
   //   this class will be aligned to cache line to be sure that 
   //   there won't be any false sharing of data betwen different 
   //   threads. Such situation could happen when cache 
   //   invalidation goes wrong. )
   class cachealign TaskState
         {
         private:
         // Indicates how many threads process this task. That 
         // also require this variable to be volatile 
         // (modificable by all threads).
         volatile uint32 busy; 

         public:
         forceinline TaskState();
   
         forceinline bool working(void) const; // Returns status of task
         void set(bool state);     // Sets status of task as seen by current thread
         };
      
   // Describes task to execute
   class Task
         {
         private:
         // Scheduler and worker threads can change task state.
         friend class Interface; 
         friend class WorkerThread;      

         // Current state of execution of task instance. It is 
         // assigned to task when it is pushed for execution. If 
         // task instance is already spawned, then it's copy is      // What to do if one instance is spawned several times?
         // created and spawned as new task. If task is spawned     // Maybe check if (m_state != NULL) don't allow another spawn.
         // as subtask it shares state with other subtasks.
         TaskState* state;  
   
         public:
         forceinline Task(TaskState* state); 
   
         virtual void work(void) = 0;      // Starts task execution
         forceinline TaskState* run(void); // Adds task to queue
         forceinline bool running(void);   // Returns current state of task
         };
   }
}








//extern class cachealign Nthread;


// Status of spawned task
// ( Task can be splitted to several subtasks
//   that will be processed simultaneously.
//   In such situation they all share one status
//   structure that describes status of logical
//   task spawned by programmer. 
//   This also require that this class will be
//   aligned to cache line to be sure that there
//   won't be any false sharing of data betwen 
//   different threads. Such situation could 
//   happen when cache invalidation goes wrong. )
class cachealign NTaskState
      {
      private:
      volatile uint32 m_busy; // Indicates how many threads 
                              // process this task. That also 
                              // require this variable to be 
                              // volatile (modificable by
                              // all threads).
      public:
      forceinline NTaskState();

      forceinline bool working(void) const; // Returns status of task
      void set(bool state);     // Sets status of task as seen
                                // by current thread
      };

forceinline NTaskState::NTaskState() : 
       m_busy(0)
{}

forceinline bool NTaskState::working(void) const
{
 return m_busy != 0;
}

// Describes task to execute
class NTask
      {
      private:
      friend class NSchedulerContext; // Both scheduler and
      friend class WorkerThread;      // it's worker threads
                                      // can change tasks
                                      // current state.

      NTaskState* m_state;  // Current state of this task
                            // execution. It is assigned
                            // to task when it is pushed
                            // for execution. If task is     // What to do if one instance is spawned several times?
                            // already spawned, then it's    // Maybe check if (m_state != NULL) don't allow another spawn.
                            // copy is created and spawned 
                            // as new task. If task is 
                            // spawned as subtask it shares
                            // state with other subtasks.

      public:
      forceinline NTask(NTaskState* state); 

      virtual void work(void) = 0;       // Starts task execution
      forceinline NTaskState* run(void); // Adds task to queue
      forceinline bool running(void);    // Returns current state of task
      };

forceinline NTask::NTask(NTaskState* state) :
       m_state(state)
{}

forceinline bool NTask::running(void)
{
 if (m_state == 0)
    return false;

 return m_state->working();
}

#endif