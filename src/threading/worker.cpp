/*

 Ngine v5.0
 
 Module      : Worker thread.
 Visibility  : Engine only.
 Requirements: none
 Description : Manages worker thread during 
               execution of sheduled tasks.

*/

#include "core/log/log.h"
#include "utilities/strings.h"

#include "core/config/config.h"
#include "core/threading/atomics.h"
#include "threading/context.h"
#include "threading/scheduler.h"
#include "utilities/random.h"

using namespace en::log;

// Task status switches
#define EN_DONE  false
#define EN_BUSY  true




namespace en
{
   namespace threads
   {
#ifdef EN_PLATFORM_WINDOWS
   // Helper function for spawning Worker Threads
   DWORD WINAPI startWorker(void* ptr)
   {
   return reinterpret_cast<WorkerThread*>(ptr)->start();
   }
#endif

   WorkerThread::WorkerThread(uint8 id) :
      mutex(),
      state(nullptr),
      id(id),
      done(false)
#ifdef EN_PLATFORM_WINDOWS
      , handle(INVALID_HANDLE_VALUE)
#endif
   {
   // Create queues for incoming tasks
   uint32 tasks = 256;
   Config.get("en.threading.maxTasks", &tasks);
   queue[0] = new Nqueue<Task*>(tasks);
   queue[1] = new Nqueue<Task*>(tasks);
   
   // Worker thread 0 will reuse main thread.
   if (id == 0)
      {
#ifdef EN_PLATFORM_WINDOWS
      handle = GetCurrentThread();
      // Main thread needs to set pointer to it's
      // Worker Thread State in TLS before there
      // will be first call of run method.
      SchedulerContext.currentState.set(this);
#endif
      }
   // Create worker thread and allow it to enter its
   // main scheduling loop. It will go sleeping until
   // some task will be spawned for execution.
   else
      {
#ifdef EN_PLATFORM_WINDOWS
      handle = CreateThread( nullptr,                                         // Thread parameters
                             65536,                                           // Thread stack size
                             static_cast<PTHREAD_START_ROUTINE>(startWorker), // Thread entry point	 
                             static_cast<LPVOID>(this),                       // Thread startup parameters
                             0,	                                              // Creation flags
                             nullptr );                                       // Thread id           
#endif
      }
   }

   WorkerThread::~WorkerThread()
   {
   // Free from queue all remaining tasks that wasn't executed 
   // before occurence of shutdown signal. For each task on queue
   // we first check if it is not shared
   Task* ptr;
   for(uint8 i=0; i<2; ++i)
      while(queue[i]->size() != 0)
           {
           queue[i]->pop(&ptr);
           ptr->state->set(EN_DONE);
           if (!ptr->state->working())
              delete ptr->state;
           delete ptr;
           }

   // Free queue
   delete queue[0];
   delete queue[1];

   // Free worker thread
#ifdef EN_PLATFORM_WINDOWS
   if (id > 0)
      CloseHandle( handle );
#endif
   }

   uint32 WorkerThread::start(void)
   {
   // Wait until scheduler will spawn all threads. Then it can proceed to main 
   // loop and query scheduler for data because it's constructor will be done.
   while (!SchedulerContext.created);
 
   // First, bind this Worker Thread State to TLS
   SchedulerContext.currentState.set(this);

   // Log current thread as running
   string txt("Worker thread ");
   txt.append(stringFrom(id));
   txt.append(" starts execution.\n");
   DebugLog( txt );

   // Check if programmer wants to work with task pool turned off, if so, 
   // emulate singlethreaded behavior.
   bool taskPoolOn = true;
   Config.get("en.threading", &taskPoolOn);

   // At the beginning all worker threads goes to sleep because Thread-Pool is 
   // empty. The only exception is main thread which will jump here after 
   // pushing work for himself. Threads will wake up when some tasks will be 
   // spawned or Thread-Pool will be closing.
   if (this != SchedulerContext.thread[0])
      sleep();

   // Main scheduling loop
   for(;;)
      {
      // If thread was waked up because Thread-Pool is closing, terminate it. 
      // Otherwise try to execute some work.
      if (Scheduler.closing())
         break;

      // Work until there is anything to execute on the queue, or it is 
      // possible to steal from other workers.
      schedule(nullptr);
 	
      // There are no more tasks for execution at the moment and it is 
      // impossible to steal work from other threads. Maybe Thread-Pool is 
      // closing? If true, terminate.
      if (Scheduler.closing())
         break;

      // If Thread-Pool is emulating singlethreaded application it needs to 
      // terminate when there is no more work to do. Otherwise whole app can 
      // wait for some event that will spawn new tasks.
      if ( !taskPoolOn &&
           SchedulerContext.thread[0] == this &&
           SchedulerContext.mainState == nullptr )
         break;
    
      // There are no tasks to execute or steal at the moment and scheduler 
      // is not closing so thread can safely go into sleep mode.
      sleep();
      }

   // Log current thread as closing
   txt = string("Worker thread ");
   txt.append(stringFrom(id));
   txt.append(" finishes execution.\n");
   DebugLog( txt );

   done = true;
   return 0;
   }

