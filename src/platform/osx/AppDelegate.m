/*

 Ngine v5.0
 
 Module      : OSX specific code.
 Description : Handles Mac/iPhone callbacks from OS
               to application.
               
*/

#import "AppDelegate.h"


// CODE CHANGES BEGIN

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#ifndef nullptr
#define nullptr NULL
#endif

// The following option controls the version of unified graphics/compute language that the compiler accepts.
//
// -std=
// Determine the language revision to use, which must be one of the following values:
// ios-metal1.0 – support the unified graphics / compute language revision 1.0 programs for iOS 8.0.
// ios-metal1.1 – support the unified graphics / compute language revision 1.1 programs for iOS 9.0.
// osx-metal1.1 – support the unified graphics / compute language revision 1.1 programs for OS X.

// CODE CHANGES END



@interface AppDelegate ()

@property (strong) IBOutlet NSWindow *window;   // (weak) in ARC
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application


   
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
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




