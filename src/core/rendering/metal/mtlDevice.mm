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

   MetalDevice::MetalDevice() :
      device(nullptr)
   {
#if defined(EN_PLATFORM_OSX)
   api = Metal_OSX_1_0;
#elif defined(EN_PLATFORM_IOS)
   api = Metal_IOS_1_0;
#endif
   }

   MetalDevice::~MetalDevice() 
   {
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
      
   Ptr<ColorAttachment> MetalDevice::create(const TextureFormat format = FormatUnsupported, const uint32 samples)
   {
   return ptr_dynamic_cast<ColorAttachment, ColorAttachmentMTL>(new ColorAttachmentMTL(format, samples));
   }

   MetalAPI::MetalAPI(string appName) 
   {
   // IOS Native window:
   // 
   // You need to create “Single View Application”
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


// it’s called when app starts, we call our drawing function to cleat the screen
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

   // Default FBO which “default color attachment 0” is in fact texture from swap-chain
   desc = [MTLRenderPassDescriptor alloc];
   desc.colorAttachments[0].texture = drawable.texture;   // <<—— HERE ASSIGN TEXTURE 
   desc.colorAttachments[0].loadAction = MTLLoadActionClear;
   desc.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);
   desc.colorAttachments[0].storeAction = MTLStoreActionStore;

   // Command Encoder is created based on given Render Pass
   id <MTLRenderCommandEncoder> cmd = [cmdBuffer renderCommandEncoderWithDescriptor: desc];

   // Draw objects here

   //present it using: MTLCommandBuffer method  (void)presentDrawable:(id<MTLDrawable>)drawable

   // We specify implicitly when to present rendered frame ( presentWithTransaction is disabled )
   [cmdBuffer presentDrawable: drawable];    // <<———— HERE USE DRAWABLE
   [cmdBuffer commit];
   



//OS X :

   // Pick device from the list of available ones
#if defined(EN_PLATFORM_OSX)
   NSArray* devices = MTLCopyAllDevices();
   uint32 count = [devices count];
   
   id<MTLdevice> device = nullptr;
   
   if (count == 1)
     device = [devices objectAtIndex:0];
   
   device = MTLCreateSystemDefaultDevice();
   bool isLowPower = device.lowPower;
#endif

   // CREATE "VIEWPORT"

   // window <- view <- metalLayer <- device

   FCocoaWindow* window;

   const NSRect rect = NSMakeRect( 0, 0, screen.width, screen.height );
   FMetalView* view = [[FMetalView alloc] initWithFrame:rect];
   [view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];   // <<— allows window resize
   [view setWantsLayer: YES]; 

   CAMetalLayer* metalLayer = [CAMetalLayer new];

   CGFloat bgColor = { 0.0, 0.0, 0.0, 1.0 };
   metalLayer.edgeAntialiasingMask   = 0;
   metalLayer.masksToBounds          = YES;
   metalLayer.backgroundColor        = CGColorCreate( CGColorSpaceCreateDeviceRGB(), bgColor );
   metalLayer.anchorPoint            = CGPointMake( 0.5, 0.5 );
   metalLayer.frame                  = rect;
   metalLayer.magnificationFilter    = kCAFilterNearest;
   metalLayer.minificationFilter     = kCAFilterNearest;
   // App sends “draw” signal to CA to switch texture in swap-chain when it’s ready
   metalLayer.presentWithTransaction = NO;
   metalLayer.drawsAsynchronously    = YES;

   [metalLayer setDevice: device ];                                                 // <<—— pass device here
   [metalLayer setPixelFormat:MTLPixelFormatBGRA8Unorm ];
   [metalLayer setFramebufferOnly: NO ];
   [metalLayer removeAllAnimations ];

   [view setLayer: metalLayer ];
   [window setContentView: view ];
   [ [window standardWindowButton:NSWindowCloseButton] setAction:@selector(performClose:) ];

// ((CAMetalLayer*)view.layer).drawableSize =
   metalLayer.drawableSize - CGSizeMake(  0, 0, screen.width, screen.height );






   }

   MetalAPI::~MetalAPI() 
   {
   }

   }
}