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

// system.h ???
constexpr uint32 MaxSupportedThreads = 256; /// Max count of threads supported by rendering backends (workers and IO threads)

namespace en
{
namespace system
{

enum Platform
{
    Unknown             = 0,
    PC                     ,
    Mac                    ,
    iPhone                 ,
    iPad                   ,        
};

enum OperatingSystem
{
    Windows             = 0,
    macOS                  ,
    iOS                    ,
    Android                ,
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
    Windows11              ,
};

struct Interface
{
    uint32 physicalCores(void); // Number of physical cores in the system
    uint32 logicalCores(void);  // Number of logical cores in the system
    Platform platform(void);    // Returns type of device
    OperatingSystem os(void);   // Returns type of Operating System
    Name name(void);            // Returns name of Operating System
    bool sleeping(void);        // Check if application is in sleep mode
};

} // en::system

extern system::Interface System;

} // en

#endif
