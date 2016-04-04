#ifndef ENG_CORE_RENDERING_VULKAN_RENDER_PASS
#define ENG_CORE_RENDERING_VULKAN_RENDER_PASS

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/renderPass.h"
#include "core/rendering/vulkan/vkDevice.h"

namespace en
{
   namespace gpu
   {
   class ColorAttachmentVK : public ColorAttachment
      {
      public:
      VkAttachmentDescription state;
      uint32 mipmap;
      uint32 layer;      
      float4 clearColor;

      // Info about resolve operation
      VkAttachmentDescription resolveState;
      bool   resolveAttachment;
      uint32 resolveMipmap;
      uint32 resolveLayer;      

      ColorAttachmentVK(const Ptr<Texture> texture,
         const uint32 mipmap = 0u,
         const uint32 layer = 0u);

      virtual void onLoad(const LoadOperation load,
         const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 0.0f));
      virtual void onStore(const StoreOperation store);
      virtual bool resolve(const Ptr<Texture> texture,
         const uint32 mipmap = 0u,
         const uint32 layer = 0u);

      virtual ~ColorAttachmentVK();
      };

   class DepthStencilAttachmentVK : public DepthStencilAttachment
      {
      public:
      VkAttachmentDescription state[4];
      uint32 mipmap[4];
      uint32 layer[4];

      float  clearDepth;
      uint32 clearStencil;

      bool   separateStencil;
      bool   resolveDepthStencil;
      bool   resolveSeparateStencil;

      DepthStencilAttachmentVK(const Ptr<Texture> depth,
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

      virtual ~DepthStencilAttachmentVK();
      };

   class RenderPassVK : public RenderPass
      {
      public:
      VkRenderPass  passId;
      VkFramebuffer framebufferId;
      VulkanDevice* gpu;
      uint32        attachments;
      
      RenderPassVK();
     ~RenderPassVK();
      };
   }
}
#endif

#endif