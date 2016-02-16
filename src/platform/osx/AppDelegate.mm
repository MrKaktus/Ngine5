/*

 Ngine v5.0
 
 Module      : OSX specific code.
 Description : Handles Mac/iPhone callbacks from OS
               to application.
               
*/
#include "platform/osx/osx_main.h"

#include "platform/osx/AppDelegate.h"

#include "input/context.h"

// CODE CHANGES BEGIN

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <pthread.h>

#ifndef nullptr
#define nullptr NULL
#endif

// The following option controls the version of unified graphics/compute language that the compiler accepts.
//
// -std=
// Determine the language revision to use, which must be one of the following values:
// ios-metal1.0 – support the unified graphics / compute language revision 1.0 programs for iOS 8.0.
// ios-metal1.1 – support the unified graphics / compute language revision 1.1 programs for iOS 9.0.
// osx-metal1.1 – support the unified graphics / compute language revision 1.1 programs for OSX 10.11.


// NSResponder - provides input events
//   NSApplication
//   NSWindow
//   NSView

// LaunchMac <- main function entry point
// UE4AppDelegate <- registers delegate (SlateViewerMainMac file stores it)
//
// FSlateApplication::Create()
// - creates per platform application class
//   - which in MacApplication.h creates Mac class, that registers to all events, and then processes them all



// NSApplication - receives events from OS, translates them and sends them to NSWindow, which sends them to NSView 

pthread_t thread;
pthread_attr_t attr;

static void* mainGameThread(void* ptr)
{
   int ret = ApplicationMainC(0, nullptr);
   return nullptr;
}

@implementation AppDelegate

- (void)applicationWillFinishLaunching:(NSNotification *)aNotification
{

}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    assert( pthread_attr_init(&attr) == 0 );
    // Spawn new thread that will handle game main loop
    assert( pthread_create(&thread, &attr, mainGameThread, nullptr) == 0 );
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
   using namespace en::input;

   //  Send to main app thread info about request to quit.
   // App should register this event and return control to this thread, then on it's thread close itself and return.
   Event event;
   memset(&event, 0, sizeof(Event));
   event.type = AppClose;
   en::InputContext.events.callback(event);

   // TODO: There should be 2 types of events.
   // Events that are stacked on producer-consumer queue. Thread 0 (this thread) pushes them, then Thread X (app main)
   // consumes them when it processes input.
   // Events that need immediate response. These events need to have registered handling function that is called on Thread 0.
   // Such function processe input event in custom way and stores result of that processing for Thread X (app main) to use in future.
   // Should such event handling functions just packaged into Tasks and pushed to Thread/Task Pool for execution ?





    // Insert code here to tear down your application
    
    self.window = nullptr;
    
    // ((CAMetalLayer*)view.layer).drawableSize =
//    metalLayer.drawableSize = CGSizeMake( screen.width, screen.height );

// IOS Stuff:
//
//    IOSAppDelegate* AppDelegate = [IOSAppDelegate GetDelegate];
//    FIOSView* GLView = AppDelegate.IOSView;
//    [GLView UpdateRenderWidth:InSizeX andHeight:InSizeY];
//
//    // check the size of the window
//    float ScalingFactor = [[IOSAppDelegate GetDelegate].IOSView contentScaleFactor];
//    CGRect ViewFrame = [[IOSAppDelegate GetDelegate].IOSView frame];
//    check(ScalingFactor * ViewFrame.size.width == InSizeX && ScalingFactor * ViewFrame.size.height == InSizeY);
//

}

@end




