
#include "core/config/config.h"

#if defined(EN_PLATFORM_OSX)
#ifdef aligned
#undef aligned
#endif
#include <mach/mach.h>
#include <mach/mach_time.h>

static mach_timebase_info_data_t timebase = { 0, 0 };
#endif

#include "platform/system.h"
#include "threading/mutex.h"

#include "core/log/log.h"

#include "threading/context.h"
#include "threading/scheduler.h"

// Specifies destination queue for task
#define EN_DEDICATED true

namespace en
{
   namespace threads
   {
   //# INTERFACE
   //##########################################################################
   
   uint32 Interface::core(void) const
   {
   assert( SchedulerContext.created );
   WorkerThread* worker = (WorkerThread*)SchedulerContext.currentState.get();
   return worker->id;
   }

   uint32 Interface::workers(void) const
   {
   return SchedulerContext.workers;
   }

   void Interface::shutdown(void)
   {
   SchedulerContext.shutdown = true;
   }

   bool Interface::closing(void) const
   {
   return SchedulerContext.shutdown;
   }

   //# IMPLEMENTATION
   //##########################################################################

   TaskState* Interface::run(Task* task, bool wait)
   {
   // If Thread-Pool is working in compatibility mode as single 
   // threaded application, execute main task immediately.
   if (SchedulerContext.workers == 1)
      {
      task->work();
      delete task;       // Scheduler deletes tasks and statuses ?
      return nullptr;
      }

   // In multithreaded mode check if this instance of task isn't 
   // already spawned for execution on other thread.
   // ( this shouldn't happen by the way! )
   if (task->running())
      return task->state;

   // Try to add task to current thread queue
   WorkerThread* worker = (WorkerThread*)SchedulerContext.currentState.get();
   TaskState* state = worker->push(task);

   // If current task don't want to wait for spawned one, return 
   // pointer to state so it's completion can be queried later. 
   if (!wait)
      return state;

   // Wait until spawned task finishes execution
   this->wait(state);
   return nullptr;
   }

   TaskState* Interface::run(Task* task, uint8 core, bool wait)
   {
   // If Thread-Pool is working in compatibility mode as single 
   // threaded application, execute main task immediately.
   if (SchedulerContext.workers == 1)
      {
      task->work();
      delete task;       // Scheduler deletes tasks and statuses ?
      return nullptr;
      }
   
   // In multithreaded mode check if this instance of task isn't 
   // already spawned for execution on other thread.
   // ( this shouldn't happen by the way! )
   if (task->running())
      return task->state;
   
   // Be sure specified worker thread exists
   if (core > SchedulerContext.workers)
      core = SchedulerContext.workers;
   
   // Try to add task to specified worker thread queue
   TaskState* state = SchedulerContext.thread[core]->push(task, EN_DEDICATED);
   
   // If current task don't want to wait for spawned one, return 
   // pointer to state so it's completion can be queried later. 
   if (!wait)
      return state;
   
   // Wait until spawned task finishes execution
   this->wait(state);
   return nullptr;
   }

   //TaskState* Interface::parallel(Task* task, Range range, bool wait)
   //{
   //
   //}

   void Interface::wait(TaskState* state)
   {
   // When waiting for task to be done, execute other tasks from queue in background.
   WorkerThread* worker = (WorkerThread*)SchedulerContext.currentState.get();
   worker->schedule(state);

   // This tasks state is no longer valid
   delete state;
   }

   void TaskState::set(bool state)
   {
   // Several threads can try to change state of this task at the same 
   // time, if it is splitted for parallel processing.
   if (state)
      AtomicIncrease(&busy);
   else 
      AtomicDecrease(&busy);
   }

   //# CONTEXT
   //##########################################################################

   Context::Context() :
#ifdef EN_PLATFORM_WINDOWS
      sleepingSignal(INVALID_HANDLE_VALUE),
      wakeUpSignal(INVALID_HANDLE_VALUE),
#endif
      thread(nullptr),
      workers(1),
      mainState(nullptr),
      started(false),
      shutdown(false)
   {
   }

   Context::~Context()
   {
   }

