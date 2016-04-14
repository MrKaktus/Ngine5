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

#include "core/rendering/vulkan/vkRenderPass.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/utilities/TintrusivePointer.h"
#include "core/rendering/vulkan/vkTexture.h"   // For TranslateTextureFormat[]
#include "core/rendering/vulkan/vkDevice.h"    // TODO: We only want Device class, not all subsystems

#include "utilities/utilities.h" // For underlyingType()

namespace en
{
   namespace gpu
   { 
   static const VkAttachmentLoadOp TranslateLoadOperation[underlyingType(LoadOperation::OperationsCount)] =
      {
      VK_ATTACHMENT_LOAD_OP_DONT_CARE,          // None
      VK_ATTACHMENT_LOAD_OP_LOAD,               // Load
      VK_ATTACHMENT_LOAD_OP_CLEAR               // Clear
      };
   
   static const VkAttachmentStoreOp TranslateStoreOperation[underlyingType(StoreOperation::OperationsCount)] =
      {
      VK_ATTACHMENT_STORE_OP_DONT_CARE,         // Discard
      VK_ATTACHMENT_STORE_OP_STORE,             // Store
      VK_ATTACHMENT_STORE_OP_DONT_CARE,         // ResolveMSAA          (TODO: Vulkan: Should it be set to don't care if in next Subpass we will resolve it ? Or is it unsupported and we always need to store?)
      VK_ATTACHMENT_STORE_OP_STORE              // StoreAndResolveMSAA
      };

   ColorAttachmentVK::ColorAttachmentVK(const Ptr<Texture> texture,
      const uint32 mipmap,
      const uint32 layer) :
      mipmap(mipmap),
      layer(layer),
      clearColor(0.0f, 0.0f, 0.0f, 1.0f),
      resolveAttachment(false),
      resolveMipmap(0),
      resolveLayer(0)
   {
   assert( texture );
 
   const Ptr<TextureCommon> textureCommon = ptr_dynamic_cast<TextureCommon, Texture>(texture);

   assert( textureCommon->state.mipmaps > mipmap );
   assert( textureCommon->state.layers > layer );

   // TODO: Vulkan: Store texture reference !
   state.flags          = 0; // VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT - if attachments may alias/overlap in the same memory
   state.format         = TranslateTextureFormat[underlyingType(textureCommon->state.format)];  // TODO: Vulakn: Expose it from Texture and implement
   state.samples        = static_cast<VkSampleCountFlagBits>(textureCommon->state.samples);
   state.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
   state.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
   state.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // Ignored
   state.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Ignored
   state.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   state.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   }

   ColorAttachmentVK::~ColorAttachmentVK()
   {
   }

   void ColorAttachmentVK::onLoad(const LoadOperation load, const float4 _clearColor)
   {
   state.loadOp         = TranslateLoadOperation[underlyingType(load)];
   clearColor           = _clearColor;
   }

   void ColorAttachmentVK::onStore(const StoreOperation store)
   {
   state.storeOp        = TranslateStoreOperation[underlyingType(store)];

   // Mark if resolve is also enabled
   if ( store == StoreOperation::ResolveMSAA ||
        store == StoreOperation::StoreAndResolveMSAA )
      resolveAttachment = true;

   // TODO: Vulkan: When RenderPass is created, it should be validated that Resolve texture is bound, if resolve operation is set to ResolveMSAA.
   }

   bool ColorAttachmentVK::resolve(const Ptr<Texture> texture, const uint32 mipmap, const uint32 layer)
   {
   assert( texture );

   const Ptr<TextureCommon> textureCommon = ptr_dynamic_cast<TextureCommon, Texture>(texture);

   assert( textureCommon->state.mipmaps > mipmap );
   assert( textureCommon->state.layers > layer );

   // If we want to resolve Color, lets set it as resolve source
// TODO: Fix it, there can be only one Layout at a time ! Transition need to be correct !
//   state.initialLayout        |= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//   state.finalLayout          |= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

   // TODO: Vulkan: Store texture reference !
   resolveState.flags          = 0; // VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT - if attachments may alias/overlap in the same memory
   resolveState.format         = TranslateTextureFormat[underlyingType(textureCommon->state.format)];  // TODO: Vulakn: Expose it from Texture and implement
   resolveState.samples        = static_cast<VkSampleCountFlagBits>(textureCommon->state.samples);
   resolveState.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   resolveState.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
   resolveState.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // Ignored
   resolveState.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Ignored

// TODO: Fix it, there can be only one Layout at a time ! Transition need to be correct !
//   resolveState.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL | VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; // TODO: Vulkan: What's the difference between the two ?
//   resolveState.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL | VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; // TODO: Vulkan: Add special resolve for presenting surface

   resolveMipmap        = mipmap;
   resolveLayer         = layer;   
   resolveAttachment    = true;  
   }







// If the format has depth and/or stencil components, loadOp and storeOp apply only to the depth data, while
// stencilLoadOp and stencilStoreOp define how the stencil data is handled





