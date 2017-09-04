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

#include "core/rendering/metal/mtlDevice.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/log/log.h"
#include "core/rendering/metal/mtlAPI.h"
#include "core/utilities/memory.h"

#include "core/rendering/metal/mtlBuffer.h"

#include "core/rendering/metal/mtlBlend.h"
#include "core/rendering/metal/mtlRenderPass.h"
#include "core/rendering/metal/mtlTexture.h"
#include "core/rendering/metal/mtlRaster.h"
#include "core/rendering/metal/mtlViewport.h"
#include "core/rendering/metal/mtlShader.h"
#include "core/rendering/metal/mtlWindow.h"
#include "core/rendering/metal/mtlDisplay.h"

#include "utilities/osxStrings.h"

#if defined(EN_PLATFORM_IOS)
// For checking IOS version
#import <Foundation/Foundation.h>
#include <Availability.h>
#endif

//#include "platform/osx/AppDelegate.h"    // For registering Window Delegate

namespace en
{
   namespace gpu
   {      
   MetalDevice::MetalDevice(id<MTLDevice> _device) :
      device(_device)
   {
#if defined(EN_PLATFORM_OSX)
   api = Metal_OSX_1_0;
#elif defined(EN_PLATFORM_IOS)
   api = Metal_IOS_1_0;
#endif

   init();

   // Create commands queue
   // Metal queue is something like "universal" or "synchronous" queue in comparison to "3d / compute / dma" queues in Vulkan
   queue = [device newCommandQueue];
   }

   MetalDevice::~MetalDevice() 
   {
   }
   
