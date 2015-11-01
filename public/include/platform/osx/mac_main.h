/*

 Ngine v4.0
 
 Module      : MacOS specific code.
 Requirements: none
 Description : Starts execution of engine code. After 
               all start up procedures are finished,
               it passes control to user application.
               It also handles exit from user program
               and safe clean-up.

*/

#ifndef ENG_MACOS_MAIN
#define ENG_MACOS_MAIN

#include "Ngine4/core/defines.h"
#if defined(ENG_MACOS) || defined(ENG_IPHONE)
#include "main.h"

// Handle of user console application main
extern "C++" int ApplicationMainC(int argc, char* argv[]);

#endif

#endif