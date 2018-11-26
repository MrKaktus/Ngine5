/*

 Ngine v5.0
 
 Module      : Task Scheduler.
 Requirements: none
 Description : Implemented using fibers in M:N mapping to worker threads.

*/

#ifndef ENG_PARALLEL_SCHEDULER
#define ENG_PARALLEL_SCHEDULER

#include "core/defines.h"
#include "core/types.h"
#include "core/memory/alignment.h"
#include "parallel/sharedAtomic.h"

// TODO: Used by D3D12Device, replace with unified representation/query across whole system
#define MaxThreads 256   

#include <memory>   // std::unique_ptr

namespace en
{
   constexpr uint32 FibersPerWorker    = 128;
   constexpr uint32 MaxFibersPerWorker = 1024;
   constexpr uint32 MaxTasksPerWorker  = 1024;

   // Task execution state is tracked by atomic counter, that represents amount
   // of worker threads that are processing this task (or have it in it's queue).
   // Counter is increased to 1, when task is passed for execution, and finally
   // decreased to 0 when task is finished.
   class TaskState : private SharedAtomic
      {
      public:
      TaskState();
      void acquire(void);  // Task is acquired by worker thread (scheduled for execution)
      void release(void);  // Task is released by worker thread (finished working on this task)
      bool finished(void); // Returns true, if given task is finished
      };

   static_assert(sizeof(TaskState) == 64, "en::TaskState size mismatch!");

   // Task is just a function that will be executed with any optional data
   // that it needs for processing. This data won't be copied (as task private
   // copy) while task is pushed for execution. Therefore ensure that any 
   // external data or state reffered by task is available until task is
   // processed, or pass data ownership to task itself (preffered method).
   // Task is responsible for releasing data passed to it.
   typedef void(*TaskFunction)(void* taskData);

   // TODO: In the future add ability to split tasks while they are processed
   // typedef void(*TaskParallelFunction)(void* taskData, uint32v2 range);

   // Values in range [0..N] indicate that given task needs to be executed
   // on specified worker thread (and assigned to it CPU core) and Fiber 
   // keeping it's state cannot migrate between cores during it's execution.
   // This may be required for e.g. for rendering tasks, as GpuDevice is 
   // tracking it's state per thread, and thus task shouldn't switch threads
   // for e.g. in the middle of encoding Command Buffer.
   // Otherwise - ThreadAny - indicates that task can be executed on any core,
   // and - ThreadMain - that it is a special task, that needs to be executed 
   // on Main thread.
   constexpr uint32 SingleThread = 0xFFFFFFFD;   // Needs to be executed on single thread (scheduler decides which one)
   constexpr uint32 MainThread   = 0xFFFFFFFE;   // Needs to be executed on main thread
   constexpr uint32 AnyThread    = 0xFFFFFFFF;   // Task execution can migrate between CPU cores

   namespace parallel
   {
   class Interface
      {
      public:

      // Creates this class instance and assigns it to "Scheduler"
      static bool create(const uint32 workers, const uint32 workerFibers, const uint32 maxWorkerTasks);

      virtual uint32 workers(void) const = 0;                // Worker Threads count

      virtual void run(TaskFunction function,                // Task to execute
                       void* data = nullptr,                 // Data to be processed by task
                       TaskState* state = nullptr) = 0;      // State to use, so that caller can synchronize
                     //const uint32 worker = AnyThread) = 0; // Specify if task execution can migrate between CPU cores

      // virtual void run(TaskParallelFunction function,        // Task to execute
      //                  void* taskData = nullptr,     // Data to be processed by task
      //                  uint32v2 range,               // Task range, allows splitting task and execution in parallel
      //                  TaskState* state = nullptr,   // State to use, so that caller can synchronize (optional)
      //                  bool lockToCore = false) = 0; // Specify if task execution can migrate between CPU cores



      // Warning:
      // Several tasks waiting for single task, are discouraged, as those tasks
      // won't know the order in which they wil be executed, after that task is
      // finished, and thus they won't know which one should release TaskState 
      // that was allocated when original task was passed for execution.
      // (Unless they use some atomic counter to track when last of them terminates).
      // 
      virtual void wait(TaskState* state) = 0;       // Waits until given task finishes

      virtual void processMainThreadTasks(void) = 0; // Will process all tasks that should be executed on main thread.
                                                     // Main thread should call it each time it processes events from OS.

      // TODO: Add support for handling I/O requests (wait & wake up)


       
      // virtual void       shutdown(void) = 0;              // Send signal to terminate all workers and finish application
      // virtual bool       closing(void) const = 0;         // Returns true if Thread-Pool is shutting down

      virtual ~Interface() {};                       // Polymorphic deletes require a virtual base destructor
      };
   }

   // Default instance of Task Scheduler
   extern std::unique_ptr<parallel::Interface> Scheduler;
}
#endif
