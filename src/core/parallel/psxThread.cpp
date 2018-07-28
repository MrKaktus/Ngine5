/*

 Ngine v5.0
 
 Module      : Threads support.
 Requirements: none
 Description : Allows easy creation and management of threads.

*/

#include "core/parallel/psxThread.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "assert.h"
using namespace std;

namespace en
{
   typedef void*(*ThreadFunctionInternal)(void* thread);

   psxThread::psxThread(ThreadFunction function, void* threadState) :
      handle(),
      localState(threadState),
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
   }

   psxThread::~psxThread()
   {
   pthread_kill(handle, 0);
   pthread_attr_destroy(&attr);
   valid = false;
   }

   void* psxThread::state(void)
   {
   return localState;
   }
   
   void psxThread::name(string threadName)
   {
   pthread_setname_np(threadName.c_str());
   }
   
   void psxThread::sleep(void)
   {
   assert( handle == pthread_self() );

   isSleeping = true;
   pause();   // TODO: Redo it in the future
   }
   
   void psxThread::wakeUp(void)
   {
   assert( handle != pthread_self() );
   
   if (!isSleeping)
      return;

   isSleeping = false;
   pthread_kill(handle, SIGCONT); // TODO: Redo it in the future
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
   
   unique_ptr<Thread> startThread(ThreadFunction function, void* threadState)
   {
   return unique_ptr<Thread>( new psxThread(function, threadState) );
   }
}
#endif

