/*

 Ngine v5.0
 
 Module      : Windows specific code.
 Requirements: none
 Description : Starts execution of engine code. After 
               all start up procedures are finished,
               it passes control to user application.
               It also handles exit from user program
               and safe clean-up.

*/

#ifndef EN_PLATFORM_WINDOWS_MAIN
#define EN_PLATFORM_WINDOWS_MAIN

#if defined(EN_PLATFORM_WINDOWS)
#include "core/defines.h"
#include "core/log/log.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Handle of user console application main
extern "C" int ApplicationMainC(int argc, const char* argv[]);

// Handle of user windows application main
extern "C" int WINAPI ApplicationMainW( HINSTANCE hInstance,     // Instance
                                        HINSTANCE hPrevInstance, // Previous Instance
                                        LPSTR lpCmdLine,         // Command Line Parameters
                                        int nCmdShow);           // Window Show State
#endif

#endif