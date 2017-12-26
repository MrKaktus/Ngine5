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

#include "core/rendering/vulkan/vkRenderPass.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/defines.h"
#include "core/rendering/vulkan/vkTexture.h"   // For TranslateTextureFormat[]
#include "core/rendering/vulkan/vkDevice.h"    // TODO: We only want Device class, not all subsystems

#include "utilities/utilities.h" // For underlyingType()

namespace en
{
   namespace gpu
   { 
   static const VkAttachmentLoadOp TranslateLoadOperation[underlyingType(LoadOperation::Count)] =
      {
      VK_ATTACHMENT_LOAD_OP_DONT_CARE,          // None
      VK_ATTACHMENT_LOAD_OP_LOAD,               // Load
      VK_ATTACHMENT_LOAD_OP_CLEAR               // Clear
      };
   
   static const VkAttachmentStoreOp TranslateStoreOperation[underlyingType(StoreOperation::Count)] =
      {
      VK_ATTACHMENT_STORE_OP_DONT_CARE,         // Discard
      VK_ATTACHMENT_STORE_OP_STORE,             // Store
      VK_ATTACHMENT_STORE_OP_DONT_CARE,         // Resolve
      VK_ATTACHMENT_STORE_OP_STORE,             // StoreAndResolve
      };

   
   // COLOR ATTACHMENT
   //////////////////////////////////////////////////////////////////////////


   #define AttColor          0
   #define AttResolve        1

   ColorAttachmentVK::ColorAttachmentVK(const Format format, const uint32 samples) :
      resolve(false)
   {
   clearValue.color.float32[0] = 0.0f;
   clearValue.color.float32[1] = 0.0f;
   clearValue.color.float32[2] = 0.0f;
   clearValue.color.float32[3] = 1.0f;

   state[AttColor].flags          = 0; // TODO: VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT - if attachments may alias/overlap in the same memory
   state[AttColor].format         = TranslateTextureFormat[underlyingType(format)];
   state[AttColor].samples        = static_cast<VkSampleCountFlagBits>(samples);
   state[AttColor].loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
   state[AttColor].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
   state[AttColor].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // Ignored
   state[AttColor].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Ignored
   state[AttColor].initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   state[AttColor].finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   }

   ColorAttachmentVK::~ColorAttachmentVK()
   {
   }

   void ColorAttachmentVK::onLoad(const LoadOperation load, const float4 _clearValue)
   {
   state[AttColor].loadOp = TranslateLoadOperation[underlyingType(load)];
   clearValue.color.float32[0] = _clearValue.r;
   clearValue.color.float32[1] = _clearValue.g;
   clearValue.color.float32[2] = _clearValue.b;
   clearValue.color.float32[3] = _clearValue.a;
   }
   
   void ColorAttachmentVK::onLoad(const LoadOperation load, const uint32v4 _clearValue)
   {
   state[AttColor].loadOp = TranslateLoadOperation[underlyingType(load)];
   clearValue.color.uint32[0]  = _clearValue.x;
   clearValue.color.uint32[1]  = _clearValue.y;
   clearValue.color.uint32[2]  = _clearValue.z;
   clearValue.color.uint32[3]  = _clearValue.w;
   }
   
   void ColorAttachmentVK::onLoad(const LoadOperation load, const sint32v4 _clearValue)
   {
   state[AttColor].loadOp = TranslateLoadOperation[underlyingType(load)];
   clearValue.color.int32[0] = _clearValue.x;
   clearValue.color.int32[1] = _clearValue.y;
   clearValue.color.int32[2] = _clearValue.z;
   clearValue.color.int32[3] = _clearValue.w;
   }

   void ColorAttachmentVK::onStore(const StoreOperation store)
   {
   state[AttColor].storeOp = TranslateStoreOperation[underlyingType(store)];
   resolve = false;

   if (store == StoreOperation::Resolve ||
       store == StoreOperation::StoreAndResolve)
      {
      // Source surface need to be MSAA to be later resolved
      assert( state[AttColor].samples > 1 );

      // If app want to just store resolved image, we discard temporary MSAA content
      if (store == StoreOperation::Resolve)
          state[AttColor].storeOp      = VK_ATTACHMENT_STORE_OP_DONT_CARE;

      state[AttResolve].flags          = 0; // TODO: VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT - if attachments may alias/overlap in the same memory
      state[AttResolve].format         = state[AttColor].format;
      state[AttResolve].samples        = VK_SAMPLE_COUNT_1_BIT;
      state[AttResolve].loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // We Resolve to it, so don't care
      state[AttResolve].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
      state[AttResolve].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // Ignored
      state[AttResolve].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Ignored
      state[AttResolve].initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 
      state[AttResolve].finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // TODO: Vulkan: Add special resolve for presenting surface
      resolve = true;
      }
   }


   // DEPTH-STENCIL ATTACHMENT
   //////////////////////////////////////////////////////////////////////////


   // Vulkan 1.0.9 WSI - Page 117:
   // "Any given element of pResolveAttachments must have the same VkFormat as its corresponding color attachment"
   //
   //  Vulkan 1.0.9 WSI - Page 367:
   //
   // vkCmdResolveImage
   //
   // "srcSubresource and dstSubresource are VkImageSubresourceLayers structures specifying the
   //  subresources of the images used for the source and destination image data, respectively. Resolve of depth/stencil
   //  images is not supported."
   //
   // This leads to conclusion that Depth/Stencil cannot be resolved by the API at all.
   //
   // pDepthStencilAttachment field in VkSubpassDescription struct allows description of only one surface, thus
   // Vulkan is not supporting separate Depth and Stencil attachments at the same time.
   //
   // Metal API 
   //
   // https://developer.apple.com/library/ios/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/MetalFeatureSetTables/MetalFeatureSetTables.html
   // and details in https://developer.apple.com/library/ios/documentation/Metal/Reference/MTLRenderPassDepthAttachmentDescriptor_Ref/#//apple_ref/c/tdef/MTLMultisampleDepthResolveFilter
   //
   // Metal allows separate Depth and Stencil attachments.
   //
   // Metal allows Depth attachment resolve on iOS GPU Family 3 v1.
   //


