/*

 Ngine v5.0
 
 Module      : Threads support.
 Requirements: none
 Description : Allows easy creation and management of threads.

*/

#include "core/parallel/psxThread.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "assert.h"

#include "threading/mutex.h"   // TODO: Should be moved to core/parallel/

#include <cpuid.h>

#define CPUID(INFO, LEAF, SUBLEAF) __cpuid_count(LEAF, SUBLEAF, INFO[0], INFO[1], INFO[2], INFO[3])

namespace en
{
   constexpr uint32 MaxThreads      = 256;

   static Mutex  lockThreadID;                 // Used when modifying thread ID translation table
   static uint32 threadsSpawned = 0;           // Amount of threads spawned since start of application
   static uint64 registeredThread[MaxThreads]; // Thread global to local ID translation table

   // There is several ways to get current thread ID:
   //
   // 1) Linux:
   // #include <sys/types.h>
   // pid_t gettid(void);
   //
   // 2) macOS:
   // pthread_threadid_np(nullptr, &tid);
   //
   // 3) BSD (?) (non mac):
   // pthread_id_np_t tid = pthread_getthreadid_np();
   //
   // 4)
   // pthread_t self = pthread_self();
   // pthread_id_np_t tid;
   // pthread_getunique_np(&self, &tid);

   uint32 currentThreadId(void)
   {
   uint64 systemThreadId;
   pthread_threadid_np(nullptr, &systemThreadId);

   // Initialize thread global ID to local ID translation table. This also means 
   // that this code is executing on main thread, and thus we can query it's 
   // global ID and init table with it.
   if (threadsSpawned == 0)   
      {
      memset(&registeredThread[0], 0, sizeof(uint64) * MaxThreads);
      registeredThread[0] = systemThreadId;
      threadsSpawned = 1;
      }

   // This thread is already created, so is already on the list. As list can only
   // grow, there is no risk of colliding with other thread being created during
   // this thread iterating over list.
   uint32 index = 0;
   for(; index<threadsSpawned; ++index)
      if (registeredThread[index] == systemThreadId)
         return index;

   // Code execution should never reach this place
   assert( 0 );
   return InvalidThreadID;

   // Alternative:
   // 
   // #include <thread>
   // 
   // std::thread::id id = std::this_thread::get_id();
   //
   // id still needs to be translated to local index that can be used to access arrays.
   }

   uint32 spawnedThreads(void)
   {
   // Initialize thread global ID to local ID translation table. This also means 
   // that this code is executing on main thread, and thus we can query it's 
   // global ID and init table with it.
   if (threadsSpawned == 0)   
      {
      memset(&registeredThread[0], 0, sizeof(uint64) * MaxThreads);
      pthread_threadid_np(nullptr, &registeredThread[0]);
      threadsSpawned = 1;
      }

   return threadsSpawned;
   }

   uint32 runningThreads(void)
   {
   // Initialize thread global ID to local ID translation table. This also means 
   // that this code is executing on main thread, and thus we can query it's 
   // global ID and init table with it.
   if (threadsSpawned == 0)   
      {
      memset(&registeredThread[0], 0, sizeof(uint64) * MaxThreads);
      pthread_threadid_np(nullptr, &registeredThread[0]);
      threadsSpawned = 1;
      }

   lockThreadID.lock();

   // Check how many threads are still running
   uint32 running = 0;
   for(uint32 i=0; i<threadsSpawned; ++i)
      if (registeredThread[i] != InvalidThreadID)
         running++;

   lockThreadID.unlock();

   return running;
   }

   uint32 currentCoreId(void)
   {
   // Linux specific implementations:
   //
   // int result = getcpu(unsigned *cpu, unsigned *node, struct getcpu_cache *tcache);
   // assert( result >= 0 );
   //
   // int result = sched_getcpu(void);
   // assert( result >= 0 );

   // WA for macOS:
   uint32 CPUInfo[4];
   CPUID(CPUInfo, 1, 0);

   if ((CPUInfo[3] & (1 << 9)) == 0) 
      { 
      // There is no APIC on chip 
      // This means that application runs on macOS, on CPU older than Pentium 4, 
      // which should never happen in real life (it's 2018 as I wrote this code).
      assert( 0 );
      }                           
        
   // CPUInfo[1] is EBX, bits 24-31 are APIC ID, identifying local executing core
   // TODO: FIXME: This will return core only on up to 8-core machines, so won't
   //              work on iMacPro or MacPro with more cores!                
   uint32 core = CPUInfo[1] >> 24;
   return core;
   }

