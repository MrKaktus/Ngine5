/*

 Ngine v5.0

 Module      : Mutex support.
 Requirements: none
 Description : Posix mutex implementation.

*/

#include "core/parallel/mutex.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
#include <assert.h>

#include "core/log/log.h"

namespace en
{

Mutex::Mutex(void) :
    handle(PTHREAD_MUTEX_INITIALIZER)
{
    int result = pthread_mutex_init(&handle, nullptr);
    
    // Removal of "unused variable" warnings
    (void)result;

    assert(result == 0);
}

Mutex::~Mutex(void)
{
    int result = pthread_mutex_destroy(&handle);

    // Removal of "unused variable" warnings
    (void)result;

    assert(result == 0);
}

bool Mutex::lock(void)
{
    // Enters critical section (thread is put to sleep if it waits for mutex)
    int result = pthread_mutex_lock(&handle);

    // Removal of "unused variable" warnings
    (void)result;

    assert(result == 0);
}

bool Mutex::tryLock(void)
{
    // Enters critical section
    int result = pthread_mutex_trylock(&handle);
    if (result == EBUSY)
    {
        return false;
    }

    return true;
}

bool Mutex::isLocked(void)
{
    // Enters critical section
    int result = pthread_mutex_trylock(&handle);
    if (result == EBUSY)
    {
        return true;
    }

    // Mutex was available so it's not locked.
    // Leave critical section
    result = pthread_mutex_unlock(&handle);
    return false;
}

void Mutex::unlock(void)
{
    // Leave critical section
    int result = pthread_mutex_unlock(&handle);

    // Removal of "unused variable" warnings
    (void)result;

    assert(result == 0);
}

} // en
#endif