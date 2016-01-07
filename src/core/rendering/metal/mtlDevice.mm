/*

 Ngine v5.0
 
 Module      : Metal GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/log/log.h"
#include "core/rendering/metal/mtlDevice.h"
#include "core/utilities/memory.h"

#include "core/rendering/metal/mtlBlend.h"
#include "core/rendering/metal/mtlRenderPass.h"
#include "core/rendering/metal/mtlTexture.h"

namespace en
{
   namespace gpu
   {
   Screen::Screen() :
      SafeObject()
   {
   }
   
   WindowSettings::WindowSettings() :
      screen(nullptr),
      position(0, 0),
      size(0, 0),
      mode(BorderlessWindow)
   {
   }
      
   ScreenMTL::ScreenMTL() :
      handle(nullptr),
      Screen()
   {
   }
   
   ScreenMTL::~ScreenMTL()
   {
   handle = nullptr;
   }
   
   WindowMTL::WindowMTL(const id<MTLDevice> device, const WindowSettings& settings, const string title) :
      needNewSurface(true)
   {
   // Determine destination screen properties
   NSScreen* handle = nullptr;
   uint32v2 resolution;
   if (settings.screen)
      {
      handle     = ptr_dynamic_cast<ScreenMTL, Screen>(settings.screen)->handle;
      resolution = settings.screen->resolution;
      }
   else
      {
      // TODO: Primary display handle and properties
      // handle = ptr_dynamic_cast<ScreenMTL, Screen>(GraphicsContext.primaryScreen)->screen;
      // NSRect info = [handle convertRectToBacking:[handle frame]];
      // resolution.width  = static_cast<uint32>(info.size.width);
      // resolution.height = static_cast<uint32>(info.size.height);
      }

   // Frame position describes windows upper-left corner position, but from lower-left corner of the screen.
   NSRect frame = NSMakeRect(settings.position.x,
                             resolution.height - settings.position.y,
                             settings.size.width,
                             settings.size.height);
      
   // Determine window type
   NSUInteger style = 0;
   if (settings.mode == Windowed)
      style = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
   else
   if (settings.mode == BorderlessWindow)
      style = NSBorderlessWindowMask;
   else
      style = NSFullScreenWindowMask;

   // Create window
   window = [[NSWindow alloc] initWithContentRect:[handle convertRectFromBacking:frame]
                                                  styleMask:style
                                                    backing:NSBackingStoreBuffered
                                                      defer:NO];
      
   // Sets window title
   [window setTitle:[NSString stringWithCString:title.c_str() encoding:[NSString defaultCStringEncoding]]];
 
   // Initially window is black
   [window setBackgroundColor:[NSColor blackColor]];
    
   // Determine if window can be moved
   if (settings.mode == Windowed)
      [window setMovable:YES];
   else
      [window setMovable:NO];

   // Switch to fullscreen mode
   if (settings.mode == Fullscreen)
      [window toggleFullScreen:nil];

   // Set window to be visible
   //[window deminiaturize:NULL];
   //[window zoom:NULL];
   //[window update];
   
   
   // Window <- View <- Metal Layer <- Metal Device
   
   // Attach View to Window
   const NSRect viewFrame = NSMakeRect(0, 0, settings.size.width, settings.size.height);
   NSView* view = [[NSView alloc] initWithFrame: viewFrame];
   
   // Allow view to resize with window
   if (settings.mode == Windowed)
      [view setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
   [view setWantsLayer:YES];

   // Create Metal Layer
   layer = [CAMetalLayer new];
    
   CGFloat bgColor[] = { 0.0, 0.0, 0.0, 1.0 };
 //layer.drawableSize            = CGSizeMake(0, 0, settings.size.width, settings.size.height);
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
    
   // Attach Device to Layer
   [layer setDevice: device ];                                                 // <<-- pass device here
   [layer setPixelFormat: MTLPixelFormatBGRA8Unorm ];
   [layer setFramebufferOnly: NO ];
   [layer removeAllAnimations ];

   // Attach Layer to View
   [view setLayer: layer ];
    
   // Replace default highest level View with Metal handling one in Window ( Window <- View )
   [window setContentView: view ];
   [ [window standardWindowButton:NSWindowCloseButton] setAction:@selector(performClose:) ];
   
   // Create Framebuffer handle
   TextureState state(Texture2DRectangle, FormatBGRA_8, settings.size.width, settings.size.height);
   framebuffer = new TextureMTL(device, state, false);
   }
   
   WindowMTL::~WindowMTL()
   {
   // TODO: Release device, layer, view, window
   }
   
   void WindowMTL::active(void)
   {
   // Set window as active one
   [window makeKeyAndOrderFront:NSApp];
   }
   
   void WindowMTL::transparent(const float opacity)
   {
   if (opacity >= 1.0f)
      [window setOpaque:YES];
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
   
   bool WindowMTL::movable(void)
   {
   return [window isMovable];
   }
   
   void WindowMTL::move(const uint32v2 position)
   {
   // Reposition window.
   // Position is from lower-left corner of the screen.
   // Both position and resolution are in points, not pixels.
   NSScreen* screen = ptr_dynamic_cast<ScreenMTL, Screen>(handle)->handle;
   NSRect frame = [screen convertRectToBacking:[window frame]];
   frame.origin.x = position.x;
   frame.origin.y = handle->resolution.height - position.y;
   [window setFrame:[screen convertRectFromBacking:frame] display:YES animate:NO];
   }

   void WindowMTL::resize(const uint32v2 size)
   {
   // Resize window.
   // Both position and resolution are in points, not pixels.
   NSScreen* screen = ptr_dynamic_cast<ScreenMTL, Screen>(handle)->handle;
   NSRect frame = [screen convertRectToBacking:[window frame]];
   frame.size.width = size.width;
   frame.size.height = size.height;
   [window setFrame:[screen convertRectFromBacking:frame] display:YES animate:NO];
   }
   
   Ptr<Texture> WindowMTL::surface(void)
   {
   if (needNewSurface)
      {
      drawable            = [layer nextDrawable];
      framebuffer->handle = drawable.texture;
      needNewSurface      = false;
      }

   return ptr_dynamic_cast<Texture, TextureMTL>(framebuffer);
   }
   
   void WindowMTL::display(void)
   {
   [drawable present];
   needNewSurface = true;
   }
   
// Volumetric Lighting Integration (references)
//
// https://www.shadertoy.com/view/XlBSRz
// http://www.slideshare.net/DICEStudio/physically-based-and-unified-volumetric-rendering-in-frostbite?from_action=save
//

// Random generators:
//
// http://www.pcg-random.org
// http://www.reedbeta.com/blog/2013/01/12/quick-and-easy-gpu-random-numbers-in-d3d11/
//

// VULKAN NVidia drivers:
//
// http://forums.laptopvideo2go.com/topic/31828-v35866-windows-10-32bit-nvidia/
// http://forums.laptopvideo2go.com/topic/31826-nvidia-geforce-driver-35866-adds-vulkan-pascal-and-volta-support/
//

// D3D12 Intel's Asteroids implementation:
//
// https://github.com/GameTechDev/asteroids_d3d12/blob/master/src/asteroids_d3d12.cpp
//

// BEGIN OF LUKASZ LAZARECKI CODE:
//
// https://github.com/b005t3r/MacOSFullScreenANE/blob/master/MacOS-x86/MacOSFullScreen.h
//
//#include "MacOSFullScreen.h"

//#define EXPORT __attribute__((visibility("default")))
//
//#ifndef MacOSFullScreen_H_
//#define MacOSFullScreen_H_
////#include "FlashRuntimeExtensions.h" // should be included via the framework, but it's not picking up
//EXPORT
//void MacOSFullScreenInitializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer);
//
//EXPORT
//void MacOSFullScreenFinalizer(void* extData);
//
//#endif /* HelloANE_H_ */
//
//#include <stdlib.h>
//
////typedef char bool;
//
//   FREObject MacOSFullScreen_enableFullScreen(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
//   {
//   NSApplicationPresentationOptions presentationOptions = [NSApplication sharedApplication].presentationOptions;
//   presentationOptions |= NSApplicationPresentationFullScreen;
//   [NSApplication sharedApplication].presentationOptions = presentationOptions;
//    
//   NSWindow *mainWindow = [NSApplication sharedApplication].mainWindow;
//    
//   if (mainWindow != nil)
//      {
//      NSWindowCollectionBehavior behavior = [mainWindow collectionBehavior];
//      behavior |= NSWindowCollectionBehaviorFullScreenPrimary;
//      [mainWindow setCollectionBehavior:behavior];
//      }
//    
//   //TODO: Return a boolean instead depending on if we found the main window
//   return NULL;
//   }
//
//   FREObject MacOSFullScreen_toggleFullScreen(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
//   {
//   NSWindow *mainWindow = [NSApplication sharedApplication].mainWindow;
//    
//   if (mainWindow != nil)
//      {
//      [mainWindow toggleFullScreen:nil];
//      }
//    
//   //TODO: Return a boolean instead depending on if we found the main window
//   return NULL;
//   }
//
//   void reg(FRENamedFunction *store, int slot, const char *name, FREFunction fn)
//   {
//   store[slot].name = (const uint8_t*)name;
//   store[slot].functionData = NULL;
//   store[slot].function = fn;
//   }
//
//   void contextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctions, const FRENamedFunction** functions)
//   {
//   *numFunctions = 2;
//   FRENamedFunction* func = (FRENamedFunction*) malloc(sizeof(FRENamedFunction) * (*numFunctions));
//   *functions = func;
//    
//   reg(func, 0, "enableFullScreen", MacOSFullScreen_enableFullScreen);
//   reg(func, 1, "toggleFullScreen", MacOSFullScreen_toggleFullScreen);
//   }
//
//   void contextFinalizer(FREContext ctx)
//   {
//   return;
//   }
//
//   void MacOSFullScreenInitializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer)
//   {
//   *ctxInitializer = &contextInitializer;
//   *ctxFinalizer = &contextFinalizer;
//   *extData = NULL;
//   }
//
//   void MacOSFullScreenFinalizer(void* extData)
//   {
//   FREContext nullCTX;
//   nullCTX = 0;
//
//   contextFinalizer(nullCTX);
//   return;
//   }
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
//    // performMiniaturize: <-- simulates the user clicking the windowÕs minimize button
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
   

   bool CommandBufferMTL::startRenderPass(const Ptr<RenderPass> pass)
   {
   if (renderEncoder != nil)
      return false;
    
   const Ptr<RenderPassMTL> temp = ptr_dynamic_cast<RenderPassMTL, RenderPass>(pass);
   
   renderEncoder = [handle renderCommandEncoderWithDescriptor:temp->desc];
   return true;
   }
   
   bool CommandBufferMTL::endRenderPass(void)
   {
   if (renderEncoder == nil)
      return false;
   [renderEncoder endEncoding];
   return true;
   }
   
   void CommandBufferMTL::commit(void)
   {
   [handle commit];
   }
    
   void CommandBufferMTL::waitUntilCompleted(void)
   {
   [handle waitUntilCompleted];
   }
   
   CommandBufferMTL::~CommandBufferMTL()
   {
   [handle release];
   }
    
    
   Ptr<CommandBuffer> MetalDevice::createCommandBuffer()
   {
   
   //   // Buffers and Encoders are single time use  ( in Vulkan CommandBuffers they can be recycled / reused !!! )
//   // Multiple buffers can be created simultaneously for one queue
//   // Buffers are execyted in order in queue
//   id <MTLCommandBuffer> buffer;
   Ptr<CommandBufferMTL> buffer = new CommandBufferMTL();
   
   buffer->handle = [queue commandBuffer];
   
   return ptr_dynamic_cast<CommandBuffer, CommandBufferMTL>(buffer);
   }
    

   
   MetalDevice::MetalDevice(id<MTLDevice> _device) :
      device(_device)
   {
#if defined(EN_PLATFORM_OSX)
   api = Metal_OSX_1_0;
#elif defined(EN_PLATFORM_IOS)
   api = Metal_IOS_1_0;
#endif

   // Create commands queue
   // Metal queue is something like "universal" or "synchronous" queue in comparison to "3d / compute / dma" queues in Vulkan
   queue = [[device newCommandQueue] autorelease];
   }

   MetalDevice::~MetalDevice() 
   {
   }

   Ptr<Window> MetalDevice::create(const WindowSettings& settings, const string title)
   {
   Ptr<WindowMTL> ptr = new WindowMTL(device, settings, title);
   return ptr_dynamic_cast<Window>(ptr);
   }

   Ptr<Texture> MetalDevice::create(const TextureState state)
   {
   Ptr<Texture> texture = nullptr;
   
   texture = ptr_dynamic_cast<Texture, TextureMTL>(new TextureMTL(device, state));
   
   return texture;
   }
      