   // If you don't want to use Depth, nor Stencil surfaces, just don't assign
   // this object at all at Render Pass creation time.
   DepthStencilAttachmentVK::DepthStencilAttachmentVK(const Format depthFormat, 
                                                      const Format stencilFormat,
                                                      const uint32 samples) :
      clearDepth(1.0f),
      clearStencil(0)
   {
   // Vulkan doesn't support separate Depth and Stencil surfaces at the same time.
   assert( !(depthFormat   != Format::Unsupported && 
             stencilFormat != Format::Unsupported) );
   assert( depthFormat   != Format::Unsupported || 
           stencilFormat != Format::Unsupported );

   // Needs to be DepthStencil, Depth or Stencil
   assert( TextureFormatIsDepthStencil(depthFormat) ||
           TextureFormatIsDepth(depthFormat)        ||
           TextureFormatIsStencil(stencilFormat) );

   state.flags          = 0; // TODO: VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT - if attachments may alias/overlap in the same memory
   state.format         = depthFormat != Format::Unsupported ? TranslateTextureFormat[underlyingType(depthFormat)] :
                                                               TranslateTextureFormat[underlyingType(stencilFormat)];
   state.samples        = static_cast<VkSampleCountFlagBits>(samples);
   state.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;    // If this is Stencil only format, it's ignored
   state.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;  // If this is Stencil only format, it's ignored
   state.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;    // If this is Depth only format, it's ignored
   state.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;  // If this is Depth only format, it's ignored
   state.initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
   state.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
   }

   DepthStencilAttachmentVK::~DepthStencilAttachmentVK()
   {
   }

   void DepthStencilAttachmentVK::onLoad(const LoadOperation loadDepthStencil,
                                         const float _clearDepth,
                                         const uint32 _clearStencil)
   {
   VkAttachmentLoadOp load = TranslateLoadOperation[underlyingType(loadDepthStencil)];

   // DepthStencil load and store operations can be different,
   // but by default we set them to the same operation.
   state.loadOp        = load;
   state.stencilLoadOp = load;

   clearDepth   = _clearDepth;
   clearStencil = _clearStencil;
   }

   void DepthStencilAttachmentVK::onStore(const StoreOperation storeDepthStencil,
                                          const DepthResolve resolveMode)
   {
   VkAttachmentStoreOp store = TranslateStoreOperation[underlyingType(storeDepthStencil)];

   // DepthStencil load and store operations can be different,
   // but by default we set them to the same operation.
   state.storeOp        = store;
   state.stencilStoreOp = store;

   // Vulkan is not supporting Depth/Stencil resolve operations.
   }

   void DepthStencilAttachmentVK::onStencilLoad(const LoadOperation loadStencil)
   {
   // Vulkan doesn't support separate Depth and Stencil surfaces at the same time.
   // (thus we don't save it in state[Stencil].stencilLoadOp )
   state.stencilLoadOp = TranslateLoadOperation[underlyingType(loadStencil)];
   }

   void DepthStencilAttachmentVK::onStencilStore(const StoreOperation storeStencil)
   {
   // Vulkan doesn't support separate Depth and Stencil surfaces at the same time.
   // (thus we don't save it in state[Stencil].stencilStoreOp )
   state.stencilStoreOp = TranslateStoreOperation[underlyingType(storeStencil)];
   }


   // FRAMEBUFFER
   //////////////////////////////////////////////////////////////////////////


   FramebufferVK::FramebufferVK(VulkanDevice* _gpu, const VkFramebuffer _handle, const uint32v2 _resolution, const uint32 _layers) :
      gpu(_gpu),
      handle(_handle),
      resolution(_resolution),
      layers(_layers)
   {
   }

   FramebufferVK::~FramebufferVK()
   {
   ValidateNoRet( gpu, vkDestroyFramebuffer(gpu->device, handle, nullptr) )
   }


   // RENDER PASS
   //////////////////////////////////////////////////////////////////////////


   RenderPassVK::RenderPassVK(VulkanDevice* _gpu,
                              const VkRenderPass _handle,
                              const uint32 _usedAttachments,
                              const uint32 _surfaces,
                              const bool   _resolve,
                              const bool   _depthStencil) :
      gpu(_gpu),
      handle(_handle),
      usedAttachments(_usedAttachments),
      surfaces(_surfaces),
      clearValues(surfaces ? new VkClearValue[surfaces] : nullptr),
      resolve(_resolve),
      depthStencil(_depthStencil)
   {
   // Init clear values array
   if (surfaces)
      memset(clearValues, 0, sizeof(float) * 4 * surfaces);
   }

   RenderPassVK::~RenderPassVK()
   {
   delete [] clearValues;
   ValidateNoRet( gpu, vkDestroyRenderPass(gpu->device, handle, nullptr) )
   }

