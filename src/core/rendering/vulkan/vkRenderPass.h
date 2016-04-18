#ifndef ENG_CORE_RENDERING_VULKAN_RENDER_PASS
#define ENG_CORE_RENDERING_VULKAN_RENDER_PASS

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/renderPass.h"
#include "core/rendering/vulkan/vkDevice.h"
#include "core/rendering/vulkan/vkTexture.h"

namespace en
{
   namespace gpu
   {
   class ColorAttachmentVK : public ColorAttachment
      {
      public:
      Ptr<TextureVK> texture[2];
      VkAttachmentDescription state[2]; // Attachment and optional Resolve
      uint32 mipmap[2];
      uint32 layer[2];      
      float4 clearColor;

      ColorAttachmentVK(const Ptr<Texture> texture,
         const uint32 mipmap = 0u,
         const uint32 layer = 0u);

      virtual void onLoad(const LoadOperation load,
         const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 1.0f));
      virtual void onStore(const StoreOperation store);
      virtual bool resolve(const Ptr<Texture> texture,
         const uint32 mipmap = 0u,
         const uint32 layer = 0u);

      virtual ~ColorAttachmentVK();
      };

   class DepthStencilAttachmentVK : public DepthStencilAttachment
      {
      public:
      Ptr<TextureVK> texture[4];
      VkAttachmentDescription state[4]; // Shared DepthStencil or Depth, Separate Stencil, Shared DepthStencil or Depth Resolve, Separate Stencil Resolve
      uint32 mipmap[4];
      uint32 layer[4];
      float  clearDepth;
      uint32 clearStencil;

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