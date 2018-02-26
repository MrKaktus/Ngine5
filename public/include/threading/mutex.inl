// Inlined: Should be in "en" namespace.

   // Blocks until acquire lock
   forceinline void Mutex::lock(void)
   {
   while (!tryLock());
   }
   
   // Tries to acquire lock only once
   forceinline bool Mutex::tryLock(void)
   {
   // AtomicSwap returns old value of the variable. If variable was 0 before 
   // writing 1 to it, we are sure that we were first, so we have a lock. If 
   // there was already 1 we need to wait or do something else.
   return AtomicSwap(&lockValue, 1) == 0;
   }
   
   forceinline bool Mutex::isLocked(void)
   {
   return lockValue > 0;
   }
   
   // Releases lock
   forceinline void Mutex::unlock(void)
   {
   // TODO: Can it be just lockValue = 0?
   AtomicSwap(&lockValue, 0);
   }