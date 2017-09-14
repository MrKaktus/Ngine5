/*

 Ngine v5.0
 
 Module      : Metal Render Pass.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_RENDER_PASS
#define ENG_CORE_RENDERING_METAL_RENDER_PASS

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/renderPass.h"
#include "core/rendering/metal/metal.h"
#include "core/rendering/metal/mtlTexture.h"

#define MaxColorAttachmentsCount 8

namespace en
{
   namespace gpu
   {
   class ColorAttachmentMTL : public ColorAttachment 
      {
      public:
      MTLRenderPassColorAttachmentDescriptor* desc;
      Format format;
      uint32 samples;
      bool   resolve;
      
      ColorAttachmentMTL(const Format format, 
                         const uint32 samples);

      virtual void onLoad(const LoadOperation load, 
                          const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 1.0f));
      virtual void onLoad(const LoadOperation load,
                          const uint32v4 clearColor = uint32v4(0u, 0u, 0u, 1u));
      virtual void onLoad(const LoadOperation load,
                          const sint32v4 clearColor = sint32v4(0, 0, 0, 1));
        
      virtual void onStore(const StoreOperation store);
         
      virtual ~ColorAttachmentMTL();
      };

   class DepthStencilAttachmentMTL : public DepthStencilAttachment
      {
      public:
      MTLRenderPassDepthAttachmentDescriptor*   descDepth;
      MTLRenderPassStencilAttachmentDescriptor* descStencil;
      Format depthFormat;
      Format stencilFormat;
      uint32 samples;
      
      DepthStencilAttachmentMTL(const Format depthFormat, 
                                const Format stencilFormat,
                                const uint32 samples);

      virtual void onLoad(const LoadOperation loadDepthStencil,
                          const float  clearDepth = 1.0f,
                          const uint32 clearStencil = 0u);
         
      virtual void onStore(const StoreOperation storeDepthStencil,
                           const DepthResolve resolveMode = DepthResolve::Sample0);
      
      // Specify Depth resolve method and destination, if it's supported by the GPU.
      //virtual bool resolve(const Ptr<TextureView> destination,
      //                     const DepthResolve mode = DepthResolve::Sample0);
         
      // Custom load and store actions can be specifid for Stencil.
      // (no matter if it's shared DepthStencil attachment, or
      //  if GPU support separate Depth and Stencil attachments).
      virtual void onStencilLoad(const LoadOperation loadStencil);

      virtual void onStencilStore(const StoreOperation storeStencil);
         
      virtual ~DepthStencilAttachmentMTL();
      };
     
   class FramebufferMTL : public Framebuffer
      {
      public:
      // Packed, kept to ensure source textures won't be freed from underneath of us.
      // (do we really need that?)
      Ptr<TextureViewMTL> views[MaxColorAttachmentsCount*2+3];
      
      // Direct Metal handles (unpacked, with holes)
      id<MTLTexture>      color[MaxColorAttachmentsCount];
      id<MTLTexture>      resolve[MaxColorAttachmentsCount];
      id<MTLTexture>      depthStencil[3];
      
      uint32v2            resolution;
      uint32              layers;

      FramebufferMTL(const uint32v2 resolution, const uint32 layers);
     ~FramebufferMTL();
      };
      
   class RenderPassMTL : public RenderPass
      {
      public:
      MTLRenderPassDescriptor* desc;
      Format format[MaxColorAttachmentsCount+2];
      uint32 samples[MaxColorAttachmentsCount+1];
      uint32 usedAttachments; // Bitmask
      bool   resolve;
      
      RenderPassMTL();
      virtual ~RenderPassMTL();
     
      virtual Ptr<Framebuffer> createFramebuffer(const uint32v2 resolution,
                                                 const uint32   layers,
                                                 const uint32   attachments,
                                                 const Ptr<TextureView>* attachment,
                                                 const Ptr<TextureView> depthStencil = nullptr,
                                                 const Ptr<TextureView> stencil      = nullptr,
                                                 const Ptr<TextureView> depthResolve = nullptr);

      // Creates framebuffer using window Swap-Chain surface.
      virtual Ptr<Framebuffer> createFramebuffer(const uint32v2 resolution,
                                                 const Ptr<TextureView> swapChainSurface,
                                                 const Ptr<TextureView> depthStencil = nullptr,
                                                 const Ptr<TextureView> stencil      = nullptr);
      
      // Creates framebuffer for rendering to temporary MSAA that is then resolved directly to
      // window Swap-Chain surface. Depth-Stencil can still be nullptr, but that need to be 
      // explicitly stated by the application (to prevent ambiguous call).
      virtual Ptr<Framebuffer> createFramebuffer(const uint32v2 resolution,
                                                 const Ptr<TextureView> temporaryMSAA,
                                                 const Ptr<TextureView> swapChainSurface,
                                                 const Ptr<TextureView> depthStencil,
                                                 const Ptr<TextureView> stencil);
      };
   }
}
#endif

#endif
