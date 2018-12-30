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
#include "parallel/task.h"

// TODO: Used by D3D12Device, replace with unified representation/query across whole system
#define MaxThreads 256   

#include <memory>   // std::unique_ptr

namespace en
{
constexpr uint32 InvalidWorkerId    = 0xFFFFFFFF;

constexpr uint32 FibersPerWorker    = 128;
constexpr uint32 MaxFibersPerWorker = 1024;
constexpr uint32 MaxTasksPerWorker  = 1024;

// TODO: In the future add ability to split tasks while they are processed
// typedef void(*TaskParallelFunction)(void* taskData, uint32v2 range);

// Values in range [0..N] indicate that given task needs to be executed
// on specified worker thread (and assigned to it CPU core) and Fiber 
// keeping it's state cannot migrate between cores during it's execution.
// This may be required for e.g. for rendering tasks, as GpuDevice is 
// tracking it's state per thread, and thus task shouldn't switch threads
// for e.g. in the middle of encoding Command Buffer.

namespace parallel
{
class Interface
{
    public:

    /// Creates this class instance and assigns it to "en::Scheduler"
    static bool create(
        const uint32 workers, 
        const uint32 workerFibers, 
        const uint32 maxWorkerTasks);
    
    virtual uint32 workers(void) const = 0;                // Worker Threads count
    
    virtual uint32 currentWorkerId(void) const = 0;   // Id of first worker thread. If current thread is not worker thread, it will return InvalidWorkerId.

    /// Task execution can migrate between CPU cores 
    /// (but once started currently will stay locked to that core, there is no Fibers migration)
    virtual void run(
        TaskFunction function,           ///< Task to execute
        void* data = nullptr,            ///< Data to be processed by task
        TaskState* state = nullptr) = 0; ///< State to use, so that caller can synchronize

    // TODO: Add:
    // runOnCurrentCore()  - Task will execute on the same CPU core as parent task
    // runOnSingleCore() - Task will execute on any core, but it cannot migrate once it's started

    virtual void runOnMainThread(
        TaskFunction function,           ///< Task to execute
        void* data = nullptr,            ///< Data to be processed by task
        TaskState* state = nullptr,      ///< State to use, so that caller can synchronize
        bool immediately = false) = 0;   ///< Indicates if this task should be processed immediately or pushed on queue (if main thread should be woken up or not)
    
    /// This method should be used only in rare cases, for e.g. when some system 
    /// needs to have internal state per CPU core or worker thread using it. This 
    /// method shouldn't be used for manual distribution of general tasks. 
    /// Scheduler will optimally distribute submitted tasks, so use run() method instead.
    virtual void runOnWorker(
        const uint32 worker,             ///< Task execution is locked to CPU core of given Worker thread
        TaskFunction function,           ///< Task to execute
        void* data = nullptr,            ///< Data to be processed by task
        TaskState* state = nullptr) = 0; ///< State to use, so that caller can synchronize



//  virtual void run(TaskParallelFunction function, // Task to execute
//                   void* taskData = nullptr,      // Data to be processed by task
//                   uint32v2 range,                // Task range, allows splitting task and execution in parallel
//                   TaskState* state = nullptr,    // State to use, so that caller can synchronize (optional)
//                   bool lockToCore = false) = 0;  // Specify if task execution can migrate between CPU cores

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

    virtual void shutdown(void) = 0;              // Send signal to terminate all workers and finish application
    virtual bool closing(void) const = 0;         // Returns true if Thread-Pool is shutting down

    virtual ~Interface() {};                       // Polymorphic deletes require a virtual base destructor
};

}

// Default instance of Task Scheduler
extern std::unique_ptr<parallel::Interface> Scheduler;
}
#endif
