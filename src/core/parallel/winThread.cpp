/*

 Ngine v5.0
 
 Module      : Threads support.
 Requirements: none
 Description : Allows easy creation and management of threads.

*/

#include "core/parallel/winThread.h"

#if defined(EN_PLATFORM_WINDOWS)

#include <assert.h>

#include "threading/mutex.h"   // TODO: Should be moved to core/parallel/
#include "utilities/strings.h"

namespace en
{

/* DEPRECATED:

   constexpr uint32 MaxThreads      = 256;

   static Mutex  lockThreadID;                 // Used when modifying thread ID translation table
   static uint32 threadsSpawned = 0;           // Amount of threads spawned since start of application
   static uint32 registeredThread[MaxThreads]; // Thread global to local ID translation table

   uint32 currentThreadId(void)
   {
   DWORD systemThreadId = GetCurrentThreadId();

   // Initialize thread global ID to local ID translation table. This also means 
   // that this code is executing on main thread, and thus we can query it's 
   // global ID and init table with it.
   if (threadsSpawned == 0)   
      {
      memset(&registeredThread[0], 0, sizeof(uint32) * MaxThreads);
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
      memset(&registeredThread[0], 0, sizeof(uint32) * MaxThreads);
      registeredThread[0] = GetCurrentThreadId();
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
      memset(&registeredThread[0], 0, sizeof(uint32) * MaxThreads);
      registeredThread[0] = GetCurrentThreadId();
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
//*/

uint64 currentThreadSystemId(void)
{
    return GetCurrentThreadId();;
}

void wakeUpMainThread(void)
{
    // Send custom application message to main thread. This will wake main thread
    // up, to process this message, and thus will allow it to also process all
    // other input and tasks queued.
    PostThreadMessageA(mainThreadId, WM_APP, 0, 0);
}

void setThreadName(std::string threadName)
{
    // Sets name of current thread
   
   // Available since Windows 10 Creators Update
   // Requires latest Windows SDK, which in turn requires Visual Studio 2017

   // TODO: FIXME! Uncomment once engine migrated to VS2017!
   // HRESULT result = SetThreadDescription(GetCurrentThread(), stringToWchar(threadName.c_str(), threadName.length()));
}

uint32 currentCoreId(void)
{
    return GetCurrentProcessorNumber();
}

   winThreadContainer::winThreadContainer(ThreadFunction _function, Thread* _threadClass) :
      function(_function),
      threadClass(_threadClass)
   {
   }

   DWORD WINAPI winThreadFunctionHandler(LPVOID lpThreadParameter)
   {
   winThreadContainer* container = reinterpret_cast<winThreadContainer*>(lpThreadParameter);

   // Call actual function   
   container->function(container->threadClass);

   return 0;
   }

   winThread::winThread(ThreadFunction function, void* threadState) :
      handle(INVALID_HANDLE_VALUE),
      sleepSemaphore(CreateSemaphore(nullptr, 0, 1, nullptr)),
      timer(CreateWaitableTimer(nullptr, true, nullptr)),
      package(function, this),
      localState(threadState),
      index(0xFFFFFFFF),
    //isSleeping(false),
      valid(true),
      Thread()
   {
   assert( sleepSemaphore != INVALID_HANDLE_VALUE );
   assert( timer != INVALID_HANDLE_VALUE );
   
   handle = CreateThread(nullptr,                       // Thread parameters
                         65536,                         // Thread stack size
                         winThreadFunctionHandler,      // Thread entry point
                         static_cast<LPVOID>(&package), // Thread startup parameters
                         0,                             // Creation flags
                         nullptr);                      // Thread id  (LPDWORD)(&m_id)
      
   assert( handle != INVALID_HANDLE_VALUE );

   // If this is first call to create thread since application start, initialize 
   // thread global ID to local ID translation table. This also means that this
   // code is executing on main thread, and thus we can query it's global ID and
   // init table with it.
   if (threadsSpawned == 0)   
      {
      memset(&registeredThread[0], 0, sizeof(uint32) * MaxThreads);
      registeredThread[0] = GetCurrentThreadId();
      threadsSpawned = 1;
      }

   // Register unique thread ID for indexing into thread specific data structures.
   // Threads should never reuse destroyed thread indexes, instead threads should
   // be kept alive through whole application lifecycle, and Thread-Pool should
   // be employed.
   lockThreadID.lock();

   assert( threadsSpawned < MaxThreads );
   index = threadsSpawned;
   registeredThread[threadsSpawned] = GetThreadId(handle);
   threadsSpawned++;

   lockThreadID.unlock();
   }

