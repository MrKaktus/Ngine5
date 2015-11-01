/*

 Ngine v4.0
 
 Module      : MacOS specific code.
 Requirements: none
 Description : Handles Mac/iPhone callbacks from OS
               to application.
               
*/

#ifndef ENG_MACOS_CALLBACKS
#define ENG_MACOS_CALLBACKS

#include "Ngine4/core/defines.h"
#if defined(ENG_MACOS) || defined(ENG_IPHONE)
#include "Ngine4/rendering/rendering.h"
#include "main.h"

#import <UIKit/UIKit.h>
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