   shared_ptr<Framebuffer> RenderPassVK::createFramebuffer(const uint32v2 resolution,
                                                           const uint32   layers,
                                                           const uint32   _surfaces,
                                                           const shared_ptr<TextureView>* surface,
                                                           const shared_ptr<TextureView> _depthStencil,
                                                           const shared_ptr<TextureView> _stencil,
                                                           const shared_ptr<TextureView> _depthResolve)
   {
   // TODO: If there is only one surface view.
   //       If this view is default, and points to one of the Swap-Chain textures of any created Window.
   //       Get this window pre-created Framebuffer object and return pointer to it.

   shared_ptr<FramebufferVK> result = nullptr;

   uint32 attachments = bitsCount(usedAttachments);

   assert( layers >= 1 );
   assert( surface );
   assert( _surfaces >= attachments );   // Used color attachments count (or double that if resolving)
   assert( _depthStencil == nullptr ||   // Vulkan is not supporting separate Depth and Stencil at the same time.
           _stencil      == nullptr );   // (but it supports Depth / DepthStencil or just Stencil setups).
   assert( _depthResolve == nullptr );   // Vulkan is not supporting Depth resolve

   // Create Framebuffer object only if render pass usues any destination surfaces
   if (attachments == 0u && depthStencil == false)
      return shared_ptr<Framebuffer>(nullptr);

   // Surface Views for all attachments (color, resolve, depth and stencil)
   VkImageView* views = new VkImageView[surfaces];

   // Gather Texture Views 
   uint32 index = 0;
   for(uint32 i=0; i<attachments; ++i)
      {
      views[index] = reinterpret_cast<TextureViewVK*>(surface[index].get())->handle;
      index++;
      }
      
   if (resolve)
      for(uint32 i=0; i<attachments; ++i)
         {
         views[index] = reinterpret_cast<TextureViewVK*>(surface[index].get())->handle;
         index++;
         }
      
   if (depthStencil)
      {
      if (_depthStencil)
         views[index] = reinterpret_cast<TextureViewVK*>(_depthStencil.get())->handle;
      else
      if (_stencil)
         views[index] = reinterpret_cast<TextureViewVK*>(_stencil.get())->handle;
      }

   // TODO: This function should only validate that all surfaces have the same
   //       resolution and layers count!
   //       Calculation of common resolution and layers should be done by the app !
   
   // Describe Framebuffer
   VkFramebufferCreateInfo framebufferInfo;
   framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
   framebufferInfo.pNext           = nullptr;
   framebufferInfo.flags           = 0; // Reserved for future.
   framebufferInfo.renderPass      = handle;
   framebufferInfo.attachmentCount = surfaces;
   framebufferInfo.pAttachments    = views;
   framebufferInfo.width           = resolution.width;  // Resolution is also used on RenderPass start on CommandBuffer
   framebufferInfo.height          = resolution.height;
   framebufferInfo.layers          = layers;

   // Create framebuffer object
   VkFramebuffer framebuffer = VK_NULL_HANDLE;
   Validate( gpu, vkCreateFramebuffer(gpu->device, &framebufferInfo, nullptr, &framebuffer) )
   if (gpu->lastResult[Scheduler.core()] == VK_SUCCESS)
      result = make_shared<FramebufferVK>(gpu, framebuffer, resolution, layers);
      
   delete [] views;

   return result;
   }
   
   shared_ptr<Framebuffer> RenderPassVK::createFramebuffer(const uint32v2 resolution,
                                                           const shared_ptr<TextureView> swapChainSurface,
                                                           const shared_ptr<TextureView> _depthStencil,
                                                           const shared_ptr<TextureView> _stencil)
   {
   assert( swapChainSurface );
   assert( _depthStencil == nullptr ||   // Vulkan is not supporting separate Depth and Stencil at the same time.
           _stencil      == nullptr );   // (but it supports Depth / DepthStencil or just Stencil setups).

   // Resolution cannot be greater than selected destination size (Swap-Chain surfaces have no mipmaps)
   TextureViewVK* ptr = reinterpret_cast<TextureViewVK*>(swapChainSurface.get());
   assert( resolution.width  <= ptr->texture->state.width );
   assert( resolution.height <= ptr->texture->state.height );

   assert( surfaces > 0 );
   assert( surfaces <= 2 );

   uint32 attachments = bitsCount(usedAttachments);
   assert( attachments == 1 );

   shared_ptr<FramebufferVK> result = nullptr;

   // Surface Views for all attachments (color, depth and stencil)
   VkImageView* views = new VkImageView[surfaces];
      
   // Gather Texture Views 
   uint32 index = 0;
   views[index] = ptr->handle;
   index++;
  
   // Check if there is depth / depth-stencil / stencil destination
   if (surfaces == 2)
      {
      assert( _depthStencil || _stencil );

      if (_depthStencil)
         views[index] = reinterpret_cast<TextureViewVK*>(_depthStencil.get())->handle;
      else
      if (_stencil)
         views[index] = reinterpret_cast<TextureViewVK*>(_stencil.get())->handle;
      }

   // TODO: This function should only validate that all surfaces have the same
   //       resolution and layers count!
   //       Calculation of common resolution and layers should be done by the app !

   // Describe Framebuffer
   VkFramebufferCreateInfo framebufferInfo;
   framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
   framebufferInfo.pNext           = nullptr;
   framebufferInfo.flags           = 0u; // Reserved for future.
   framebufferInfo.renderPass      = handle;
   framebufferInfo.attachmentCount = surfaces;
   framebufferInfo.pAttachments    = views;
   framebufferInfo.width           = resolution.width;  // Resolution is also used on RenderPass start on CommandBuffer
   framebufferInfo.height          = resolution.height;
   framebufferInfo.layers          = 1u;

   // Create framebuffer object
   VkFramebuffer framebuffer = VK_NULL_HANDLE;
   Validate( gpu, vkCreateFramebuffer(gpu->device, &framebufferInfo, nullptr, &framebuffer) )
   if (gpu->lastResult[Scheduler.core()] == VK_SUCCESS)
      result = make_shared<FramebufferVK>(gpu, framebuffer, resolution, 1u);
      
   delete [] views;

   return result;
   }
      
