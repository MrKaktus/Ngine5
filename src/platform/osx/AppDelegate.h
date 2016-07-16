/*

 Ngine v5.0
 
 Module      : OSX specific code.
 Description : Handles Mac/iPhone callbacks from OS
               to application.
               
*/

#import <Cocoa/Cocoa.h>


//@interface AppView : NSView
//
//@end


// Class "AppDelegate", based on NSObject that conforms to both NSApplicationDelegate and NSWindowDelegate protocol.
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>

//@property (strong) IBOutlet NSWindow *window;   // (weak) in ARC

@end

//// Class "AppWindowDelegate", based on NSObject that conforms to NSWindowDelegate protocol.
//@interface AppWindowDelegate : NSObject <NSWindowDelegate>
//
//
//@end
