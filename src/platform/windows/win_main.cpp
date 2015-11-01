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

#include "core/defines.h"
#include "input/input.h"

namespace en
{
extern void initHalfs(void);
}

#include "core/storage/context.h"
#include "core/config/context.h"
#include "core/log/context.h"
#include "core/rendering/context.h"
#include "audio/context.h"
#include "platform/context.h"
#include "threading/context.h"
#include "input/context.h"
#include "scene/context.h"

#ifdef EN_PLATFORM_WINDOWS
#include "core/types.h"
#include "platform/windows/win_main.h"       
#include "threading/scheduler.h"

// Protects Ngine entry points from renaming
// by define's declared outside in header file.
// That defines will rename main entry points
// of user application. Then after that, this 
// two define's will safely rename Ngine entry
// points to proper one. This mechanism allows
// Ngine to gain control over user application
// on it's startup to initialize itself quietly
// and on user application exit for safe
// deinitialization.
#define ConsoleMain   main
#define WindowsMain   WinMain

// Entry point for console applications
int ConsoleMain(int argc, char **argv)
{
// Hide console window in release build
#ifndef EN_DEBUG
HWND hWnd = GetConsoleWindow();
ShowWindow( hWnd, SW_HIDE );
#endif

// Init Math
en::initHalfs();

// Init modules in proper order
en::StorageContext.create();
en::ConfigContext.create();
en::LogContext.create();
en::SystemContext.create();
en::SchedulerContext.create();
en::GpuContext.create();
en::AudioContext.create();
en::InputContext.create();

en::StateContext.create();

// Main thread starts to work like other worker 
// threads, by executing application as first 
// task. When it will terminate from inside, it 
// will return here. This will allow sheduler 
// destructor to close rest of worker threads.
en::SchedulerContext.start(new MainTask(argc,argv));

// Close modules in order
en::StateContext.destroy();

en::InputContext.destroy();
en::AudioContext.destroy();
en::GpuContext.destroy();
en::SchedulerContext.destroy();
en::SystemContext.destroy();
en::LogContext.destroy();
en::ConfigContext.destroy();
en::StorageContext.destroy();
return 0;
}

// Entry point for windows applications
int WINAPI WindowsMain( HINSTANCE hInstance,
                        HINSTANCE hPrevInstance,
                        LPSTR     lpCmdLine,
                        int       nCmdShow )
{

 uint32 ret = ApplicationMainC(0,NULL);
 //int ret = ApplicationMainW(hInstance,NULL,lpCmdLine,SW_SHOWNORMAL);

 return ret;
}
#endif