   shared_ptr<Framebuffer> RenderPassVK::createFramebuffer(const uint32v2 resolution,
                                                           const shared_ptr<TextureView> temporaryMSAA,
                                                           const shared_ptr<TextureView> swapChainSurface,
                                                           const shared_ptr<TextureView> _depthStencil,
                                                           const shared_ptr<TextureView> _stencil)
   {
   assert( temporaryMSAA );
   assert( swapChainSurface );
   assert( _depthStencil == nullptr ||   // Vulkan is not supporting separate Depth and Stencil at the same time.
           _stencil      == nullptr );   // (but it supports Depth / DepthStencil or just Stencil setups).

   // Resolution cannot be greater than selected destination size (Swap-Chain surfaces have no mipmaps)
   TextureViewVK* source      = reinterpret_cast<TextureViewVK*>(temporaryMSAA.get());
   TextureViewVK* destination = reinterpret_cast<TextureViewVK*>(swapChainSurface.get());
   assert( source->texture->state.width  == destination->texture->state.width );
   assert( source->texture->state.height == destination->texture->state.height );
   assert( resolution.width  <= destination->texture->state.width );
   assert( resolution.height <= destination->texture->state.height );

   assert( surfaces >= 2 );
   assert( surfaces <= 3 );

   uint32 attachments = bitsCount(usedAttachments);
   assert( attachments == 1 );

   shared_ptr<FramebufferVK> result = nullptr;

   // Surface Views for all attachments (color, resolve, depth and stencil)
   VkImageView* views = new VkImageView[surfaces];
      
   // Gather Texture Views 
   uint32 index = 0;
   views[index] = source->handle;
   index++;

   // Resolve to Swap-Chain
   views[index] = destination->handle;
   index++;
  
   // Check if there is depth-stencil destination
   if (surfaces == 3)
      {
      if (_depthStencil)
         views[index] = reinterpret_cast<TextureViewVK*>(_depthStencil.get())->handle;
      else
      if (_stencil)
         views[index] = reinterpret_cast<TextureViewVK*>(_stencil.get())->handle;
      }

   // TODO: This function should only validate that all surfaces have the same
   //       resolution and layers count!
   //       Calculation of common resolution and layers should be done by the app !

   // Describe Framebuffer
   VkFramebufferCreateInfo framebufferInfo;
   framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
   framebufferInfo.pNext           = nullptr;
   framebufferInfo.flags           = 0; // Reserved for future.
   framebufferInfo.renderPass      = handle;
   framebufferInfo.attachmentCount = surfaces;
   framebufferInfo.pAttachments    = views;
   framebufferInfo.width           = resolution.width;  // Resolution is also used on RenderPass start on CommandBuffer
   framebufferInfo.height          = resolution.height;
   framebufferInfo.layers          = 1u;

   // Create framebuffer object
   VkFramebuffer framebuffer = VK_NULL_HANDLE;
   Validate( gpu, vkCreateFramebuffer(gpu->device, &framebufferInfo, nullptr, &framebuffer) )
   if (gpu->lastResult[Scheduler.core()] == VK_SUCCESS)
      result = make_shared<FramebufferVK>(gpu, framebuffer, resolution, 1u);
      
   delete [] views;

   return result;
   }
   

   // DEVICE
   //////////////////////////////////////////////////////////////////////////


   shared_ptr<ColorAttachment> VulkanDevice::createColorAttachment(const Format format, const uint32 samples)
   {
   return make_shared<ColorAttachmentVK>(format, samples);
   }

   shared_ptr<DepthStencilAttachment> VulkanDevice::createDepthStencilAttachment(const Format depthFormat, 
                                                                                 const Format stencilFormat,
                                                                                 const uint32 samples)
   {
   return make_shared<DepthStencilAttachmentVK>(depthFormat, stencilFormat, samples);
   }
   
   // Creates render pass which's output goes to window framebuffer.
   // Swap-Chain surface may be destination of MSAA resolve operation.
   shared_ptr<RenderPass> VulkanDevice::createRenderPass(const shared_ptr<ColorAttachment> swapChainSurface,
                                                         const shared_ptr<DepthStencilAttachment> depthStencil)
   {
   shared_ptr<RenderPassVK> result = nullptr;
   
   assert( swapChainSurface );

   uint32 surfaces = 0u;
   bool   resolve  = false;
   uint32 usedAttachments = 1u;

   ColorAttachmentVK* ptr = reinterpret_cast<ColorAttachmentVK*>(swapChainSurface.get());
   
   // Color attachment
   VkAttachmentReference attColor;
   attColor.attachment = surfaces;
   attColor.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 
   surfaces++;

   // Optional Color Resolve
   VkAttachmentReference attResolve;
   if (resolve)
      {
      attResolve.attachment = surfaces;
      attResolve.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      surfaces++;
      }
      
   // Optional Depth-Stencil / Depth / Stencil
   VkAttachmentReference attDepthStencil;
   if (depthStencil)
      {
      attDepthStencil.attachment = surfaces;
      attDepthStencil.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      surfaces++;
      
      // TODO: How to determine below situation ?
      // VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL - is a once rendered depth buffer, now used for depth test OR sample as shadow map
      // We would need to know from the current pipeline object if depth/stencil test/write is enabled to safely set it.
      }

   // Engine is currently not supporting multiple sub-passes described as one dependency graph
   VkSubpassDescription subInfo;
   subInfo.flags                   = 0u;  // Reserved for future
   subInfo.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
   subInfo.inputAttachmentCount    = 0u;
   subInfo.pInputAttachments       = nullptr;
   subInfo.colorAttachmentCount    = 1u;
   subInfo.pColorAttachments       = &attColor;
   subInfo.pResolveAttachments     = resolve      ? &attResolve      : nullptr;
   subInfo.pDepthStencilAttachment = depthStencil ? &attDepthStencil : nullptr;
   subInfo.preserveAttachmentCount = 0;
   subInfo.pPreserveAttachments    = nullptr;

   // Gather global Attachment states (for all subpasses)
   uint32 index = 0;
   VkAttachmentDescription* attachment = new VkAttachmentDescription[surfaces];
   memcpy(&attachment[index], &ptr->state[AttColor], sizeof(VkAttachmentDescription));
   index++;

   if (resolve)
      {
      memcpy(&attachment[index], &ptr->state[AttResolve], sizeof(VkAttachmentDescription));
      index++;
      }

   if (depthStencil)
      {
      DepthStencilAttachmentVK* ptr = reinterpret_cast<DepthStencilAttachmentVK*>(depthStencil.get());
      memcpy(&attachment[index], &ptr->state, sizeof(VkAttachmentDescription));
      index++;
      }

   // Sanity check
   assert( surfaces == index );

   // Describe final Render Pass
   VkRenderPassCreateInfo passInfo;
   passInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
   passInfo.pNext           = nullptr;
   passInfo.flags           = 0u;
   passInfo.attachmentCount = surfaces;
   passInfo.pAttachments    = attachment;
   passInfo.subpassCount    = 1;
   passInfo.pSubpasses      = &subInfo;
   passInfo.dependencyCount = 0;
   passInfo.pDependencies   = nullptr;

   // Create renderpass object
   VkRenderPass renderpass = VK_NULL_HANDLE;
   Validate( this, vkCreateRenderPass(device, &passInfo, nullptr, &renderpass) )
   if (lastResult[Scheduler.core()] == VK_SUCCESS)
      {
      result = make_shared<RenderPassVK>(this,
                                         renderpass,
                                         usedAttachments,
                                         surfaces,
                                         resolve,
                                         depthStencil ? true : false);
      assert( result );

      // Those values will be used at beginning of RenderPass
      index = 0;
      result->clearValues[index] = ptr->clearValue;
      index++;

      // Were not specifying clear values for MSAA resolve destinations (but need to keep entries)
      if (resolve)
         index *= 2;
      
      if (depthStencil)
         {
         DepthStencilAttachmentVK* ptr = reinterpret_cast<DepthStencilAttachmentVK*>(depthStencil.get());
         result->clearValues[index].depthStencil.depth   = ptr->clearDepth;
         result->clearValues[index].depthStencil.stencil = ptr->clearStencil;
         }
      }

   delete [] attachment;

   return result;
   }

