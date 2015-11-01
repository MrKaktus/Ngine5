///*
//
// Ngine v4.0
// 
// Module      : Threads support.
// Visibility  : Full version only.
// Requirements: none
// Description : Allows easy creation and management of
//               threads in safe manner.
//
//*/
//
//#include "Ngine4/threading/thread.h"
//
//// Thread constructor
//Nthread::Nthread(void) :
//#ifdef EN_PLATFORM_WINDOWS
//         m_handle(INVALID_HANDLE_VALUE),
//         m_wakeUp(INVALID_HANDLE_VALUE),
//#endif
//         //m_id(0),
//         m_sleeping(false),
//         //m_finish(false),
//         m_idle(true)
//{
//}
//
//// Thread destructor
//Nthread::~Nthread(void) 
//{
// stop();
//}
//
//// Start thread execution
//bool Nthread::start(void* function, void* params)
//{
//#ifdef EN_PLATFORM_WINDOWS
// // If thread is supposed to be already created check if it is still true
// if (!m_idle)
//    {
//    if (WaitForSingleObject(m_handle, 0) == WAIT_OBJECT_0)
//       {
//       m_handle = INVALID_HANDLE_VALUE;
//       m_idle = true;
//       }
//    }
//
// if (m_idle)
//    {
//    m_wakeUp = CreateSemaphore(NULL, 0, 1, NULL);
//    m_handle = CreateThread( NULL,                                         // Thread parameters
//                             65536,                                        // Thread stack size
//                             static_cast<PTHREAD_START_ROUTINE>(function), // Thread entry point	 
//                             static_cast<LPVOID>(params),                  // Thread startup parameters
//                             0,	                                          // Creation flags
//                             NULL                                          // Thread id                     (LPDWORD)(&m_id)
//                             );	
//    if (m_handle != INVALID_HANDLE_VALUE)
//       m_idle = false;
//    }
//#endif
// return !m_idle;
//}
//
//// Catches current thread to this class
//bool Nthread::current(void)
//{
// if (working())
//    return false;
//
// m_handle = GetCurrentThread();
// m_idle = false;
// return true;
//}
//
//// Thread goes to sleep
//bool Nthread::sleep(void)
//{
//#ifdef EN_PLATFORM_WINDOWS
// if (m_handle != GetCurrentThread())
//    return false;
//
// m_sleeping = true;
// WaitForSingleObject(m_wakeUp,INFINITE);
// return true;
//#endif
//}
//
//// Other thread wakes him up
//bool Nthread::wakeUp(void)
//{
//#ifdef EN_PLATFORM_WINDOWS
// if (!m_sleeping)
//    return false;
//
// m_sleeping = false;
// ReleaseSemaphore(m_wakeUp, 1, NULL);
// return true;
//#endif
//}
//
//// Checks thread status
//bool Nthread::working(void)
//{
// if (WaitForSingleObject(m_handle, 0) == WAIT_OBJECT_0)
//    {
//    m_handle = INVALID_HANDLE_VALUE;
//    m_idle = true;
//    return false;
//    }
// return true;
//}
//
//// Thread finishes his execution
//void Nthread::exit(uint32 ret)
//{
//#ifdef EN_PLATFORM_WINDOWS
// if (m_handle != GetCurrentThread())
//    return;
//
// m_handle = INVALID_HANDLE_VALUE;
// m_idle = true;
// ExitThread(ret);
//#endif
//}
//
//// Any thread stops thread execution
//bool Nthread::stop(void)
//{
//#ifdef EN_PLATFORM_WINDOWS
// if (working())
//    {
//    m_idle = (bool)TerminateThread( m_handle, 0 );
//    CloseHandle( m_handle );
//    m_handle = INVALID_HANDLE_VALUE;
//    }
// return m_idle;
//#endif
//}