/*

 Ngine v5.0
 
 Module      : Threads and task pooling support.
 Requirements: none
 Description : Enables easy interface to features
               of task pooling and parallel execution.
               This file contains definition of
               task that will execute application 
               main function.

*/

#ifndef ENG_THREADING_MAIN_TASK
#define ENG_THREADING_MAIN_TASK

#include "core/defines.h"
#include "core/types.h"
#include "threading/task.h"
#ifdef EN_PLATFORM_ANDROID
#include "platform/android/and_main.h"
#endif
#ifdef EN_PLATFORM_BLACKBERRY
#include "platform/blackberry/bb_main.h"
#endif
#ifdef EN_PLATFORM_OSX
#include "platform/osx/osx_main.h"
#endif
#ifdef EN_PLATFORM_WINDOWS
#include "platform/windows/win_main.h"
#endif

class MainTask : public en::threads::Task
      {
      private:
      sint32  m_argc;   // Program parameters count
      const char**  m_argv;   // Table with null terminated parameters
      uint32  m_return; // Task return status

      public:
      MainTask(int argc, const char* argv[]);
      virtual void work(void);
      uint32 returned(void);
      };

#endif