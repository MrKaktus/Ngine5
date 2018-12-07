/*

 Ngine v5.0
 
 Module      : Threads support.
 Requirements: none
 Description : Allows easy creation and management of threads.

*/

#include "core/log/log.h"
#include "platform/system.h"
#include "scheduler.h"

#include "utilities/random.h"

#include "core/parallel/thread.h"


#include "core/utilities/memory.h"    // allocate, TODO: move to core/memory

namespace en
{
   // Other implementations of fiber based task managers:
   //
   // NaughtyDog simple system:
   // https://www.gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine
   //
   // Google "Filament" 
   // https://github.com/google/filament/blob/master/libs/utils/src/JobSystem.cpp
   //
   // Facebook "Folly" 
   // https://github.com/facebook/folly/tree/master/folly/fibers
   //
   // General rules for Task schedulers:
   //
   // 1) IO Threads need to have ability to push Task to Scheduler, even though
   //    they are not part of Thread-Pool themselves. The same refers to Main thread.
   // 2) Only IO threads are allowed to block/sleep. Tasks executed in Thread
   //    Pool are forbidden from any kind of blocking, and should pass such work
   //    to IO threads (that will spawn new task to handle results once they are
   //    available).


   // For fiber-task sync points:
   //
   // - start execution
   // - run other task
   // - wait for other task
   // - end execution

   // TODO: Check if task local state is needed, or can it be completly skipped.


   TaskState::TaskState() :
      SharedAtomic()
   {
   }
   
   void TaskState::acquire(void)
   {
   // Performs atomic pre-increment. Equivalent to fetch_add(1) + 1
   value++;
   }

   void TaskState::release(void)
   {
   //  Performs atomic pre-decrement. Equivalent to fetch_sub(1) - 1
   value--;
   }

   bool TaskState::finished(void)
   {
   uint32 currentValue = std::atomic_load_explicit(&value, std::memory_order_relaxed);
   if (currentValue == 0)
      return true;

   return false;
   }


    // Select 
    //switchToFiber(*workerState.currentFiber, workerState.localFibers[fiberIndex]);

// Scheduler can be entered in two cases:
// A - current fiber calls wait
// B - current task execution finished (or no task was selected yet by the fiber)
//
// Scheduler exits in one of two ways:
// A - pick new task and call its function (start execution)
// B - resume one of waiting fibers (switch to that fiber, then exit scheduler 
//     function to resume execution on exit from wait() call).
//

// Fiber states:
// - executing code (entered through task function call)
// - waiting for other task (called wait() and switched to other fiber)
// - resuming from wait (was switched to, should resume exiting wait())
// - not executing (finished task, or didnt picked one yet)
//


// wait -> resume
// resume -> execute
// -> exit -> select
// wait -> select -> execute

//     _____________________________
//    /                             \
//  [Select] -> [Execute] -> [Exit]-/
//    |    ^     |   ^
//    |    |     v   |
//    |   [ Wait ]   |
//    |              |
//    \----->[ Resume ]
//

// Resume -> Switches Fibers
// Wait   -> Switches Fibers



// Single executing Fiber
// List of ready Fibers
// List of waitings Fibers

// Fiber can move from Waiting list to Ready list:
// - finished task
// - there is fiber waiting that can be resumed
// - move self to Ready list
// - switch to Waiting fiber 
// - waiting fiber decides to resume (it savet its own state before switching on wait)


   constexpr uint32 FiberStackSize        = 65536;   // 64KB
   constexpr uint32 MaxFiberStackSize     = 1048576; // 1MB
   constexpr uint32 WorkerThreadFibers    = 128;
   constexpr uint32 WorkerThreadTasks     = 256;
   constexpr uint32 MaxWorkerThreadFibers = 256;
   constexpr uint32 MaxWorkerThreadTasks  = 1024;
   constexpr uint32 MainThreadTasks       = 64;
   constexpr uint32 MaxMainThreadTasks    = 256;

void* schedulingFunction(TaskScheduler& scheduler, uint32 thisWorker);

void schedulerFiber(void* data)
{
    uint32 thisWorker = *(uint32*)(data);

    schedulingFunction(*(TaskScheduler*)(en::Scheduler.get()), thisWorker);
}

void* workerFunction(Thread* thread)
{
    TaskScheduler& scheduler = *(TaskScheduler*)(thread->state());
    
    // Wait until scheduler finished initialization of all threads
    bool executing = false;
    while(executing == false) 
    {
        _mm_pause();
        executing = std::atomic_load_explicit(&scheduler.executing, std::memory_order_relaxed);
    }
    
    // Determine current worker thread
    uint32 threadId   = currentThreadId();
    uint32 thisWorker = threadId - scheduler.firstWorkerId;
    
    // Acquire handle to worker state
    Worker& workerState = *(Worker*)( &scheduler.worker[thisWorker] );
    
    // Finish initialization of worker thread state, by converting it to fiber
    workerState.localFibers[0] = convertToFiber();
    
    // Start executing tasks
    schedulingFunction(scheduler, thisWorker);
    
    // TODO: Worker thread cleanup
    
    // Worker thread function is not returning anything
    return nullptr;
}

