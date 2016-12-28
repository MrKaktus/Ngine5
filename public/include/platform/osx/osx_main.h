/*

 Ngine v5.0
 
 Module      : MacOS specific code.
 Requirements: none
 Description : Starts execution of engine code. After 
               all start up procedures are finished,
               it passes control to user application.
               It also handles exit from user program
               and safe clean-up.

*/

// TODO: This should be a private engine header. Not public one!

#ifndef ENG_MACOS_MAIN
#define ENG_MACOS_MAIN

#include "core/defines.h"
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/types/basic.h"

extern sint32 returnValue;

// Handle of user console application main
extern "C++" int ApplicationMainC(int argc, const char* argv[]);

#endif

#endif