   bool Context::create(void)
   {
   Log << "Starting module: Thread-Pool Scheduler." << endl;

   // TODO: Move it to per worker thread initialization step, and propagate for all OS'es
#if defined(EN_PLATFORM_OSX)
   // Set high thread priority, to prevent timers coalescing.
   const int64_t nanoSecondsPerSecond = 1000 * 1000 * 1000;
   mach_timebase_info(&timebase);
   const int64_t HZ = ( nanoSecondsPerSecond * timebase.numer ) / timebase.denom;
      
   // Thread needs to run with 90Hz candence (to match classic refrsh rate of HMD's).
   // In each cycle it need's 1/4th of that time to do calculations, and they need to
   // be finished no later than in half time of single frame.
   //
   // TODO: In future set it only for pacing threads, or figure out if whole frame time
   //       may be reserved by single thread.
   struct thread_time_constraint_policy ttcpolicy;
   ttcpolicy.period      = HZ / 90;  // 90Hz
   ttcpolicy.computation = HZ / 360; // 360Hz - need 1/4 of the next 90Hz
   ttcpolicy.constraint  = HZ / 180; // 180Hz - our 1/4 of 90Hz must be w/in 1/2 of the 90Hz
   ttcpolicy.preemptible = 1;
      
   thread_port_t threadport = pthread_mach_thread_np( pthread_self() );
   kern_return_t result = thread_policy_set( threadport, THREAD_TIME_CONSTRAINT_POLICY, (thread_policy_t)&ttcpolicy, THREAD_TIME_CONSTRAINT_POLICY_COUNT );
   if (result != KERN_SUCCESS)
      en::Log << "Warning! Couldn't set main thread real-time priority!\n";
#endif
      
   // Create Thread Local Storage that will direct each thread to 
   // its corresponding descriptor structure.
   currentState.create();

   // Choose number of worker threads
   uint32 cores = System.cores();
   Config.get("en.threading.workers", &workers);
   if (workers > cores)
      workers = cores;

   // Create semaphores that will control sleeping and waking up of 
   // worker threads depending on current amount of work to execute.
#ifdef EN_PLATFORM_WINDOWS
   sleepingSignal = CreateSemaphore(nullptr, 0, workers, nullptr);
   wakeUpSignal   = CreateSemaphore(nullptr, 0, workers, nullptr);
#endif

   // Create worker threads states, also for current main thread. It 
   // means that main thread can work like other threads and process 
   // tasks if programmer will want to. Otherwise it will distribute 
   // work or emulate Thread-Pool on single threaded architectures.
   thread = new WorkerThread*[workers];
   for(uint8 i=0; i<workers; ++i)
      thread[i] = new WorkerThread(i);

   // Now Task Pool is ready and all workers are waiting for some 
   // tasks to execute. 
   created = true;
   return true;
   }
 
   void Context::destroy(void)
   {
   Log << "Closing module: Thread-Pool Scheduler." << endl;

   // Check if scheduler is running.
   assert( created );

   // Signal to workers that scheduler is closing. This will result 
   // in early exit from all worker threads.
   shutdown = true;

   // Wake up sleeping workers so they can also terminate properly.
   wakeUpAllThreads();

   // Wait until all workers have succesfullfy finished execution.
   if (workers > 1)
      for(uint8 i=1; i<workers; ++i)
         while( !thread[i]->finished() );

   // Free worker threads and their descriptors.
   for(uint8 i=0; i<workers; ++i)
      delete thread[i];
   delete [] thread;

   // Free Thread Local Storage
   currentState.destroy();
   }
 
   bool Context::start(Task* task)
   {
   // This method will be called only once after context of
   // Thread-Pool Scheduler is created.
   assert( created );

   // It can be called only by main thread.
   // (this check should never fail! ) 
#ifdef EN_PLATFORM_WINDOWS
   if (thread[0]->handle != GetCurrentThread())
      {
      assert(0);
      delete task;
      return false;
      }
#endif

   // If Thread-Pool is working in compatibility mode as single 
   // threaded application, execute main task immediately.
   if (workers == 1)
      {
      task->work();
      delete task;
      return true;
      }

   // Try to add task to current thread queue
   WorkerThread* worker = (WorkerThread*)currentState.get();
   if (!worker->push(task))
      return false;

   // Main thread jumps into worker thread entry point. From now on 
   // it will execute tasks until one of two exit signals occurs. 
   // First is calling scheduler method shutdown, second is situation 
   // in which all workers have no more tasks to execute and there 
   // is no waiting for events to spawn new ones.
   thread[0]->start();
   return true;
   }

