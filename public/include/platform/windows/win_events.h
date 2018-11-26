/*

 Ngine v5.0
 
 Module      : Windows specific code.
 Requirements: none
 Description : Captures and processes 
               asynchronous callbacks from
               operating system to support
               input signals.

*/

#ifndef EN_PLATFORM_WINDOWS_EVENTS
#define EN_PLATFORM_WINDOWS_EVENTS

#include "core/defines.h"
#include "input/input.h"

#ifdef EN_PLATFORM_WINDOWS
LRESULT CALLBACK WinEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

#endif