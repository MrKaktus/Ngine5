/*

 Ngine v5.0
 
 Module      : Thread-Pool Scheduler.
 Requirements: none
 Description : 

*/

#ifndef ENG_CORE_PARALLEL_WINDOWS_SCHEDULER
#define ENG_CORE_PARALLEL_WINDOWS_SCHEDULER

#include "parallel/scheduler.h"

#include "core/parallel/thread.h"
#include "core/parallel/fiber.h"

#include "core/utilities/poolAllocator.h"
#include "memory/circularQueue.h"
#include "memory/workStealingDeque.h"

namespace en
{
   // Container keeping all neccessary information about given task
   struct Task
      {
      TaskFunction function;        // Function to execute
      TaskState*   state;           // Task current state
      void*        data;            // Data to process
      uint64       localState : 1;  // If true, pointed state is local, and
                                    // needs to be released after task is finished
    //uint64       locked     : 1;  // Locked to current worker thread, migration is forbidden
      uint64                  : 63; // Padding to 32 bytes
      };

   static_assert(sizeof(Task) == 32, "en::Task size mismatch!");

   // Each worker has it's own pool of fibers and tasks to minimize amount of 
   // communication between threads (and CPU cores they are assigned to).
   struct Worker
      {
      std::unique_ptr<Thread>   thread; // Worker thread handle

    // TODO: Both Fibers and Tasks can migrate between worker threads.
    //       Thus thread-safe pool allocator implementation is required
    //       to ensure that they can be freed from other thread (as well
    //       as allocaed from other thread, when IO threads push tasks
    //       for execution from outside).
    //       At the same time Fiber is of unknown size (as it is platform
    //       dependent) and thus it's impossible to have pool allocator
    //       based on generic interface class. It would need to know backing
    //       type, and then scheduler class itself would be platform dependent.
    //       Thus it is better to keep Fibers as array of pointers to them.
    //
    //PoolAllocator<Fiber>      fibers; // Local pool of fibers (they may be exchanged with other workers)
    //PoolAllocator<Task>       tasks;  // Local pool of tasks
       


      // Tasks local to this worker thread

      std::unique_ptr<Fiber>*   localFibers;        // Pool of local Fibers 
      CircularQueue<Task*>      localTasks;         // Tasks that wait for execution on this worker thread
      CircularQueue<Fiber*>     localFibersWaiting; // Tasks that are waiting for other work to finish (or IO event)


      // Tasks that can be executed on any worker

      WorkStealingDeque<Task*>  queueOfTasks;        // Tasks ready and waiting to be executed
    //WorkStealingDeque<Fiber*> queueOfTasksStalled; // Tasks that are paused (as control was switched to resume execution of other task)
    //WorkStealingDeque<Fiber*> queueOfTasksWaiting; // Tasks that are waiting for other work to finish (or IO event)
  
      uint32 index;                     // Index of worker thread in the pool
      uint32 tasksCount;
      uint32 stalledTasksCount;

      Worker(const uint32 index);
     ~Worker();
      };

   static_assert(sizeof(Worker) == 128+16, "en::Worker size mismatch!");  // 248

   class TaskScheduler : public parallel::Interface
      {
      public:
      uint32 workerThreads;                // Count of threads in Thread-Pool
      uint32 firstWorkerId;                // Id of first worker thread in a pool 
                                           // (all workers have consecutive ID's).
      Worker* worker;                      // Array of worker thread states
      std::atomic<bool> executing;         // Synchronizes start of worker threads execution

      CircularQueue<Task*> mainThreadQueue; // Separate queue of tasks to execute by main thread
      PoolAllocator<Task>  mainThreadTasks;

      TaskScheduler(const uint32 workerThreads, const uint32 fibersPerWorker);

      virtual uint32 workers(void) const;

      virtual void run(TaskFunction function,            // Task to execute
                       void* data = nullptr,             // Data to be processed by task
                       TaskState* state = nullptr);      // State to use, so that caller can synchronize
                     //const uint32 worker = AnyThread); // Specify if task execution can migrate between CPU cores

      // virtual void run(TaskParallelFunction function,      // Task to execute
      //                  void* taskData = nullptr,   // Data to be processed by task
      //                  uint32v2 range,             // Task range, allows splitting task and execution in parallel
      //                  TaskState* state = nullptr, // State to use, so that caller can synchronize (optional)
      //                  bool lockToCore = false);   // Specify if task execution can migrate between CPU cores

      virtual void wait(TaskState* state);        // Waits until given task finishes

      virtual void processMainThreadTasks(void);  // Will process all tasks that should be executed on main thread.
                                                  // Main thread should call it each time it processes events from OS.

      virtual ~TaskScheduler();
      };
}
#endif