/*

 Ngine v5.0
 
 Module      : Threading primitive.
 Visibility  : Full version only.
 Requirements: none
 Description : Supports programmer with platform independent
               implementation of mutex threading primitive.

*/

#ifndef ENG_THREADING_MUTEX
#define ENG_THREADING_MUTEX

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/aligment.h"
#include "core/threading/atomics.h"

class cachealign Nmutex
      {
      public:
      // It uses whole cache line to prevent of false sharing case.
      volatile uint32 m_lock;

      public:
      Nmutex();
     ~Nmutex();

      forceinline void lock(void);
      forceinline bool tryLock(void);
      forceinline bool isLocked(void);
      forceinline void unlock(void);
      };

#include "threading/mutex.inl"

#endif