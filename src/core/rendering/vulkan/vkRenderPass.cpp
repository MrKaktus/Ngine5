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

#include "core/utilities/TintrusivePointer.h"
#include "core/rendering/texture.h"
#include "core/rendering/vulkan/vkDevice.h"    // TODO: We only want Device class, not all subsystems

namespace en
{
   namespace gpu
   { 
   // PUBLIC HEADER
   //////////////////////////////////////////////////////////////////////////////////////////////////////

   // VULKAN HEADER
   //////////////////////////////////////////////////////////////////////////////////////////////////////

   class ColorAttachmentVK : public ColorAttachment 
      {
      public:
      VkAttachmentDescription state;

      ColorAttachmentVK::ColorAttachmentVK(const TextureFormat format = FormatUnsupported, 
                                           const uint32 samples = 1);

      virtual void onLoad(const LoadOperation load, 
                          const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 0.0f));
      virtual void onStore(const StoreOperation store);
      virtual bool resolve(const Ptr<Texture> texture, 
                           const uint32 mipmap = 0,
                           const uint32 layer = 0);
      };

   class FramebufferVK : public Framebuffer
      {
      public:
      VkFramebuffer id;
      VulkanDevice* gpu;

      FramebufferVK();
     ~FramebufferVK();
      };

   class RenderPassVK : public RenderPass
      {
      public:
      VkRenderPass  id;
      VulkanDevice* gpu;
      uint32        attachments;

      RenderPassVK();
     ~RenderPassVK();
      };



   // COMMON IMPLEMENTATION
   //////////////////////////////////////////////////////////////////////////////////////////////////////


   ColorAttachment::~ColorAttachment()
   {
   }

   // VULKAN IMPLEMENTATION
   //////////////////////////////////////////////////////////////////////////////////////////////////////

   static const VkAttachmentLoadOp TranslateLoadOperation[LoadOperationsCount] =
      {
      VK_ATTACHMENT_LOAD_OP_DONT_CARE,          // DontLoad
      VK_ATTACHMENT_LOAD_OP_LOAD,               // Load
      VK_ATTACHMENT_LOAD_OP_CLEAR               // Clear
      };
   
   static const VkAttachmentStoreOp TranslateStoreOperation[StoreOperationsCount] = 
      {
      VK_ATTACHMENT_STORE_OP_DONT_CARE,         // DontStore
      VK_ATTACHMENT_STORE_OP_STORE,             // Store
      VK_ATTACHMENT_STORE_OP_STORE              // ResolveMSAA
      };

   ColorAttachmentVK::ColorAttachmentVK(const TextureFormat format, const uint32 samples)
   {
   state.flags          = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;   // Allow aliasing/overlapping of temporary color attachments in the same memory
   state.format         = TranslateTextureFormat[format];
   state.samples        = samples;
   state.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
   state.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
   state.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;
   state.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
   state.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   state.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   }

   ColorAttachmentVK::onLoad(const LoadOperation load, const float4 clearColor)
   {
   state.loadOp = TranslateLoadOperation[load];
   // TODO: What about clear color ?
   }

   ColorAttachmentVK::onStore(const StoreOperation store)
   {
   state.storeOp = TranslateStoreOperation[store];
   }

   ColorAttachmentVK::resolve(const Ptr<Texture> texture, const uint32 mipmap, const uint32 layer)
   {
   assert( texture );
   // TODO: Where do we store texture/mipmap/layer ?
   }

   RenderPassVK::RenderPassVK() :
      gpu(nullptr),
      attachments(0)
   {
   }

   RenderPassVK::~RenderPassVK()
   {
   vkDestroyRenderPass(gpu->device, id, &gpu->defaultAllocCallbacks); 
   }

   FramebufferVK::FramebufferVK() :
      gpu(nullptr)
   {
   }

   FramebufferVK::~FramebufferVK()
   {
   vkDestroyFramebuffer(gpu->device, id, &gpu->defaultAllocCallbacks);
   }


