/*

 Ngine v5.0
 
 Module      : Androd specific code.
 Requirements: none
 Description : Starts execution of engine code. After 
               all start up procedures are finished,
               it passes control to user application.
               It also handles exit from user program
               and safe clean-up.

*/

#ifndef EN_PLATFORM_ANDROID_MAIN
#define EN_PLATFORM_ANDROID_MAIN

#ifdef EN_PLATFORM_ANDROID
#include "core/defines.h"
#include "core/log/log.h"
#include <android/native_activity.h>

// Application main entry point
void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize);

// Handle of user console application main
extern int ApplicationMainC(int argc, char* argv[]);

// Dummy function.
void dummy(void);

#endif

#endif