/*

 Ngine v5.0

 Module      : Mutex support.
 Requirements: none
 Description : Allows easy creation and management of mutexes.

*/

#ifndef ENG_CORE_PARALLEL_MUTEX
#define ENG_CORE_PARALLEL_MUTEX

#include "core/defines.h"
#include "core/types.h"
#include "utilities/timer.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
#include <pthread.h>
#endif
#if defined(EN_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace en
{

class Mutex
{
private:
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
    pthread_mutex_t handle;
#endif
#if defined(EN_PLATFORM_WINDOWS)
    HANDLE handle;
#endif

public:
    Mutex(void);
   ~Mutex(void);

    bool lock(void);     ///< Locks mutex, or waits until mutex is released by other thread.
    bool tryLock(void);  ///< Tries to lock mutex, but doesn't wait if its already taken. Instead it returns false in such case.
    bool isLocked(void); ///< Returns true if mutex is currently locked.
    void unlock(void);   ///< Unlocks mutex for other threads to take ownership of it.
};

} // en
#endif
