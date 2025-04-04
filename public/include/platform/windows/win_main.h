/*

 Ngine v5.0
 
 Module      : Windows specific code.
 Requirements: none
 Description : Declares where to search for application entry point and where to
               starts execution of application code. After all start up procedures 
               are finished, engine passes control to user application. It also 
               handles exit from user program and safe clean-up.

*/

#ifndef EN_PLATFORM_WINDOWS_MAIN
#define EN_PLATFORM_WINDOWS_MAIN

#if defined(EN_PLATFORM_WINDOWS)
// Lets Ngine know, that there is application entry point declared in application
// source code, that should be called as first task to execute. Used entry point 
// names will replace origina main and WinMain entry points in user code, so that 
// Ngine can take control over it at the beginning (by exposing it's own main and
// WinMain entry points), and initialize all required subsystems.

// Handle of user console application main
extern "C" int ApplicationMainC(int argc, const char* argv[]);

// Handle of user windows application main
extern "C" int WINAPI ApplicationMainW(HINSTANCE hInstance,     // Instance
                                       HINSTANCE hPrevInstance, // Previous Instance
                                       LPSTR lpCmdLine,         // Command Line Parameters
                                       int nCmdShow);           // Window Show State
#endif

#endif