   void Context::wakeUpAllThreads(void)
   {
#ifdef EN_PLATFORM_WINDOWS
   ReleaseSemaphore(wakeUpSignal, workers-1, nullptr);
#endif
   }

   }

threads::Context   SchedulerContext;
threads::Interface Scheduler;
}

// Thread Local Storage. It holds pointer to current 
// worker thread state class. Therefore each thread 
// knows which state it should use for it's operations.
en::threads::TLS en::threads::Context::currentState;

// Created flag need to be static so threads spawned
// during execution of schedulers constructor can 
// wait until it will be done, and then proceed with
// operations on it.
bool en::threads::Context::created = false;
























//
//// Specifies destination queue for task
//#define EN_DEDICATED true
//
//// Thread Local Storage. It holds pointer to current 
//// worker thread state class. Therefore each thread 
//// knows which state it should use for it's operations.
//Ntls NSchedulerContext::m_currentState;
//
//// Created flag need to be static so threads spawned
//// during execution of schedulers constructor can 
//// wait until it will be done, and then proceed with
//// operations on it.
//bool NSchedulerContext::m_created;
//
//// Constructor
//NSchedulerContext::NSchedulerContext() :
//       m_mainState(NULL),
//       m_started(false),
//       m_shutdown(false)
//{
// init();
//}
//
//// Destructor
//NSchedulerContext::~NSchedulerContext()
//{
// destroy();
//}
//
//// Creates and inits all structures
//void NSchedulerContext::init(void)
//{
// using namespace en;
//
// Log << "Starting module: Tasks Pool Scheduler." << endl;
//
// m_created = false;
//
// // Create Thread Local Storage that 
// // will direct each thread to its
// // corresponding descriptor structure.
// m_currentState.create();
//
// // Choose number of worker threads
// uint32 cores = System.cores();
// m_workers = 1;
// en::Config.get("en.threading.workers", &m_workers);
// if (m_workers > cores)
//    m_workers = cores;
//
// // Create semaphores that will control
// // sleeping and waking up of worker 
// // threads depending on current amount
// // of work to execute.
//#ifdef EN_PLATFORM_WINDOWS
// m_sleepingSignal = CreateSemaphore(NULL, 0, m_workers, NULL);
// m_wakeUpSignal   = CreateSemaphore(NULL, 0, m_workers, NULL);
//#endif
//
// // Create worker threads states,
// // also for current main thread. It 
// // means that main thread can work 
// // like other threads and process 
// // tasks if programmer will want to.
// // Otherwise it will distribute work
// // or emulate Task Pool on single
// // threaded architectures.
// m_thread = new WorkerThread*[m_workers];
// for(uint8 i=0; i<m_workers; i++)
//    m_thread[i] = new WorkerThread(i);
//
// // Now Task Pool is ready and all workers 
// // are waiting for some tasks to execute. 
// m_created = true;
//}
//
//// Destroys task pool
//void NSchedulerContext::destroy(void)
//{
// using namespace en;
//
// Log << "Closing module: Tasks Pool Scheduler.\n";
//
// // Check if scheduler is running.
// if (!m_created)
//    return;
//
// // Signal to workers that scheduler is
// // closing. This will result in early
// // exit from all worker threads.
// shutdown();
//
// // Wake up sleeping workers so they can
// // also terminate properly.
// wakeUpSignal();
//
// // Do nothing until all workers have
// // succesfullfy finished execution.
// if (m_workers > 1)
//    for(uint8 i=1; i<m_workers; i++)
//       while( !m_thread[i]->finished() );
//
// // Free worker threads and their
// // descriptors.
// for(uint8 i=0; i<m_workers; i++)
//    delete m_thread[i];
// delete [] m_thread;
//
// // Free Thread Local Storage
// m_currentState.destroy();
//}
//
//// Main thread starts execution of first task
//bool NSchedulerContext::start(NTask* task)
//{
// // This method can be called only once
// // after Task Pool scheduler is created.
// if (!m_created || m_started)
//    return false;
//
// // It can be called only by main thread.
//#ifdef EN_PLATFORM_WINDOWS
// if (m_thread[0]->m_handle != GetCurrentThread())
//    return false;
//#endif
//
// // If Task Pool is working in compatibility
// // mode as single threaded application, 
// // execute main task immediately.
// if (m_workers == 1)
//    {
//    task->work();
//    return true;
//    }
//
// // Try to add task to current thread queue
// WorkerThread* worker = (WorkerThread*)m_currentState.get();
// if (!worker->push(task))
//    return false;
//
// // Main thread jumps into worker thread 
// // entry point. From now on it will execute
// // tasks until one of two exit signals 
// // occurs. First is calling scheduler
// // method shutdown, second is situation 
// // in which all workers have no more 
// // tasks to execute and there is no waiting
// // for events to spawn new ones.
// m_thread[0]->start();
// return true;
//}
//
//// Add task for execution on any thread
//NTaskState* NSchedulerContext::run(NTask* task, bool wait)
//{
// // If Task Pool is working in compatibility
// // mode as single threaded application, 
// // execute task immediately.
// if (m_workers == 1)
//    {
//    task->work();
//    return NULL;
//    }
//
// // In multithreaded mode check if this 
// // instance of task isn't already spawned 
// // for execution on other thread.
// // ( this shouldn't happen by the way! )
// if (task->running())
//    return task->m_state;
//
// // Try to add task to current thread queue
// WorkerThread* worker = (WorkerThread*)m_currentState.get();
// NTaskState* state = worker->push(task);
//
// // If current task don't want to wait for
// // spawned one, return pointer to state so
// // it's completion can be queried later. 
// if (!wait)
//    return state;
//
// // Wait until spawned task finishes execution
// this->wait(state);
// return NULL;
//}
//
//// Adds task for execution on specified core 
//NTaskState* NSchedulerContext::run(NTask* task, uint8 core, bool wait)
//{
// // If Task Pool is working in compatibility
// // mode as single threaded application, 
// // execute task immediately.
// if (m_workers == 1)
//    {
//    task->work();
//    return NULL;
//    }
//
// // In multithreaded mode check if this 
// // instance of task isn't already spawned 
// // for execution on other thread.
// // ( this shouldn't happen by the way! )
// if (task->running())
//    return task->m_state;
//
// // Be sure specified worker thread exists
// if (core > m_workers)
//    core = m_workers;
//
// // Try to add task to specified worker 
// // thread queue
// NTaskState* state = m_thread[core]->push(task,EN_DEDICATED);
//
// // If current task don't want to wait for
// // spawned one, return pointer to state so
// // it's completion can be queried later. 
// if (!wait)
//    return state;
//
// // Wait until spawned task finishes execution
// this->wait(state);
// return NULL;
//
//}
//
//// Adds splittable task for execution and returns handle to it's state
////NTaskState* NSchedulerContext::parallel(NTask* task, NRange range, bool wait)
////{
////
////}
//
//// Wait for specified task to be done
//void NSchedulerContext::wait(NTaskState* state)
//{
// // When waiting for task to be done, execute 
// // other tasks from queue in background.
// WorkerThread* worker = (WorkerThread*)m_currentState.get();
// worker->schedule(state);
//
// // This tasks state is no longer valid
// delete state;
//}
//
//// Core on which current code is executed
//uint8 NSchedulerContext::current(void)
//{
// if (m_created)
//    {
//    WorkerThread* worker = (WorkerThread*)m_currentState.get();
//    return worker->m_id;
//    }
// return 0;
//}
//
//// Count of all working cores
//uint8 NSchedulerContext::cores(void)
//{
// if (m_created)
//    return m_workers;
//
// return 0;
//}
//
//// Starts termination of Task Pool
//void NSchedulerContext::shutdown(void)
//{
// if (m_created)
//    m_shutdown = true;
//}
//
//// Returns state of scheduler
//bool NSchedulerContext::closing(void)
//{
// return m_shutdown;
//}
//
//// Wakes worker threads
//void NSchedulerContext::wakeUpSignal(void)
//{
//#ifdef EN_PLATFORM_WINDOWS
// ReleaseSemaphore(m_wakeUpSignal, m_workers-1, NULL);
//#endif
//}
