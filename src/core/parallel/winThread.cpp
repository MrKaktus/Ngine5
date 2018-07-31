/*

 Ngine v5.0
 
 Module      : Threads support.
 Requirements: none
 Description : Allows easy creation and management of threads.

*/

#include "core/parallel/winThread.h"

#if defined(EN_PLATFORM_WINDOWS)

#include <assert.h>

#include "utilities/strings.h"

namespace en
{
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
      package(function, this),
      localState(threadState),
      isSleeping(false),
      valid(true),
      Thread()
   {
   handle = CreateThread(nullptr,                       // Thread parameters
                         65536,                         // Thread stack size
                         winThreadFunctionHandler,      // Thread entry point
                         static_cast<LPVOID>(&package), // Thread startup parameters
                         0,                             // Creation flags
                         nullptr);                      // Thread id  (LPDWORD)(&m_id)
      
   assert( handle != INVALID_HANDLE_VALUE );
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
   // Requires latest Windows SDK, which in turn requires Visual Studio 2017

   // TODO: FIXME! Uncomment once engine migrated to VS2017!
   // HRESULT result = SetThreadDescription(handle, stringToWchar(threadName.c_str(), threadName.length()));
   }
   
   void winThread::sleep(void)
   {
   assert( handle == GetCurrentThread() );

   isSleeping = true;
   WaitForSingleObject(sleepSemaphore, INFINITE);
   }
   
   void winThread::wakeUp(void)
   {
   if (!isSleeping)
      return;

   isSleeping = false;
   ReleaseSemaphore(sleepSemaphore, 1, nullptr);
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
