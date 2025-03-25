/*

 Ngine v5.0
 
 Module      : Metal Window.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlWindow.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/metal/mtlDevice.h"
#include "core/rendering/metal/mtlDisplay.h"

namespace en
{
namespace gpu
{

// Window and View related calls need to be executed on Main Thread

WindowMTL::WindowMTL(const MetalDevice* gpu, const WindowSettings& settings, const std::string title) :
    window(nil),
    layer(nil),
    drawable(nil),
    framebuffer(nullptr),
    CommonWindow()
{
    _position    = settings.position;
    _mode        = settings.mode;
    verticalSync = settings.verticalSync;

    id<MTLDevice> device = gpu->device;
   
    // Determine destination screen properties
    //
    // For rendring in native resolution on Retina displays, application need to
    // opt-in to Retina support by setting in it's Info.plist file:
    //
    // <key>NSHighResolutionCapable</key>
    // <string>True</string>
    //
    NSScreen* handle = nullptr;
    uint32v2 resolution;
    if (settings.display)
    {
        _display = std::dynamic_pointer_cast<CommonDisplay>(settings.display);
      
        DisplayMTL* ptr = reinterpret_cast<DisplayMTL*>(_display.get());
        handle     = ptr->handle;
        resolution = ptr->_resolution;
        //NSRect info = [handle convertRectToBacking:[handle frame]];
        //CGFloat factor = [handle backingScaleFactor];
        //resolution.width  = static_cast<uint32>(info.size.width);
        //resolution.height = static_cast<uint32>(info.size.height);
    }
    else
    {
        // Primary display handle and properties
        _display = std::dynamic_pointer_cast<CommonDisplay>(gpu->display(0u));
      
        handle = reinterpret_cast<DisplayMTL*>(_display.get())->handle;
        NSRect info = [handle convertRectToBacking:[handle frame]];
        resolution.width  = static_cast<uint32>(info.size.width);
        resolution.height = static_cast<uint32>(info.size.height);
    }
      
    // Frame position describes windows lower-left corner position, from lower-left corner of the screen.
    NSRect frame = NSMakeRect(settings.position.x,
                              resolution.height - (settings.position.y + settings.size.height),
                              settings.size.width,
                              settings.size.height);
   
    // Determine window type
    NSUInteger style = 0;
    if (settings.mode == Windowed)
    {
        style = NSWindowStyleMaskTitled |
                NSWindowStyleMaskClosable |
                NSWindowStyleMaskMiniaturizable |
                NSWindowStyleMaskResizable;
    }
    else
    if (settings.mode == BorderlessWindow)
    {
        style = NSWindowStyleMaskBorderless;
    }
    else
    {
        style = NSWindowStyleMaskTitled |
                NSWindowStyleMaskFullScreen;
        frame.origin.x = 0;
        frame.origin.y = 0;
    }

    // Create window
    window = [[NSWindow alloc] initWithContentRect:[handle convertRectFromBacking:frame]
                                         styleMask:style
                                           backing:NSBackingStoreBuffered
                                             defer:NO];

    if (settings.mode == Fullscreen)
    {
        // This is primary Fullscreen window
        NSWindowCollectionBehavior behavior = [window collectionBehavior];
        behavior |= NSWindowCollectionBehaviorFullScreenPrimary;
        [window setCollectionBehavior:behavior];
    }

    // Set engine custom delegate to handle window events
    [window setDelegate:(id<NSWindowDelegate>)[NSApp delegate]]; //
    [window setAcceptsMouseMovedEvents:TRUE];
   
    // Sets window title
    [window setTitle:[NSString stringWithCString:title.c_str() encoding:[NSString defaultCStringEncoding]]];
 
    // Initially window is black
    [window setBackgroundColor:[NSColor blackColor]];
    
    // Determine if window can be moved
    if (settings.mode == Windowed)
    {
        [window setMovable:YES];
    }
    else
    {
        [window setMovable:NO];
    }

    // Window <- View <- Metal Layer <- Metal Device
   
    // Attach View to Window
    const NSRect viewFrame = NSMakeRect(0, 0, settings.size.width, settings.size.height);
    view = [[NSView alloc] initWithFrame: viewFrame];
   
    // Allow view to resize with window
    if (settings.mode == Windowed)
    {
        [view setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
    }
    [view setWantsLayer:YES];
   
    // Create Metal Layer
    layer = [CAMetalLayer new];
    
    CGFloat bgColor[] = { 0.0, 0.0, 0.0, 1.0 };
    layer.drawableSize            = CGSizeMake(settings.size.width, settings.size.height);
    layer.edgeAntialiasingMask    = 0;
    layer.masksToBounds           = YES;
    layer.backgroundColor         = CGColorCreate( CGColorSpaceCreateDeviceRGB(), bgColor );
    layer.anchorPoint             = CGPointMake( 0.5, 0.5 );
    layer.frame                   = viewFrame;
    layer.magnificationFilter     = kCAFilterNearest;
    layer.minificationFilter      = kCAFilterNearest;
    // App sends draw signal to CA to switch texture in swap-chain when it's ready
    layer.presentsWithTransaction = NO;
    layer.drawsAsynchronously     = YES;
    
    // TODO: Hardcoded for now!
    assert( settings.format == Format::BGRA_8 );

    // Attach Device to Layer
    [layer setDevice:device ];                                                 // <<-- pass device here
    [layer setPixelFormat:MTLPixelFormatBGRA8Unorm ];     // <--- TODO: What about HDR? 30bpp? Wide Gamut? sRGB?
    [layer setFramebufferOnly:NO ];
    [layer removeAllAnimations ];

    // Attach Layer to View
    [view setLayer: layer ];
    
    // Replace default highest level View with Metal handling one in Window ( Window <- View )
    [window setContentView: view ];
    [ [window standardWindowButton:NSWindowCloseButton] setAction:@selector(performClose:) ];
      
    // Switch to fullscreen mode
    if (settings.mode == Fullscreen)
    {
        //active();
        [window setIsVisible:TRUE];
        assert( [window canBecomeMainWindow] );
        [window makeMainWindow];
        [window toggleFullScreen:nil];
    }
      
    // Set window to be visible
    //[window deminiaturize:NULL];
    //[window zoom:NULL];
    //[window update];
   
   
    // Create Framebuffer handle
    TextureState state(TextureType::Texture2D,
                       Format::BGRA_8,
                       TextureUsage::RenderTargetWrite,
                       settings.size.width,
                       settings.size.height);
      
    framebuffer = std::make_shared<TextureMTL>(nullptr, state, false);
}
   
WindowMTL::~WindowMTL()
{
    // Present last surface if there is any
    present();
   
    deallocateObjectiveC(layer);
    deallocateObjectiveC(view);
    deallocateObjectiveC(window);
}
   
bool WindowMTL::movable(void)
{
    return [window isMovable];
}
   
void WindowMTL::move(const uint32v2 position)
{
    if (_mode != WindowMode::Windowed)
    {
        return;
    }
      
    // Reposition window.
    // Position is from lower-left corner of the screen in OSX.
    // Both position and resolution are in points, not pixels.
    DisplayMTL* metalDisplay = reinterpret_cast<DisplayMTL*>(_display.get());
   
    NSScreen* screen = metalDisplay->handle;
    NSRect frame = [screen convertRectToBacking:[window frame]];
    frame.origin.x = position.x;
    frame.origin.y = metalDisplay->_resolution.height - position.y;
    [window setFrame:[screen convertRectFromBacking:frame] display:YES animate:NO];
    _position = position;
}

void WindowMTL::resize(const uint32v2 size)
{
    if (_mode != WindowMode::Windowed)
    {
        return;
    }
      
    // Resize window.
    // Both position and resolution are in points, not pixels.
    DisplayMTL* metalDisplay = reinterpret_cast<DisplayMTL*>(_display.get());
   
    NSScreen* screen = metalDisplay->handle;
    NSRect frame = [screen convertRectToBacking:[window frame]];
    frame.size.width = size.width;
    frame.size.height = size.height;
    [window setFrame:[screen convertRectFromBacking:frame] display:YES animate:NO];
   
    // TODO: This should go in pair with Swap-Chain resizing/reinitialization!
}
   
void WindowMTL::active(void)
{
    // Main window - current active window
    // Key window  - active window on top of main window like "Save as" window that needs to be responded
    // [window makeMainWindow];
   
    // Move window up front on the desktop and make it most active one
    [window makeKeyAndOrderFront:nil]; // Why messages sender is nil and not NSApp ?
    [window setIsVisible:TRUE];
}
   
void WindowMTL::transparent(const float opacity)
{
    if (opacity >= 1.0f)
    {
        [window setOpaque:YES];
    }
    else
    {
        [window setOpaque:NO];
        [window setAlphaValue:opacity];
    }
}

void WindowMTL::opaque(void)
{
    [window setOpaque:YES];
}
   
Texture* WindowMTL::surface(const Semaphore* signalSemaphore)
{
    // signalSemaphore is ignored, as Metal API waits for presentation
    // engine to finish reading from given surface before returning it
    // in drawable.
   
    if (needNewSurface)
    {
        surfaceAcquire.lock();

        if (needNewSurface)
        {            
            drawable = [layer nextDrawable];
            assert( drawable );
         
            framebuffer->handle = [drawable.texture retain];
            assert( framebuffer->handle );
         
            needNewSurface = false;
        }

        surfaceAcquire.unlock();
    }

    return framebuffer;
}
   
void WindowMTL::present(const Semaphore* waitForSemaphore)
{
    // Does Metal ensure that CommandBuffers on Queue will be executed ?
   
    // TODO: To disable VSync, we need to have access to CommandBuffer / Queue on which rendering is performed!
    if (!needNewSurface)
    {
        [drawable present];
      
        deallocateObjectiveC(framebuffer->handle);
        deallocateObjectiveC(drawable);

        if (verticalSync)
        {
            DisplayMTL* metalDisplay = reinterpret_cast<DisplayMTL*>(_display.get());

            Time current = currentTime();
            Time nextVSyncTime = metalDisplay->nextVSyncTime[0];
            if (nextVSyncTime < current)
            {
                nextVSyncTime = metalDisplay->nextVSyncTime[1];
            }

            // It's possible that prediction of next VSync is not ready yet
            if (nextVSyncTime < currentTime())
            {
                // Some displays may not return their refresh rate,
                // in such case 60Hz is a safety fallback
                uint32 frequency = metalDisplay->refreshRate();
                if (frequency == 0)
                {
                    frequency = 60;
                }
               
                Time frameTime;
                frameTime.seconds(1.0 / double(frequency));
                nextVSyncTime += frameTime;
            }
            
            // Sleep here until VSync
            sleepUntil(nextVSyncTime);
        }

        needNewSurface = true;
        _frame++;
    }
}

// BEGIN OF LUKASZ LAZARECKI CODE:
//
// https://github.com/b005t3r/MacOSFullScreenANE/blob/master/MacOS-x86/MacOSFullScreen.h
//
// END OF LUKASZ LAZARECKI CODE


//    [_window animationResizeTime:];  // <--- sets/returns animation resize time ?
//    
//    if ([_window inLiveResize])
//    {
//    // User is currently in the process of resizing this window
//    }
//
//    - (IBAction)showAdditionalControls:sender
//    {
//        NSRect frame = [myWindow frame];
//        if (frame.size.width <= MIN_WIDTH_WITH_ADDITIONS)
//            frame.size.width = MIN_WIDTH_WITH_ADDITIONS;
//        frame.size.height += ADDITIONS_HEIGHT;
//        frame.origin.y -= ADDITIONS_HEIGHT;
//
//        [myWindow setFrame:frame display:YES animate:YES];
//        // implementation continues...
//    }
//    
  
//    // Windows Delegate - receives events
//    // windowDidResize:  <-- event when window is resized
//    
//    // setContentMinSize: <-- 1280x720 ? overriden only by offline rendering resolution
//    // setContentMaxSize: <-- set to current display native resolution, overwritten only by offline rendering resolution
//    // setContentAspectRatio: <-- window aspect ration can be different than Metal Layer one ( if screen physical aspect differs) Cinematic black bars should be then added accordingly.
//    
//    // [_window miniaturize: sender id ? ];
//    // deminiaturize:
//    // performMiniaturize: <-- simulates the user clicking the window’s minimize button
//    // setMiniwindowImage: <-- Image/Title in "mini window" ? how it differs from the dock ?
//    // setMiniwindowTitle:
//
//    // Specifying window toolbars
//    NSTitledWindowMask
//    NSClosableWindowMask
//    NSMiniaturizableWindowMask
//    NSResizableWindowMask
//    
//    NSBorderlessWindowMask
//    
//    NSFullScreenWindowMask
//    NSFullSizeContentViewWindowMask
//    
//    NSButton *closeButton = [window standardWindowButton:NSWindowCloseButton];
//    [closeButton setEnabled:NO];
  

} // en::gpu
} // en
#endif
