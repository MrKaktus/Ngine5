///*
//
// Ngine v5.0
// 
// Module      : Threads support.
// Visibility  : Full version only.
// Requirements: none
// Description : Allows easy creation and management of
//               threads in safe manner.
//
//*/
//
//#ifndef ENG_THREADING_THREAD
//#define ENG_THREADING_THREAD
//
//#include "core/defines.h"
//#include "core/types.h"
//#include "core/config/config.h"
//
//class cachealign Nthread
//      {
//      private:
//#ifdef EN_PLATFORM_WINDOWS
//      HANDLE  m_handle;     // Thread system id
//      HANDLE  m_wakeUp;     // Sleeping semaphore
//#endif
//      //uint32  m_id;         // Thread id
//      bool    m_sleeping;   // Sleep mode flag
//      //bool    m_finish;     // Termination indicator
//      bool    m_idle;       // Nothing doing
//
//      public:
//      Nthread(void);
//     ~Nthread(void);
//      bool start(void* function, void* params); // Thread is starting execution
//      bool current(void);    // Bind current thread to instance of this class
//      bool sleep(void);      // Threads goes to slepp mode
//      bool wakeUp(void);     // Thread is waked up by other thread
//      bool working(void);    // Check if thread still exist
//      void exit(uint32 ret); // Thread terminates its execution
//      bool stop(void);       // Thread is terminated by other thread
//      };
//
//#endif