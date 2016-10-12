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

namespace en
{
   namespace gpu
   {
   class ColorAttachmentMTL : public ColorAttachment 
      {
      public:
      MTLRenderPassColorAttachmentDescriptor* desc;

      ColorAttachmentMTL(const Ptr<TextureView> source);

      virtual void onLoad(const LoadOperation load, 
                          const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 1.0f));
      virtual void onLoad(const LoadOperation load,
                          const uint32v4 clearColor = uint32v4(0u, 0u, 0u, 1u));
      virtual void onLoad(const LoadOperation load,
                          const sint32v4 clearColor = sint32v4(0, 0, 0, 1));
        
      virtual void onStore(const StoreOperation store);
      virtual bool resolve(const Ptr<TextureView> destination);
         
      virtual ~ColorAttachmentMTL();
      };

   class DepthStencilAttachmentMTL : public DepthStencilAttachment
      {
      public:
      MTLRenderPassDepthAttachmentDescriptor*   descDepth;
      MTLRenderPassStencilAttachmentDescriptor* descStencil;

      DepthStencilAttachmentMTL(const Ptr<TextureView> depth,
                                const Ptr<TextureView> stencil = nullptr);

      virtual void onLoad(const LoadOperation loadDepthStencil,
                          const float  clearDepth = 1.0f,
                          const uint32 clearStencil = 0u);
         
      virtual void onStore(const StoreOperation storeDepthStencil);
      
      // Specify Depth resolve method and destination, if it's supported by the GPU.
      virtual bool resolve(const Ptr<TextureView> destination,
                           const DepthResolve mode = DepthResolve::Sample0);
         
      // Custom load and store actions can be specifid for Stencil.
      // (no matter if it's shared DepthStencil attachment, or
      //  if GPU support separate Depth and Stencil attachments).
      virtual void onStencilLoad(const LoadOperation loadStencil);

      virtual void onStencilStore(const StoreOperation storeStencil);
         
      virtual ~DepthStencilAttachmentMTL();
      };
      
   class RenderPassMTL : public RenderPass
      {
      public:
      MTLRenderPassDescriptor* desc;
      
      RenderPassMTL();
     ~RenderPassMTL();
      };
   }
}
#endif

#endif