   Worker::Worker(const uint32 _index) :
      thread(nullptr),
      localFibers(nullptr),
    //localTasks(MaxWorkerThreadTasks),
    //localFibersWaiting(MaxWorkerThreadFibers),
      waitingFibersCount(0),
      currentFiber(0),
      queueOfIncomingTasks(WorkerThreadTasks, MaxWorkerThreadTasks),
      queueOfIncomingLocalTasks(WorkerThreadTasks, MaxWorkerThreadTasks),
      queueOfTasks(WorkerThreadTasks, MaxWorkerThreadTasks),       
    //queueOfTasksStalled(WorkerThreadFibers, MaxWorkerThreadFibers),
    //queueOfTasksWaiting(WorkerThreadFibers, MaxWorkerThreadFibers),
      index(_index),
      tasksCount(0),
      stalledTasksCount(0)
   {
   // Allocate pool of fibers 
   // (all except of fiber 0 which will be init by converting worker thread to it)
   // Pointer to Worker thread local state is directly passed to fibers. That is
   // fine as the moment worker thread will terminate, all fibers should terminate
   // already anyway (in fact any fiber terminating will terminate worker thread).
   localFibers = new std::unique_ptr<Fiber>[MaxWorkerThreadFibers];
   for(uint32 i=1; i<MaxWorkerThreadFibers; ++i)
      localFibers[i] = createFiber(schedulerFiber, (void*)&index, FiberStackSize, MaxFiberStackSize);
   }

   Worker::~Worker()
   {
   // Release worker thread fibers
   for(uint32 i=0; i<MaxWorkerThreadFibers; ++i)
      localFibers[i] = nullptr;

   // Release fibers pool
   delete [] localFibers;

   // Release thread object
   thread = nullptr;
   }

   TaskScheduler::TaskScheduler(const uint32 _workerThreads, const uint32 fibersPerWorker) :
      workerThreads(_workerThreads),
      firstWorkerId(0),
      worker(nullptr),
      executing(false),
      queueOfMainThreadTasks(MainThreadTasks, MaxMainThreadTasks),
      //mainThreadQueue(MaxMainThreadTasks),
      mainThreadTasks(MainThreadTasks, MaxMainThreadTasks)
   {
   // Allocate per worker thread states
   worker = allocate<Worker>(workerThreads, cacheline);

   // Spawn worker threads (they will be spinning until execution flag is not set)
   for(uint32 i=0; i<workerThreads; ++i)
      {
      // Init worker state
      new (&worker[i]) Worker(i);
      worker[i].thread = startThread(workerFunction, static_cast<void*>(this));

      // Store ID of first worker thread. Implementation requires that all worker
      // threads have ID's that are part of consecutive range, so that they can
      // be easily used to index internal arrays of per thread states (after 
      // subtracting first thread ID).
      if (i == 0)
         firstWorkerId = worker[i].thread->id();
      else
         {
         assert( worker[i].thread->id() == firstWorkerId + i );
         }

      // Each worker thread is assigned to separate CPU core (physical or logical
      // depending on if Hyper-Threading cores are used) and cannot migrate between
      // those CPU cores.
      uint64 executionMask = 0;
      setBit(executionMask, static_cast<uint64>(i));
      worker[i].thread->executeOn(executionMask);
      }

   // Start worker threads execution
   std::atomic_store_explicit(&executing, true, std::memory_order_relaxed);
   }

   TaskScheduler::~TaskScheduler()
   {
   // Notify all worker threads that they should terminate. As this termination 
   // is performed in Scheduler destructor, it means that application terminates
   // immediatly. From this moment, workers won't process any new job, but 
   // terminate as soon as current task ends (or calls wait).
   //
   // TODO: Is above scenario even possible? If we've reached destructor, it
   //       means that main thread events loop was ended. By that time all tasks
   //       should be drained properly.
   //
   std::atomic_store_explicit(&executing, false, std::memory_order_relaxed);

   // Wait until all worker threads are done
   for(uint32 i=0; i<workerThreads; ++i)
      worker[i].thread->waitUntilCompleted();

   // Release worker states
   for(uint32 i=0; i<workerThreads; ++i)
      worker[i].~Worker();
   
   deallocate<Worker>(worker);
   }

uint32 TaskScheduler::workers(void) const
{
    return workerThreads;
}

uint32 TaskScheduler::currentWorkerId(void) const
{
    uint32 threadId = currentThreadId();
    
    // Check for special case when Task is being added by external thread not 
    // being part of Thread-Pool (for e.g. main thread or IO thread pushes task 
    // to handle incoming event).
    if (threadId < firstWorkerId || (firstWorkerId + workerThreads - 1) < threadId)
    {
        return InvalidWorkerId;
    }

    return threadId - firstWorkerId;
}


/* TODO:

   // Task is executed on main thread (can migrate between CPU cores)
   void TaskScheduler::runOnMainThread(TaskFunction function,
                                       void* data,
                                       TaskState* state)
   {
   // TODO: Multiple-Producers Single Consumer queue implementation is required,
   //       or simple queue protected with mutex. Those calls should be rare,
   //       and only main thread processes pushed tasks which cannot be stolen.
   }

   // Task is executed on worker thread assigned to given CPU core (cannot migrate between CPU cores)
   void TaskScheduler::runOnCore(const uint16 core,
                                 TaskFunction function,
                                 void* data,
                                 TaskState* state)
   {
   // TODO: Multiple-Producers Single Consumer queue implementation is required,
   //       or simple queue protected with mutex. Those calls should be rare,
   //       and only main thread processes pushed tasks which cannot be stolen.
   }

//*/


