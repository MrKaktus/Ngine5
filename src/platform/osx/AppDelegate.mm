/*

 Ngine v5.0
 
 Module      : OSX specific code.
 Description : Handles Mac/iPhone callbacks from OS
               to application.
               
*/
#include "platform/osx/osx_main.h"

#include "platform/osx/AppDelegate.h"

#include "input/osxInput.h"

// CODE CHANGES BEGIN

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <pthread.h>

#ifndef nullptr
#define nullptr NULL
#endif

using namespace en::input;

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
   //returnValue = ApplicationMainC(0, nullptr);
   return nullptr;
}




//@implementation AppView
//- (bool)acceptsFirstResponder:
//{
//   YES;
//}
//@end


@implementation AppDelegate

//- (bool)acceptsFirstResponder:
//{
//   YES;
//}

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
   //  Send to main app thread info about request to quit.
   // App should register this event and return control to this thread, then on it's thread close itself and return.
   Event event(AppClose);
   //shared_ptr<en::input::OSXInterface> ptr = en::ptr_reinterpret_cast<en::input::OSXInterface>(&Input);
   //ptr->callback(event);

   // TODO: There should be 2 types of events.
   // Events that are stacked on producer-consumer queue. Thread 0 (this thread) pushes them, then Thread X (app main)
   // consumes them when it processes input.
   // Events that need immediate response. These events need to have registered handling function that is called on Thread 0.
   // Such function processe input event in custom way and stores result of that processing for Thread X (app main) to use in future.
   // Should such event handling functions just packaged into Tasks and pushed to Thread/Task Pool for execution ?





    // Insert code here to tear down your application
    

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

// Dock Menu quit event
- (void)handleQuitEvent:(NSAppleEventDescriptor*)inputEvent withReplyEvent:(NSAppleEventDescriptor*)ReplyEvent
{
   //  Send to main app thread info about request to quit.
   // App should register this event and return control to this thread, then on it's thread close itself and return.
   Event event(AppClose);
   //en::InputContext.events.callback(event);
}

//@end
//@implementation AppWindowDelegate

- (NSSize)window:(NSWindow *)window willUseFullScreenContentSize:(NSSize)proposedSize
{
   NSLog(@"\n\nwindow:willUseFullScreenContentSize:");
return proposedSize;
}

- (NSApplicationPresentationOptions)window:(NSWindow *)window
      willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions
{
   NSLog(@"\n\nwindow:willUseFullScreenPresentationOptions:");

   // Allow application to enter Fullscreen mode
   NSApplicationPresentationOptions presentationOptions; // = [application currentSystemPresentationOptions];
   presentationOptions  = NSApplicationPresentationHideDock;
   presentationOptions |= NSApplicationPresentationHideMenuBar;
   presentationOptions |= NSApplicationPresentationFullScreen;
   return presentationOptions; //[application setPresentationOptions:presentationOptions]; // NSFullScreenWindowMask
}

- (void)windowWillEnterFullScreen:(NSNotification *)notification
{
   NSLog(@"\n\nwindowWillEnterFullScreen:");
   NSWindow* window = (NSWindow*)[notification object];
}

- (void)windowDidEnterFullScreen:(NSNotification *)notification
{
   NSLog(@"\n\nwindowDidEnterFullScreen:");
}

- (void)windowWillExitFullScreen:(NSNotification *)notification
{
   NSLog(@"\n\nwindowWillExitFullScreen:");
}

- (void)windowDidExitFullScreen:(NSNotification *)notification
{
   NSLog(@"\n\nwindowDidExitFullScreen:");
}

@end


//   // Allow application to enter Fullscreen mode
//   NSApplicationPresentationOptions presentationOptions; // = [application currentSystemPresentationOptions];
//   presentationOptions  = NSApplicationPresentationHideDock;
//   presentationOptions |= NSApplicationPresentationHideMenuBar;
//   presentationOptions |= NSApplicationPresentationFullScreen;
//   [application setPresentationOptions:presentationOptions]; // NSFullScreenWindowMask
//
// Swift example:
//
//class ViewController: NSViewController {
//    override func viewWillAppear() {
//        let presentationOptions: NSApplicationPresentationOptions = [
//                .HideDock                  ,   // Dock is entirely unavailable. Spotlight menu is disabled.
//                .DisableProcessSwitching   ,   // Cmd+Tab UI is disabled. All Exposé functionality is also disabled.
//                .DisableForceQuit          ,   // Cmd+Opt+Esc panel is disabled.
//                .DisableSessionTermination ,   // PowerKey panel and Restart/Shut Down/Log Out are disabled.
//                .DisableHideApplication    ,   // Application "Hide" menu item is disabled.
//                .FullScreen
//        ]
//
//        if let screen = NSScreen.mainScreen() {
//            view.enterFullScreenMode(screen,
//                withOptions: [NSFullScreenModeApplicationPresentationOptions:
//                    NSNumber(unsignedLong: presentationOptions.rawValue)])
//        }
//    }
//}
//
// Another Swift example:
//
////----------------------------------------------
//    override func viewDidAppear() {
//        let presOptions: NSApplicationPresentationOptions =
//        //----------------------------------------------
//        // These are all the options for the NSApplicationPresentationOptions
//        // BEWARE!!!
//        // Some of the Options may conflict with each other
//        //----------------------------------------------
//
//        //  .Default                   |
//        //  .AutoHideDock              |   // Dock appears when moused to
//        //  .AutoHideMenuBar           |   // Menu Bar appears when moused to
//        //  .DisableForceQuit          |   // Cmd+Opt+Esc panel is disabled
//        //  .DisableMenuBarTransparency|   // Menu Bar's transparent appearance is disabled
//        //  .FullScreen                |   // Application is in fullscreen mode
//            .HideDock                  |   // Dock is entirely unavailable. Spotlight menu is disabled.
//            .HideMenuBar               |   // Menu Bar is Disabled
//            .DisableAppleMenu          |   // All Apple menu items are disabled.
//            .DisableProcessSwitching   |   // Cmd+Tab UI is disabled. All Exposé functionality is also disabled.
//            .DisableSessionTermination |   // PowerKey panel and Restart/Shut Down/Log Out are disabled.
//            .DisableHideApplication    |   // Application "Hide" menu item is disabled.
//            .AutoHideToolbar
//
//        let optionsDictionary = [NSFullScreenModeApplicationPresentationOptions :
//            NSNumber(unsignedLong: presOptions.rawValue)]
//    
//        self.view.enterFullScreenMode(NSScreen.mainScreen()!, withOptions:optionsDictionary)
//        self.view.wantsLayer = true
//    }




