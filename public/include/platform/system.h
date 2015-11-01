/*

 Ngine v5.0
 
 Module      : Platform and system capabilities.
 Requirements: none
 Description : Holds methods that support information
               about capabilities of operating system
               and hardware platform on which game is
               executed.

*/

#ifndef ENG_SYSTEM
#define ENG_SYSTEM

#include "core/defines.h"
#include "core/types.h"

namespace en
{
   namespace system
   { 
   enum Platform
        {
        Unknown             = 0,
        PC                     ,
        iPhone                 ,
        iPad                   ,        
        };

   enum OperatingSystem
        {
        Windows             = 0,
        Android                ,
        BlackBerry             ,
        iOS                    ,
        OSX                    ,
        };

   enum Name
        {
        Unsupported         = 0,
        Windows2000            ,
        WindowsXP              ,
        WindowsServer2003      ,
        WindowsServer2003r2    ,
        WindowsVista           ,
        WindowsServer2008      ,
        WindowsServer2008r2    ,
        Windows7               ,
        WindowsServer2012      ,
        Windows8               ,
        WindowsServer2012r2    ,
        Windows8_1             ,
        Windows10              ,
        };

   struct Interface
          {
          uint32 cores(void);       // Number of physical cores in the system
          Platform platform(void);  // Returns type of device
          OperatingSystem os(void); // Returns type of Operating System
          Name name(void);          // Returns name of Operating System
          bool sleeping(void);      // Check if application is in sleep mode
          };
   }

extern system::Interface System;
}

#endif