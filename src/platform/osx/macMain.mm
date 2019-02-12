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
#include "platform/comMain.h"

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_IOS)

#if defined(EN_PLATFORM_IOS)
#import <UIKit/UIKit.h>
#endif
#if defined(EN_PLATFORM_OSX)
#import <AppKit/AppKit.h>
#include "platform/osx/AppDelegate.h"
#endif


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
//#include "threading/context.h"
#include "input/context.h"
#include "scene/context.h"

#include "core/types.h"
#include "platform/system.h"
#include "platform/osx/osx_main.h"
#include "parallel/scheduler.h"

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

sint32 mainResult = 0;

void taskMain(void* taskData)
{
   Arguments& arguments = *(Arguments*)(taskData);

   mainResult = ApplicationMainC(arguments.argc, arguments.argv);
}

#include "core/parallel/parallel.h" // Core - Parallel

// Entry point for console applications
int ConsoleMain(int argc, const char* argv[]) 
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    @try
    {
        // Init engine modules
        en::init(argc, argv);
    
        // Connect application to Window Server and Display Server. Init global variable NSApp.
        // https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Classes/NSApplication_Class/index.html#//apple_ref/occ/cl/NSApplication
        NSApplication* application = [NSApplication sharedApplication];
    
        // Create custom Application Delegate and assign it to global NSApp
        AppDelegate* appDelegate = allocateObjectiveC(AppDelegate);
        [application setDelegate:appDelegate];
        [application setActivationPolicy:NSApplicationActivationPolicyRegular];

/*
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
//*/

        // Give control over main thread to macOS, to send callbacks to Application Delegate
        [application run];
   
// TODO: There was no task pooling possible on iPhone in the past, verify if it's still true.
// iOS alternative: ?
// result = UIApplicationMain(argc, argv, nil, nil);

        deallocateObjectiveC(appDelegate);
    
        // Deinitialize all engine modules
        en::destroy();
    }
    @catch(NSException* exception)
    {
        en::Log << "Application crashed with exception:\n";
        en::Log << exception.reason << std::endl;
    }
    [pool release];

    return mainResult;
}

#endif





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
//   std::shared_ptr<GpuDevice> gpu = Graphics->primaryDevice();
//
//   std::shared_ptr<class Window> window = gpu->create(settings, std::string("Ngine 5.0"));   // remove "class" when old Rendering API Abstraction is removed
//   window->active();
//   
//   std::shared_ptr<CommandBuffer> command = gpu->createCommandBuffer();
//   
//   TextureState state(Texture2DRectangle, FormatSD_8_32_f, settings.size.width, settings.size.height);
//   std::shared_ptr<Texture> zbuffer = gpu->create(state);
//
//   std::shared_ptr<ColorAttachment> attachment = gpu->createColorAttachment(window->surface());
//   attachment->onLoad(LoadOperation::Clear, float4(1.0f, 0.5f, 0.0f, 0.0f));
//   
//   std::shared_ptr<DepthStencilAttachment> depthStencil = gpu->createDepthStencilAttachment(zbuffer, nullptr);
// 
//   std::shared_ptr<RenderPass> pass = gpu->create(attachment, depthStencil); //window->surface()
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
//   std::shared_ptr<Keyboard> keyboard = nullptr;
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