   // Type of surface in DepthStencilAttachment
   enum AttachmentType
      {
      DepthStencil           = 0, // Depth or DepthStencil render target
      Stencil                   , // Stencil render target (for separate Depth and Stencil textures)
      ResolveDepthStencil       , // Target of resolve operation on Depth or DepthStencil texture
      ResolveSeparateStencil      // Target of resolve operation for separate Stencil texture
      };


   DepthStencilAttachmentVK::DepthStencilAttachmentVK(const Ptr<Texture> _depth,
      const Ptr<Texture> _stencil,
      const uint32 _mipmap,
      const uint32 _layer) : 
      clearDepth(1.0f),
      clearStencil(0),
      separateStencil(false),
      resolveDepthStencil(false),
      resolveSeparateStencil(false)
   {
   assert( _depth );

   for(uint32 i=0; i<4; ++i)
      {
      mipmap[i] = 0;
      layer[i]  = 0;
      }

   const Ptr<TextureCommon> depthCommon = ptr_dynamic_cast<TextureCommon, Texture>(_depth);

   assert( depthCommon->state.mipmaps > _mipmap );
   assert( depthCommon->state.layers > _layer );

   // TODO: Vulkan: Store texture reference !
   state[DepthStencil].flags          = 0; // VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT - if attachments may alias/overlap in the same memory
   state[DepthStencil].format         = TranslateTextureFormat[underlyingType(depthCommon->state.format)];  // TODO: Vulakn: Expose it from Texture and implement
   state[DepthStencil].samples        = static_cast<VkSampleCountFlagBits>(depthCommon->state.samples);
   state[DepthStencil].loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
   state[DepthStencil].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
   state[DepthStencil].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   state[DepthStencil].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
   if (TextureFormatIsDepthStencil(depthCommon->state.format))
      {
      state[DepthStencil].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;
      state[DepthStencil].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
      }
   state[DepthStencil].initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
   state[DepthStencil].finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

   mipmap[DepthStencil] = _mipmap;
   layer[DepthStencil]  = _layer;

   if (_stencil)
      {
      const Ptr<TextureCommon> stencilCommon = ptr_dynamic_cast<TextureCommon, Texture>(_stencil);

      // TODO: Vulkan: Validate that device supports separate Depth-Stencil attachments
      //               App should do that at the beginning, as this is device dependent run-time check

      // Separate Stencil must have the same type as Depth attachment.
      assert( stencilCommon->state.type == depthCommon->state.type );
      assert( stencilCommon->state.mipmaps > _mipmap );
      assert( stencilCommon->state.layers > _layer );

      // TODO: Vulkan: Store texture reference !
      state[Stencil].flags          = 0; // VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT - if attachments may alias/overlap in the same memory
      state[Stencil].format         = TranslateTextureFormat[underlyingType(stencilCommon->state.format)];  // TODO: Vulakn: Expose it from Texture and implement
      state[Stencil].samples        = static_cast<VkSampleCountFlagBits>(stencilCommon->state.samples);
      state[Stencil].loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // Ignored
      state[Stencil].storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Ignored
      state[Stencil].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;
      state[Stencil].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
      state[Stencil].initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      state[Stencil].finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      mipmap[Stencil] = _mipmap;
      layer[Stencil]  = _layer;
      separateStencil = true;
      }
   }

   DepthStencilAttachmentVK::~DepthStencilAttachmentVK()
   {
   }

