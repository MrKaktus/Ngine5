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

#include "core/defines.h"
#include "input/input.h"

#include "core/storage/storage.h"
#include "core/config/context.h"
#include "core/log/context.h"
#include "core/rendering/context.h"
#include "audio/context.h"
#include "platform/context.h"
#include "input/context.h"
#include "scene/context.h"
#include "monetization/context.h"

#ifdef EN_PLATFORM_BLACKBERRY
#include "platform/blackberry/bb_main.h"  

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
   using namespace en;

   // Init modules in proper order
   en::storage::Interface::create();
   ConfigContext.create(argc, argv);
   LogContext.create();
   SystemContext.create();
   GpuContext.create();
   AudioContext.create();
   InputContext.create();
   StateContext.create();
   MonetizationContext.create();




   // Run user application
   sint32 result = ApplicationMainC(argc, argv);

//en::StateContext.create();
//
// // Get subsystems singletons handles
// NSchedulerContext& enScheduler = NSchedulerContext::getInstance();
//
// // Main thread starts to work like other worker 
// // threads, by executing application as first 
// // task. When it will terminate from inside, it 
// // will return here. This will allow sheduler 
// // destructor to close rest of worker threads.
// enScheduler.start(new MainTask(argc,argv));
//
   // Close modules in order
   MonetizationContext.destroy();
   StateContext.destroy();
   InputContext.destroy();
   AudioContext.destroy();
   GpuContext.destroy();
   SystemContext.destroy();
   LogContext.destroy();
   ConfigContext.destroy();
   Storage = nullptr;

   return result;
   }

namespace en
{
   namespace blackberry
   {

   }
}

#endif
