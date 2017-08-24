/*
 
 Ngine v5.0
 
 Module      : OSX specific code.
 Description : Starts execution of engine code. After 
			   all start up procedures are finished,
               it passes control to user application.
			   It also handles exit from user program
               and safe clean-up.
 
 */

#include "core/configuration.h"
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_IOS)

#if defined(EN_PLATFORM_IOS)
#import <UIKit/UIKit.h>
#endif
#if defined(EN_PLATFORM_OSX)
#import <AppKit/AppKit.h>
#include "platform/osx/AppDelegate.h"
#endif

sint32 returnValue = 0;

namespace en
{
extern void initHalfs(void);
}

#include "core/storage/storage.h"
#include "core/config/context.h"
#include "core/log/context.h"
//#include "core/rendering/context.h"
#include "core/rendering/device.h"
#include "audio/context.h"
#include "platform/context.h"
#include "threading/context.h"
#include "input/context.h"
#include "scene/context.h"

#include "core/types.h"
#include "platform/system.h"
#include "platform/osx/osx_main.h"
#include "threading/scheduler.h"

namespace en
{
   namespace input
   {
#if defined(EN_PLATFORM_OSX)
#endif
   }
}

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
#undef main
#define ConsoleMain   main




// Entry point for console applications
int ConsoleMain(int argc, const char* argv[]) 
{
//NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
//@try
{
   // Init Math
   en::initHalfs();
      
   // Init modules in proper order
   en::storage::Interface::create();
   en::ConfigContext.create(argc, argv);
   en::LogContext.create();
   en::SystemContext.create();
   en::SchedulerContext.create();

   en::gpu::GraphicAPI::create();

   en::AudioContext.create();  // TODO: Figure out why it locks
   en::input::Interface::create();
   en::StateContext.create();

   // TODO: Store input parameters somewhere (config?)

   // Connect application to Window Server and Display Server. Init global variable NSApp.
   // https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Classes/NSApplication_Class/index.html#//apple_ref/occ/cl/NSApplication
   NSApplication* application = [NSApplication sharedApplication];

   // Create custom Application Delegate and assign it to global NSApp
   AppDelegate* appDelegate = [[AppDelegate alloc] init];
   [application setDelegate:appDelegate];
   [application setActivationPolicy:NSApplicationActivationPolicyRegular];
   
   // Example "Menu" creation:
//   id menubar = [[NSMenu new] autorelease];
//   id appMenuItem = [[NSMenuItem new] autorelease];
//   [menubar addItem:appMenuItem];
//   [application setMainMenu:menubar];
//   id appMenu = [[NSMenu new] autorelease];
//   id appName = [[NSProcessInfo processInfo] processName];
//   id quitTitle = [@"Quit " stringByAppendingString:appName];
//   id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
//   [appMenu addItem:quitMenuItem];
//   [appMenuItem setSubmenu:appMenu];
//   [window setTitle:appName];
   
   // Instead of passing control to NSApp Event loop, we handle it ourselves
   //[application run];
   [application finishLaunching];

   // Main thread starts to work like other worker
   // threads, by executing application as first
   // task. When it will terminate from inside, it
   // will return here. This will allow sheduler
   // destructor to close rest of worker threads.
   en::SchedulerContext.start(new MainTask(argc,argv));
}
//@catch(NSException* exception)
//{
//   en::Log << "Application crashed with exception:" << endl;
//   en::Log << exception.reason << endl;
//}
//[pool release];
   



    
// BEGIN: OSX & IOS temporary init code



    // Set configuration variables
    // const char* appPath = getenv("HOME");
    // NcfgAppPath.assign(appPath);
#ifdef DEBUG
    // NcfgAppPath.assign("Users/ukasz/Desktop/Engine 4.0/trunk/projects/Invaders/");
#endif
    
    // TODO: There was no task pooling possible on iPhone in the past, verify if it's still true.
/*
    // We need to give whole control over app to damn OS!
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    @try
    {
        result = UIApplicationMain(argc, argv, nil, nil);
    }
    @catch(NSException* exception)
    {
        cout << "!!! CRITICAL CRASH OF APPLICATION !!!" << endl;
        cout << "Exception: " << exception.reason << endl;
    }
    [pool release];
 */
    
   
// END: OSX & IOS temporary init code

    
// Close modules in order
en::StateContext.destroy();
 
en::Input = nullptr;

//en::InputContext.destroy();
en::AudioContext.destroy();

en::Graphics = nullptr;
//en::GpuContext.destroy();
en::SchedulerContext.destroy();
en::SystemContext.destroy();
en::LogContext.destroy();
en::ConfigContext.destroy();
en::Storage = nullptr;
return returnValue;
}

#endif



// TEMP:


//   using namespace en;
//   
//   // Window and View related calls need to be executed on Main Thread !
//   
//printf("Running on Thread 0 !!!!\n");
//
//   WindowSettings settings;
//   settings.screen   = nullptr; // Primary Screen
//   settings.position = uint32v2(300, 300);
//   settings.size     = uint32v2(1440, 900);
//   settings.mode     = Windowed;
//
//   Ptr<GpuDevice> gpu = Graphics->primaryDevice();
//
//   Ptr<class Window> window = gpu->create(settings, string("Ngine 5.0"));   // remove "class" when old Rendering API Abstraction is removed
//   window->active();
//   
//   Ptr<CommandBuffer> command = gpu->createCommandBuffer();
//   
//   TextureState state(Texture2DRectangle, FormatSD_8_32_f, settings.size.width, settings.size.height);
//   Ptr<Texture> zbuffer = gpu->create(state);
//
//   Ptr<ColorAttachment> attachment = gpu->createColorAttachment(window->surface());
//   attachment->onLoad(LoadOperation::Clear, float4(1.0f, 0.5f, 0.0f, 0.0f));
//   
//   Ptr<DepthStencilAttachment> depthStencil = gpu->createDepthStencilAttachment(zbuffer, nullptr);
// 
//   Ptr<RenderPass> pass = gpu->create(attachment, depthStencil); //window->surface()
//
//   command->start();
//   command->startRenderPass(pass);
//
//   // TODO: Draw here
//
//   // Finish
//   command->endRenderPass();
//   command->commit();
//   command->waitUntilCompleted();
//   
//   pass = nullptr;
//   depthStencil = nullptr;
//   attachment = nullptr;
//   zbuffer = nullptr;
//   command = nullptr; // Destructor will commit and waitUntilCompleted
//    
//   window->display();
//
//printf("Running on Thread 0 !!!!\n");
//
//
//   Ptr<Keyboard> keyboard = nullptr;
//   if (Input->available(IO::Keyboard))
//      keyboard = Input->keyboard();
//   
//   while(!keyboard->pressed(Key::Esc))
//      {
//      Input->update();
//      
//      // TODO: Here main loop
//      }
//
//   window = nullptr;
//   gpu = nullptr;
//   
//   
//   
//   // About app main loop:
//   // http://stackoverflow.com/questions/6732400/cocoa-integrate-nsapplication-into-an-existing-c-mainloop
//   
//   // Really good minimalistic application compiled from gcc (no NIB, no PLIST):
//   // http://www.cocoawithlove.com/2010/09/minimalist-cocoa-programming.html
//
//   // TaskPool experiment with Threads:
//   // http://www.cocoawithlove.com/2010/09/overhead-of-spawning-threads.html