   // Creates render pass. Entries in "color" array, match output
   // color attachment slots in Fragment Shader. Entries in this 
   // array may be set to nullptr, which means that given output
   // color attachment slot has no bound resource descriptor.
   shared_ptr<RenderPass> VulkanDevice::createRenderPass(const uint32 attachments,
                                                         const shared_ptr<ColorAttachment>* color,
                                                         const shared_ptr<DepthStencilAttachment> depthStencil)
   {
   shared_ptr<RenderPassVK> result = nullptr;
   
   assert( attachments < support.maxColorAttachments );

   // Vulkan allows pass without any color and depth attachments.
   // In such case, rasterization is still performed in width x height x layers x sampler space,
   // but it is expected that results will be outputted as a result of side effects operation.
   //
   // Metal is not supporting that !
   //
   // TODO: Make this method universal and internal. Provide common width/height/layers paarameters.
   //       There should be separate method for creating such RenderPass that accepts only those parameters.
   //
   uint32 surfaces = 0u;
   bool   resolve  = false;
   
   // Render Pass attachments layouy:
   // 0..7  - Up to 8 Color Attachments (tightly packed)
   // 8..15 - Up to 8 Resolve surfaces (tightly packed)
   // 16    - Depth-Stencil / Depth / Stencil
   
   uint32 usedAttachments = 0u;

   // Optional Color Attachments
   VkAttachmentReference* attColor = nullptr;
   if (attachments)
      {
      attColor = new VkAttachmentReference[attachments];
      for(uint32 i=0; i<attachments; ++i)
         {
         // Passed in array of Color Attachment descriptors may have empty slots.
         attColor[i].attachment = VK_ATTACHMENT_UNUSED;
         attColor[i].layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
         if (color[i])
            {
            if (reinterpret_cast<ColorAttachmentVK*>(color[i].get())->resolve)
               resolve = true;
            attColor[i].attachment = surfaces;
            setBit(usedAttachments, i);
            surfaces++;
            }
         }
      }
      
   // Optional Color Resolve
   VkAttachmentReference* attResolve = nullptr;
   if (resolve)
      {
      attResolve = new VkAttachmentReference[attachments];
      for(uint32 i=0; i<attachments; ++i)
         {
         attResolve[i].attachment = VK_ATTACHMENT_UNUSED;
         attResolve[i].layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
         if (checkBit(usedAttachments, i))
            {
            attResolve[i].attachment = surfaces;
            surfaces++;
            }
         }
      }
      
   // Optional Depth-Stencil / Depth / Stencil
   VkAttachmentReference attDepthStencil;
   if (depthStencil)
      {
      attDepthStencil.attachment = surfaces;
      attDepthStencil.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      surfaces++;
      
      // TODO: How to determine below situation ?
      // VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL - is a once rendered depth buffer, now used for depth test OR sample as shadow map
      // We would need to know from the current pipeline object if depth/stencil test/write is enabled to safely set it.
      }

   // Engine is currently not supporting multiple sub-passes described as one dependency graph
   VkSubpassDescription subInfo;
   subInfo.flags                   = 0u;  // Reserved for future
   subInfo.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
   subInfo.inputAttachmentCount    = 0u;
   subInfo.pInputAttachments       = nullptr;
   subInfo.colorAttachmentCount    = attachments;
   subInfo.pColorAttachments       = attachments  ? attColor         : nullptr;
   subInfo.pResolveAttachments     = resolve      ? attResolve       : nullptr;
   subInfo.pDepthStencilAttachment = depthStencil ? &attDepthStencil : nullptr;
   subInfo.preserveAttachmentCount = 0;
   subInfo.pPreserveAttachments    = nullptr;


   // Gather global Attachment states (for all subpasses)
   uint32 index = 0;
   VkAttachmentDescription* attachment = nullptr;
   if (surfaces)
      attachment = new VkAttachmentDescription[surfaces];
   for(uint32 i=0; i<attachments; ++i)
      if (checkBit(usedAttachments, i))
         {
         ColorAttachmentVK* ptr = reinterpret_cast<ColorAttachmentVK*>(color[i].get());
         memcpy(&attachment[index], &ptr->state[AttColor], sizeof(VkAttachmentDescription));
         index++;
         }
      
   if (resolve)
      for(uint32 i=0; i<attachments; ++i)
         if (checkBit(usedAttachments, i))
            {
            ColorAttachmentVK* ptr = reinterpret_cast<ColorAttachmentVK*>(color[i].get());
            memcpy(&attachment[index], &ptr->state[AttResolve], sizeof(VkAttachmentDescription));
            index++;
            }
            
   if (depthStencil)
      {
      DepthStencilAttachmentVK* ptr = reinterpret_cast<DepthStencilAttachmentVK*>(depthStencil.get());
      memcpy(&attachment[index], &ptr->state, sizeof(VkAttachmentDescription));
      index++;
      }

   // Sanity check
   assert( surfaces == index );

   // Describe final Render Pass
   VkRenderPassCreateInfo passInfo;
   passInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
   passInfo.pNext           = nullptr;
   passInfo.attachmentCount = surfaces;
   passInfo.pAttachments    = attachment;
   passInfo.subpassCount    = 1;
   passInfo.pSubpasses      = &subInfo;
   passInfo.dependencyCount = 0;
   passInfo.pDependencies   = nullptr;

   // Create renderpass object
   VkRenderPass renderpass = VK_NULL_HANDLE;
   Validate( this, vkCreateRenderPass(device, &passInfo, nullptr, &renderpass) )
   if (lastResult[Scheduler.core()] == VK_SUCCESS)
      {
      result = make_shared<RenderPassVK>(this,
                                         renderpass,
                                         usedAttachments,
                                         surfaces,
                                         resolve,
                                         depthStencil ? true : false);
      assert( result );

      // Those values will be used at beginning of RenderPass
      index = 0;
      for(uint32 i=0; i<attachments; ++i)
         if (checkBit(usedAttachments, i))
            {
            ColorAttachmentVK* ptr = reinterpret_cast<ColorAttachmentVK*>(color[i].get());
            result->clearValues[index] = ptr->clearValue;
            index++;
            }
         
      // Were not specifying clear values for MSAA resolve destinations (but need to keep entries)
      if (resolve)
         index *= 2;
      
      if (depthStencil)
         {
         DepthStencilAttachmentVK* ptr = reinterpret_cast<DepthStencilAttachmentVK*>(depthStencil.get());
         result->clearValues[index].depthStencil.depth   = ptr->clearDepth;
         result->clearValues[index].depthStencil.stencil = ptr->clearStencil;
         }
      }

   delete [] attColor;
   delete [] attResolve;
   delete [] attachment;

   return result;
   }







