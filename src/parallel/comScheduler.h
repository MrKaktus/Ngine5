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
#include "core/memory/alignedAllocator.h"
#include "memory/circularQueue.h"
#include "memory/workStealingDeque.h"


// temp for MPSC
#include "threading/mutex.h"

namespace en
{


// TEMP Implementation !!!! DIRTY SHIT FIXME!!!
template<typename T>
class MPSCDeque
{
    private:
    Mutex                lockPush;  // Ensures producers won't collide, this is low frequency container so we don't care for now
    WorkStealingDeque<T> container; // Will be used in reverse order so that consumer steals wihout locking

    public:
    // First element starting address is always aligned to page size (4KB).
    // When deque is growing, it's size is doubling until reaching doubleSizeUntil
    // size in bytes. After that, it always grows by that size, until reaching
    // maxCapacity.
    MPSCDeque(const uint32 capacity,                               // In entries
              const uint32 maxCapacity,                            // In entries
              const uint32 doubleSizeUntil = PoolDoublingBarrier); // In bytes

    DequeResult take(T& value);   // Called only by consumer thread
    void        push(T value);    // Called by producer threads
};

// CompileTimeSizeReporting( MPSCDeque<void*> );
static_assert(sizeof(MPSCDeque<void*>) == 128, "en::MPSCDeque<void*> size mismatch!");

template<typename T>
MPSCDeque<T>::MPSCDeque(const uint32 capacity,                     // In entries
                        const uint32 maxCapacity,                  // In entries
                        const uint32 doubleSizeUntil) :
    lockPush(),
    container(capacity, maxCapacity, doubleSizeUntil)
{
}

template<typename T>
void MPSCDeque<T>::push(T value)
{
    // Pushing elements to deque by multiple producers is guarded with mutex
    // critical section. Thats fine because threads pushing tasks will be IO
    // threads so they can be stalled for a moment in case of concurrent access.
    // It's more important that Worker thread consuming those elements won't ever
    // block.
    lockPush.lock();
    container.push(value);
    lockPush.unlock();
}

template<typename T>
DequeResult MPSCDeque<T>::take(T& value)
{
    // Non-blocking try to take something from this queue
    return container.steal(value);
}
// TEMP Implementation !!!! DIRTY SHIT FIXME!!!





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
    // Tasks local to this worker thread
   
    MPSCDeque<Task*>          queueOfIncomingTasks;      ///< Tasks submitted for execution by IO threads
    MPSCDeque<Task*>          queueOfIncomingLocalTasks; ///< Tasks submitted for execution on this worker CPU core (locked)

    // Tasks that can be executed on any worker

    WorkStealingDeque<Task*>  queueOfTasks;        ///< Tasks ready and waiting to be executed

    // Fibers that can be executed on any worker (migration of tasks in progress)

  //WorkStealingDeque<Fiber*> queueOfTasksStalled; ///< Tasks that are paused (as control was switched to resume execution of other task)
  //WorkStealingDeque<Fiber*> queueOfTasksWaiting; ///< Tasks that are waiting for other work to finish (or IO event)
  
    // Fibers local to this worker thread
   
    // [WWWWWWWWW][E][RRRRRRRRRRRRRRR]
    // W - Waiting Fibers
    // E - Currently executing Fiber (starts to wait, resumes, or picks new task)
    // R - Ready Fibers, waiting
    //
    Fiber** localFibers;
  //std::unique_ptr<Fiber>*   localFibers;        ///< Pool of local Fibers (array of pointers to platform dependent objects)
    uint32                    currentFiber;       ///< Pointer to fiber currently executing on this worker thread
    uint32                    fibers;             ///< Count of fibers managed by this worker thread
      
      
    std::unique_ptr<Thread>   thread;      // Worker thread handle
    std::atomic<bool>         sleeping;    // Indicates if given worker thread is sleeping
      
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

    uint32 index;                     // Index of worker thread in the pool

    Worker(const uint32 index, const uint32 fibers);
   ~Worker();
};

// CompileTimeSizeReporting( Worker );
static_assert(sizeof(Worker) == 384, "en::Worker size mismatch!");  // 336 padded to 384

class TaskScheduler : public parallel::Interface
{
    public:
    uint32 workerThreads;                // Count of threads in Thread-Pool
    uint32 firstWorkerId;                // Id of first worker thread in a pool
                                         // (all workers have consecutive ID's).
    Worker** worker;                     // Array of worker thread states
  //std::unique_ptr<Worker>* worker;
    std::atomic<bool> executing;         // Synchronizes start of worker threads execution
    std::atomic<bool> appQuit;           // Signals to main thread that application finished teardown on it's side

    // Tasks submitted for execution by IO threads
    MPSCDeque<Task*>    queueOfMainThreadTasks; // Separate queue of tasks for execution on main thread

    //CircularQueue<Task*> mainThreadQueue; // Separate queue of tasks to execute by main thread
    PoolAllocator<Task>  mainThreadTasks;



    TaskScheduler(const uint32 workerThreads, const uint32 fibersPerWorker);

    virtual uint32 workers(void) const;

    virtual uint32 currentWorkerId(void) const;   // Id of first worker thread

    virtual void run(TaskFunction function,            // Task to execute
                     void* data = nullptr,             // Data to be processed by task
                     TaskState* state = nullptr);      // State to use, so that caller can synchronize
                                                       // Task execution can migrate between CPU cores

    virtual void runOnMainThread(TaskFunction function,      // Task to execute
                                 void* data = nullptr,       // Data to be processed by task
                                 TaskState* state = nullptr, // State to use, so that caller can synchronize
                                 bool immediately = false);  // Indicates if this task should be processed immediately or pushed on queue (if main thread should be woken up or not)

    virtual void runOnWorker(const uint32 worker,         // Task execution is locked to CPU core of given Worker thread
                             TaskFunction function,       // Task to execute
                             void* data = nullptr,        // Data to be processed by task
                             TaskState* state = nullptr); // State to use, so that caller can synchronize

  //virtual void run(TaskParallelFunction function, // Task to execute
  //                 void* taskData = nullptr,      // Data to be processed by task
  //                 uint32v2 range,                // Task range, allows splitting task and execution in parallel
  //                 TaskState* state = nullptr,    // State to use, so that caller can synchronize (optional)
  //                 bool lockToCore = false);      // Specify if task execution can migrate between CPU cores

    virtual void wait(TaskState* state);        // Waits until given task finishes

  //void resumeStalledWorkers(const uint32 thisWorker); // Wakes worker thread, if one of its fibers can be resumed

    virtual void processMainThreadTasks(void);  // Will process all tasks that should be executed on main thread.
                                                // Main thread should call it each time it processes events from OS.
    virtual void shutdown(void);                // Send signal to terminate all workers and finish application
    virtual bool closing(void) const;           // Returns true if Thread-Pool is shutting down

    void* operator new(size_t size)
    {
        // New and delete are overloaded to make sure that Mutex is always 
        // aligned at proper adress. This also allows application to use 
        // std::unique_ptr() to manage object lifecycle, without growing 
        // unique pointer size (as there is no custom deleter needed).
        return en::allocate<TaskScheduler>(1, cacheline);
    }

    void operator delete(void* pointer)
    {
        en::deallocate<TaskScheduler>(static_cast<TaskScheduler*>(pointer));
    }

    virtual ~TaskScheduler();
};

}
#endif
