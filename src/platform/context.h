/*

 Ngine v5.0
 
 Module      : Platform and system capabilities.
 Requirements: none
 Description : Holds methods that support information
               about capabilities of operating system
               and hardware platform on which game is
               executed.

*/

#ifndef ENG_SYSTEM_CONTEXT
#define ENG_SYSTEM_CONTEXT

#include "core/defines.h"
#include "core/types.h"
#include "platform/system.h"

namespace en
{
   namespace system
   { 
   struct Context
          {
          uint32          cores;
          Platform        platform;
          OperatingSystem system;
          Name            name;

          Context();
         ~Context();

          bool create(void);
          void destroy(void);
          };
   }

extern system::Context SystemContext;
}
#endif