   void WorkerThread::schedule(TaskState* expected)
   {
   // Main loop that processes tasks from the queue. It finishes execution if 
   // there are no more tasks on any of the queues and if there are no more 
   // tasks to steal from other threads. It is also called when task wants to 
   // work in background during waiting for other task (then pointer to that 
   // other tasks state is passed as parameter to break execution when it will 
   // finish). Because tasks can spawn recursively other tasks this method 
   // holds copies of all important data on stack.
   Task*      task;
   TaskState* last;

   for(;;)
      {
      // Work until there are tasks on the queues that can be executed.
      for(;;)
         {
         // First execute dedicated tasks if there are any, otherwise execute 
         // regular ones.
         if (!pop(&task, true))
            if (!pop(&task, false))
               break;

         // Save state of task that was executing when it called scheduler 
         // (allows recursive calls and execution).
         last = state;

         // Set task as currently executed, and when finished mark it as done 
         // or partially done for splitted one.
         state = task->state;
         task->work();
         state->set(EN_DONE);

//TODO: Where should be deleted tasks and their states ?
// States should be deleted by tasks that receive them locally, tasks commit suicide?

         if (!state->working())
            {
            // If it was a main task and it ended, mark that there is no main task.
            if (state == SchedulerContext.mainState)
               SchedulerContext.mainState = nullptr;

            delete state;
            task->state = nullptr;
            delete task;
            }



         // Restore state of task that called scheduler (so we can continue to
         // execute it properly).
         state = last;


         // If this scheduler instance is processing tasks in background, check 
         // if task that we are waiting for didn't finished. If so, break and 
         // resume work of waiting task.
         if (expected)
            if (!expected->working())
               return;
         }

      // Task queues are empty, try to steal some work from other threads.
      if (!steal())
         break;
      }
   }

   TaskState* WorkerThread::push(Task* task, bool dedicated)
   {
   // Decide on which queue to operate
   uint8 id = dedicated ? 1 : 0;

   // Create state descriptor for new task. Even if it won't be needed, task 
   // must have it initiated, when it is added to the queue. 
   task->state = new TaskState();
   task->state->set(EN_BUSY);

   // If tasks queue is full, new task is executed on the fly, otherwise it is
   // seen on the queue and can be picked up by current or other thread.
   if (!queue[id]->push(task))
      {
      task->work();
      delete task->state;
      task->state = nullptr;
      return nullptr;
      }
  
   if (SchedulerContext.mainState == nullptr)
      {
      // At the beginning of Thread-Pool main task is missing. First task that 
      // will be spawned for execution will be marked as it, because it will 
      // probably contain main function of application. 
      if (!SchedulerContext.started)
         {
         // This task becomes main task starting Thread-Pool readiness for 
         // execution.
         SchedulerContext.mainState = task->state; 
         SchedulerContext.started   = true; 
         return task->state;
         }
  
      // During work of Thread-Pool, if there is no main task, it means, it 
      // finished executing but it's sub tasks are still working and spawning 
      // new ones. In such situation first new spawned task will be marked as 
      // new main task. This can be done simultaneously by few worker threads 
      // so atomic CAS is required.
      CompareAndSwap((volatile uint32*)&SchedulerContext.mainState, (uint32)task->state, (uint32)nullptr);   // TODO: Fix for 64bit pointers !!!
      return task->state;
      }
  
   // Because this task is one of many tasks spawned for execution it is
   // good to try to wake up some thread, maybe it will catch it and execute.
   SchedulerContext.wakeUpAllThreads();
   return task->state;
   }
   