   void MetalDevice::init(void)
   {
   // Metal Feature Set Tables:
   //
   // https://developer.apple.com/library/ios/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/MetalFeatureSetTables/MetalFeatureSetTables.html
   //
   
   // iPhone 5S      - A7 - PowerVR G6430
   // iPhone 6S Plus - A9 - PowerVR GT7600
   // iPad Mini 4    - A8 - PowerVR GX6450
   // iPad Air 2    - A8X - PowerVR GXA6850
   
   // Gather information about supported API capabilites
   
   // Attribute formats 
   support.attribute.set();
   support.attribute.reset(underlyingType(Attribute::u8_norm));               
   support.attribute.reset(underlyingType(Attribute::s8_norm));
   support.attribute.reset(underlyingType(Attribute::u8));
   support.attribute.reset(underlyingType(Attribute::s8));
   support.attribute.reset(underlyingType(Attribute::u16_norm));
   support.attribute.reset(underlyingType(Attribute::s16_norm));
   support.attribute.reset(underlyingType(Attribute::u16));
   support.attribute.reset(underlyingType(Attribute::s16));
   support.attribute.reset(underlyingType(Attribute::f16));
   support.attribute.reset(underlyingType(Attribute::u64));
   support.attribute.reset(underlyingType(Attribute::s64));                
   support.attribute.reset(underlyingType(Attribute::f64));
   support.attribute.reset(underlyingType(Attribute::v2u64));
   support.attribute.reset(underlyingType(Attribute::v2s64));
   support.attribute.reset(underlyingType(Attribute::v2f64));
   support.attribute.reset(underlyingType(Attribute::v3u8_srgb));
   support.attribute.reset(underlyingType(Attribute::v3u64));
   support.attribute.reset(underlyingType(Attribute::v3s64));
   support.attribute.reset(underlyingType(Attribute::v3f64));
   support.attribute.reset(underlyingType(Attribute::v4u64));
   support.attribute.reset(underlyingType(Attribute::v4s64));
   support.attribute.reset(underlyingType(Attribute::v4f64));
   support.attribute.reset(underlyingType(Attribute::v3f11_11_10));
   support.attribute.reset(underlyingType(Attribute::v4u10_10_10_2));
   support.attribute.reset(underlyingType(Attribute::v4s10_10_10_2));
   support.attribute.reset(underlyingType(Attribute::v4u10_10_10_2_norm_rev));
   support.attribute.reset(underlyingType(Attribute::v4s10_10_10_2_norm_rev));
   support.attribute.reset(underlyingType(Attribute::v4u10_10_10_2_rev));
   support.attribute.reset(underlyingType(Attribute::v4s10_10_10_2_rev));
   
   // Texture formats
   support.sampling.set();

   // TODO: Distinguish in below tables Sampling from Rendering

#if defined(EN_PLATFORM_OSX)
   // Partially suported
   if (![device isDepth24Stencil8PixelFormatSupported])
      support.sampling.reset(underlyingType(Format::DS_24_8));

   // Unsupported
   support.sampling.reset(underlyingType(Format::R_8_sRGB));
   support.sampling.reset(underlyingType(Format::RG_8_sRGB));
   support.sampling.reset(underlyingType(Format::RGB_8));
   support.sampling.reset(underlyingType(Format::RGB_8_sRGB));
   support.sampling.reset(underlyingType(Format::RGB_8_sn));
   support.sampling.reset(underlyingType(Format::RGB_8_u));
   support.sampling.reset(underlyingType(Format::RGB_8_s));
   support.sampling.reset(underlyingType(Format::RGB_16));
   support.sampling.reset(underlyingType(Format::RGB_16_sn));
   support.sampling.reset(underlyingType(Format::RGB_16_u));
   support.sampling.reset(underlyingType(Format::RGB_16_s));
   support.sampling.reset(underlyingType(Format::RGB_16_hf));
   support.sampling.reset(underlyingType(Format::RGB_32_u));
   support.sampling.reset(underlyingType(Format::RGB_32_s));
   support.sampling.reset(underlyingType(Format::RGB_32_f));
   support.sampling.reset(underlyingType(Format::D_16));
   support.sampling.reset(underlyingType(Format::D_24));
   support.sampling.reset(underlyingType(Format::D_24_8));
   support.sampling.reset(underlyingType(Format::D_32));
   support.sampling.reset(underlyingType(Format::DS_16_8));
   support.sampling.reset(underlyingType(Format::RGB_5_6_5));
   support.sampling.reset(underlyingType(Format::BGR_5_6_5));
   support.sampling.reset(underlyingType(Format::BGR_8));
   support.sampling.reset(underlyingType(Format::BGR_8_sRGB));
   support.sampling.reset(underlyingType(Format::BGR_8_sn));
   support.sampling.reset(underlyingType(Format::BGR_8_u));
   support.sampling.reset(underlyingType(Format::BGR_8_s));
   support.sampling.reset(underlyingType(Format::BGRA_8_sn));
   support.sampling.reset(underlyingType(Format::BGRA_8_u));
   support.sampling.reset(underlyingType(Format::BGRA_8_s));
   support.sampling.reset(underlyingType(Format::BGRA_5_5_5_1));
   support.sampling.reset(underlyingType(Format::ARGB_1_5_5_5));
   support.sampling.reset(underlyingType(Format::ABGR_1_5_5_5));
   support.sampling.reset(underlyingType(Format::BGRA_10_10_10_2));
   support.sampling.reset(underlyingType(Format::BC1_RGB));
   support.sampling.reset(underlyingType(Format::BC1_RGB_sRGB));
   support.sampling.reset(underlyingType(Format::BC2_RGBA_pRGB));
   support.sampling.reset(underlyingType(Format::BC3_RGBA_pRGB));
   support.sampling.reset(underlyingType(Format::ETC2_R_11));
   support.sampling.reset(underlyingType(Format::ETC2_R_11_sn));
   support.sampling.reset(underlyingType(Format::ETC2_RG_11));
   support.sampling.reset(underlyingType(Format::ETC2_RG_11_sn));
   support.sampling.reset(underlyingType(Format::ETC2_RGB_8));
   support.sampling.reset(underlyingType(Format::ETC2_RGB_8_sRGB));
   support.sampling.reset(underlyingType(Format::ETC2_RGBA_8));
   support.sampling.reset(underlyingType(Format::ETC2_RGBA_8_sRGB));
   support.sampling.reset(underlyingType(Format::ETC2_RGB8_A1));
   support.sampling.reset(underlyingType(Format::ETC2_RGB8_A1_sRGB));
   support.sampling.reset(underlyingType(Format::PVRTC_RGB_2));
   support.sampling.reset(underlyingType(Format::PVRTC_RGB_2_sRGB));
   support.sampling.reset(underlyingType(Format::PVRTC_RGB_4));
   support.sampling.reset(underlyingType(Format::PVRTC_RGB_4_sRGB));
   support.sampling.reset(underlyingType(Format::PVRTC_RGBA_2));
   support.sampling.reset(underlyingType(Format::PVRTC_RGBA_2_sRGB));
   support.sampling.reset(underlyingType(Format::PVRTC_RGBA_4));
   support.sampling.reset(underlyingType(Format::PVRTC_RGBA_4_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_4x4));
   support.sampling.reset(underlyingType(Format::ASTC_5x4));
   support.sampling.reset(underlyingType(Format::ASTC_5x5));
   support.sampling.reset(underlyingType(Format::ASTC_6x5));
   support.sampling.reset(underlyingType(Format::ASTC_6x6));
   support.sampling.reset(underlyingType(Format::ASTC_8x5));
   support.sampling.reset(underlyingType(Format::ASTC_8x6));
   support.sampling.reset(underlyingType(Format::ASTC_8x8));
   support.sampling.reset(underlyingType(Format::ASTC_10x5));
   support.sampling.reset(underlyingType(Format::ASTC_10x6));
   support.sampling.reset(underlyingType(Format::ASTC_10x8));
   support.sampling.reset(underlyingType(Format::ASTC_10x10));
   support.sampling.reset(underlyingType(Format::ASTC_12x10));
   support.sampling.reset(underlyingType(Format::ASTC_12x12));
   support.sampling.reset(underlyingType(Format::ASTC_4x4_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_5x4_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_5x5_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_6x5_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_6x6_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_8x5_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_8x6_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_8x8_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_10x5_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_10x6_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_10x8_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_10x10_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_12x10_sRGB));
   support.sampling.reset(underlyingType(Format::ASTC_12x12_sRGB));
#endif
#if defined(EN_PLATFORM_IOS)

#define SYSTEM_VERSION_EQUAL_TO(v)                  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedSame)
#define SYSTEM_VERSION_GREATER_THAN(v)              ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedDescending)
#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)
#define SYSTEM_VERSION_LESS_THAN(v)                 ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedAscending)
#define SYSTEM_VERSION_LESS_THAN_OR_EQUAL_TO(v)     ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedDescending)

   // Supported since IOS 9.0
   if (SYSTEM_VERSION_LESS_THAN(@"9.0"))
      support.sampling.reset(underlyingType(Format::DS_32_f_8));

   // Unsupported
   support.sampling.reset(underlyingType(Format::RGB_8));
   support.sampling.reset(underlyingType(Format::RGB_8_sRGB));
   support.sampling.reset(underlyingType(Format::RGB_8_sn));
   support.sampling.reset(underlyingType(Format::RGB_8_u));
   support.sampling.reset(underlyingType(Format::RGB_8_s));
   support.sampling.reset(underlyingType(Format::RGB_16));
   support.sampling.reset(underlyingType(Format::RGB_16_sn));
   support.sampling.reset(underlyingType(Format::RGB_16_u));
   support.sampling.reset(underlyingType(Format::RGB_16_s));
   support.sampling.reset(underlyingType(Format::RGB_16_hf));
   support.sampling.reset(underlyingType(Format::RGB_32_u));
   support.sampling.reset(underlyingType(Format::RGB_32_s));
   support.sampling.reset(underlyingType(Format::RGB_32_f));
   support.sampling.reset(underlyingType(Format::D_16));
   support.sampling.reset(underlyingType(Format::D_24));
   support.sampling.reset(underlyingType(Format::D_24_8));
   support.sampling.reset(underlyingType(Format::D_32));
   support.sampling.reset(underlyingType(Format::DS_16_8));
   support.sampling.reset(underlyingType(Format::DS_24_8));
   support.sampling.reset(underlyingType(Format::RGB_5_6_5));
   support.sampling.reset(underlyingType(Format::BGR_8));
   support.sampling.reset(underlyingType(Format::BGR_8_sRGB));
   support.sampling.reset(underlyingType(Format::BGR_8_sn));
   support.sampling.reset(underlyingType(Format::BGR_8_u));
   support.sampling.reset(underlyingType(Format::BGR_8_s));
   support.sampling.reset(underlyingType(Format::BGRA_8_sn));
   support.sampling.reset(underlyingType(Format::BGRA_8_u));
   support.sampling.reset(underlyingType(Format::BGRA_8_s));
   support.sampling.reset(underlyingType(Format::ARGB_1_5_5_5));
   support.sampling.reset(underlyingType(Format::BGRA_10_10_10_2));
   support.sampling.reset(underlyingType(Format::BC1_RGB));
   support.sampling.reset(underlyingType(Format::BC1_RGB_sRGB));
   support.sampling.reset(underlyingType(Format::BC1_RGBA));
   support.sampling.reset(underlyingType(Format::BC1_RGBA_sRGB));
   support.sampling.reset(underlyingType(Format::BC2_RGBA_pRGB));
   support.sampling.reset(underlyingType(Format::BC2_RGBA));
   support.sampling.reset(underlyingType(Format::BC2_RGBA_sRGB));
   support.sampling.reset(underlyingType(Format::BC3_RGBA_pRGB));
   support.sampling.reset(underlyingType(Format::BC3_RGBA));
   support.sampling.reset(underlyingType(Format::BC3_RGBA_sRGB));
   support.sampling.reset(underlyingType(Format::BC4_R));
   support.sampling.reset(underlyingType(Format::BC4_R_sn));
   support.sampling.reset(underlyingType(Format::BC5_RG));
   support.sampling.reset(underlyingType(Format::BC5_RG_sn));
   support.sampling.reset(underlyingType(Format::BC6H_RGB_f));
   support.sampling.reset(underlyingType(Format::BC6H_RGB_uf));
   support.sampling.reset(underlyingType(Format::BC7_RGBA));
   support.sampling.reset(underlyingType(Format::BC7_RGBA_sRGB));
#endif
     
   // Textures support - Render Targets
   
   // TODO: Finish
     
   // Input Assembler
   support.maxInputLayoutBuffersCount    = 31;
   support.maxInputLayoutAttributesCount = 31;

   // Texture
//   support.maxTextureSize;                    // Maximum 2D/1D texture image dimension
//   support.maxTextureRectSize;                // Maximum size of rectangle texture dimmension
//   support.maxTextureCubeSize;                // Maximum cube map texture image dimension
//   support.maxTexture3DSize;                  // Maximum 3D texture image dimension
//   support.maxTextureLayers;                  // Maximum number of layers for texture arrays
//   support.maxTextureBufferSize;              // No. of addressable texels for buffer textures
//   support.maxTextureLodBias;                 // Maximum absolute texture level of detail bias
//
//   // Sampler
//   support.maxAnisotropy;                     // Maximum anisotropic filtering factor
      
   // Rasterizer
#if defined(EN_PLATFORM_IOS)
   if ([device supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily1_v1])
      support.maxColorAttachments = 4;
   else
      support.maxColorAttachments = 8;
#endif
#if defined(EN_PLATFORM_OSX)
      support.maxColorAttachments = 8;
#endif
   
   
   //for(uint16 i=0; i<underlyingType(Attribute::Count); ++i)
   //   if (api.release >= AttributeVersion[i].release)

   //if ([device supportsFeatureSet:MTLFeatureSet_tvOS_GPUFamily2_v1])
   //   {
   //   }
   
   CommonDevice::init();
   }