   // Very good description of different types of Producer-Consumer Queues:
   // http://www.1024cores.net/home/lock-free-algorithms/queues

void TaskScheduler::run(TaskFunction function,
                        void* data,
                        TaskState* state)
{
    // Allocate task ( TODO: from a pool? )
 //Task* task = worker[thisWorker].tasks.allocate(); 
    Task* task = new Task();
    
    // Init task state
    task->function   = function;
    task->data       = data;
    task->state      = state;
    task->localState = false;
    if (!task->state)
    {
        // When dynamically allocated, ensure that it is aligned to cache line
        task->state = allocate<TaskState>(1, cacheline);
        new (task->state) TaskState();
        task->localState = true;
    }
    
    task->state->acquire();

    uint32 threadId = currentThreadId();
    
    // Check for special case when Task is being added by external thread not 
    // being part of Thread-Pool (for e.g. main thread or IO thread pushes task 
    // to handle incoming event).
    if (threadId < firstWorkerId || (firstWorkerId + workerThreads - 1) < threadId)
    {
        // Add task to worker thread, that executes on current CPU core. This way
        // we're guaranteed that this worker thread execution is paused. It could 
        // be paused when it was executing task, or in rare case, when it was in
        // the middle of pushing (or grabing) task from queue.
        uint32 selectedWorker = firstWorkerId + currentCoreId();
        
        // If there is less worker threads than CPU cores (for e.g. Scheduler is
        // reduced to one worker thread for debugging purposes), push this task 
        // on first worker thread. This is fine, as long as MPSC queue is used
        // to accept external tasks.
        if (selectedWorker >= workerThreads)
        {
            selectedWorker = 0;
        }

        // Due to above, tasks from IO threads are always pushed on separate MPSC
        // queue, that needs to be at least non-blocking for worker thread that is
        // it's consumer. 
      
        // Queue task for execution
        worker[selectedWorker].queueOfIncomingTasks.push(task);
        
        // Selected worker thread may be sleeping, in such case wake it up
        if (worker[selectedWorker].thread->sleeping())
        {
            worker[selectedWorker].thread->wakeUp();
        }
        
        return;
    }
    
    uint32 thisWorker = threadId - firstWorkerId;
        
    // Queue task for execution
    worker[thisWorker].queueOfTasks.push(task);
}

void TaskScheduler::runOnMainThread(TaskFunction function,
                                    void* data,
                                    TaskState* state)
{
    // Allocator is not thread safe
    Mutex lock;
    lock.lock();
    Task* task = mainThreadTasks.allocate();
    assert( task );
    lock.unlock();
    
    // Init task state
    task->function   = function;
    task->data       = data;
    task->state      = state;
    task->localState = false;
    if (!task->state)
    {
        // When dynamically allocated, ensure that it is aligned to cache line
        task->state = allocate<TaskState>(1, cacheline);
        new (task->state) TaskState();
        task->localState = true;
    }
    
    task->state->acquire();

   // Queue task for execution on main thread
    queueOfMainThreadTasks.push(task);
}

void TaskScheduler::runOnWorker(const uint32 selectedWorker,             
                                TaskFunction function,           
                                void* data,            
                                TaskState* state) 
{
    assert( selectedWorker < workerThreads );

    // Allocate task ( TODO: from a pool? )
    Task* task = new Task();
    
    // Init task state
    task->function   = function;
    task->data       = data;
    task->state      = state;
    task->localState = false;
    if (!task->state)
       {
       // When dynamically allocated, ensure that it is aligned to cache line
       task->state = allocate<TaskState>(1, cacheline);
       new (task->state) TaskState();
       task->localState = true;
       }
    
    task->state->acquire();
    
    // Queue task for execution on given worker thread
    worker[selectedWorker].queueOfIncomingLocalTasks.push(task);
    
    // Selected worker thread may be sleeping, in such case wake it up
    if (worker[selectedWorker].thread->sleeping())
    {
        worker[selectedWorker].thread->wakeUp();
    }
    
    return;
}

void TaskScheduler::wait(TaskState* state)
{
    // Current implementation assumes that Fibers never migrate between Workers
    // (so idle worker won't be able to steal waiting fiber that could be resumed).
    // Above assumption is enough to ensure that management of fibers list is
    // thread safe, as this list will always be modified by single fiber of that
    // worker at a time. Other workers can iterate over that list and check 
    // fiber states only when given worker is sleeping (to wake it up).
    assert( state );
    
    uint32 threadId = currentThreadId();
    
    // Check for special case when external thread, that is not part of Thread-Pool 
    // tries to wait on Task (for e.g. main thread or IO thread). 
    assert( threadId >= firstWorkerId && threadId < (firstWorkerId + workerThreads) );

    uint32 thisWorker = threadId - firstWorkerId;

    Worker& workerState = worker[thisWorker];

    Fiber* fiber = workerState.localFibers[workerState.currentFiber].get();

    // This fiber is now waiting for some task to finish
    fiber->waitingForTask = state;

    // Move this fiber from Executing state to Waiting state, by moving Current
    // marker to first Ready fiber. This worker thread will try to switch to
    // newly selected current fiber, unless it will find other Waiting fiber to
    // resume (or untill all fibers are now in Waiting state).
    workerState.currentFiber++;

    // Execute scheduling loop, until task scheduler terminates
    std::atomic<bool> isExecuting = std::atomic_load_explicit(&executing, std::memory_order_relaxed);
    while(isExecuting)
    {
        // This loop executes in one of Waiting fibers, but Current marker is 
        // already pointing on first Ready fiber.

        // Iterate over waiting fibers, if any of them can be resumed do that
        for(uint32 i=0; i<workerState.currentFiber; ++i)
        {
            Fiber* waitingFiber = workerState.localFibers[i].get();
            assert( waitingFiber->waitingForTask );
        
            if (waitingFiber->waitingForTask->finished())
            {
                // If selected fiber is somewhere inside of sub-group of Waiting 
                // fibers, switch it with last one on the list (one before current).
                if (i < (workerState.currentFiber - 1))
                {
                    workerState.localFibers[workerState.currentFiber - 1].swap(workerState.localFibers[i]);
                }
        
                // Move selected fiber from Waiting state to Executing, by 
                // decreasing count of fibers that are in Waiting state
                workerState.currentFiber--;
        
                // Resume selected fiber
                switchToFiber(*fiber, *waitingFiber);
        
                // <===================== PAUSE / RESUME ==========================>
        
                // This fiber is just resumed. It means that it was moved from 
                // Waiting state to Executing state and task state that it was
                // waiting for is finished.
                assert( fiber->waitingForTask->finished() );
        
                fiber->waitingForTask = nullptr;
        
                // Resume execution
                return;
            }
        }
        
        // All waiting fibers of this worker thread, are waiting for some work to 
        // finish, that is executing on other worker threads. If there are available
        // fibers in Ready state, switch to one, to let it execute other task in
        // the meantime.
        if (workerState.currentFiber < MaxWorkerThreadFibers)
        {
            Fiber* readyFiber = workerState.localFibers[workerState.currentFiber].get();

            // Resume fiber ready to pick new task
            switchToFiber(*fiber, *readyFiber);

            // <===================== PAUSE / RESUME ==========================>

            // This fiber is just resumed. It means that it was moved from 
            // Waiting state to Executing state and task state that it was
            // waiting for is finished.
            assert( fiber->waitingForTask->finished() );
        
            fiber->waitingForTask = nullptr;
        
            // Resume execution
            return;
        }
        else
        {
            // All fibers of this worker thread are blocked waiting for task
            // that is executing on other worker thread (blocked directly or
            // indirectly one on another). This worker thread will go to sleep, 
            // until it will be woken up by other thread (once one of blocking 
            // tasks will finish, unblocking one of the fibers).
            workerState.thread->sleep();

            // <===================== SLEEP / WAKEUP ==========================>

            // This worker thread was just woken up. This mean that one of fibers
            // in Waiting state is unblocked, and can resume execution. Check if 
            // its not current fiber.
            if (fiber->waitingForTask->finished())
            {
                fiber->waitingForTask = nullptr;
        
                // Move current fiber from Waiting state to Executing, by decreasing
                // count of fibers that are in Waiting state.
                workerState.currentFiber--;

                // Resume execution
                return;
            }

            // Worker thread is resumed, so there is some fiber to wake up, but 
            // it's not this one. Loop will repeat to find that fiber.
        }
        
        isExecuting = std::atomic_load_explicit(&executing, std::memory_order_relaxed);
    }

    // If code execution reached this point, it means that Scheduler terminates
    // immediately. All waiting tasks are trashed, and thread will terminate 
    // immediately.

    // TODO: Terminate worker thread and all active fibers
}

// Core scheduler function. Executes between Tasks and decides which Fiber
// should execute next, which Task to execute and how worker thread should
// behave (for e.g. go to sleep).
void* schedulingFunction(TaskScheduler& scheduler, uint32 thisWorker)
{
    // Acquire handle to worker state
    Worker* workerState = &scheduler.worker[thisWorker];

    // Verify that thread executes on expected CPU core
    assert( currentCoreId() == workerState->index );

    // Execute main scheduling loop, until task scheduler terminates
    std::atomic<bool> executing = std::atomic_load_explicit(&scheduler.executing, std::memory_order_relaxed);
    while(executing)
    {
        // Iterate over waiting fibers, if any of them can be resumed do that
        for(uint32 i=0; i<workerState->currentFiber; ++i)
        {
            Fiber* waitingFiber = workerState->localFibers[i].get();
            assert( waitingFiber->waitingForTask );
        
            if (waitingFiber->waitingForTask->finished())
            {
                Fiber* fiber = workerState->localFibers[workerState->currentFiber].get();

                // If selected fiber is somewhere inside of sub-group of Waiting 
                // fibers, switch it with last one on the list (one before current).
                if (i < (workerState->currentFiber - 1))
                {
                    workerState->localFibers[workerState->currentFiber - 1].swap(workerState->localFibers[i]);
                }
        
                // Move selected fiber from Waiting state to Executing, by 
                // decreasing count of fibers that are in Waiting state
                workerState->currentFiber--;
        
                // Resume selected fiber
                switchToFiber(*fiber, *waitingFiber);
        
                // <===================== PAUSE / RESUME ==========================>
        
                // This fiber is just resumed. It means that other fiber moved to
                // Waiting state, and couldn't resume any other fiber from waiting
                // list. Thus it switched to this one, to try to execute some new 
                // task in parallel.
                assert( fiber->waitingForTask == nullptr );
        
                // Break iterating over waiting fibers as we already know there
                // is none to resume. Try to run new task instead.
                break;
            }
        }

        // Select task to execute

        Task* task = nullptr;
        DequeResult result = DequeResult::Abort;

        // A) Check if there are any tasks generated by IO threads
        while(result == DequeResult::Abort)
        {
            result = workerState->queueOfIncomingTasks.take(task);
        }

        // B) Check if there are any tasks, that need to execute on this CPU core 
        if (!task)
        {
            result = DequeResult::Abort;
            while(result == DequeResult::Abort)
            {
                result = workerState->queueOfIncomingLocalTasks.take(task);
            }
        }

        // C) Check if there are any tasks waiting in a local queue
        //    (generated on this thread, not started yet, can be stolen by other workers)
        if (!task)
        {
            result = DequeResult::Abort;
            while(result == DequeResult::Abort)
            {
                result = workerState->queueOfTasks.take(task);
            }
        }

        // D) Check if other workers have paused tasks that can be resumed
        //    (resume other worker paused Fiber on this worker)
        //
        // TODO: This requires Fibers stealing, so they need to be randomly 
        //       accessible and available for pick-up by multiple-threads in 
        //       random order. 
        //       Single-Producer Multiple-Consumers container is required.
        //       This complicates system a lot, so for now Fibers cannot be 
        //       stolen and are kept local to given worker.
        
        // E) Try to steal task from other randomly selected worker
        //    (task generated by other worker, not started yet)
        if (!task)
        {
            // Iterates over all workers starting from randomly selected one, 
            // trying to steal work from any of them.
            uint32 startIndex = random(scheduler.workerThreads);
            for(uint32 i=0; i<scheduler.workerThreads; ++i)
            {
                uint32 workerId = (startIndex + i) % scheduler.workerThreads;
                if (workerId != thisWorker)
                {
                    // If stealing won't succeed first time, try again (unless queue is empty)
                    result = DequeResult::Abort;
                    while(result == DequeResult::Abort)
                    {
                        result = scheduler.worker[workerId].queueOfTasks.steal(task);
                    }
                }
            }
        }
        
        // Execute task 
        if (task && result == DequeResult::Success)
        {
            // Fiber executes task 
            // (it may be paused during that process, and thread can switch to other one)
            task->function(task->data);

            // Fiber exits from task function

            // Indicate that this thread finished executing task (task state may be 
            // shared by several tasks, to easily wait for all of them to finish, or 
            // in future, to allow task splitting for parallel execution).
            task->state->release();
            
// TODO: Is this section really thread-safe?
//       Use mutex on each sleeping thread while traversing it's Tasks list!

            // If there are tasks waiting for this task, they now point to task 
            // state that is finished (assuming it is, when shared). If their 
            // parent worker thread is sleeping, this thread needs to wake it.
            for(uint32 i=0; i<scheduler.workerThreads; ++i)
            {
                if (i != thisWorker)
                {
                    if (scheduler.worker[i].thread->sleeping())
                    {
                        // TODO: What if other thread will wake it up in the middle of searching this list?

                        // Check if this worker has fibers that could be resumed
                        // and thus, if there is a point of waking it up.
                        for(uint32 f=0; f<scheduler.worker[i].currentFiber; ++f)
                        {
                            if (scheduler.worker[i].localFibers[f]->waitingForTask->finished())
                            {
                                // Wake this thread up and proceed to the next one
                                scheduler.worker[i].thread->wakeUp();
                                break;
                            }
                        }
                    }
                }
            }

// TODO: Is this section really thread-safe?

            // Release task local state (nobody waited on it)
            if (task->localState)
            {
                deallocate<TaskState>(task->state);
            }
         
            // Release task container (this doesn't release task state)
            delete task;
            
            // Determine worker thread on which fiber finished execution
            // (fiber could have migrated between workers during task execution)
            thisWorker = currentThreadId() - scheduler.firstWorkerId;
            workerState = &scheduler.worker[thisWorker];
        }
        else // Worker is idle waiting for work (or for it's fibers to be resumed)
        {
            // If work cannot be stolen from any worker, it means that all workers
            // are executing last task or sleeping waiting for more tasks, or this
            // worker just executed last task in the system. At this point, if system
            // is not terminating, thread goes to sleep, until it will be woken up
            // to process more work (from IO threads or main thread), or shutdown.
            
            // Check if task scheduler is still executing
            executing = std::atomic_load_explicit(&scheduler.executing, std::memory_order_relaxed);
            if (executing)
            {
                workerState->thread->sleep();
            }
        }

        // Check if task scheduler is still executing
        executing = std::atomic_load_explicit(&scheduler.executing, std::memory_order_relaxed);
    }

    // TODO: Worker thread is terminating, cleanup



    // Fiber selects new task to execute


    return nullptr;
}


















