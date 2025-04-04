/*

 Ngine v5.0

 Module      : Mutex support.
 Requirements: none
 Description : Windows mutex implementation.

*/

#include "core/parallel/mutex.h"

#if defined(EN_PLATFORM_WINDOWS)

#include <assert.h>

#include "core/log/log.h"

namespace en
{

Mutex::Mutex(void)
{
    // non-signaled - means locked
    // signaled     - means unlocked
    handle = CreateMutex(nullptr,   // Default security attributes
                         FALSE,     // No initial owner
                         nullptr);  // Name for debuging purposes
    assert(handle);
}

Mutex::~Mutex(void)
{
    if (handle)
    {
        CloseHandle(handle);
    }
}

bool Mutex::lock(void)
{
    // Currently no timeout
    DWORD waitForMilliseconds = INFINITE;
    DWORD result = WaitForSingleObject(handle, waitForMilliseconds);
    if (result == WAIT_OBJECT_0)
    {
        return true;
    }

    if (result == WAIT_TIMEOUT)
    {
        return false;
    }

    if (result == WAIT_ABANDONED)
    {
        // Mutex was not released by the thread that owned the mutex before the owning thread terminated.
        // Ownership of the mutex object is granted to the calling thread and the mutex state is set to nonsignaled (locked).
        return true; // TODO: But signal death of previous owner?
    }

    assert(result == WAIT_FAILED);
    Log << "Mutex::lock() failed with error code: "<< GetLastError() << std::endl;
    return false;
}

bool Mutex::tryLock(void)
{
    DWORD result = WaitForSingleObject(handle, 0u);
    if (result == WAIT_OBJECT_0)
    {
        return true;
    }

    if (result == WAIT_TIMEOUT)
    {
        return false;
    }

    if (result == WAIT_ABANDONED)
    {
        // Mutex ownership was transitioned from 
        // dead thread to this thread. It is locked.
        return true;
    }

    assert(result == WAIT_FAILED);
    Log << "Mutex::tryLock() failed with error code: " << GetLastError() << std::endl;
    return false;
}

bool Mutex::isLocked(void)
{
    DWORD result = WaitForSingleObject(handle, 0u);
    if (result == WAIT_OBJECT_0)
    {
        // Mutex was available so it's not locked.
        ReleaseMutex(handle);
        return false;
    }

    if (result == WAIT_ABANDONED)
    {
        // Mutex ownership was transitioned from 
        // dead thread to this thread. It was
        // locked, but we decide to properly 
        // release it and state its unlocked now.
        ReleaseMutex(handle);
        return false;
    }

    if (result == WAIT_FAILED)
    {
        // We assume its locked.
        Log << "Mutex::isLocked() failed with error code: " << GetLastError() << std::endl;
    }

    return true;
}

void Mutex::unlock(void)
{
    if (!ReleaseMutex(handle))
    {
        Log << "This thread does not own this mutex. Error code: " << GetLastError() << std::endl;
    }
}

} // en

#endif
