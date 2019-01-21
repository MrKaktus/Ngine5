/*

 Ngine v5.0
 
 Module      : Platform and system capabilities.
 Requirements: none
 Description : Holds methods that support information
               about capabilities of operating system
               and hardware platform on which game is
               executed.

*/

#include "core/log/log.h"
#include "platform/context.h"

#if defined(EN_PLATFORM_ANDROID)
#include "platform/android/and_events.h"
#endif

#if defined(EN_PLATFORM_OSX)
#include <sys/sysctl.h>
#endif

#if defined(EN_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#endif

namespace en
{
   namespace system
   {
   // TODO: Refactor this whole subsystem to per OS implementation and move to CORE
   
   Context::Context() :
      physicalCores(1),
      logicalCores(1),
      platform(Unknown),
#if   defined( EN_PLATFORM_ANDROID )
      system(Android),
#elif defined( EN_PLATFORM_BLACKBERRY )
      system(BlackBerry),
#elif defined( EN_PLATFORM_IOS )
      system(iOS),
#elif defined( EN_PLATFORM_OSX )
      system(OSX),
#elif defined( EN_PLATFORM_WINDOWS )
      system(Windows),
#else
      system(Unknown)   // Compile time assert !
#endif
      name(Unsupported)
   {
   // Determine CPU cores count
   /////////////////////////////////////////////////////////////////////////////
   
#if defined(EN_PLATFORM_OSX)
    // Query count of physical cores
    size_t bufferLength = sizeof(physicalCores);
    int ret = sysctlbyname("hw.physicalcpu_max", &physicalCores, &bufferLength, nullptr, 0);
    assert( ret == 0 );

    // Query count of logical cores
    bufferLength = sizeof(logicalCores);
    ret = sysctlbyname("hw.logicalcpu_max", &logicalCores, &bufferLength, nullptr, 0);
    assert( ret == 0 );
#endif

#if defined(EN_PLATFORM_WINDOWS)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    physicalCores = si.dwNumberOfProcessors;
    logicalCores  = physicalCores;
   
/*
   // TOOD: Distinguish amount of physical & logical cores

   #define MaxCores 256;


   PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX logicalProcessorInfo[MaxCores];
   DWORD returnedSize = sizeof(logicalProcessorInfo);
   bool hyperthreading = false;

   BOOL result = GetLogicalProcessorInformationEx(RelationProcessorCore, 
                                                 &logicalProcessorInfo,
                                                 &returnedSize);
   if (result)
      {
      uint32 entries = returnedSize / sizeof(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX);
      for(uint32 i=0; i<entries; ++i)
         {
         if (logicalProcessorInfo[i].Processor.Flags == LTP_PC_SMT)
            {
            hyperthreading = true;

            string info = ""
            Log << info;
            }
         }
      };
*/

#endif

   // Determine Platform type
   /////////////////////////////////////////////////////////////////////////////
   
   // TODO: Separate concepts of:
   //       - CPU Architecture (ARM/x64)
   //       - device type (PC/iPad/iPhone)
   //       - and OS (Android, iOS, tvOS, macOS, Windows)
   
#if defined(EN_PLATFORM_ANDROID)
   platform = ARM;
   // TODO: Check if platform is not x86/x64 for rare Intel cases and Chromebooks? !!!
   
#elif defined(EN_PLATFORM_BLACKBERRY)
    platform = ARM;
   
#elif defined(EN_PLATFORM_IOS)
    platform = iPhone;
    #if (__IPHONE_OS_VERSION_MAX_ALLOWED >= 30200)
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
        platform = iPad;
    #endif
   
#elif defined(EN_PLATFORM_OSX)
    platform = PC;
   
#elif defined(EN_PLATFORM_WINDOWS)
    platform = PC;
    
#else
   assert(0);
   
#endif

   // Determine major version of OS
#ifdef EN_PLATFORM_WINDOWS
   OSVERSIONINFOEX os;
   memset(&os, 0, sizeof(OSVERSIONINFOEX));
   os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   GetVersionEx((OSVERSIONINFO*)&os);

   // Windows 10 
   if ( (name == Unsupported) && (os.dwMajorVersion == 6) && (os.dwMinorVersion == 4) )
      name = Windows10;

   // Windows 8.1
   if ( (name == Unsupported) && (os.dwMajorVersion == 6) && (os.dwMinorVersion == 3) && (os.wProductType == VER_NT_WORKSTATION) )
      name = Windows8_1;

   // Windows Server 2012 R2
   if ( (name == Unsupported) && (os.dwMajorVersion == 6) && (os.dwMinorVersion == 3) && (os.wProductType != VER_NT_WORKSTATION) )
      name = WindowsServer2012r2;

   // Windows 8
   if ( (name == Unsupported) && (os.dwMajorVersion == 6) && (os.dwMinorVersion == 2) && (os.wProductType == VER_NT_WORKSTATION) )
      name = Windows8;

   // Windows Server 2012 
   if ( (name == Unsupported) && (os.dwMajorVersion == 6) && (os.dwMinorVersion == 2) && (os.wProductType != VER_NT_WORKSTATION) )
      name = WindowsServer2012;

   // Windows 7
   if ( (name == Unsupported) && (os.dwMajorVersion == 6) && (os.dwMinorVersion == 1) && (os.wProductType == VER_NT_WORKSTATION) )
      name = Windows7;

   // Windows Server 2008 R2 
   if ( (name == Unsupported) && (os.dwMajorVersion == 6) && (os.dwMinorVersion == 1) && (os.wProductType != VER_NT_WORKSTATION) )
      name = WindowsServer2008r2;

   // Windows Server 2008
   if ( (name == Unsupported) && (os.dwMajorVersion == 6) && (os.dwMinorVersion == 0) && (os.wProductType != VER_NT_WORKSTATION) )
      name = WindowsServer2008;

   // Windows Vista
   if ( (name == Unsupported) && (os.dwMajorVersion == 6) && (os.dwMinorVersion == 0) && (os.wProductType == VER_NT_WORKSTATION) )
      name = WindowsVista;

   // Windows Server 2003 R2 
   if ( (name == Unsupported) && (os.dwMajorVersion == 5) && (os.dwMinorVersion == 2) && (GetSystemMetrics(SM_SERVERR2) != 0) )
      name = WindowsServer2003r2;

   // Windows Server 2003
   if ( (name == Unsupported) && (os.dwMajorVersion == 5) && (os.dwMinorVersion == 2) && (GetSystemMetrics(SM_SERVERR2) == 0) )
      name = WindowsServer2003;

   // Windows XP
   if ( (name == Unsupported) && (os.dwMajorVersion == 5) && (os.dwMinorVersion == 1) )
      name = WindowsXP;

   // Windows 2000
   if ( (name == Unsupported) && (os.dwMajorVersion == 5) && (os.dwMinorVersion == 0) )
      name = Windows2000;
#endif
   }

   Context::~Context()
   {
   }

   bool Context::create(void)
   {
   Log << "Starting module: System.\n";
   return true;
   }

   void Context::destroy(void)
   {
   Log << "Closing module: System.\n";
   }

   uint32 Interface::physicalCores(void)
   {
   return SystemContext.physicalCores;
   }

   uint32 Interface::logicalCores(void)
   {
   return SystemContext.logicalCores;
   }
   
   Platform Interface::platform(void)
   {
   return SystemContext.platform;
   }

   OperatingSystem Interface::os(void)
   {
   return SystemContext.system;
   }

   Name Interface::name(void)
   {
   return SystemContext.name;
   }

   }

system::Context   SystemContext;
system::Interface System;
}





