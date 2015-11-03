/*

 Ngine v5.0
 
 Module      : MacOS specific code.
 Requirements: none
 Description : Handles Mac/iPhone callbacks from OS
               to application.
               
*/

#ifndef ENG_MACOS_CALLBACKS
#define ENG_MACOS_CALLBACKS

#include "core/defines.h"
#include "core/rendering/rendering.h"

#include "main.h" // TODO: Remove integration of Project and Library!!!

#if defined(EN_PLATFORM_IOS)
#import <UIKit/UIKit.h>        // For IOS
#endif
#if defined(EN_PLATFORM_OSX)
#import <AppKit/AppKit.h>      // For OS X
#endif

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
#import <QuartzCore/QuartzCore.h>

// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// (setting the view non-opaque will only work if the EAGL surface has an alpha channel).
@interface EAGLView : UIView
{    
@private
    NRenderingContext* m_gpu;
    
    BOOL animating;
    BOOL displayLinkSupported;
    NSInteger animationFrameInterval;
    // CADisplayLink will link to the main display and fire every vsync when added to a given run-loop.
    // The NSTimer class is used only as fallback when running on a pre 3.1 device where CADisplayLink
    // isn't available.
    id displayLink;
    NSTimer *animationTimer;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

// Methods of UIView that call true application methods
- (void)AppStart;
- (void)AppUpdate:(id)sender;
- (void)AppSleep;
- (void)AppWakeUp;
- (void)AppQuit;

@end
#endif

#endif