   //shared_ptr<RenderPass> VulkanDevice::createRenderPass(const shared_ptr<ColorAttachment> color,
   //                                               const shared_ptr<DepthStencilAttachment> depthStencil)
   //{
   //// At least one of the two needs to be present
   //assert( color || depthStencil );

   //// Calculate resolution of mipmap used as rendering destination,
   //// and count of available layers. Validate that they are identical
   //// between all used surfaces.
   //uint32v2 resolution;
   //uint32 layers = 1;
   //bool selected = false;
   //if (color)
   //   {
   //   ColorAttachmentVK* ptr = reinterpret_cast<ColorAttachmentVK*>(color.get());
   //   shared_ptr<TextureViewVK> view = ptr->view[Color];
   //   uint32 mipmap = view->mipmaps.base;
   //   resolution = view->texture->resolution(mipmap);
   //   layers     = view->layers.count;
   //   selected   = true;
   //   }
   //else
   //if (depthStencil)
   //   {
   //   DepthStencilAttachmentVK* ptr = reinterpret_cast<DepthStencilAttachmentVK*>(depthStencil.get());
   //   shared_ptr<TextureViewVK> view = ptr->view;
   //   uint32 mipmap = view->mipmaps.base;

   //   if (!selected)
   //      {
   //      resolution = view->texture->resolution(mipmap);
   //      layers     = view->layers.count;
   //      selected = true;
   //      }
   //   else
   //      {
   //      assert( resolution == view->texture->resolution(mipmap) );
   //      assert( layers     == view->layers.count );
   //      }
   //   }
   //assert( selected );

   //return createRenderPass(1, &color, depthStencil, resolution, layers);
   //}
   
      
   //shared_ptr<RenderPass> VulkanDevice::createRenderPass(uint32 _attachments,
   //                                               const shared_ptr<ColorAttachment>* color,
   //                                               const shared_ptr<DepthStencilAttachment> depthStencil)
   //{
   //// At least one type of the two needs to be present
   //assert( _attachments > 0 || depthStencil );
   //
   //// Calculate resolution of mipmap used as rendering destination,
   //// and count of available layers. Validate that they are identical
   //// between all used surfaces.
   //uint32v2 resolution;
   //uint32   layers = 1;
   //bool     selected = false;
   //if (_attachments)
   //   {
   //   for(uint32 i=0; i<_attachments; ++i)
   //      if (color[i]) // Allow empty entries in the input array.
   //         {
   //         ColorAttachmentVK* ptr = reinterpret_cast<ColorAttachmentVK*>(color[i].get());
   //         shared_ptr<TextureViewVK> view = ptr->view[Color];
   //         uint32 mipmap = view->mipmaps.base;

   //         if (!selected)
   //            {
   //            resolution = view->texture->resolution(mipmap);
   //            layers     = view->layers.count;
   //            selected = true;
   //            }
   //         else
   //            {
   //            assert( resolution == view->texture->resolution(mipmap) );
   //            assert( layers     == view->layers.count );
   //            }
   //         }
   //   }
   //if (depthStencil)
   //   {
   //   DepthStencilAttachmentVK* ptr = reinterpret_cast<DepthStencilAttachmentVK*>(depthStencil.get());
   //   shared_ptr<TextureViewVK> view = ptr->view;
   //   uint32 mipmap = view->mipmaps.base;

   //   if (!selected)
   //      {
   //      resolution = view->texture->resolution(mipmap);
   //      layers     = view->layers.count;
   //      selected = true;
   //      }
   //   else
   //      {
   //      assert( resolution == view->texture->resolution(mipmap) );
   //      assert( layers     == view->layers.count );
   //      }
   //   }
   //assert( selected );

   //return createRenderPass(_attachments, color, depthStencil, resolution, layers);



   ////// Calculate smallest common resolution and layers count of passed attachments,
   ////// or use explicitly pased ones if this Render Pass is not rendering anything
   ////// (has no attachments, uses shader side effects).
   ////uint32v2 resolution = explicitResolution;
   ////uint32   layers     = explicitLayers;
   ////if (surfaces)
   ////   {
   ////   for(uint32 i=0; i<attachments; ++i)
   ////      {
   ////      ColorAttachmentVK* ptr = reinterpret_cast<ColorAttachmentVK*>(color[i].get());
   ////      resolution.width  = min(resolution.width,  ptr->view[Color]->texture->state.width);
   ////      resolution.height = min(resolution.height, ptr->view[Color]->texture->state.height);
   ////      layers            = min(layers,            ptr->view[Color]->layers.count);
   ////      if (resolve)
   ////         {
   ////         resolution.width  = min(resolution.width,  ptr->view[Resolve]->texture->state.width);
   ////         resolution.height = min(resolution.height, ptr->view[Resolve]->texture->state.height);
   ////         layers            = min(layers,            ptr->view[Resolve]->layers.count);
   ////         }
   ////      }
   ////      
   ////   if (depthStencil)
   ////      {
   ////      DepthStencilAttachmentVK* ptr = reinterpret_cast<DepthStencilAttachmentVK*>(depthStencil.get());
   ////      resolution.width  = min(resolution.width,  ptr->view->texture->state.width);
   ////      resolution.height = min(resolution.height, ptr->view->texture->state.height);
   ////      layers            = min(layers,            ptr->view->layers.count);
   ////      }
   ////   }