    // Select fiber to switch to (if one of wating ones can be resumed) or pick
    // other task from the queue of waiting ones. If there are no other fibers, 
    // nor tasks to switch to, go to sleep. 
    // After each task is finished, it's worker thread will check if there are 
    // threads that are sleeping. If those threads have currentFiber->waitingForTask
    // pointer set to task state of just finished task, that they can be woken up.



    // TODO: In future, add separate queue, on which core-locked tasks will be 
    //       stored for execution. Those tasks can be added by any thread, and 
    //       thus, such queue need to be of type MPSC.
    //       Worker thread owning given queue, needs to then be able to check 
    //       if there are tasks on it, in lock-free manner, so that whole task
    //       selection process is lock-free when that queue is empty. If there 
    //       are tasks on MPSC queue, then worker may lock when pooling them
    //       as it will be a rare case.




void TaskScheduler::processMainThreadTasks(void)
{
    Task* task = nullptr;
    DequeResult result = queueOfMainThreadTasks.take(task);
    while(result != DequeResult::Empty)
    { 
        if (result == DequeResult::Success)
        {
            // Execute task
            task->function(task->data);
            
            // Mark task as done
            task->state->release();
            
            // Release completed task
            if (task->localState)
            {
                deallocate<TaskState>(task->state);
            }
            else
            {
                // There may be worker threds waiting for this task to finish

// TODO: Is this section really thread-safe?
//       Use mutex on each sleeping thread while traversing it's Tasks list!

                // If there are tasks waiting for this task, they now point to task 
                // state that is finished (assuming it is, when shared). If their 
                // parent worker thread is sleeping, this thread needs to wake it.
                for(uint32 i=0; i<workerThreads; ++i)
                {
                    if (worker[i].thread->sleeping())
                    {
                        // TODO: What if other thread will wake it up in the middle of searching this list?
                    
                        // Check if this worker has fibers that could be resumed
                        // and thus, if there is a point of waking it up.
                        for(uint32 f=0; f<worker[i].currentFiber; ++f)
                        {
                            if (worker[i].localFibers[f]->waitingForTask->finished())
                            {
                                // Wake this thread up and proceed to the next one
                                worker[i].thread->wakeUp();
                                break;
                            }
                        }
                    }
                }
                
// TODO: Is this section really thread-safe?
            }
            
            // Task is raw data so doesn't need to call explicitly destructor
            mainThreadTasks.deallocate(*task);
        }

        // Try to query next task from the queue
        result = queueOfMainThreadTasks.take(task);
    }
}










