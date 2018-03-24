/*

 Ngine v5.0
 
 Module      : Threads support.
 Requirements: none
 Description : Allows easy creation and management of threads.

*/

#include "core/parallel/winThread.h"

#if defined(EN_PLATFORM_WINDOWS)

namespace en
{
   winThread::winThread(ThreadFunction function, void* threadState) :
      handle(INVALID_HANDLE_VALUE),
      sleepSemapthore(CreateSemaphore(nullptr, 0, 1, nullptr)),
      localState(threadState),
      isSleeping(false),
      valid(false),
      Thread()
   {
   handle = CreateThread(nullptr,                                      // Thread parameters
                         65536,                                        // Thread stack size
                         static_cast<PTHREAD_START_ROUTINE>(function), // Thread entry point
                         static_cast<LPVOID>(threadState),             // Thread startup parameters
                         0,                                            // Creation flags
                         nullptr);                                     // Thread id  (LPDWORD)(&m_id)
      
   assert( handle != INVALID_HANDLE_VALUE );
   valid = true;
   }

   winThread::~winThread() 
   {
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
   HRESULT result = SetThreadDescription(handle, threadName.c_str());  // L"name"
   }
   
   void winThread::sleep(void)
   {
   assert( handle == GetCurrentThread() );

   isSleeping = true;
   WaitForSingleObject(sleepSemapthore, INFINITE);
   }
   
   void winThread::wakeUp(void)
   {
   if (!isSleeping)
      return;

   isSleeping = false;
   ReleaseSemaphore(sleepSemapthore, 1, nullptr);
   }

   bool winThread::sleeping(void)
   {
   return isSleeping;
   }
   
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
   assert( handle == GetCurrentThread() );

   handle = INVALID_HANDLE_VALUE;
   ExitThread(ret);
   }
   
   void winThread::waitUntilCompleted(void)
   {
   assert( handle != GetCurrentThread() );
   
   WaitForSingleObject(handle, INFINITE);
   }
   
   unique_ptr<Thread> startThread(ThreadFunction function, void* threadState)
   {
   return make_unique<winThread>(function, threadState);
   }
}
#endif