   ////// Calculate resolution of mipmap used as rendering destination,
   ////// and count of available layers.


   ////   ColorAttachmentVK* ptr = reinterpret_cast<ColorAttachmentVK*>(color.get());
   ////   shared_ptr<TextureViewVK> view = ptr->view[Color];
   ////   uint32 mipmap = view->mipmaps.base;
   ////   resolution = view->texture->resolution(mipmap);
   ////   layers = view->layers.count;
   ////   }
   ////else
   ////if (depthStencil)
   ////   {
   ////   DepthStencilAttachmentVK* ptr = reinterpret_cast<DepthStencilAttachmentVK*>(depthStencil.get());
   ////   shared_ptr<TextureViewVK> view = ptr->view;
   ////   uint32 mipmap = view->mipmaps.base;
   ////   resolution = view->texture->resolution(mipmap);
   ////   layers = view->layers.count;
   ////   }

   //}












   //assert( view[Color]->texture->state.samples > 1 );              // Cannot resolve non-MSAA source
   //assert( view[Resolve]->texture->state.samples == 1 );           // Cannot resolve to MSAA destination
   //assert( view[Resolve]->viewFormat == view[Color]->viewFormat ); // Cannot resolve between different Pixel Formats
   //
   //// Cannot resolve between different resolutions
   //assert( view[Resolve]->texture->state.width   == view[Color]->texture->state.width );
   //assert( view[Resolve]->texture->state.height  == view[Color]->texture->state.height );

   //// Vulkan doesn't support separate Depth and Stencil surfaces at the same time.
   //assert( !(_depth && _stencil) );
   //assert( _depth || _stencil );
   //
   //if (_depth)
   //   view = ptr_reinterpret_cast<TextureViewVK>(&_depth);