   /* Determining if this is worker thread and which: 

   uint32 threadId = currentThreadId();

   bool workerThread = false;
   if ((threadId >= firstWorkerId) && (threadId < firstWorkerId + workerThreads))
      workerThread = true;

   uint32 workerIndex = threadId - firstWorkerId;
   */








   namespace parallel
   {

   bool Interface::create(const uint32 workers, const uint32 workerFibers, const uint32 maxWorkerTasks)
   {
   if (Scheduler)
      return true;

   Log << "Starting module: Thread-Pool Scheduler.\n";

   Scheduler = std::make_unique<TaskScheduler>(workers, workerFibers);

   return (Scheduler == nullptr) ? false : true;
   }

   }

   std::unique_ptr<parallel::Interface> Scheduler;
}





/*


// worker.cpp
// (engine internal, platform independent)

#define FibersPerThread    64
#define MaxFibersPerThread 128

namespace en
{
   WorkerState::WorkerState(const uint16 queueMaxSize) :
      readyTasks(queueMaxSize, cacheline),
      waitingTasks(queueMaxSize, cacheline),
      fibers(FibersPerThread, MaxFibersPerThread)
   {
   }
}



// Scheduler.h
// (public)

namespace en
{
   class Scheduler
      {
      bool terminating; // Indicates if system is shutting down
      
      void loop(void);          // Scheduler main loop, picks tasks, executes them, switches between fibers, manages worker thread
      void run(Task& task);
      void wait(Task& task);
      };
   
   void Scheduler::run(Task& task)
   {
   // -> push task on a queue of tasks to execute
   // -> if queue was empty, wake up worker thread
   }
   
   void Scheduler::wait(Task& task)
   {
   // -> save this fiber state, push it on queue of waiting ones
   // -> return to main loop to pick next fiber
   }

   void Scheduler::loop(void)
   {
   // One instance executing on each thread (switching between fibers in pool)
   
   // Execute tasks until whole system is not terminating
   while(!terminating)
      {
      //  -> pick task to execute
      //     -> steal task from other thread
      //        -> or sleep current thread if no tasks are available

      };
   //
   
   // push task on a queue of tasks to execute
   }

}

*/





