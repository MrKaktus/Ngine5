
// Blocks until acquire lock
forceinline void Nmutex::lock(void)
{
while (!tryLock());
}

// Tries to acquire lock only once
forceinline bool Nmutex::tryLock(void)
{
// InterlockedExchange returns old value of
// the variable. If variable was 0 before
// writing 1 to it, we are sure that we were
// first, so we have a lock. If there was 
// already 1 we need to wait or do something
// else.
return AtomicSwap(&m_lock, 1) == 0;
}

forceinline bool Nmutex::isLocked(void)
{
return m_lock > 0 ? true : false;
}

// Releases lock
forceinline void Nmutex::unlock(void)
{
// TODO: Can it be just m_lock = 0?
AtomicSwap(&m_lock, 0);
}