   //if (_stencil)
   //   view = ptr_reinterpret_cast<TextureViewVK>(&_stencil);


























//typedef struct VkSubpassDependency {
//    uint32_t                                    srcSubpass;
//    uint32_t                                    dstSubpass;
//    VkPipelineStageFlags                        srcStageMask;
//    VkPipelineStageFlags                        dstStageMask;
//    VkAccessFlags                               srcAccessMask;
//    VkAccessFlags                               dstAccessMask;
//    VkDependencyFlags                           dependencyFlags;
//} VkSubpassDependency;



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






//
//   // VULKAN FRAMEBUFFER
//   //-------------------------------------------------------------
//
//   shared_ptr<Framebuffer> Create(const shared_ptr<DepthStencilAttachment> depthStencil,
//                           const shared_ptr<ColorAttachment> color0 );
//
//   shared_ptr<Framebuffer> Create(const shared_ptr<DepthStencilAttachment> depthStencil,
//                           const shared_ptr<ColorAttachment> color0,
//                           const shared_ptr<ColorAttachment> color1 = nullptr,
//                           const shared_ptr<ColorAttachment> color2 = nullptr,
//                           const shared_ptr<ColorAttachment> color3 = nullptr,
//                           const shared_ptr<ColorAttachment> color4 = nullptr,
//                           const shared_ptr<ColorAttachment> color5 = nullptr,
//                           const shared_ptr<ColorAttachment> color6 = nullptr,
//                           const shared_ptr<ColorAttachment> color7 = nullptr);
//
//   shared_ptr<Framebuffer> Create(const shared_ptr<DepthStencilAttachment> depthStencil,
//                           const shared_ptr<ColorAttachment> color[MaxColorAttachmentsCount]);
//
//   shared_ptr<Framebuffer> Create(const uint32v2 resolution,
//                           const shared_ptr<DepthStencilAttachment> depthStencil,
//                           const shared_ptr<ColorAttachment> color[MaxColorAttachmentsCount])
//   {
//   shared_ptr<Framebuffer> result;
//
//   // Calculate color attachments count
//   uint32 colorAttachments = 0;
//   for(sint32 i=7; i>=0; --i)
//      if (color[i] != nullptr)
//         {
//         colorAttachments = i + 1;
//         break;
//         }
//
//   VkColorAttachmentBindInfo colorBindInfo[MaxColorAttachmentsCount];
//   for(uint8 i=0; i<MaxColorAttachmentsCount; ++i)
//      {
//      colorBindInfo[i].view    = reinterpret_cast<vkColorAttachment*>(color[i].get())->id;
//      colorBindInfo[i].layout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//      }
//
//   VkDepthStencilBindInfo depthStencilBindInfo;
//   depthStencilBindInfo.view   = reinterpret_cast<vkDepthStencilAttachment*>(depthStencil.get())->id;
//   depthStencilBindInfo.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//                              // VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
//
//   VkFramebufferCreateInfo framebufferInfo;
//   framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//   framebufferInfo.pNext                   = nullptr;
//   framebufferInfo.colorAttachmentCount    = colorAttachments;
//   framebufferInfo.pColorAttachments       = &colorBindInfo[0];
//   framebufferInfo.pDepthStencilAttachment = &depthStencilBindInfo;
//
//   // Describes behaviour of Framebuffer when there are no attachments.
//   // Need to be always specified, even when attachments are present.
//   // Rendering area can be smaller than common intersection of attachments.
//   // (greater size undefined?) 
//   framebufferInfo.sampleCount             = 1;                      // We can specify more samples for multisampling RT's
//   framebufferInfo.width                   = resolution.width;       // ???? - Is this FBO shared size, or rectangle that will be updated (for optimization purposes?)
//   framebufferInfo.height                  = resolution.height;
//   framebufferInfo.layers                  = 1;                      // ???? - Can we specify 6 layers when rendering to CubeMap's or Tex Arrays - ?????
//
//   VkFramebuffer fbo;
//   VkResult res = vkCreateFramebuffer( gpu[i].handle, &framebufferInfo, &fbo );
//   if (!res)
//      {
//      shared_ptr<vkFramebuffer> vkFbo = make_shared<vkFramebuffer>();
//      vkFbo->id               = fbo;
//      vkFbo->colorAttachments = colorAttachments;
//      result = vkFbo;
//      }
//
//   return result;
//   }
//   
//
//
//
//
//
//
//
//   shared_ptr<RenderPass> VulkanDevice::create(const shared_ptr<ColorAttachment> color,
//                                        const shared_ptr<DepthStencilAttachment> depthStencil)
//   {
//   // TODO: !!!
//   }
//
//   shared_ptr<RenderPass> VulkanDevice::create(uint32 _attachments,
//                                        const shared_ptr<ColorAttachment> color[MaxColorAttachmentsCount],
//                                        const shared_ptr<DepthStencilAttachment> depthStencil)
//   {
//   shared_ptr<RenderPass> result = nullptr;
//   
//   assert( _attachments < MaxColorAttachmentsCount );
//
//
//
//
//   // Gather Attachment states
//   VkAttachmentDescription* attachment = new VkAttachmentDescription[_attachments];
//   for(uint32 i=0; i<_attachments; ++i)
//      memcpy(&attachment[i], &color[i]->state, sizeof(VkAttachmentDescription));
//
//      // Render Pass attachments layouy:
//      // 0..7 - Up to 8 Color Attachments
//      // 8    - Depth / Depth-Stencil 
//      // 9    - Separate Stencil if supported
//
//   VkAttachmentReference* attColor   = nullptr;
//new VkAttachmentReference[_attachments];
//
//   VkAttachmentReference* attResolve = nullptr;
//new VkAttachmentReference[_attachments];
//
//   VkAttachmentReference attDepthStencil;
//   attDepthStencil.attachment = VK_ATTACHMENT_UNUSED;
//   attDepthStencil.layout     = VK_IMAGE_LAYOUT_UNDEFINED;
//   if (depthStencil)
//      {
//      attDepthStencil->attachment = 8;   
//      attDepthStencil->layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; 
//      // TODO: How to determine below situation ?
//      // VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;  // When DepthStencil texture is filled earlier in Depth pre-pass, and now only used for discarding
//      }
//
//   // Engine is currently not supporting multiple sub-passes described as one dependency graph
//   VkSubpassDescription subInfo;
//#if defined(EN_DISCRETE_GPU)
//   subInfo.flags                   = 0;
//#elif defined(EN_MOBILE_GPU)
//   subInfo.flags                   = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
//#endif
//   subInfo.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
//   subInfo.inputAttachmentCount    = 0;
//   subInfo.pInputAttachments       = nullptr;
//   subInfo.colorAttachmentCount    = _attachments;
//   subInfo.pColorAttachments       = attColor;
//   subInfo.pResolveAttachments     = attResolve;
//   subInfo.pDepthStencilAttachment = &attDepthStencil;
//   subInfo.preserveAttachmentCount = 0;
//   subInfo.pPreserveAttachments    = nullptr;
//
//
//	subpassDescription.depthStencilAttachment.attachment = ( framebufferLayout->internalDepthFormat != VK_FORMAT_UNDEFINED ) ? 1 : VK_ATTACHMENT_UNUSED;
//	subpassDescription.depthStencilAttachment.layout = ( framebufferLayout->internalDepthFormat != VK_FORMAT_UNDEFINED ) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
//	subpassDescription.preserveCount = 0;
//	subpassDescription.pPreserveAttachments = NULL;
//
//
//   VkRenderPassCreateInfo passInfo;
//   passInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//   passInfo.pNext           = nullptr;
//   passInfo.attachmentCount = attachments;
//   passInfo.pAttachments    = attachment;
//   passInfo.subpassCount    = 1;
//   passInfo.pSubpasses      = &subInfo;
//   passInfo.dependencyCount = 0;
//    const VkSubpassDependency*                  pDependencies;
//
//   // Create renderpass object
//   VkRenderPass renderpass;
//   VkResult res = vkCreateRenderPass(device, &passInfo, &defaultAllocCallbacks, &renderpass);
//   if (res == VK_SUCCESS)
//      {
//      shared_ptr<RenderPassVK> rpo = make_shared<RenderPassVK>();
//      rpo->id          = renderpass;
//      rpo->gpu         = this;
//      rpo->attachments = _attachments;
//      result = rpo;
//      }
//
//   delete [] attachment;
//
//   return result;
//   }
//
//   shared_ptr<Framebuffer> RenderPassVK::create(const uint32v2 resolution, // Common attachments resolution
//                                         const uint32   layers)     // Common attachments layers count
//   {
//   shared_ptr<Framebuffer> result = nullptr;
//   
//   VkImageView attachmentHandle[8];
//
//   VkFramebufferCreateInfo framebufferInfo;
//   framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//   framebufferInfo.pNext           = nullptr;
//   framebufferInfo.flags           = 0;        // Reserved for future.
// //framebufferInfo.renderPass      =           // TODO: Assign RenderPass ID here
//   framebufferInfo.attachmentCount = attachments;
//   framebufferInfo.pAttachments    = attachmentHandle;
//   framebufferInfo.width           = resolution.width;
//   framebufferInfo.height          = resolution.height;
//   framebufferInfo.layers          = layers;
//
//   // Create framebuffer object
//   VkFramebuffer framebuffer;
//   VkResult res = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer); 
//   if (!res)
//      {
//      shared_ptr<FramebufferVK> fbo = make_shared<FramebufferVK>();
//      fbo->id  = framebuffer;
//      fbo->gpu = gpu;
//      result = fbo;
//      }
//
//   return result;
//   }



   }
}
#endif