// setcontext is one of a family of C library functions (the others being getcontext, makecontext and swapcontext
// See also: https://rethinkdb.com/blog/making-coroutines-fast/




// Main thread is I/O thread:
// - handle system events
// - execute commands restricted to main thread (delegated from other threads)
//   So it's special worker thread that executes "special tasks"
//   and does so only once per frame when it wakes up on incoming system events
//   (or no later than every Nms - this may be triggered by registering redraw or timer event).
//
// - Create bunch of worker threads (whole pool in advance)
// - Create bunch of fibers per thread (whole pool in advance)
//   ( convert worker threads to fibers if needed)
// - Assign worker threads to physical cores
//










// Work-Stealing:
// 
// deque of data 
// each deque is local to some worker thread
// worker performs push/pop operations on "bottom" of the local deque (LIFO)
// LIFO - Last In First Out -> so stack, where tasks are thrown at the top, and worker is picking them up from there as well 
// (so why it's said it picks and pushes at the "bottom"? it can cause tasks to be infinitely stalled on the "top" if new ones are constantly pushed and poped)
// other threads can steal work from the "top" - FIFO Pop 
// First In First Out - pipe like processing, the "top" pop would refer to stealing tasks from the Input side of the queue (pushed later), where worker pops work from the Output side (but it also pushes there, why??)
// 
// Implementation:
// We use only loads and stores for PushBottom and PopBottom in the common case, and transition in a lock-free manner to using a costly CAS only when a potential conflict requires processes to reach consensus.
// Only worker can update next/previous pointers in it's deque, to avoid using CAS.



