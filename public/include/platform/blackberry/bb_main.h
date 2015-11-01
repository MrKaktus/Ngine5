/*

 Ngine v5.0
 
 Module      : BlackBerry specific code.
 Requirements: none
 Description : Starts execution of engine code. After 
               all start up procedures are finished,
               it passes control to user application.
               It also handles exit from user program
               and safe clean-up.

*/

#ifndef EN_PLATFORM_BLACKBERRY_MAIN
#define EN_PLATFORM_BLACKBERRY_MAIN

#include "core/defines.h"
#include "core/log/log.h"

#ifdef EN_PLATFORM_BLACKBERRY
// Handle of user console application main
extern "C" int ApplicationMainC(int argc, char* argv[]);
#endif

#endif