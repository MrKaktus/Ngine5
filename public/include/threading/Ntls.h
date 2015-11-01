/*

 Ngine v5.0
 
 Module      : Thread Local Storage.
 Visibility  : Any version.
 Requirements: none
 Description : Delivers easy interface for
               handling Thread Local Storage.

*/

#ifndef ENG_THREADING_TLS
#define ENG_THREADING_TLS

#include "core/defines.h"
#include "core/types.h"

// It should always be declared as static
class Ntls
      {
      private:
      bool  m_created; // Indicates if TLS is initialized
#ifdef EN_PLATFORM_WINDOWS
      DWORD m_index;   // Index in TLS pointers table
#endif

      public:
      Ntls();
     ~Ntls();

      bool  create(void);
      void  destroy(void);
      void  set(void* ptr);
      void* get(void);
      };

#endif