   Ptr<ColorAttachment> VulkanDevice::create(const TextureFormat format = FormatUnsupported, const uint32 samples)
   {
   return ptr_dynamic_cast<ColorAttachment, ColorAttachmentVK>(new ColorAttachmentVK(format, samples));
   }









typedef struct VkSubpassDependency {
    uint32_t                                    srcSubpass;
    uint32_t                                    dstSubpass;
    VkPipelineStageFlags                        srcStageMask;
    VkPipelineStageFlags                        dstStageMask;
    VkAccessFlags                               srcAccessMask;
    VkAccessFlags                               dstAccessMask;
    VkDependencyFlags                           dependencyFlags;
} VkSubpassDependency;



//typedef struct VkAttachmentDescription {
//    VkAttachmentDescriptionFlags                flags;
//    VkFormat                                    format;
//    VkSampleCountFlagBits                       samples;
//    VkAttachmentLoadOp                          loadOp;
//    VkAttachmentStoreOp                         storeOp;
//    VkAttachmentLoadOp                          stencilLoadOp;
//    VkAttachmentStoreOp                         stencilStoreOp;
//    VkImageLayout                               initialLayout;
//    VkImageLayout                               finalLayout;
//} VkAttachmentDescription;


   Ptr<RenderPass> VulkanDevice::create(uint32 _attachments,
                                        const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
                                        const Ptr<DepthStencilAttachment> depthStencil)
   {
   Ptr<RenderPass> result = nullptr;
   
   assert( _attachments < MaxColorAttachmentsCount );



// VK_IMAGE_LAYOUT_UNDEFINED = 0,
// VK_IMAGE_LAYOUT_GENERAL = 1,
// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2,
// VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
// VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
// VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL = 5,
// VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL = 6,
// VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL = 7,
// VK_IMAGE_LAYOUT_PREINITIALIZED = 8,
// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR = 1000001002,


   // Gather Attachment states
   VkAttachmentDescription* attachment = new VkAttachmentDescription[_attachments];
   for(uint32 i=0; i<_attachments; ++i)
      memcpy(&attachment[i], &color[i]->state, sizeof(VkAttachmentDescription));

      // Render Pass attachments layouy:
      // 0..7 - Up to 8 Color Attachments
      // 8    - Depth / Depth-Stencil 
      // 9    - Separate Stencil if supported

   VkAttachmentReference* attColor   = nullptr;
new VkAttachmentReference[_attachments];

   VkAttachmentReference* attResolve = nullptr;
new VkAttachmentReference[_attachments];

   VkAttachmentReference attDepthStencil;
   attDepthStencil.attachment = VK_ATTACHMENT_UNUSED;
   attDepthStencil.layout     = VK_IMAGE_LAYOUT_UNDEFINED;
   if (depthStencil)
      {
      attDepthStencil->attachment = 8;   
      attDepthStencil->layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; 
      // TODO: How to determine below situation ?
      // VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;  // When DepthStencil texture is filled earlier in Depth pre-pass, and now only used for discarding
      }

   // Engine is currently not supporting multiple sub-passes described as one dependency graph
   VkSubpassDescription subInfo;
#if defined(EN_DISCRETE_GPU)
   subInfo.flags                   = 0;
#elif defined(EN_MOBILE_GPU)
   subInfo.flags                   = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
#endif
   subInfo.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
   subInfo.inputAttachmentCount    = 0;
   subInfo.pInputAttachments       = nullptr;
   subInfo.colorAttachmentCount    = _attachments;
   subInfo.pColorAttachments       = attColor;
   subInfo.pResolveAttachments     = attResolve;
   subInfo.pDepthStencilAttachment = &attDepthStencil;
   subInfo.preserveAttachmentCount = 0;
   subInfo.pPreserveAttachments    = nullptr;


	subpassDescription.depthStencilAttachment.attachment = ( framebufferLayout->internalDepthFormat != VK_FORMAT_UNDEFINED ) ? 1 : VK_ATTACHMENT_UNUSED;
	subpassDescription.depthStencilAttachment.layout = ( framebufferLayout->internalDepthFormat != VK_FORMAT_UNDEFINED ) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
	subpassDescription.preserveCount = 0;
	subpassDescription.pPreserveAttachments = NULL;


   VkRenderPassCreateInfo passInfo;
   passInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
   passInfo.pNext           = nullptr;
   passInfo.attachmentCount = attachments;
   passInfo.pAttachments    = attachment;
   passInfo.subpassCount    = 1;
   passInfo.pSubpasses      = &subInfo;
   passInfo.dependencyCount = 0;
    const VkSubpassDependency*                  pDependencies;

   // Create renderpass object
   VkRenderPass renderpass;
   VkResult res = vkCreateRenderPass(device, &passInfo, &defaultAllocCallbacks, &renderpass);
   if (res == VK_SUCCESS)
      {
      Ptr<RenderPassVK> rpo = new RenderPassVK();
      rpo->id          = renderpass;
      rpo->gpu         = this;
      rpo->attachments = _attachments;
      result = ptr_dynamic_cast<RenderPass, RenderPassVK>(rpo);
      }

   delete [] attachment;

   return result;
   }


// typedef void (VKAPI_PTR *PFN_vkCmdBeginRenderPass)(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents);
// typedef void (VKAPI_PTR *PFN_vkCmdNextSubpass)(VkCommandBuffer commandBuffer, VkSubpassContents contents);
// typedef void (VKAPI_PTR *PFN_vkCmdEndRenderPass)(VkCommandBuffer commandBuffer);