// en::gpu - nested namespaces since C++17




/*
   assert( (worker < workerThreads) ||
           worker == SingleThread ||
           worker == MainThread ||
           worker == AnyThread );

   uint32 selectedWorker = InvalidWorkerID;

   // Select thread to execute task
   if (worker == SingleThread ||
       worker == AnyThread)
      {
      // Find worker thread with smallest amount of tasks to process, and assign
      // this task to it's queue. Amount of work on each worker thread queue can 
      // change during iterating over them, but thats fine - such estimation is 
      // coarse anyway, as different tasks will take different amount of time to 
      // execute (so it's hard to easily load balance amount of work in terms of
      // execution time).
      uint32 minimumTasks = MaxWorkerThreadTasks;
      for(uint32 i=0; i<workerThreads; ++i)
         {
         uint32 tasksCount = workerState[i].tasksCount; // (!) Possible hazard
         if (tasksCount < minimumTasks)
            {
            selectedWorker = i;
            minimumTasks   = tasksCount;
            if (minimumTasks == 0)
               break;
            }
         }

      assert( selectedWorker != InvalidWorkerID );
      }
   else
   if (worker < workerThreads)
      selectedWorker = worker;
*/


   // bool local = false;   // Indicates if task should be locked to current worker thread









// Other work-stealing algorithms:
// http://supertech.csail.mit.edu/papers/steal.pdf
// http://www.aladdin.cs.cmu.edu/papers/pdfs/y2000/locality_spaa00.pdf

//
// Rust, Haskell implementations:
// https://github.com/toffaletti/rust-code/blob/master/chase_lev_deque.rs
// https://github.com/ekmett/structures/blob/deque/src/Control/Concurrent/Deque.hs
// http://hackage.haskell.org/package/atomic-primops-0.4/docs/Data-Atomics-Counter-Reference.html
// 
// C++ implementation:
// https://github.com/toffaletti/chase-lev
//
// Multiple-Producer Single-Consumer Queue:
// https://github.com/samanbarghi/MPSCQ




// A dynamic-sized non-blocking work stealing deque
//
// Full paper can be found at:
// http://www.cs.tau.ac.il/~shanir/nir-pubs-web/Papers/Work_Stealing_Deque.pdf
//