   void DepthStencilAttachmentVK::onLoad(const LoadOperation loadDepthStencil, const float _clearDepth, const uint32 _clearStencil)
   {
   VkAttachmentLoadOp load = TranslateLoadOperation[underlyingType(loadDepthStencil)];

   // DepthStencil shared attachment load and store operations can be different.
   state[DepthStencil].loadOp        = load;
   state[DepthStencil].stencilLoadOp = load;

   clearDepth   = _clearDepth;
   clearStencil = _clearStencil;
   }

   void DepthStencilAttachmentVK::onStore(const StoreOperation storeDepthStencil)
   {
   VkAttachmentStoreOp store = TranslateStoreOperation[underlyingType(storeDepthStencil)];

   // DepthStencil shared attachment load and store operations can be different.
   state[DepthStencil].storeOp        = store;
   state[DepthStencil].stencilStoreOp = store; 

   // TODO: Vulkan: When using shared DepthStencil attachment, how looks resolve operation ???
   //               It needs to be set for both I assume. How is Stencil resolved ?

   //// Mark if resolve is also enabled
   //if ( store == StoreOperation::ResolveMSAA ||
   //     store == StoreOperation::StoreAndResolveMSAA )
   //   resolve = true;

   // TODO: Vulkan: When RenderPass is create, it should be validated that Resolve texture is bound, if resolve operation is set to ResolveMSAA.
   }

