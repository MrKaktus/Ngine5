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
      Ptr<TextureViewVK>      view[2];
      VkAttachmentDescription state[2]; // Attachment and optional Resolve
      VkClearValue clearValue;

      ColorAttachmentVK(const Ptr<TextureView> source);

      virtual void onLoad(const LoadOperation load,
         const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 1.0f));
      virtual void onLoad(const LoadOperation load,
         const uint32v4 clearColor = uint32v4(0u, 0u, 0u, 1u));
      virtual void onLoad(const LoadOperation load,
         const sint32v4 clearColor = sint32v4(0, 0, 0, 1));
         
      virtual void onStore(const StoreOperation store);
      virtual bool resolve(const Ptr<TextureView> destination);

      virtual ~ColorAttachmentVK();
      };

   class DepthStencilAttachmentVK : public DepthStencilAttachment
      {
      public:
      Ptr<TextureViewVK>      view;
      VkAttachmentDescription state; // Shared DepthStencil, Depth or Stencil
      float  clearDepth;
      uint32 clearStencil;

      DepthStencilAttachmentVK(const Ptr<TextureView> depth,
                               const Ptr<TextureView> stencil = nullptr);

      virtual void onLoad(const LoadOperation loadDepthStencil,
         const float  clearDepth = 1.0f,
         const uint32 clearStencil = 0u);

      virtual void onStore(const StoreOperation storeDepthStencil);

      // Specify Depth resolve method and destination, if it's supported by the GPU.
      virtual bool resolve(const Ptr<TextureView> destination,
                           const DepthResolve mode = DepthResolve::Sample0);

      virtual void onStencilLoad(const LoadOperation loadStencil);

      virtual void onStencilStore(const StoreOperation storeStencil);

      virtual ~DepthStencilAttachmentVK();
      };

   class RenderPassVK : public RenderPass
      {
      public:
      VulkanDevice* gpu;
      VkRenderPass  passHandle;
      VkFramebuffer framebufferHandle;
      uint32v2      resolution;
      uint32        attachments;
      VkClearValue* clearValues;   // Array of clear values per attachment
      
      RenderPassVK(VulkanDevice* _gpu, const VkRenderPass passHandle, const uint32 _attachments);
     ~RenderPassVK();
      };
   }
}
#endif

#endif
