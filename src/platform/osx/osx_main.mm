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

#include "core/types.h"
#include "platform/system.h"
#include "platform/osx/osx_main.h"
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
#undef main
#define ConsoleMain   main

// Entry point for console applications
int ConsoleMain(int argc, const char* argv[])
{
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

    
// BEGIN: OSX & IOS temporary init code
uint32 result = 0;

    // Set configuration variables
    // const char* appPath = getenv("HOME");
    // NcfgAppPath.assign(appPath);
#ifdef DEBUG
    // NcfgAppPath.assign("Users/ukasz/Desktop/Engine 4.0/trunk/projects/Invaders/");
#endif
    
    // TODO: There was no task pooling possible on iPhone in the past, verify if it's still true.

    // We need to give whole control over app to damn OS!
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    @try
    {
        // OLD WAY:
        //result = UIApplicationMain(argc, argv, nil, nil);
        
        // NEW WAY:
        // TODO: Needs to connect AppDelegate with call to UIApplicationMain
        [NSApplication sharedApplication];
        
        AppDelegate *appDelegate = [[AppDelegate alloc] init];
        [NSApp setDelegate:appDelegate];
        [NSApp run];
        
    }
    @catch(NSException* exception)
    {
        cout << "!!! CRITICAL CRASH OF APPLICATION !!!" << endl;
        cout << "Exception: " << exception.reason << endl;
    }
    [pool release];
    
// Main thread starts to work like other worker
// threads, by executing application as first
// task. When it will terminate from inside, it
// will return here. This will allow sheduler
// destructor to close rest of worker threads.
//en::SchedulerContext.start(new MainTask(argc,argv));
    
// END: OSX & IOS temporary init code

    
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
return result;
}

#endif