   Ptr<Framebuffer> RenderPassVK::create(const uint32v2 resolution, // Common attachments resolution
                                         const uint32   layers)     // Common attachments layers count
   {
   Ptr<Framebuffer> result = nullptr;
   
   VkFramebufferCreatenInfo framebufferInfo;
   framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
   framebufferInfo.pNext           = nullptr;
   framebufferInfo.flags           = 0;        // TODO: Check what flags are available.
   framebufferInfo.renderPass      = state;
   framebufferInfo.attachmentCount = attachments;
   framebufferInfo.    const VkImageView*                          pAttachments;
   framebufferInfo.width           = resolution.width;
   framebufferInfo.height          = resolution.height;
   framebufferInfo.layers          = layers;

   // Create framebuffer object
   VkFramebuffer framebuffer;
   VkResult res = vkCreateFramebuffer(device, &framebufferInfo, const VkAllocationCallbacks* pAllocator, &framebuffer); 
   if (!res)
      {
      Ptr<FramebufferVK> fbo = new FramebufferVK();
      fbo->id  = framebuffer;
      fbo->gpu = gpu;
      result = ptr_dynamic_cast<Framebuffer, FramebufferVK>(fbo);
      }

   return result;
   }


   // D3D12 IMPLEMENTATION
   //////////////////////////////////////////////////////////////////////////////////////////////////////



   // DX12 Doesn't support Tiled Renderers, and as such, don't have load and store operations.



   // METAL IMPLEMENTATION
   //////////////////////////////////////////////////////////////////////////////////////////////////////














   Ptr<ColorAttachment> Create(const Ptr<Texture> texture, 
                               const TextureFormat format,
                               const uint32 mipmap,
                               const uint32 layer,
                               const uint32 layers)
   {
   assert( texture );

   Ptr<ColorAttachmentMTL> attachment = new mtlColorAttachment();

   Ptr<TextureMTL> color = ptr_dynamic_cast<TextureMTL, Texture>(texture);

   attachment->desc.texture       = color->id;
   attachment->desc.level         = mipmap;
   attachment->desc.slice         = 0;
   attachment->desc.depthPlane    = 0;

   // Metal is currently not supporting 
   // Texture2DMultisampleArray, nor
   // TextureCubeMapArray
   if (texture->state.type == Texture1DArray ||
       texture->state.type == Texture2DArray ||
       texture->state.type == TextureCubeMap)
      attachment->desc.slice      = layer;
   else
   if (texture->state.type == Texture3D)
      attachment->desc.depthPlane = layer;

   // Metal is ignoring "layers" parameter as it has no
   // way to select RT layer due to missing Geometry Shader.
   // It is also ignoring "format" parameter for now.

   // TODO: Do we need to store "format" for some reason ?

   return ptr_dynamic_cast<ColorAttachment, ColorAttachmentMTL>(attachment);
   }














































