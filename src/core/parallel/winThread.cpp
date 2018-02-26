/*

 Ngine v5.0
 
 Module      : Threads support.
 Requirements: none
 Description : Allows easy creation and management of threads.

*/

#include "core/parallel/winThread.h"

namespace en
{
   winThread::winThread() :
      handle(INVALID_HANDLE_VALUE),
      sleepSemapthore(INVALID_HANDLE_VALUE),

      //m_id(0),
      m_sleeping(false),
      //m_finish(false),
      idle(true),
      Thread()
   {
   }

   winThread::~winThread() 
   {
   stop();
   }

// Start thread execution
   bool winThread::start(void* function, void* params)
   {

   // If thread is supposed to be already created check if it is still true
   if (!idle)
      {
      if (WaitForSingleObject(handle, 0) == WAIT_OBJECT_0)
         {
         handle = INVALID_HANDLE_VALUE;
         idle = true;
         }
      }
   
   if (idle)
      {
      m_wakeUp = CreateSemaphore(NULL, 0, 1, NULL);
      m_handle = CreateThread( NULL,                                         // Thread parameters
                               65536,                                        // Thread stack size
                               static_cast<PTHREAD_START_ROUTINE>(function), // Thread entry point	 
                               static_cast<LPVOID>(params),                  // Thread startup parameters
                               0,	                                          // Creation flags
                               NULL                                          // Thread id                     (LPDWORD)(&m_id)
                               );	
      if (m_handle != INVALID_HANDLE_VALUE)
         idle = false;
      }
   
   return !idle;
   }

// Catches current thread to this class
bool winThread::current(void)
{
 if (working())
    return false;

 m_handle = GetCurrentThread();
 m_idle = false;
 return true;
}

// Thread goes to sleep
bool winThread::sleep(void)
{
 if (m_handle != GetCurrentThread())
    return false;

 m_sleeping = true;
 WaitForSingleObject(m_wakeUp,INFINITE);
 return true;
}

// Other thread wakes him up
bool winThread::wakeUp(void)
{
 if (!m_sleeping)
    return false;

 m_sleeping = false;
 ReleaseSemaphore(m_wakeUp, 1, NULL);
 return true;
}

   bool winThread::working(void)
   {
   if (WaitForSingleObject(handle, 0) == WAIT_OBJECT_0)
      {
      handle = INVALID_HANDLE_VALUE;
      idle = true;
      return false;
      }

   return true;
   }

   void winThread::exit(uint32 ret)
   {
   if (handle != GetCurrentThread())
      return;

   handle = INVALID_HANDLE_VALUE;
   idle = true;
   ExitThread(ret);
   }

   bool winThread::stop(void)
   {
   if (working())
      {
      idle = (bool)TerminateThread(handle, 0);
      CloseHandle(handle);
      handle = INVALID_HANDLE_VALUE;
      }

   return m_idle;
   }
}