#if defined(EN_PLATFORM_WINDOWS) 
#include "core/rendering/d3d12/dx12Raster.h"
#endif
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
//#include "core/rendering/metal/mtlRaster.h"
#endif
#if defined(EN_PLATFORM_WINDOWS) 
//#include "core/rendering/vulkan/vkRaster.h"
#endif

   Ptr<RasterState> MetalDevice::create(const RasterStateInfo& state)
   {
   // TODO: Finish !
   
//   return ptr_dynamic_cast<RasterState, RasterStateD3D12>(new RasterStateD3D12(state));
//   return ptr_dynamic_cast<RasterState, RasterStateVK>(new RasterStateVK(state));

//   return ptr_dynamic_cast<RasterState, RasterStateMTL>(new RasterStateMTL(state));
   return Ptr<RasterState>(nullptr);
   }
   
   Ptr<BlendState> MetalDevice::create(const BlendStateInfo& state,
                                       const uint32 attachments,
                                       const BlendAttachmentInfo* color)
   {
   // We don't support Logic Operations for now
   // for(uint32 i=0; i<attachments; ++i)
   //    assert( !(color[0].logicOperation && color[i].blending) );
         
   return ptr_dynamic_cast<BlendState, BlendStateMTL>(new BlendStateMTL(state, attachments, color));
   }
      



   
   
   
   MetalAPI::MetalAPI(void) :
      devicesCount(0),
      preferLowPowerGPU(false)
   {
   NSArray*  allScreens    = [NSScreen screens];
   NSScreen* primaryScreen = (NSScreen*)[allScreens objectAtIndex: 0];

   // Calculate amount of available displays.
   displaysCount = 0;
   for(NSScreen* Screen in allScreens)
      if (Screen)
         displaysCount++;

   display = new ScreenMTL[displaysCount];
   virtualDisplay = new ScreenMTL();
   
   // Gather information about available displays.
   uint32 displayId = 0;
   for(NSScreen* Screen in allScreens)
      if (Screen)
         {
         NSRect info = [Screen convertRectToBacking:[Screen frame]];
         
         display[displayId].position.x        = static_cast<uint32>(info.origin.x);
         display[displayId].position.y        = static_cast<uint32>(info.origin.y);
         display[displayId].resolution.width  = static_cast<uint32>(info.size.width);
         display[displayId].resolution.height = static_cast<uint32>(info.size.height);
         display[displayId].handle            = Screen;
         
         // Bounding box containing all displays represents virtual desktop position and size in the same coordinate system.
         if (virtualDisplay->position.x > display[displayId].position.x)
            virtualDisplay->position.x = display[displayId].position.x;
         if (virtualDisplay->position.y > display[displayId].position.y)
            virtualDisplay->position.y = display[displayId].position.y;
         if ((virtualDisplay->position.x + virtualDisplay->resolution.width) <
             (display[displayId].position.x + display[displayId].resolution.width))
            virtualDisplay->resolution.width = (display[displayId].position.x + display[displayId].resolution.width) - virtualDisplay->position.x;
         if ((virtualDisplay->position.y + virtualDisplay->resolution.height) <
             (display[displayId].position.y + display[displayId].resolution.height))
            virtualDisplay->resolution.height = (display[displayId].position.y + display[displayId].resolution.height) - virtualDisplay->position.y;
            
         displayId++;
         }

   // Pick device from the list of available ones
   id<MTLDevice> primaryDevice = nullptr;
   id<MTLDevice> supportingDevice = nullptr;
   
#if defined(EN_PLATFORM_OSX)
   NSArray* devices = MTLCopyAllDevices();
   devicesCount = static_cast<uint32>([devices count]);
   
   if (devicesCount == 1)
      primaryDevice = [devices objectAtIndex:0];
   else
      {
      // We prefer High Power GPU
      if (!preferLowPowerGPU)
         {
         primaryDevice = MTLCreateSystemDefaultDevice();
         
         for(uint32 i=0; i<devicesCount; ++i)
            {
            id<MTLDevice> device = [devices objectAtIndex:i];
            if (device == primaryDevice)
               continue;
            
            // If it's configuration: Discrete GPU + Integrated GPU (headless)
            if (device.headless)
               {
               supportingDevice = device;
               break;
               }
            
            // If it's configuration: Mobile GPU + Integrated GPU (switchable)
            if (device.lowPower)
               {
               supportingDevice = device;
               break;
               }
            }
         }
      else
         {
         // It's configuration: Mobile GPU + Integrated GPU (switchable)
         // We prefer Integrated GPU, Mobile GPU can still perform as supporting one.
         for(uint32 i=0; i<devicesCount; ++i)
            {
            id<MTLDevice> device = [devices objectAtIndex:i];
            if (device.lowPower && !device.headless)
               primaryDevice = device;
               break;
            }
         assert( primaryDevice );
         for(uint32 i=0; i<devicesCount; ++i)
            {
            id<MTLDevice> device = [devices objectAtIndex:i];
            if (device != primaryDevice)
               {
               supportingDevice = device;
               break;
               }
            }
         }
      }
#else
   // On IOS devices there is always only one GPU
   devicesCount = 1;
   primaryDevice = MTLCreateSystemDefaultDevice();
#endif

   // Create device interfaces
   device[0] = ptr_dynamic_cast<GpuDevice, MetalDevice>(new MetalDevice(primaryDevice));
   if (supportingDevice)
      device[1] = ptr_dynamic_cast<GpuDevice, MetalDevice>(new MetalDevice(supportingDevice));




// ((CAMetalLayer*)view.layer).drawableSize =
//   metalLayer.drawableSize - CGSizeMake(  0, 0, screen.width, screen.height );









   
    
    
   WindowSettings settings;
 //settings.screen   =
   settings.position = uint32v2(100, 100);
   settings.size     = uint32v2(1440, 900);
   settings.mode     = Windowed;

   Ptr<Window> window = device[0]->create(settings, string("Ngine 5.0"));
    
   Ptr<CommandBuffer> command = device[0]->createCommandBuffer();
   
   //Ptr<ColorAttachment> attachment = device[0]->createColorAttachment(window->surface());

   TextureState state(Texture2DRectangle, FormatSD_8_32_f, settings.size.width, settings.size.height);
   Ptr<Texture> zbuffer = device[0]->create(state);

   Ptr<DepthStencilAttachment> depthStencil = device[0]->createDepthStencilAttachment(zbuffer, nullptr);
   
   Ptr<RenderPass> pass = device[0]->create(window->surface(), depthStencil);

   command->startRenderPass(pass);

   // TODO: Draw here

   // Finish
   command->endRenderPass();
   command->commit();
   command->waitUntilCompleted();
   command = nullptr;
    
   window->display();
   window = nullptr;

    //[device release];
    
    
    





   }

   MetalAPI::~MetalAPI() 
   {
   }

   }
}