   // typedef enum {
   //     VK_IMAGE_LAYOUT_UNDEFINED = 0,
   //     VK_IMAGE_LAYOUT_GENERAL = 1,
   //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2,
   //     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
   //     VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
   //     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL = 5,
   //     VK_IMAGE_LAYOUT_TRANSFER_SOURCE_OPTIMAL = 6,
   //     VK_IMAGE_LAYOUT_TRANSFER_DESTINATION_OPTIMAL = 7,
   //     VK_IMAGE_LAYOUT_PREINITIALIZED = 8,
   // } VkImageLayout;



   class DepthStencilAttachmentMTL : public DepthStencilAttachment
      {
      public:
      MTLRenderPassDepthAttachmentDescriptor*   depth;
      MTLRenderPassStencilAttachmentDescriptor* stencil;

      DepthStencilAttachmentMTL();
     ~DepthStencilAttachmentMTL();

      onLoad(const LoadOperation load, 
             const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 0.0f));
      onStore(const StoreOperation store);
      resolve(const Ptr<Texture> color, 
              const uint32 mipmap = 0,
              const uint32 depth = 0,
              const uint32 layer = 0);
      };

   DepthStencilAttachmentMTL::DepthStencilAttachmentMTL() :
      depth([[MTLRenderPassDepthAttachmentDescriptor alloc] autorelease]),
      stencil([[MTLRenderPassStencilAttachmentDescriptor alloc] autorelease])
   {
   }

   DepthStencilAttachmentMTL::~DepthStencilAttachmentMTL()
   {
   [depth   release];
   [stencil release];
   }




   Ptr<DepthStencilAttachment> Create(const Ptr<Texture> , 
                               const TextureFormat format,
                               const uint32 mipmap,
                               const uint32 depth,
                               const uint32 layer,
                               const uint32 layers

                               const Ptr<Texture> stencil, 
                               const TextureFormat stencilFormat,
                               const uint32 stencilMipmap,
                               const uint32 stencilLayer,

                               const Ptr<Texture> resolveMsaa, 
                               const uint32 resolveMipmap,
                               const uint32 resolveLayer,
)
   {
   Ptr<DepthAttachment> result = nullptr;

   Ptr<mtlTexture> mtlDepth = ptr_dynamic_cast<mtlTexture, Texture>(depth);
   Ptr<mtlTexture> mtlMsaa  = ptr_dynamic_cast<mtlTexture, Texture>(resolveMsaa);

   MTLRenderPassDepthAttachmentDescriptor* depthAttachmentInfo = [[MTLRenderPassDepthAttachmentDescriptor alloc] init];
   depthAttachmentInfo.texture           = depth ? mtlDepth->id : nil;
   depthAttachmentInfo.level             = depthMipMap;
   depthAttachmentInfo.slice             = depthLayer;
   depthAttachmentInfo.depthPlane        = 0;                   // depth in 3D texture
   depthAttachmentInfo.loadAction        =
   depthAttachmentInfo.storeAction       =
   depthAttachmentInfo.resolveTexture    = resolveMsaa ? mtlMsaa->id : nil;
   depthAttachmentInfo.resolveLevel      = msaaMipMap;
   depthAttachmentInfo.resolveSlice      = msaaLayer;
   depthAttachmentInfo.resolveDepthPlane = 0;                   // depth in 3D texture
   // Depth Attachment specific
   depthAttachmentInfo.clearDepth        = clearValue; // double
   }






















   }
}