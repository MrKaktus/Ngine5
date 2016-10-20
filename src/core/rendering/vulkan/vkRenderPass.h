/*

 Ngine v5.0
 
 Module      : Vulkan Render Pass.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_RENDER_PASS
#define ENG_CORE_RENDERING_VULKAN_RENDER_PASS

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

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
      VkAttachmentDescription state[2];   // Attachment and optional Resolve
      VkClearValue            clearValue;
      bool                    resolve;

      ColorAttachmentVK(const Format format, const uint32 samples);

      virtual void onLoad(const LoadOperation load,
         const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 1.0f));
      virtual void onLoad(const LoadOperation load,
         const uint32v4 clearColor = uint32v4(0u, 0u, 0u, 1u));
      virtual void onLoad(const LoadOperation load,
         const sint32v4 clearColor = sint32v4(0, 0, 0, 1));
         
      virtual void onStore(const StoreOperation store);

      virtual ~ColorAttachmentVK();
      };

   class DepthStencilAttachmentVK : public DepthStencilAttachment
      {
      public:
      VkAttachmentDescription state;        // Shared DepthStencil, Depth or Stencil
      float                   clearDepth;
      uint32                  clearStencil;

      DepthStencilAttachmentVK(const Format depthFormat, 
                               const Format stencilFormat = Format::Unsupported,
                               const uint32 samples = 1u);

      virtual void onLoad(const LoadOperation loadDepthStencil,
                          const float  clearDepth = 1.0f,
                          const uint32 clearStencil = 0u);

      // Specify store operation, and Depth resolve method if it's supported by the GPU.
      virtual void onStore(const StoreOperation storeDepthStencil,
                           const DepthResolve resolveMode = DepthResolve::Sample0);

      virtual void onStencilLoad(const LoadOperation loadStencil);

      virtual void onStencilStore(const StoreOperation storeStencil);

      virtual ~DepthStencilAttachmentVK();
      };

   class FramebufferVK : public Framebuffer
      {
      public:
      VulkanDevice* gpu;
      VkFramebuffer handle;
      uint32v2      resolution;
      uint32        layers;
      
      FramebufferVK(VulkanDevice* gpu, const VkFramebuffer handle, const uint32v2 resolution, const uint32 layers);
     ~FramebufferVK();
      };

   class RenderPassVK : public RenderPass
      {
      public:
      VulkanDevice* gpu;
      VkRenderPass  handle;
      uint32        usedAttachments;  // Bitmask
      uint32        surfaces;
      VkClearValue* clearValues;   // Array of clear values per attachment
      bool          resolve;
      bool          depthStencil;
      
      RenderPassVK(VulkanDevice* gpu, 
                   const VkRenderPass handle, 
                   const uint32 usedAttachments, 
                   const uint32 surfaces,
                   const bool   _resolve,
                   const bool   _depthStencil);

     ~RenderPassVK();

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
      // window Swap-Chain surface.
      virtual Ptr<Framebuffer> createFramebuffer(const uint32v2 resolution,
                                                 const Ptr<TextureView> temporaryMSAA,
                                                 const Ptr<TextureView> swapChainSurface,
                                                 const Ptr<TextureView> depthStencil = nullptr,
                                                 const Ptr<TextureView> stencil      = nullptr);
      };
   }
}
#endif

#endif
