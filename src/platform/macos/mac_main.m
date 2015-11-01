/*
 
 Ngine v4.0
 
 Module      : MacOS specific code.
 Requirements: none
 Description : Starts execution of engine code. After 
			   all start up procedures are finished,
               it passes control to user application.
			   It also handles exit from user program
               and safe clean-up.
 
 */

#include "Ngine4/core/defines.h"
#if defined(ENG_MACOS) || defined(ENG_IPHONE)
#import <UIKit/UIKit.h>
#include "Ngine4/core/types.h"
#include "Ngine4/core/configuration.h"
#include "Ngine4/core/log/log.h"
#include "Ngine4/core/config/config.h"
#include "Ngine4/platform/system.h"
//#include "Ngine4/platform/windows/win_main.h"       
#include "Ngine4/platform/macos/mac_main.h"
//#include "Ngine4/threading/sheduler.h"

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
int ConsoleMain(int argc, char* argv[])
{
 // Get subsystems singletons handles
 NStorageContext& enStorage = NStorageContext::getInstance();
 NConfigContext&  enConfig  = NConfigContext::getInstance();
 NLogContext&     enLog     = NLogContext::getInstance();
 NSystemContext&  enSystem  = NSystemContext::getInstance();	
 NRenderingContext& enGpu   = NRenderingContext::getInstance();

 // Set configuration variables
 // const char* appPath = getenv("HOME");	
 // NcfgAppPath.assign(appPath);
#ifdef DEBUG
 // NcfgAppPath.assign("Users/ukasz/Desktop/Engine 4.0/trunk/projects/Invaders/");
#endif
	
 // There is no task pooling possible on iPhone.
 //NTasksContext&  enTasks  = NTasksContext::getInstance();
	
 // We need to give whole control over app to damn OS!
 NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
 uint32 retVal;
 @try
	{
	retVal = UIApplicationMain(argc, argv, nil, nil);
	}
 @catch(NSException* exception)
	{
	cout << "!!! CRITICAL CRASH OF APPLICATION !!!" << endl;
	cout << "Exception: " << exception.reason << endl;
	}
 [pool release];
    	
 return retVal;
}
#endif