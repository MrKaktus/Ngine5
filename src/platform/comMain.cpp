/*

 Ngine v5.0
 
 Module      : Main engine initialization and deinitialization path.
 Requirements: none
 Description : Starts execution of engine code. After
               all start up procedures are finished,
               it passes control to user application.
               It also handles exit from user program
               and safe clean-up.

*/


/*
#include "core/defines.h"
*/

#include "core/storage/storage.h"   // Core - Storage
#include "core/config/context.h"    // Core - Config
#include "core/log/context.h"       // Core - Log
#include "platform/context.h"       // Core - System
#include "core/parallel/parallel.h" // Core - Parallel

#include "parallel/comScheduler.h"  // Scheduler
#include "core/rendering/device.h"  // Core - Graphics
#include "core/xr/interface.h"      // Core - XR
#include "audio/context.h"          // Audio
#include "input/input.h"            // Input
//#include "resources/context.h"    // Resources
#include "scene/context.h"          // Scene

#include "platform/comMain.h"

extern void taskMain(void* taskData);

namespace en
{
extern void initHalfs(void);

/*
//#include "input/context.h"


#ifdef EN_PLATFORM_WINDOWS
#include "core/input/winInput.h"
*/

// Entry point for engine console applications
void init(int argc, const char **argv)
{
    // Init global variables
    ////////////////////////////////////////////////////////////////////////////
   
    // Init Math
    en::initHalfs();
    en::parallel::init();

    // Init modules
    ////////////////////////////////////////////////////////////////////////////
   
    en::storage::Interface::create();
    en::ConfigContext.create(argc,argv);
    en::LogContext.create();
    en::SystemContext.create();

    uint32 workers;
    en::setConfigValue<uint32>("en.parallel.workers", workers, en::System.physicalCores(), en::System.logicalCores());
    uint32 fibers;
    en::setConfigValue<uint32>("en.parallel.fibers", fibers, en::FibersPerWorker, en::MaxFibersPerWorker);

    en::parallel::Interface::create(workers, fibers, en::MaxTasksPerWorker);

    en::gpu::GraphicAPI::create();
  //en::xr::Interface::create();    <-- TODO: Disabled until rest of engine is cleaned up, brought back to function and this component is completed.
    en::AudioContext.create();
    en::input::Interface::create();
  //en::ResourcesContext.create();  <-- TODO: FIXME: Creation before Window causes Vulkan to crash on NV :/
    en::StateContext.create();

    // Enqueue main task (application main function) for execution
    en::Scheduler->run(taskMain, (void*)new Arguments(argc, argv));
   
    // Control of main thread is now returned to OS specific event handling loop.
}

void destroy(void)
{
    // Event handling loop is terminated and application is closing.
   
    // Close modules in reverse order to their initialization
    ///////////////////////////////////////////////////////////

    en::StateContext.destroy();
  //en::ResourcesContext.destroy();
    en::Input     = nullptr;
    en::AudioContext.destroy();
    en::XR        = nullptr;
    en::Graphics  = nullptr;
    en::Scheduler = nullptr;
    en::SystemContext.destroy();
    en::LogContext.destroy();
    en::ConfigContext.destroy();
    en::Storage   = nullptr;
}

}