   typedef void*(*ThreadFunctionInternal)(void* thread);

   psxThread::psxThread(ThreadFunction function, void* threadState) :
      handle(),
      mutex(PTHREAD_MUTEX_INITIALIZER),
      localState(threadState),
      index(0xFFFFFFFF),
      isSleeping(false),
      valid(false),
      Thread()
   {
   int result = pthread_attr_init(&attr);
   assert( result == 0 );
   result = pthread_create(&handle, (const pthread_attr_t*)&attr, (ThreadFunctionInternal)function, (void*)this);
   assert( result == 0 );
   if (!result)
      valid = true;

   // If this is first call to create thread since application start, initialize 
   // thread global ID to local ID translation table. This also means that this
   // code is executing on main thread, and thus we can query it's global ID and
   // init table with it.
   if (threadsSpawned == 0)   
      {
      memset(&registeredThread[0], 0, sizeof(uint64) * MaxThreads);
      pthread_threadid_np(nullptr, &registeredThread[0]);
      threadsSpawned = 1;
      }

   // Register unique thread ID for indexing into thread specific data structures.
   // Threads should never reuse destroyed thread indexes, instead threads should
   // be kept alive through whole application lifecycle, and Thread-Pool should
   // be employed.
   lockThreadID.lock();

   assert( threadsSpawned < MaxThreads );
   index = threadsSpawned;
   pthread_threadid_np(handle, &registeredThread[threadsSpawned]);
   threadsSpawned++;

   lockThreadID.unlock();
   }

   psxThread::~psxThread()
   {
   // Mark that thread as terminated
   registeredThread[index] == InvalidThreadID;

   pthread_kill(handle, 0);
   pthread_attr_destroy(&attr);
   valid = false;
   }

   void* psxThread::state(void)
   {
   return localState;
   }
   
   void psxThread::name(std::string threadName)
   {
   pthread_setname_np(threadName.c_str());
   }
   
   uint32 psxThread::id(void) 
   {
   return index;
   }

   uint64 psxThread::coresExecutionMask(void)
   {
   uint64 result = 0;

   cpu_set_t coreSet;
   int result = pthread_getaffinity_np(handle, sizeof(cpu_set_t), &coreSet);
   assert( result == 0 );

   for(uint32 i=0; i<min(64, CPU_SETSIZE); ++i)
      if (CPU_ISSET(i, &coreSet))
         setBit(result, i);

   return result;
   }

   void psxThread::executeOn(const uint64 coresMask)
   {
   cpu_set_t coreSet;
   CPU_ZERO(&coreSet);
   for(uint32 i=0; i<64; ++i)
      if (checkBit(coresMak, i))
         CPU_SET(i, &coreSet);

   int result = pthread_setaffinity_np(handle, sizeof(cpu_set_t), &coreSet);
   assert( result == 0 );
   }

   void psxThread::sleep(void)
   {
   assert( handle == pthread_self() );

   isSleeping = true;
   
   int result = pthread_cond_init(&cond, nullptr);
   assert( result == 0 );
   
   // Sleep by waiting for signal
   pthread_mutex_lock(&mutex);
   result = pthread_cond_wait(&cond, &mutex);
   assert( result == 0 );
   pthread_mutex_unlock(&mutex);
   
   // Alternative implementation:
   //pause();
   }
   
   void psxThread::wakeUp(void)
   {
   assert( handle != pthread_self() );
   
   if (!isSleeping)
      return;

   isSleeping = false;
   
   // Wake-up thread by sending signal
   pthread_cond_signal(&cond);
   
   // Alternative implementation:
   //pthread_kill(handle, SIGCONT);
   }

   bool psxThread::sleeping(void)
   {
   return isSleeping;
   }
   
   bool psxThread::working(void)
   {
   // TODO: This will fail if thread crashes, or is early terminated
   return valid;
   }

   void psxThread::exit(uint32 ret)
   {
   assert( handle == pthread_self() );

   pthread_exit(nullptr);
   }
   
   void psxThread::waitUntilCompleted(void)
   {
   assert( handle != pthread_self() );
   
   pthread_join(handle, nullptr);
   }
   
   std::unique_ptr<Thread> startThread(ThreadFunction function, void* threadState)
   {
   return std::unique_ptr<Thread>( new psxThread(function, threadState) );
   }
}
#endif