/*
struct DequeNode
   {
   enum{ArraySize=8};                  // 8 size of array in a node (can tune this)
   ThreadInfo itsDataArr[ArraySize];
   DequeNode* prev;
   DequeNode* next;
   };

// Needs to fit in single CAS operand (so max 64bits)
struct BottomStruct
   {
   DequeNode* nodeP;
   int cellIndex;
   };

// Needs to fit in single CAS operand (so max 64bits)
struct TopStruct
   {
   DequeNode* nodeP;
   int cellIndex;
   int tag;           // Used to avoid ABA situation
   };

class DynamicDecque
   {
   public:
   void PushBottom(ThreadInfo theData);
   ThreadInfo PopTop(void);
   ThreadInfo PopBottom(void);

   BottomStruct Bottom;  // aligned to cache line (!)
   TopStruct    Top;     // aligned to cache line (!)
   };





void DecodeBottom(const struct BottomStruct, DequeNode*& node, int& index)
{
	// TODO: Decompress node-index pair from CAS friendly compressed representation

	// std::atomic::load( , memory_order_acquire ) ?
}

BottomStruct EncodeBottom(const DequeNode* node, const int index)
{
	// TODO: Compress node-index pair to CAS friendly compressed representation

}

void DecodeTop(const struct TopStruct, DequeNode*& node, int& index, int& tag)
{
	// TODO: Decompress node-index-tag tuple(?) from CAS friendly compressed representation

	// std::atomic::load( , memory_order_acquire ) ?
}

TopStructure EncodeTop(const DequeNode* node, int index, int tag)
{
	// TODO: Compress node-index-tag tuple(?) to CAS friendly compressed representation

}


bool EmptinessTest(const TopStructure top, const BottomStructure bottom)
{
	// TODO: !
}

void FreeOldNodeIfNeeded(void)
{
	// TODO: !
}


bool IndicateEmpty(BottomStruct bottomVal, TopStruct topVal)
{
   DequeNode* botNode  = nullptr;
   int        botIndex = 0;
   DecodeBottom(bottomVal, botNode, botIndex); 

   DequeNode* topNode  = nullptr;
   int        topIndex = 0;
   int        topTag   = 0;
   DecodeTop(topVal, topNode, topIndex, topTag);

   if (botNode == topNode &&
       (botIndex == topIndex || botIndex == (topIndex + 1)))
      return true;

   if ((botNode == topNode->next) &&
       (botIndex == 0) &&
       (topIndex == (DequeNode::ArraySize - 1)))
      return true;

   return false;
}



DynamicDecque::DynamicDecque()
{
	DequeNode* nodeA = AllocateNode();
	DequeNode* nodeB = AllocateNode();
	nodeA->next = nodeB;
	nodeB->prev = nodeA;
	Bottom = EncodeBottom(nodeA, DequeNode::ArraySize - 1);
	Top    = EncodeTop(nodeA, DequeNode::ArraySize - 1, 0);
}




void DynamicDecque::PushBottom(ThreadInfo theData)
{
DequeNode* newNode  = nullptr;
int        newIndex = 0;

DequeNode* currNode  = nullptr;
int        currIndex = 0;
DecodeBottom(Bottom, currNode, currIndex);  // Read Bottom description

currNode->itsDataArr[currIndex] = theData;   // Write data in current bottom cell  <-- std::atomic::store, memory_order_relaxed ?
if (currIndex != 0)
   {
   newNode  = currNode;
   newIndex = currIndex - 1;
   }
else // Allocate and link a new node
   {
   newNode        = AllocateNode();
   newNode->next  = currNode;
   currNode->prev = newNode;
   newIndex       = DequeNode::ArraySize - 1;
   }
Bottom = EncodeBottom(newNode, newIndex); // Update bottom
}




ThreadInfo DynamicDecque::PopTop(void)
{
   TopStructure currTop = Top;           // Read Top   <--- std::atomic::load, memory_order_acquire ?

   DequeNode* currTopNode  = nullptr;
   int        currTopIndex = 0;
   int        currTopTag   = 0;
   DecodeTop(currTop, currTopNode, currTopIndex, currTopTag);

   BottomStructure currBottom = Bottom;  // Read Bottom <--- std::atomic::load, memory_order_acquire ?

   if (EmptinessTest(currTop, currBottom))
      {
      if (currTop == Top)                // Atomic Compare ? memory_order_acquire
         return EMPTY;
      else
         return ABORT;
      }

   DequeNode* newTopNode = nullptr;
   int        newTopIndex = 0;
   int        newTopTag   = 0;
   if (currTopIndex != 0) // if deque isn't empty calculate next top pointer
      { // stay at current node:
      newTopNode  = currTopNode;
      newTopIndex = currTopIndex - 1;
      newTopTag   = currTopTag;
      }
   else // move to next node and update tag:
      {
      newTopNode  = currTopNode->prev;
      newTopIndex = DequeNode::ArraySize - 1;
      newTopTag   = currTopTag + 1;
      }

   ThreadInfo retVal = currTopNode->itsDataArr[currTopIndex];    // Read value

   TopStructure newTopVal;
   EncodeTop(newTopVal, newTopNode, newTopIndex, newTopTag);

   if (CAS(&Top, currTop, newTopVal))   // Try to update Top using CAS
      {
      FreeOldNodeIfNeeded(void);
      return retVal;
      }
   else
      {
      return ABORT;
      }
}



ThreadInfo DynamicDecque::PopBottom(void)
{
   BottomStruct oldBottom = Bottom; // Read Bottom description  <-- std::atomicload, memory_order_acquire

   DequeNode* oldBotNode  = nullptr;
   int        oldBotIndex = 0;

   DecodeBottom(oldBottom, oldBotNode, oldBotIndex);  

   if (oldBotIndex != (DequeNode::ArraySize - 1))
      {
      newBotNode  = oldBotNode;
      newBotIndex = oldBotIndex + 1;
      }
   else
      {
      newBotNode  = oldBotNode->next;
      newBotIndex = 0;
      }
	
   ThreadInfo retVal = newBotNode->itsDataArr[newBotIndex];  // Read data to be popped

   Bottom = EncodeBottom(newBotNode, newBotIndex); // Update bottom

   TopStructure currTop = Top;           // Read Top   <--- std::atomic::load, memory_order_acquire ?

   DequeNode* currTopNode  = nullptr;
   int        currTopIndex = 0;
   int        currTopTag   = 0;
   DecodeTop(currTop, currTopNode, currTopIndex, currTopTag);

   // Case 1: if Top has crossed Bottom
   if (oldBotNode == currTopNode &&
       oldBotIndex == currTopIndex)
      {
      // Return bottom to its old position
      Bottom = EncodeBottom(oldBotNode, oldBotIndex);
      return EMPTY;
      }
   else // Case 2: when popping the last entry in the deque (ie. deque is empty after the update of bottom)
   if (newBotNode == currTopNode &&
       newBotIndex == currTopIndex)
      {
      // Try to update Top's tag so no concurrent PopTop operation will also pop the same entry:
      ThreadInfo newTopVal = EncodeTop(currTopNode, currTopIndex, currTopTag + 1);
      if (CAS(&Top, currTop, newTopVal))
         {
         FreeOldNodeIfNeeded();
         return retVal;
         }
      else // if CAS failed (i.e. a concurrent PopTop operation already popped the last entry)
         {
         // Return bottom to it's old position
         Bottom = EncodeBottom(oldBotNode, oldBotIndex);
         return EMPTY;
         }
      }
   else // Case 3: Regular case (i.e. there was at least one entry in the deque <<after>> bottom's update)
      {
      FreeOldNodeIfNeeded();
      return retVal;
      }
}

*/