   uint32 MetalDevice::displays(void) const
   {
   // Currently all Metal devices share all available displays
   Ptr<MetalAPI> api = ptr_reinterpret_cast<MetalAPI>(&en::Graphics);
   return api->displaysCount;
   }
   
   Ptr<Display> MetalDevice::display(uint32 index) const
   {
   // Currently all Metal devices share all available displays
   Ptr<MetalAPI> api = ptr_reinterpret_cast<MetalAPI>(&en::Graphics);
     
   assert( api->displaysCount > index );
   
   return ptr_reinterpret_cast<Display>(&api->_display[index]);
   }

   Ptr<Window> MetalDevice::createWindow(const WindowSettings& settings, const string title)
   {
   Ptr<WindowMTL> ptr = new WindowMTL(this, settings, title);
   return ptr_reinterpret_cast<Window>(&ptr);
   }

   uint32 MetalDevice::queues(const QueueType type) const
   {
   if (type == QueueType::Universal)
      return 1u;
   return 0u;
   }




   
   
   MetalAPI::MetalAPI(void) :
      devicesCount(0),
      preferLowPowerGPU(false),
      _display(nullptr),
      virtualDisplay(nullptr),
      displaysCount(0)
   {
   NSArray*  allScreens    = [NSScreen screens];
   NSScreen* primaryScreen = (NSScreen*)[allScreens objectAtIndex: 0];

   // Calculate amount of available displays.
   for(NSScreen* Screen in allScreens)
      if (Screen)
         displaysCount++;

   _display = new Ptr<CommonDisplay>[displaysCount];
   virtualDisplay = new CommonDisplay();
   
   // Gather information about available displays.
   uint32 displayId = 0;
   for(NSScreen* handle in allScreens)
      if (handle)
         {
         NSRect info = [handle convertRectToBacking:[handle frame]];
         
         Ptr<DisplayMTL> currentDisplay = new DisplayMTL();
 
         currentDisplay->_position.x        = static_cast<uint32>(info.origin.x);
         currentDisplay->_position.y        = static_cast<uint32>(info.origin.y);
         currentDisplay->_resolution.width  = static_cast<uint32>(info.size.width);
         currentDisplay->_resolution.height = static_cast<uint32>(info.size.height);
         currentDisplay->handle             = handle;
         
         // Calculate upper-left corner position, and size of virtual display.
         // It's assumed that X axis increases right, and Y axis increases down.
         // Virtual Display is a bounding box for all available displays.
         if (displayId == 0)
            {
            virtualDisplay->_position   = currentDisplay->_position;
            virtualDisplay->_resolution = currentDisplay->_resolution;
            }
         else
            {
            if (currentDisplay->_position.x < virtualDisplay->_position.x)
               {
               virtualDisplay->_resolution.width += (virtualDisplay->_position.x - currentDisplay->_position.x);
               virtualDisplay->_position.x = currentDisplay->_position.x;
               }
            if (currentDisplay->_position.y < virtualDisplay->_position.y)
               {
               virtualDisplay->_resolution.height += (virtualDisplay->_position.y - currentDisplay->_position.y);
               virtualDisplay->_position.y = currentDisplay->_position.y;
               }
            uint32 virtualRightBorder = virtualDisplay->_position.x + virtualDisplay->_resolution.width;
            uint32 currentRightBorder = currentDisplay->_position.x + currentDisplay->_resolution.width;
            if (virtualRightBorder < currentRightBorder)
               virtualDisplay->_resolution.width = currentRightBorder - virtualDisplay->_position.x;
            uint32 virtualBottomBorder = virtualDisplay->_position.y + virtualDisplay->_resolution.height;
            uint32 currentBottomBorder = currentDisplay->_position.y + currentDisplay->_resolution.height;
            if (virtualBottomBorder < currentBottomBorder)
               virtualDisplay->_resolution.height = currentBottomBorder - virtualDisplay->_position.y;
            }
            
         _display[displayId] = ptr_reinterpret_cast<CommonDisplay>(&currentDisplay);
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
               {
               primaryDevice = device;
               break;
               }
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
   Ptr<MetalDevice> ptr = new MetalDevice(primaryDevice);
   device[0] = ptr_reinterpret_cast<GpuDevice>(&ptr);
   if (supportingDevice)
      {
      Ptr<MetalDevice> ptr = new MetalDevice(supportingDevice);
      ptr->init();
      device[1] = ptr_reinterpret_cast<GpuDevice>(&ptr);
      }



// ((CAMetalLayer*)view.layer).drawableSize =
//   metalLayer.drawableSize - CGSizeMake(  0, 0, screen.width, screen.height );





   }
   
   RenderingAPI MetalAPI::type(void) const
   {
   return RenderingAPI::Metal;
   }

   uint32 MetalAPI::devices(void) const
   {
   return devicesCount;
   }
      
   Ptr<GpuDevice> MetalAPI::primaryDevice(void) const
   {
   return device[0];
   }

   uint32 MetalAPI::displays(void) const
   {
   return displaysCount;
   }

   Ptr<Display> MetalAPI::primaryDisplay(void) const
   {
   return ptr_reinterpret_cast<Display>(&_display[0]);
   }
   
   Ptr<Display> MetalAPI::display(uint32 index) const
   {
   assert( index < displaysCount );
   return ptr_reinterpret_cast<Display>(&_display[index]);
   }

   MetalAPI::~MetalAPI() 
   {
   Nmutex lock;
   if (!lock.tryLock())
      return;
   
   // Here destroy any API globals
   device[0] = nullptr;
   device[1] = nullptr;
   
   for(uint32 i=0; i<displaysCount; ++i)
      _display[i] = nullptr;
   delete [] _display;
   
   virtualDisplay = nullptr;
   
   Graphics = nullptr;
   
   lock.unlock();
   }

   }
}
#endif




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
   desc = allocateObjectiveC(MTLRenderPassDescriptor);
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