/*
   
   // IOS Native window:
   // 
   // You need to create "Single View Application"
   // You need to link:
   // Metal.framework
   // QuartzCore.framework ( CoreAnimation )
   // 
   //             UIView
   // OpenGL <- (CALayer) -> UIKit
   // 
   //                  EAGLContext ??
   // 
   //             UIView
   // Metal <- (CAMetalLayer) -> UIKit
   //       (subclass of CALayer)
   // 
   // 
   // CAMetalLayer (CoreAnimation layer) stores swap-chain pool of Metal textures

@implementation MetalView  // <-- name of your class that inheriths from UIView

+ (id)layerClass  // <--- overload "layerClass" method to return CAMetalLayer instead of standatd CALayer
{
    return [CAMetalLayer class];

   //return [CAEAGLLayer class];  <--- for OpenGL
}
 
@end


// “init” your view layer subclass
 
- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    if ((self = [super initWithCoder:aDecoder]))
    {
        metalLayer = (CAMetalLayer *)[self layer];
        _device = MTLCreateSystemDefaultDevice();     // <<—— Choose which IG/DG device to create, or multiple ????
        metalLayer.device      = _device;
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;

        // App sends “draw” signal to CA to switch texture in swap-chain when it’s ready
        metalLayer.presentsWithTransaction = NO;
        metalLayer.drawsAsynchronously     = YES;
    }
 
    return self;
}


// it's called when app starts, we call our drawing function to cleat the screen
- (void)didMoveToWindow
{
[self redraw];
}




   // Each time IOS calls to render a frame:
   // acquire Metal texture to render to, using (id<CAMetalDrawable>)nextDrawable
   // Metal texture is packaged into CAMetalDrawable :
   
   id<CAMetalDrawable> drawable = [self.metalLayer nextDrawable];
   id<MTLTexture> texture = drawable.texture;
   
   // render content to texture using MTLRenderCommandEncoder:

   // Default FBO which "default color attachment 0" is in fact texture from swap-chain
   desc = [MTLRenderPassDescriptor alloc];
   desc.colorAttachments[0].texture = drawable.texture;   // <<-- HERE ASSIGN TEXTURE
   desc.colorAttachments[0].loadAction = MTLLoadActionClear;
   desc.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);
   desc.colorAttachments[0].storeAction = MTLStoreActionStore;

   // Command Encoder is created based on given Render Pass
   id <MTLRenderCommandEncoder> cmd = [cmdBuffer renderCommandEncoderWithDescriptor: desc];

   // Draw objects here

   //present it using: MTLCommandBuffer method  (void)presentDrawable:(id<MTLDrawable>)drawable

   // We specify implicitly when to present rendered frame ( presentWithTransaction is disabled )
   [cmdBuffer presentDrawable: drawable];    // <<----- HERE USE DRAWABLE
   [cmdBuffer commit];
*/