   // Take one task from the queue for execution
   bool WorkerThread::pop(Task** task, bool dedicated)
   {
   // Decide on which queue to operate
   uint8 id = dedicated ? 1 : 0;
   
   // Try to get pointer of the task from the top of the queue (LIFO).
   return queue[id]->pop(task);
   
   // TODO: In future implement ability to split tasks into subtasks
   }

   // Try to get some work from other threads
   bool WorkerThread::steal(void)
   {
   // Iterate through all other worker threads to find, if one of them can give
   // up some work. Every time start iterating from other randomly choosen 
   // thread to prevent possibilities of potential problematic patterns 
   // (like thread starvation etc.).
   uint32 start = random(SchedulerContext.workers);
   for(uint32 i=0; i<SchedulerContext.workers; ++i)
      {
      uint32 n = (i + start) % SchedulerContext.workers;
   
      WorkerThread* thread = SchedulerContext.thread[n];
   
      // Ask all other threads but not yourself
      if (thread == this)
         continue;

      // Try to enter critical section of selected thread only once. If this 
      // worker thread is already giving up some work, try to get it from 
      // another one.
      if (!thread->mutex.tryLock())
         continue;
   
      uint32 size = thread->queue[0]->size();
      // Nothing to take 
      if (size < 2)
         {
         // TODO: In future add support for splitting
         //       tasks ( here, if there is only one
         //       task waiting on the queue ).
   
         // Leaving Critical Section of selected thread
         thread->mutex.unlock();
         continue;
         }
   
      // Try to take, maximum half of, tasks that were present at thread queue 
      // during check. Take them from the bottom of the queue as they would be 
      // executed as last ones.
      uint32 count = (size + 1) / 2;
      uint32 taken = 0;
      while (thread->queue[0]->size() > count)
            {
            // Transfer tasks between queues using atomic queue operations 
            Task* temp;
            if (thread->queue[0]->popBack(&temp))
               {
               queue[0]->push(temp);
               taken++;
               }
            };
//234567890123456789012345678901234567890123456789012345678901234567890123456789 
      // Leaving Critical Section of selected thread
      thread->mutex.unlock();
   
      // If none of the tasks was successfully taken try with another thread.
      if (!taken) 
         continue;
   
      return true;
      }
   
   // We couldn't steal tasks from any of the worker threads.
   return false;
   }


//// Try to give up some work to idle thread
//bool WorkerThread::give(WorkerThread* dst)
//{
// // Try to enter critical section only once.
// // If this worker thread is already giving
// // up some work don't wait to give up some
// // more.
// if (!m_mutex.tryLock())
//    return false;
//
// // Nothing to give
// if (m_queue[0]->size() < 2)
//    {
//    // Leaving Critical Section
//    m_mutex.unlock();
//    return false;
//    }
//
// // Lock asking threads queue
// dst->m_mutex.lock();
//
// // TODO: In future add support for splitting
// //       tasks ( here, if there is only one
// //       task waiting on the queue ).
//
// // Give half of tasks to asking thread from
// // the bottom of the queue as they would be
// // executed as last ones.
// uint32 count = (m_tasks + 1) / 2;
// uint32 i = 0;
// for(; i<count; i++)
//    dst->m_queue[dst->m_tasks] =  m_queue[i];
// dst->m_tasks += count;
//
// // Then move down our tasks
// for(; i<m_tasks; i++)
//    m_queue[i-count] = m_queue[i];
// m_tasks -= count;
//
// // Unlock asking thread queue
// dst->m_mutex.unlock();
//
// // Leaving Critical Section
// m_mutex.unlock();
// return true;
//}

   void WorkerThread::sleep(void)
   {
   using namespace en;
   using namespace en::log;

   string txt("Worker thread ");
   txt.append(stringFrom(id));
   txt.append(" goes to sleep!\n");
   DebugLog( txt );
  
#ifdef EN_PLATFORM_WINDOWS
   // Before going to sleep raise semaphore counter of sleeping threads. 
   // It will be used to check if Thread-Pool is idle.
   ReleaseSemaphore( SchedulerContext.sleepingSignal, 1, nullptr );
   
   // Worker thread will sleep until wake-up semaphore will be raised. 
   // At the moment of waking up, thread will again decrease the semaphore.
   WaitForSingleObject( SchedulerContext.wakeUpSignal, INFINITE );
#endif 
   }

   bool WorkerThread::finished(void)
   {
   return done;
   }

   }
}
