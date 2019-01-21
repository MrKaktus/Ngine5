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

#include "core/storage/storage.h"
#include "core/config/context.h"
#include "core/log/context.h"
#include "core/rendering/device.h"
#include "audio/context.h"
#include "platform/context.h"
//#include "threading/context.h"
//#include "input/context.h"
#include "resources/context.h"
#include "scene/context.h"
#include "parallel/scheduler.h"

#ifdef EN_PLATFORM_WINDOWS
#include "core/types.h"
#include "core/input/winInput.h"
#include "parallel/comScheduler.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "platform/windows/win_main.h"       

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


// Container for main task
struct Arguments
   {
   int argc;
   const char **argv;
   };

DWORD  mainThreadId;
uint32 mainResult;

void taskMain(void* taskData)
{
   Arguments& arguments = *(Arguments*)(taskData);

   mainResult = ApplicationMainC(arguments.argc, arguments.argv);
}

// Entry point for console applications
int ConsoleMain(int argc, const char **argv)
{
    // Init global variables
    //////////////////////////

    mainThreadId = GetCurrentThreadId();

    // Init Math
    en::initHalfs();

    // Console window
    ///////////////////

    // TODO: Move it to proper place in code

    // Hide console window in Release build
    #ifndef EN_DEBUG
    HWND hWnd = GetConsoleWindow();
    ShowWindow( hWnd, SW_HIDE );
    #else
    // Show console window in Debug build, and make it bigger
    HWND hWnd = GetConsoleWindow();
    
    system("mode con cols=160 lines=50");
    
    //ShowWindow( hWnd, SW_HIDE );
    //
    //BOOL WINAPI SetConsoleWindowInfo(
    //  _In_  HANDLE hConsoleOutput,
    //  _In_  BOOL bAbsolute,
    //  _In_  const SMALL_RECT *lpConsoleWindow
    //);
    #endif






// Init modules in proper order
en::storage::Interface::create();
en::ConfigContext.create(argc,argv);
en::LogContext.create();
en::SystemContext.create();


uint32 workers;
en::setConfigValue<uint32>("en.parallel.workers", workers, en::System.physicalCores(), en::System.logicalCores());
uint32 fibers;
en::setConfigValue<uint32>("en.parallel.fibers", fibers, en::FibersPerWorker, en::MaxFibersPerWorker);


/*
uint32 cores = System.cores();

// Choose number of worker threads (by default equal to logical cores count)
uint32 workers = cores;

// Worker threads count can be overriden from config file
sint64 value = workers;
Config.get("en.threading.workers", &value);
workers = static_cast<uint32>(value);

// Ensure worker threads count doesn't exceed logical cores count
if (workers > cores)
   workers = cores;
*/

en::parallel::Interface::create(workers, fibers, en::MaxTasksPerWorker);
//en::GpuContext.create();
en::gpu::GraphicAPI::create();
en::AudioContext.create();
en::input::Interface::create();
//en::ResourcesContext.create();  <-- TODO: FIXME: Creation before Window causes Vulkan to crash on NV :/

en::StateContext.create();

/*
// Main thread starts to work like other worker 
// threads, by executing application as first 
// task. When it will terminate from inside, it 
// will return here. This will allow sheduler 
// destructor to close rest of worker threads.
en::SchedulerContext.start(new MainTask(argc,argv));
*/



    // Package input arguments
    Arguments arguments;
    arguments.argc = argc;
    arguments.argv = argv;

    // Enqueue main task for execution
    en::Scheduler->run(taskMain, (void*)&arguments);

// Run main event loop and proces scheduler events dedicated to main thread 

// Main thread sleeps waiting for incoming messages.
// It is woken up by sending special message to it, that
// indicates that there are tasks to process.
// https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-postthreadmessagea



    en::input::WinInput* input = reinterpret_cast<en::input::WinInput*>(en::Input.get());

    MSG  msg;               // Message handle
    BOOL messageResult = 0; // Bool, yet can have -1, 0, >0 states

    // Query incoming messages (from system, or wakeup one from worker threads).
    // If there are no messages left, sleep to yeld CPU to worker thread.
    // For more details see:
    // https://en.wikipedia.org/wiki/Message_loop_in_Microsoft_Windows
    while((messageResult = GetMessage(&msg, nullptr, 0, 0)) != 0)
    {
        if (messageResult == -1)
        {
            // This shouldn't happened
        }
        else
        {
            // Decode first mesage after woking up, then let Input system peek remaining messages in non blocking way.
            // Once all messages are processed from all inputs, tasks dedicated to main thread will be processed,
            // and finally, main thread will sleep again on calling GetMessage().
            input->decodeMessage(msg);
            input->updateIO();
        }

        // Break processing of incoming events if application decided to quit on it's own
        if (en::Scheduler->closing())
        {
            break;
        }
    }

    // Main thread loop is terminated due to quit signal received from OS. 
    if (messageResult == 0)
    {
        assert(msg.message == WM_QUIT);
        
        // Application was requested by the system to terminate.
        // Engine notifies application about such fact, and lets it teardown.
        input->forwardEvent(new en::input::Event(en::input::AppClose));
        
        // Wait here for app to terminate
        while(!en::Scheduler->closing())
        {
            // All inputs and tasks dedicated to main thread are still processed during application shutdown
            input->update();
        }
    }


    //while(true)
    //{
    //    en::Input->update();
    //    en::Scheduler->processMainThreadTasks();
    //}



    // Close modules in reverse order to their initialization
    ///////////////////////////////////////////////////////////

en::StateContext.destroy();

//en::ResourcesContext.destroy();

en::Input = nullptr;
//en::InputContext.destroy();
en::AudioContext.destroy();
//en::GpuContext.destroy();
en::Graphics = nullptr;
en::Scheduler = nullptr;
en::SystemContext.destroy();
en::LogContext.destroy();
en::ConfigContext.destroy();
en::Storage = nullptr;

return mainResult;
}

// Entry point for Windows non-console applications
int WINAPI WindowsMain( HINSTANCE hInstance,
                        HINSTANCE hPrevInstance,
                        LPSTR     lpCmdLine,
                        int       nCmdShow )
{
uint32 ret = ApplicationMainC(0, nullptr);
 //int ret = ApplicationMainW(hInstance,NULL,lpCmdLine,SW_SHOWNORMAL);

return ret;
}
#endif
