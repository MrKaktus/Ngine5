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

#ifndef ENG_CORE_RENDERING_METAL_DEVICE
#define ENG_CORE_RENDERING_METAL_DEVICE

#include <string>
#include "core/rendering/metal/metal.h"
#include "core/rendering/common/device.h"

#include "core/rendering/inputAssembler.h"
#include "core/rendering/blend.h"
#include "core/rendering/raster.h"
#include "core/rendering/multisampling.h"
#include "core/rendering/viewport.h"
#include "core/rendering/depthStencil.h"
#include "core/rendering/pipeline.h"
#include "core/rendering/renderPass.h"
#include "core/rendering/metal/mtlTexture.h"

#import <AppKit/AppKit.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

using namespace std;
//using namespace en::gpu;

namespace en
{
   namespace gpu
   {
   class ScreenMTL : public Screen
      {
      public:
      NSScreen* handle;     // Pointer to screen in [NSScreen screens] array
      
      ScreenMTL();
     ~ScreenMTL();
      };
     
   class MetalDevice;
   
   class WindowMTL : public Window
      {
      public:
      Ptr<Screen>   handle;
      uint32v2      windowPosition;
      
      NSWindow*     window;
      CAMetalLayer* layer;
      id <CAMetalDrawable> drawable;
      Ptr<TextureMTL> framebuffer;
      bool          needNewSurface;
      
      virtual Ptr<Screen> screen(void);
      virtual uint32v2 position(void);
      virtual bool movable(void);
      virtual void move(const uint32v2 position);
      virtual void resize(const uint32v2 size);
      virtual void active(void);
      virtual void transparent(const float opacity);
      virtual void opaque(void);
      virtual Ptr<Texture> surface(void);
      virtual void display(void);
      
      WindowMTL(const MetalDevice* gpu, const WindowSettings& settings, const string title); //id<MTLDevice> device
      virtual ~WindowMTL();
      };
   
   class CommandBufferMTL : public CommandBuffer
      {
      public:
      id <MTLCommandBuffer> handle;
      id <MTLRenderCommandEncoder> renderEncoder;
      bool commited;
      
      virtual void bind(const Ptr<RasterState> raster);
      //virtual void bind(const Ptr<ViewportScissorState> viewportScissor);
      virtual void bind(const Ptr<DepthStencilState> depthStencil);
      virtual void bind(const Ptr<BlendState> blend);
      
      virtual bool startRenderPass(const Ptr<RenderPass> pass);
      virtual void set(const Ptr<Pipeline> pipeline);
      virtual bool endRenderPass(void);
      virtual void commit(void);
      virtual void waitUntilCompleted(void);
   
      CommandBufferMTL();
      virtual ~CommandBufferMTL();
      };

    
    
   class MetalDevice : public CommonDevice
      {
      public:
      id<MTLDevice> device;
      id<MTLCommandQueue> queue;
      
      MetalDevice(id<MTLDevice> handle);
     ~MetalDevice();


      uint32 screens(void);           // Screens count the device can render to
      Ptr<Screen> screen(uint32 id) const;  // Return N'th screen handle
      Ptr<Window> create(const WindowSettings& settings,
                        const string title); // Create window
      
      virtual Ptr<Texture> create(const TextureState state);
      
      virtual Ptr<CommandBuffer>   createCommandBuffer(void);

      virtual Ptr<RasterState>     create(const RasterStateInfo& state);
      virtual Ptr<BlendState>      create(const BlendStateInfo& state,
                                          const uint32 attachments,
                                          const BlendAttachmentInfo* color);

   // When binding 3D texture, pass it's plane "depth" through "layer" parameter,
   // similarly when binding CubeMap texture, pass it's "face" through "layer".
      virtual Ptr<ColorAttachment> createColorAttachment(const Ptr<Texture> texture,
                                          const uint32 mipmap = 0,
                                          const uint32 layer = 0,
                                          const uint32 layers = 1);

      virtual Ptr<DepthStencilAttachment> createDepthStencilAttachment(const Ptr<Texture> depth, const Ptr<Texture> stencil,
                                                 const uint32 mipmap = 0,
                                                 const uint32 layer = 0,
                                                 const uint32 layers = 1);



      //virtual Ptr<ColorAttachment> create(const TextureFormat format = FormatUnsupported,
      //                                    const uint32 samples = 1);


      // Creates simple render pass with one color destination
      virtual Ptr<RenderPass> create(const Ptr<ColorAttachment> color,
                                     const Ptr<DepthStencilAttachment> depthStencil);
      
      virtual Ptr<RenderPass> create(const uint32 _attachments,
                                     const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
                                     const Ptr<DepthStencilAttachment> depthStencil);
        
      // Creates render pass which's output goes to window framebuffer
      virtual Ptr<RenderPass> create(const Ptr<Texture> framebuffer,
                                     const Ptr<DepthStencilAttachment> depthStencil);
      
      // Creates render pass which's output is resolved from temporary MSAA target to window framebuffer
      virtual Ptr<RenderPass> create(const Ptr<Texture> temporaryMSAA,
                                     const Ptr<Texture> framebuffer,
                                     const Ptr<DepthStencilAttachment> depthStencil);


      // Creates InputAssembler description based on single buffer attributes
      virtual Ptr<InputAssembler> create(const Ptr<BufferView> buffer);

      // Creates InputAssembler description combining attributes from several buffers
      virtual Ptr<InputAssembler> create(const InputAssemblerSettings& attributes);


      virtual Ptr<MultisamplingState> create(const uint32 samples,
                                             const bool enableAlphaToCoverage,
                                             const bool enableAlphaToOne);

      virtual Ptr<Pipeline> create(const Ptr<RenderPass> renderPass,
                                   const Ptr<InputAssembler> inputAssembler,
                                   const Ptr<ViewportState>  viewportState,
                                   const Ptr<RasterState>    rasterState,
                                   const Ptr<MultisamplingState> multisamplingState,
                                   const Ptr<DepthStencilState> depthStencilState,
                                   const Ptr<BlendState>     blendState//,
                                   /*const Ptr<PipelineLayout> pipelineLayout*/);
      };
   }
}

#endif