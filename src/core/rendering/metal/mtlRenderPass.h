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

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/renderPass.h"

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
      };

   class DepthStencilAttachmentMTL : public DepthStencilAttachment
      {
      public:
      MTLRenderPassDepthAttachmentDescriptor*   descDepth;
      MTLRenderPassStencilAttachmentDescriptor* descStencil;

      DepthStencilAttachmentMTL(/* TODO */);

      virtual void onLoad(const LoadOperation loadDepth,
                          const LoadOperation loadStencil,
                          const float clearDepth = 0.0f,
                          const uint32 clearStencil = 0u);
      virtual void onStore(const StoreOperation storeDepth,
                           const StoreOperation storeStencil);
      virtual bool resolve(const Ptr<Texture> texture, 
                           const uint32 mipmap = 0u,
                           const uint32 layer = 0u);
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