   bool DepthStencilAttachmentVK::resolve(const Ptr<Texture> _depth, const uint32 _mipmap, const uint32 _layer)
   {
   assert( _depth );

   const Ptr<TextureCommon> depthCommon = ptr_dynamic_cast<TextureCommon, Texture>(_depth);

   assert( depthCommon->state.mipmaps > _mipmap );
   assert( depthCommon->state.layers > _layer );

   // If we want to resolve DepthStencil / Depth & Stencil, lets set it as resolve source
// TODO: Fix it, there can be only one Layout at a time ! Transition need to be correct !
//   state[DepthStencil].initialLayout |= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//   state[DepthStencil].finalLayout   |= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

   // TODO: Vulkan: Does Vulkan allow resolve separate Depth & Stencil attachments to one DepthStencil ?
   if (separateStencil) // We could just set it and let RenderPass decide
      {
// TODO: Fix it, there can be only one Layout at a time ! Transition need to be correct !
//      state[Stencil].initialLayout   |= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//      state[Stencil].finalLayout     |= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      }

   // TODO: Vulkan: Store texture reference !
   state[ResolveDepthStencil].flags          = 0; // VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT - if attachments may alias/overlap in the same memory
   state[ResolveDepthStencil].format         = TranslateTextureFormat[underlyingType(depthCommon->state.format)];  // TODO: Vulakn: Expose it from Texture and implement
   state[ResolveDepthStencil].samples        = static_cast<VkSampleCountFlagBits>(depthCommon->state.samples);
   state[ResolveDepthStencil].loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   state[ResolveDepthStencil].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
   state[ResolveDepthStencil].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   state[ResolveDepthStencil].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
// TODO: Fix it, there can be only one Layout at a time ! Transition need to be correct !
//   state[ResolveDepthStencil].initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL | VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; // TODO: Vulkan: What's the difference between the two ?
//   state[ResolveDepthStencil].finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL | VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; 

   mipmap[ResolveDepthStencil] = _mipmap;
   layer[ResolveDepthStencil]  = _layer;
   
//   resolve       = true;  
   }

   void DepthStencilAttachmentVK::onStencilLoad(const LoadOperation loadStencil)
   {
   VkAttachmentLoadOp load = TranslateLoadOperation[underlyingType(loadStencil)];

   if (separateStencil) 
      state[Stencil].stencilLoadOp = load;  
   else
      state[DepthStencil].stencilLoadOp = load;
   }

   void DepthStencilAttachmentVK::onStencilStore(const StoreOperation storeStencil)
   {
   VkAttachmentStoreOp store = TranslateStoreOperation[underlyingType(storeStencil)];

   if (separateStencil)
      {
      state[Stencil].stencilStoreOp = store; 

      // Mark if Stencil resolve is also enabled
      if ( store == StoreOperation::ResolveMSAA ||
           store == StoreOperation::StoreAndResolveMSAA )
         resolveStencilSeparate = true;  // TODO: Vulkan: When RenderPass is created, it validate that Separate Stencil Resolve texture is bound.
      }
   else
      {
      // TODO: Vulkan: When using shared DepthStencil attachment, how looks resolve operation ???

      state[DepthStencil].stencilStoreOp = store;
      }
   }

   bool DepthStencilAttachmentVK::resolveStencil(const Ptr<Texture> _stencil, const uint32 _mipmap, const uint32 _layer)
   {
   assert( _stencil );

   // Resolving DepthStencil to separate Depth & Stencil is not supported
   assert( separateStencil );

   const Ptr<TextureCommon> stencilCommon = ptr_dynamic_cast<TextureCommon, Texture>(_stencil);

   assert( stencilCommon->state.mipmaps > mipmap );
   assert( stencilCommon->state.layers > layer );

   // If we want to resolve Stencil, lets set it as resolve source
   state[Stencil].initialLayout   |= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
   state[Stencil].finalLayout     |= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

   // TODO: Vulkan: Store texture reference !
   state[ResolveSeparateStencil].flags          = 0; // VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT - if attachments may alias/overlap in the same memory
   state[ResolveSeparateStencil].format         = TranslateTextureFormat[stencilCommon->state.format];  // TODO: Vulakn: Expose it from Texture and implement
   state[ResolveSeparateStencil].samples        = stencilCommon->state.samples;
   state[ResolveSeparateStencil].loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   state[ResolveSeparateStencil].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
   state[ResolveSeparateStencil].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   state[ResolveSeparateStencil].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
   state[ResolveSeparateStencil].initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL | VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; // TODO: Vulkan: What's the difference between the two ?
   state[ResolveSeparateStencil].finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL | VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; 

   mipmap[ResolveSeparateStencil] = _mipmap;
   layer[ResolveSeparateStencil]  = _layer; 
  
   resolveStencilSeparate = true;  
   }























   Ptr<RenderPass> VulkanDevice::create(uint32 _attachments,
                                        const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
                                        const Ptr<DepthStencilAttachment> depthStencil)
   {
   Ptr<RenderPass> result = nullptr;
   
   assert( _attachments < MaxColorAttachmentsCount );


   // const uint32v2 resolution, // Common attachments resolution
   // const uint32   layers)     // Common attachments layers count

   Ptr<Framebuffer> result = nullptr;
   
   VkImageView attachmentHandle[8];

	VkAttachmentDescription attachments[2];
	attachments[0].format = colorformat;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	attachments[1].format = depthFormat;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorReference = {};
	colorReference.attachment = 0;
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthReference = {};
	depthReference.attachment = 1;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.flags = 0;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = NULL;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorReference;
	subpass.pResolveAttachments = NULL;
	subpass.pDepthStencilAttachment = &depthReference;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = NULL;

   VkRenderPassCreateInfo renderPassInfo = {};
   renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
   renderPassInfo.pNext           = nullptr;
   renderPassInfo.attachmentCount = attachments;
   renderPassInfo.pAttachments    = attachmentDescription;
   renderPassInfo.subpassCount    = 1;
   renderPassInfo.pSubpasses      = &subInfo;
   renderPassInfo.dependencyCount = 0;



	renderPassInfo.attachmentCount = 2;
	renderPassInfo.pAttachments = attachments;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 0;
	renderPassInfo.pDependencies = NULL;

	VkResult err;

	err = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
	assert(!err);

   VkFramebufferCreateInfo framebufferInfo;
   framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
   framebufferInfo.pNext           = nullptr;
   framebufferInfo.flags           = 0;        // Reserved for future.
 //framebufferInfo.renderPass      =           // TODO: Assign RenderPass ID here
   framebufferInfo.attachmentCount = attachments;
   framebufferInfo.pAttachments    = attachmentHandle;
   framebufferInfo.width           = resolution.width;
   framebufferInfo.height          = resolution.height;
   framebufferInfo.layers          = layers;

   // Create framebuffer object
   VkFramebuffer framebuffer;
   VkResult res = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer); 
   if (!res)
      {
      Ptr<FramebufferVK> fbo = new FramebufferVK();
      fbo->id  = framebuffer;
      fbo->gpu = gpu;
      result = ptr_dynamic_cast<Framebuffer, FramebufferVK>(fbo);
      }

   return result;

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





   // Framebuffer


   class vkDepthStencilAttachment : public DepthStencilAttachment
      { 
      public:
      VkDepthStencilView id;
      };

   class vkFramebuffer : public Framebuffer
      {
      public:
      vkFramebuffer id;
      uint32        colorAttachments;
      };

   class vkCommandBuffer : public CommandBuffer
      {
      public:
      VkCmdBuffer id;

      void bind(const Ptr<RasterState> raster);
      void bind(const Ptr<ViewportScissorState> viewportScissor);
      void bind(const Ptr<DepthStencilState> depthStencil);
      void bind(const Ptr<BlendState> blend);
      };

    // VULKAN DEPTH ATTACHMENT
   //--------------------------------------------------------------------------

   Ptr<DepthStencilAttachment> Create(const Ptr<Texture> depthStencil, 
                                      const Ptr<Texture> resolveMsaa, 
                                      const uint32 depthStencilMipMap,
                                      const uint32 msaaMipMap,
                                      const uint32 depthStencilLayer,
                                      const uint32 msaaLayer,
                                      const uint32 layers)
   {
   Ptr<DepthStencilAttachment> result = nullptr;

   Ptr<vkTexture> vkDepthStencil = ptr_dynamic_cast<vkTexture, Texture>(depthStencil);
   Ptr<vkTexture> vkMsaa         = ptr_dynamic_cast<vkTexture, Texture>(resolveMsaa);

   // Determine depth-stencil attachment type
   VkImageAspect aspect = VK_IMAGE_ASPECT_DEPTH | VK_IMAGE_ASPECT_STENCIL;
   if ( (depthStencil->state.format == FormatD_16) ||
        (depthStencil->state.format == FormatD_24) ||
        (depthStencil->state.format == FormatD_32) ||
        (depthStencil->state.format == FormatD_32_f) )
      aspect = VK_IMAGE_ASPECT_DEPTH;

   VkImageSubresourceRange resourceRange;
   resourceRange.aspect                    = aspect;
   resourceRange.baseMipLevel              = msaaMipMap;
   resourceRange.mipLevels                 = 1;
   resourceRange.baseArraySlice            = msaaLayer;
   resourceRange.arraySize                 = layers;

   VkDepthStencilViewCreateInfo depthStencilInfo;
   depthStencilInfo.sType                  = VK_STRUCTURE_TYPE_DEPTH_STENCIL_VIEW_CREATE_INFO;
   depthStencilInfo.pNext                  = nullptr;
   depthStencilInfo.image                  = vkDepthStencil->id;
   depthStencilInfo.mipLevel               = depthStencilMipMap;
   depthStencilInfo.baseArraySlice         = depthStencilLayer;
   depthStencilInfo.arraySize              = layers;
   depthStencilInfo.msaaResolveImage       = vkMsaa->id;
   depthStencilInfo.msaaResolveSubResource = resourceRange;
   depthStencilInfo.flags                  = 0;
                                          // VK_DEPTH_STENCIL_VIEW_CREATE_FLAGS:
                                          // VK_DEPTH_STENCIL_VIEW_CREATE_READ_ONLY_DEPTH_BIT
                                          // VK_DEPTH_STENCIL_VIEW_CREATE_READ_ONLY_STENCIL_BIT  

   VkDepthStencilView depthStencilView;
   VkResult res = vkCreateDepthStencilView( gpu[i].handle, &depthStencilInfo, &depthStencilView );
   if (!res)
      {
      Ptr<vkDepthStencilAttachment> vkAttachment = new vkDepthStencilAttachment();
      vkAttachment->id = depthStencilView;
      result = ptr_dynamic_cast<DepthStencilAttachment, vkDepthStencilAttachment>(vkAttachment);
      }

   return result;
   }

   // VULKAN FRAMEBUFFER
   //-------------------------------------------------------------

   Ptr<Framebuffer> Create(const Ptr<DepthStencilAttachment> depthStencil,
                           const Ptr<ColorAttachment> color0 );

   Ptr<Framebuffer> Create(const Ptr<DepthStencilAttachment> depthStencil,
                           const Ptr<ColorAttachment> color0,
                           const Ptr<ColorAttachment> color1 = nullptr,
                           const Ptr<ColorAttachment> color2 = nullptr,
                           const Ptr<ColorAttachment> color3 = nullptr,
                           const Ptr<ColorAttachment> color4 = nullptr,
                           const Ptr<ColorAttachment> color5 = nullptr,
                           const Ptr<ColorAttachment> color6 = nullptr,
                           const Ptr<ColorAttachment> color7 = nullptr);

   Ptr<Framebuffer> Create(const Ptr<DepthStencilAttachment> depthStencil,
                           const Ptr<ColorAttachment> color[MaxColorAttachmentsCount]);

   Ptr<Framebuffer> Create(const uint32v2 resolution,
                           const Ptr<DepthStencilAttachment> depthStencil,
                           const Ptr<ColorAttachment> color[MaxColorAttachmentsCount])
   {
   Ptr<Framebuffer> result;

   // Calculate color attachments count
   uint32 colorAttachments = 0;
   for(sint32 i=7; i>=0; --i)
      if (color[i] != nullptr)
         {
         colorAttachments = i + 1;
         break;
         }

   VkColorAttachmentBindInfo colorBindInfo[MaxColorAttachmentsCount];
   for(uint8 i=0; i<MaxColorAttachmentsCount; ++i)
      {
      colorBindInfo[i].view    = ptr_dynamic_cast<vkColorAttachment, ColorAttachment>(color[i])->id;
      colorBindInfo[i].layout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      }

   VkDepthStencilBindInfo depthStencilBindInfo;
   depthStencilBindInfo.view   = ptr_dynamic_cast<vkDepthStencilAttachment, DepthStencilAttachment>(depthStencil)->id;
   depthStencilBindInfo.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                              // VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL

   VkFramebufferCreateInfo framebufferInfo;
   framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
   framebufferInfo.pNext                   = nullptr;
   framebufferInfo.colorAttachmentCount    = colorAttachments;
   framebufferInfo.pColorAttachments       = &colorBindInfo[0];
   framebufferInfo.pDepthStencilAttachment = &depthStencilBindInfo;

   // Describes behaviour of Framebuffer when there are no attachments.
   // Need to be always specified, even when attachments are present.
   // Rendering area can be smaller than common intersection of attachments.
   // (greater size undefined?) 
   framebufferInfo.sampleCount             = 1;                      // We can specify more samples for multisampling RT's
   framebufferInfo.width                   = resolution.width;       // ???? - Is this FBO shared size, or rectangle that will be updated (for optimization purposes?)
   framebufferInfo.height                  = resolution.height;
   framebufferInfo.layers                  = 1;                      // ???? - Can we specify 6 layers when rendering to CubeMap's or Tex Arrays - ?????

   VkFramebuffer fbo;
   VkResult res = vkCreateFramebuffer( gpu[i].handle, &framebufferInfo, &fbo );
   if (!res)
      {
      Ptr<vkFramebuffer> vkFbo = new vkFramebuffer();
      vkFbo->id               = fbo;
      vkFbo->colorAttachments = colorAttachments;
      result = ptr_dynamic_cast<Framebuffer, vkFramebuffer>(vkFbo);
      }

   return result;
   }
   









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
   
   VkImageView attachmentHandle[8];

   VkFramebufferCreateInfo framebufferInfo;
   framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
   framebufferInfo.pNext           = nullptr;
   framebufferInfo.flags           = 0;        // Reserved for future.
 //framebufferInfo.renderPass      =           // TODO: Assign RenderPass ID here
   framebufferInfo.attachmentCount = attachments;
   framebufferInfo.pAttachments    = attachmentHandle;
   framebufferInfo.width           = resolution.width;
   framebufferInfo.height          = resolution.height;
   framebufferInfo.layers          = layers;

   // Create framebuffer object
   VkFramebuffer framebuffer;
   VkResult res = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer); 
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


   }
}
#endif
