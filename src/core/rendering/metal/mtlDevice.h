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

#include "core/defines.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include <string>
#include "core/rendering/metal/metal.h"
#include "core/rendering/common/device.h"
#include "core/rendering/common/inputAssembler.h"

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

using namespace std;
//using namespace en::gpu;

namespace en
{
   namespace gpu
   {
   class DisplayMTL : public CommonDisplay
      {
      public:
      NSScreen* handle;     // Pointer to screen in [NSScreen screens] array
      
      DisplayMTL();
     ~DisplayMTL();
      };
     
   class MetalDevice;
   
   class WindowMTL : public CommonWindow
      {
      public:
      NSWindow*     window;
      CAMetalLayer* layer;
      id <CAMetalDrawable> drawable;
      Ptr<TextureMTL> framebuffer;
      bool          needNewSurface;
      
      virtual Ptr<Display> display(void);
      virtual uint32v2 position(void);
      virtual bool movable(void);
      virtual void move(const uint32v2 position);
      virtual void resize(const uint32v2 size);
      virtual void active(void);
      virtual void transparent(const float opacity);
      virtual void opaque(void);
      virtual Ptr<Texture> surface(void);
      virtual void present(void);
      
      WindowMTL(const MetalDevice* gpu, const WindowSettings& settings, const string title); //id<MTLDevice> device
      virtual ~WindowMTL();
      };
   

   class MetalDevice : public CommonDevice
      {
      public:
      id<MTLDevice> device;
      id<MTLCommandQueue> queue;
      
      MetalDevice(id<MTLDevice> handle);
     ~MetalDevice();

      // Internal
      
      void init(void);
      
      // Interface

      uint32 displays(void) const;           // Screens count the device can render to
      Ptr<Display> display(uint32 id) const;  // Return N'th screen handle
      Ptr<Window> create(const WindowSettings& settings,
                        const string title); // Create window
      
      virtual Ptr<Buffer> create(const BufferType type, const uint32 size);
      virtual Ptr<Buffer> create(const BufferType type, const uint32 size, const void* data);
    
                                    
      virtual Ptr<Texture> create(const TextureState state);
      
      virtual Ptr<Shader>  create(const string& source, const string& entrypoint);
      
      
      virtual Ptr<Heap>    create(uint32 size);

      virtual uint32 queues(const QueueType type) const;
      
      virtual Ptr<CommandBuffer> createCommandBuffer(const QueueType type = QueueType::Universal,
                                        const uint32 parentQueue = 0u);

      virtual Ptr<InputAssembler>  create(const DrawableType primitiveType,
                                          const uint32 controlPoints,
                                          const uint32 usedAttributes,
                                          const uint32 usedBuffers,
                                          const AttributeDesc* attributes,
                                          const BufferDesc* buffers);



   // When binding 3D texture, pass it's plane "depth" through "layer" parameter,
   // similarly when binding CubeMap texture, pass it's "face" through "layer".
      virtual Ptr<ColorAttachment> createColorAttachment(const Ptr<TextureView> source);

      virtual Ptr<DepthStencilAttachment> createDepthStencilAttachment(const Ptr<TextureView> depth,
                                                                       const Ptr<TextureView> stencil);



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

      virtual Ptr<DepthStencilState>  create(const DepthStencilStateInfo& desc);
      virtual Ptr<MultisamplingState> create(const uint32 samples,
                                             const bool enableAlphaToCoverage,
                                             const bool enableAlphaToOne);

      virtual Ptr<RasterState>     create(const RasterStateInfo& state);
      
      virtual Ptr<BlendState>      create(const BlendStateInfo& state,
                                          const uint32 attachments,
                                          const BlendAttachmentInfo* color);
      
      virtual Ptr<ViewportState>      create(const uint32 count,
                                             const ViewportStateInfo* viewports,
                                             const ScissorStateInfo* scissors);
         
      virtual Ptr<Pipeline> create(const Ptr<RenderPass> renderPass,
                                   const Ptr<InputAssembler> inputAssembler,
                                   const Ptr<ViewportState>  viewportState,
                                   const Ptr<RasterState>    rasterState,
                                   const Ptr<MultisamplingState> multisamplingState,
                                   const Ptr<DepthStencilState> depthStencilState,
                                   const Ptr<BlendState>     blendState,
                                   const Ptr<Shader>         vertex,
                                   const Ptr<Shader>         fragment
                                   /*const Ptr<PipelineLayout> pipelineLayout*/);
      };
   }
}
#endif

#endif
