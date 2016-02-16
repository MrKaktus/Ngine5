/*

 Ngine v5.0
 
 Module      : Render Pass.
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

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

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

      ColorAttachmentMTL(const Ptr<Texture> texture,
                         const uint32 mipmap = 0u,
                         const uint32 layer = 0u);

      virtual void onLoad(const LoadOperation load, 
                          const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 0.0f));
      virtual void onStore(const StoreOperation store);
      virtual bool resolve(const Ptr<Texture> texture, 
                           const uint32 mipmap = 0u,
                           const uint32 layer = 0u);
         
      virtual ~ColorAttachmentMTL();
      };

   class DepthStencilAttachmentMTL : public DepthStencilAttachment
      {
      public:
      MTLRenderPassDepthAttachmentDescriptor*   descDepth;
      MTLRenderPassStencilAttachmentDescriptor* descStencil;

      DepthStencilAttachmentMTL(const Ptr<Texture> depth,
                                const Ptr<Texture> stencil = nullptr,
                                const uint32 mipmap = 0u,
                                const uint32 layer = 0u);

      virtual void onLoad(const LoadOperation loadDepthStencil,
                          const float  clearDepth = 0.0f,
                          const uint32 clearStencil = 0u);
         
      virtual void onStore(const StoreOperation storeDepthStencil);
      
      virtual bool resolve(const Ptr<Texture> depth,
                           const uint32 mipmap = 0u,
                           const uint32 layer = 0u);
         
      // If GPU supports separate Depth and Stencil atachments,
      // custom load and store actions; and MSAA resolve destination
      // can be specifid for Stencil.
      virtual void onStencilLoad(const LoadOperation loadStencil);

      virtual void onStencilStore(const StoreOperation storeStencil);
      
      virtual bool resolveStencil(const Ptr<Texture> stencil,
                                  const uint32 mipmap = 0u,
                                  const uint32 layer = 0u);
         
      virtual ~DepthStencilAttachmentMTL();
      };
      
   //class FramebufferMTL : public Framebuffer
   //   {
   //   public:
   //   // Metal Framebuffer ???
   //
   //   FramebufferMTL();
   //  ~FramebufferMTL();
   //   };

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