   winThread::~winThread() 
   {
   // Mark that thread as terminated
   registeredThread[index] = InvalidThreadID;

   valid = !(bool)TerminateThread(handle, 0);
   CloseHandle(handle);
   CloseHandle(sleepSemaphore);
   handle = INVALID_HANDLE_VALUE;
   sleepSemaphore = INVALID_HANDLE_VALUE;
   }

   void* winThread::state(void)
   {
   return localState;
   }
   
   void winThread::name(std::string threadName)
   {
   // Available since Windows 10 Creators Update 
   // Requires latest Windows SDK, which in turn requires Visual Studio 2017

   // TODO: FIXME! Uncomment once engine migrated to VS2017!
   // HRESULT result = SetThreadDescription(handle, stringToWchar(threadName.c_str(), threadName.length()));
   }
   
   uint32 winThread::id(void) 
   {
   return index;
   }

   uint64 winThread::coresExecutionMask(void)
   {
   uint64 result = 0;

   // Receive oiginal cores mask, during setting up temporary one, then restore
   uint64 tempMask = 1;
   result = SetThreadAffinityMask(handle, tempMask);
   SetThreadAffinityMask(handle, result);

   return result;
   }

   void winThread::executeOn(const uint64 coresMask)
   {
   SetThreadAffinityMask(handle, coresMask);
   }

void winThread::sleep(void)
{
    assert( GetThreadId(handle) == GetCurrentThreadId() );

    // TODO: Possible race condition?
    //       What if this thread sets atomic to true, other thread acquires that
    //       and tries to wake up this thread before propert sleep is called?
    //isSleeping.store(true, std::memory_order_release);
   
    DWORD result = WaitForSingleObject(sleepSemaphore, INFINITE);
    assert( result == WAIT_OBJECT_0 );
}

void winThread::sleepFor(const Time time)
{
    assert( GetThreadId(handle) == GetCurrentThreadId() );
   
    // Relative time to sleep is provided as negative value in 100 nanosecond intervals.
    LARGE_INTEGER timeInterval;
    timeInterval.QuadPart = -static_cast<sint64>(time.nanoseconds() / 100);
   
    SetWaitableTimer(timer, &timeInterval, 0, NULL, NULL, 0);
   
    DWORD result = WaitForSingleObject(timer, INFINITE);
    assert( result == WAIT_OBJECT_0 );
}

void winThread::sleepUntil(const Time time)
{
    assert( GetThreadId(handle) == GetCurrentThreadId() );
    
    // Absolute time to sleep is provided as positive value in 100 nanosecond intervals.
    LARGE_INTEGER momentInTime;
    momentInTime.QuadPart =(time.nanoseconds() / 100);
   
    SetWaitableTimer(timer, &momentInTime, 0, NULL, NULL, 0);
   
    DWORD result = WaitForSingleObject(timer, INFINITE);
    assert( result == WAIT_OBJECT_0 );
}

// The time after which the state of the timer is to be set to signaled,
// in 100 nanosecond intervals. Use the format described by the FILETIME structure.
// Positive values indicate absolute time. Be sure to use a UTC-based absolute time,
// as the system uses UTC-based time internally. Negative values indicate relative
// time. The actual timer accuracy depends on the capability of your hardware.
// For more information about UTC-based time, see System Time.

void winThread::wakeUp(void)
{
    // It doesn't matter if this thread sleeps on timer or infinitely, just
    // signal both semaphores to make sure it's woken up.
    ReleaseSemaphore(sleepSemaphore, 1, nullptr);
    ReleaseSemaphore(timer, 1, nullptr);
    //isSleeping.store(false, std::memory_order_release);
}

/*
   bool winThread::sleeping(void)
   {
   return isSleeping.load(std::memory_order_acquire);
   }
*/
   bool winThread::working(void)
   {
   if (WaitForSingleObject(handle, 0) == WAIT_OBJECT_0)
      {
      handle = INVALID_HANDLE_VALUE;
      return false;
      }

   return true;
   }

   void winThread::exit(uint32 ret)
   {
   assert( GetThreadId(handle) == GetCurrentThreadId() );

   handle = INVALID_HANDLE_VALUE;
   ExitThread(ret);
   }
   
   void winThread::waitUntilCompleted(void)
   {
   assert( GetThreadId(handle) != GetCurrentThreadId() );
   
   WaitForSingleObject(handle, INFINITE);
   }
   
   std::unique_ptr<Thread> startThread(ThreadFunction function, void* threadState)
   {
   return std::make_unique<winThread>(function, threadState);
   }
}
#endif
