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
      };

   
   // COLOR ATTACHMENT
   //////////////////////////////////////////////////////////////////////////


   #define Color          0
   #define Resolve        1

   ColorAttachmentVK::ColorAttachmentVK(const Ptr<TextureView> source)
   {
   assert( source );
 
   clearValue.color.float32[0] = 0.0f;
   clearValue.color.float32[1] = 0.0f;
   clearValue.color.float32[2] = 0.0f;
   clearValue.color.float32[3] = 1.0f;

   view[Color] = ptr_dynamic_cast<TextureViewVK, TextureView>(source);

   state[Color].flags          = 0; // TODO: VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT - if attachments may alias/overlap in the same memory
   state[Color].format         = TranslateTextureFormat[underlyingType(view[Color]->viewFormat)];
   state[Color].samples        = static_cast<VkSampleCountFlagBits>(view[Color]->texture->state.samples);
   state[Color].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
   state[Color].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
   state[Color].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // Ignored
   state[Color].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Ignored
   state[Color].initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   state[Color].finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

   // Resolve destination
   view[Resolve] = nullptr;
   }

   ColorAttachmentVK::~ColorAttachmentVK()
   {
   view[Color]   = nullptr;
   view[Resolve] = nullptr;
   }

   void ColorAttachmentVK::onLoad(const LoadOperation load, const float4 clearValue)
   {
   state[Color].loadOp = TranslateLoadOperation[underlyingType(load)];
   clearValue.color.float32[0] = clearValue.r;
   clearValue.color.float32[1] = clearValue.g;
   clearValue.color.float32[2] = clearValue.b;
   clearValue.color.float32[3] = clearValue.a;
   }
   
   void ColorAttachmentVK::onLoad(const LoadOperation load, const uint32v4 clearValue)
   {
   state[Color].loadOp = TranslateLoadOperation[underlyingType(load)];
   clearValue.color.uint32[0]  = clearValue.x;
   clearValue.color.uint32[1]  = clearValue.y;
   clearValue.color.uint32[2]  = clearValue.z;
   clearValue.color.uint32[3]  = clearValue.w;
   }
   
   void ColorAttachmentVK::onLoad(const LoadOperation load, const sint32v4 clearValue)
   {
   state[Color].loadOp = TranslateLoadOperation[underlyingType(load)];
   clearValue.color.int32[0] = clearValue.x;
   clearValue.color.int32[1] = clearValue.y;
   clearValue.color.int32[2] = clearValue.z;
   clearValue.color.int32[3] = clearValue.w;
   }

   void ColorAttachmentVK::onStore(const StoreOperation store)
   {
   state[Color].storeOp = TranslateStoreOperation[underlyingType(store)];
   }

   bool ColorAttachmentVK::resolve(const Ptr<TextureView> destination)
   {
   assert( destination );

   view[Resolve] = ptr_dynamic_cast<TextureViewVK, TextureView>(destination);

   assert( view[Color]->texture->state.samples > 1 );              // Cannot resolve non-MSAA source
   assert( view[Resolve]->texture->state.samples == 1 );           // Cannot resolve to MSAA destination
   assert( view[Resolve]->viewFormat == view[Color]->viewFormat ); // Cannot resolve between different Pixel Formats
   
   // Cannot resolve between different resolutions
   assert( view[Resolve]->texture->state.width   == view[Color]->texture->state.width );
   assert( view[Resolve]->texture->state.height  == view[Color]->texture->state.height );

   state[Resolve].flags          = 0; // TODO: VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT - if attachments may alias/overlap in the same memory
   state[Resolve].format         = state[Color].format;
   state[Resolve].samples        = VK_SAMPLE_COUNT_1_BIT;
   state[Resolve].loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // We Resolve to it, so don't care
   state[Resolve].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
   state[Resolve].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;  // Ignored
   state[Resolve].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Ignored
   state[Resolve].initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; 
   state[Resolve].finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // TODO: Vulkan: Add special resolve for presenting surface

   return true;
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
   DepthStencilAttachmentVK::DepthStencilAttachmentVK(const Ptr<TextureView> _depth,
                                                      const Ptr<TextureView> _stencil) :
      texture(nullptr),
      mipmap(0),
      layer(0),
      clearDepth(1.0f),
      clearStencil(0)
   {
   // Vulkan doesn't support separate Depth and Stencil surfaces at the same time.
   assert( !(_depth && _stencil) );
   assert( _depth || _stencil );
   
   if (_depth)
      view = ptr_dynamic_cast<TextureViewVK, TextureView>(_depth);

   if (_stencil)
      view = ptr_dynamic_cast<TextureViewVK, TextureView>(_stencil);

   // Needs to be DepthStencil, Depth or Stencil
   Format format = view->viewFormat;
   assert( TextureFormatIsDepthStencil(format) ||
           TextureFormatIsDepth(format)        ||
           TextureFormatIsStencil(format) );

   state.flags          = 0; // TODO: VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT - if attachments may alias/overlap in the same memory
   state.format         = TranslateTextureFormat[underlyingType(view->viewFormat)];
   state.samples        = static_cast<VkSampleCountFlagBits>(view->texture->state.samples);
   state.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;   // If this is Stencil only format, it's ignored
   state.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;  // If this is Stencil only format, it's ignored
   state.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;    // If this is Depth only format, it's ignored
   state.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;  // If this is Depth only format, it's ignored
   state.initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
   state.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
   }

   DepthStencilAttachmentVK::~DepthStencilAttachmentVK()
   {
   view = nullptr;
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

   void DepthStencilAttachmentVK::onStore(const StoreOperation storeDepthStencil)
   {
   VkAttachmentStoreOp store = TranslateStoreOperation[underlyingType(storeDepthStencil)];

   // DepthStencil load and store operations can be different,
   // but by default we set them to the same operation.
   state.storeOp        = store;
   state.stencilStoreOp = store;
   }

   bool DepthStencilAttachmentVK::resolve(const Ptr<TextureView> destination)
   {
   // Vulkan is not supporting Depth/Stencil resolve operations.
   return false;
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


   // RENDER PASS
   //////////////////////////////////////////////////////////////////////////


   RenderPassVK::RenderPassVK(VulkanDevice* _gpu, const VkRenderPass _passHandle, const uint32 _attachments) :
      gpu(_gpu),
      passHandle(_passHandle),
      attachments(_attachments),
      clearValues(new VkClearValue[attachments])
   {
   // Init clear values array
   memset(clearValues, 0, sizeof(float) * 4 * attachments);
   }

   RenderPassVK::~RenderPassVK()
   {
   delete [] clearValues;
   ProfileNoRet( gpu, vkDestroyFramebuffer(gpu->device, framebufferHandle, nullptr) )
   ProfileNoRet( gpu, vkDestroyRenderPass(gpu->device, passHandle, nullptr) )
   }

   Ptr<ColorAttachment> VulkanDevice::createColorAttachment(const Ptr<TextureView> texture,
                                                            const uint32 layers)
   {
   // TODO: Layers is unused!
   return ptr_dynamic_cast<ColorAttachment, ColorAttachmentVK>(Ptr<ColorAttachmentVK>(new ColorAttachmentVK(texture, mipmap, layer)));
   }

   Ptr<DepthStencilAttachment> VulkanDevice::createDepthStencilAttachment(const Ptr<TextureView> depth,
      const Ptr<TextureView> stencil,
      const uint32 layers)
   {
   // TODO: Layers is unused!
   return ptr_dynamic_cast<DepthStencilAttachment, DepthStencilAttachmentVK>(Ptr<DepthStencilAttachmentVK>(new DepthStencilAttachmentVK(depth, stencil, mipmap, layer)));
   }
   
   // Creates simple render pass with one color destination
   Ptr<RenderPass> VulkanDevice::create(const Ptr<ColorAttachment> color,
                                        const Ptr<DepthStencilAttachment> depthStencil)
   {
//   Ptr<RenderPass> result = nullptr;
//   
//   assert( color );
//   
//   uint32 freeIndex = 0u;
//   uint32 attachments = 1u;
//   
//   VkAttachmentReference attColor;
//   attColor.attachment = freeIndex;
//   attColor.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//   freeIndex++;
//   
//   bool resolve = color->texture[Resolve] ? true : false;
//   
//   // Optional Color Resolve
//   VkAttachmentReference attResolve;
//   attResolve.attachment = VK_ATTACHMENT_UNUSED;
//   attResolve.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//   if (resolve)
//      {
//      attResolve.attachment = freeIndex;
//      freeIndex++;
//      }
//      
//   // Optional Depth-Stencil / Depth / Stencil
//   VkAttachmentReference attDepthStencil;
//   attDepthStencil.attachment = VK_ATTACHMENT_UNUSED;
//   attDepthStencil.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//   if (depthStencil)
//      {
//      attDepthStencil.attachment = freeIndex;
//      freeIndex++;
//      }
//
//   // Engine is currently not supporting multiple sub-passes described as one dependency graph
//   VkSubpassDescription subInfo;
//   subInfo.flags                   = 0u;  // Reserved for future
//   subInfo.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
//   subInfo.inputAttachmentCount    = 0u;
//   subInfo.pInputAttachments       = nullptr;
//   subInfo.colorAttachmentCount    = 1u;
//   subInfo.pColorAttachments       = &attColor;
//   subInfo.pResolveAttachments     = resolve      ? &attResolve      : nullptr;
//   subInfo.pDepthStencilAttachment = depthStencil ? &attDepthStencil : nullptr;
//   subInfo.preserveAttachmentCount = 0;
//   subInfo.pPreserveAttachments    = nullptr;
//
//   // Calculate amount of used surfaces
//   uint32 surfaces = attachments;
//   if (resolve)
//      surfaces *= 2;
//   if (depthStencil)
//      surfaces++;
//   
//   // Render Pass attachments layouy:
//   // 0..7  - Up to 8 Color Attachments
//   // 8..15 - Up to 8 Resolve surfaces
//   // 16    - Depth-Stencil / Depth / Stencil
//   
//   // Gather Attachment states
//   VkAttachmentDescription* attachment = new VkAttachmentDescription[attachments];
//   uint32 index = 0;
//   for(; index<attachments; ++index)
//      {
//      Ptr<ColorAttachmentVK> ptr = ptr_dynamic_cast<ColorAttachmentVK, ColorAttachment>(color); // color[index]
//      memcpy(&attachment[index], &ptr->state[Color], sizeof(VkAttachmentDescription));
//      }
//      
//   if (resolve)
//      for(uint32 i=0; i<attachments; ++i)
//         {
//         Ptr<ColorAttachmentVK> ptr = ptr_dynamic_cast<ColorAttachmentVK, ColorAttachment>(color); // color[i]
//         memcpy(&attachment[index], &ptr->state[Resolve], sizeof(VkAttachmentDescription));
//         index++;
//         }
//      
//   if (depthStencil)
//      {
//      Ptr<DepthStencilAttachmentVK> ptr = ptr_dynamic_cast<DepthStencilAttachmentVK, DepthStencilAttachment>(depthStencil);
//      memcpy(&attachment[index], &ptr->state, sizeof(VkAttachmentDescription));
//      }
//
//   VkRenderPassCreateInfo passInfo;
//   passInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//   passInfo.pNext           = nullptr;
//   passInfo.attachmentCount = surfaces;
//   passInfo.pAttachments    = attachment;
//   passInfo.subpassCount    = 1;
//   passInfo.pSubpasses      = &subInfo;
//   passInfo.dependencyCount = 0;
//   passInfo.pDependencies   = nullptr;
//
//   // Create renderpass object
//   VkRenderPass renderpass;
//   Profile( this, vkCreateRenderPass(device, &passInfo, nullptr, &renderpass) )
//   if (lastResult[Scheduler.core()] == VK_SUCCESS)
//      result = ptr_dynamic_cast<RenderPass, RenderPassVK>(new RenderPassVK(this, renderpass, attachments));
//
//   delete [] attachment;
//
//   return result;
   
   return create(1, color, depthStencil);
   }
   
   // Internal universal method to create Render Pass
   Ptr<RenderPass> VulkanDevice::create(const uint32 attachments,
                                        const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
                                        const Ptr<DepthStencilAttachment> depthStencil,
                                        const uint32v2 explicitResolution,
                                        const uint32v2 explicitLayers)
   {
   Ptr<RenderPass> result = nullptr;
   
   assert( attachments < support.maxColorAttachments );

   // Vulkan allows pass without any color and depth attachments.
   // In such case, rasterization is still performed in width x height x layers x sampler space,
   // but it is expected that results will be outputted as a result of side effects operation.
   //
   // TODO: Make this method universal and internal. Provide common width/height/layers paarameters.
   //       There should be separate method for creating such RenderPass that accepts only those parameters.
   //
   assert( color[0] );

   uint32 surfaces = 0u;
   bool   resolve  = false;
   
   // Render Pass attachments layouy:
   // 0..7  - Up to 8 Color Attachments
   // 8..15 - Up to 8 Resolve surfaces
   // 16    - Depth-Stencil / Depth / Stencil
   
   // Optional Color Attachments
   VkAttachmentReference* attColor = nullptr;
   if (attachments)
      {
      attColor = new VkAttachmentReference[attachments];
      for(uint32 i=0; i<attachments; ++i)
         {
         // All passed in Color Attachments need to be valid
         assert( color[i] );
         
         attColor[i].attachment = surfaces;
         attColor[i].layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
         }
         
      surfaces += attachments;
         
      if (color[0]->view[Resolve])
         resolve = true;
      }
      
   // Optional Color Resolve
   VkAttachmentReference* attResolve = nullptr;
   if (resolve)
      {
      attResolve = new VkAttachmentReference[attachments];
      
      for(uint32 i=0; i<attachments; ++i)
         {
         // If one Color Attachment is beeing resolved, all of them need to be resolved
         assert( color[i]->view[Resolve] != nullptr );
         
         attResolve[i].attachment = surfaces;
         attResolve[i].layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
         }

      surfaces += attachments;
      }
      
   // Optional Depth-Stencil / Depth / Stencil
   VkAttachmentReference attDepthStencil;
   if (depthStencil)
      {
      attDepthStencil.attachment = surfaces;
      attDepthStencil.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      surfaces++;
      
      // TODO: How to determine below situation ?
      // VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
      // When DepthStencil texture is filled earlier in Depth pre-pass, and now only used for discarding
      // Or is this flag supposed to be used only for ShadowMapping? Sampling with Z Test?
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

   // Surface Views
   VkImageView* handles = nullptr;
   if (surfaces)
      handles = new VkImageView[surfaces];
      
   // Gather Attachment states and Texture Views for Framebuffer
   uint32 index = 0;
   VkAttachmentDescription* attachment = nullptr;
   if (surfaces)
      attachment = new VkAttachmentDescription[surfaces];
   for(uint32 i=0; i<attachments; ++i)
      {
      Ptr<ColorAttachmentVK> ptr = ptr_dynamic_cast<ColorAttachmentVK, ColorAttachment>(color[i]);
      memcpy(&attachment[index], &ptr->state[Color], sizeof(VkAttachmentDescription));
      handles[index] = ptr->view[Color]->handle;
      index++;
      }
      
   if (resolve)
      for(uint32 i=0; i<attachments; ++i)
         {
         Ptr<ColorAttachmentVK> ptr = ptr_dynamic_cast<ColorAttachmentVK, ColorAttachment>(color[i]);
         memcpy(&attachment[index], &ptr->state[Resolve], sizeof(VkAttachmentDescription));
         handles[index] = ptr->view[Resolve]->handle;
         index++;
         }
      
   if (depthStencil)
      {
      Ptr<DepthStencilAttachmentVK> ptr = ptr_dynamic_cast<DepthStencilAttachmentVK, DepthStencilAttachment>(depthStencil);
      memcpy(&attachment[index], &ptr->state, sizeof(VkAttachmentDescription));
      handles[index] = ptr->view->handle;
      }

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
   VkRenderPass renderpass;
   Profile( this, vkCreateRenderPass(device, &passInfo, nullptr, &renderpass) )
   if (lastResult[Scheduler.core()] == VK_SUCCESS)
      result = ptr_dynamic_cast<RenderPass, RenderPassVK>(new RenderPassVK(this, renderpass, attachments));

   delete [] attColor;
   delete [] attResolve;
   delete [] attachment;

   // If Render Pass creation failed, early out
   if (!result)
      {
      delete [] handles;
      return result;
      }

   // TODO: This function should only validate that all surfaces have the same
   //       resolution and layers count!
   
   // Calculate smallest common resolution and layers count of passed attachments,
   // or use explicitly pased ones if this Render Pass is not rendering anything
   // (has no attachments, uses shader side effects).
   uint32v2 resolution = explicitResolution;
   uint32   layers     = explicitLayers;
   if (surfaces)
      {
      for(uint32 i=0; i<attachments; ++i)
         {
         Ptr<ColorAttachmentVK> ptr = ptr_dynamic_cast<ColorAttachmentVK, ColorAttachment>(color[i]);
         resolution.width  = min(resolution.width,  ptr->view[Color]->texture->state.width);
         resolution.height = min(resolution.height, ptr->view[Color]->texture->state.height);
         layers            = min(layers,            ptr->view[Color]->_layers.count);
         if (resolve)
            {
            resolution.width  = min(resolution.width,  ptr->view[Resolve]->texture->state.width);
            resolution.height = min(resolution.height, ptr->view[Resolve]->texture->state.height);
            layers            = min(layers,            ptr->view[Resolve]->_layers.count);
            }
         }
         
      if (depthStencil)
         {
         Ptr<DepthStencilAttachmentVK> ptr = ptr_dynamic_cast<DepthStencilAttachmentVK, DepthStencilAttachment>(depthStencil);
         resolution.width  = min(resolution.width,  ptr->view->texture->state.width);
         resolution.height = min(resolution.height, ptr->view->texture->state.height);
         layers            = min(layers,            ptr->view->_layers.count);
         }
      }

   
   // Describe Framebuffer
   VkFramebufferCreateInfo framebufferInfo;
   framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
   framebufferInfo.pNext           = nullptr;
   framebufferInfo.flags           = 0; // Reserved for future.
   framebufferInfo.renderPass      = renderpass;
   framebufferInfo.attachmentCount = surfaces;
   framebufferInfo.pAttachments    = handles;
   framebufferInfo.width           = resolution.width;
   framebufferInfo.height          = resolution.height;
   framebufferInfo.layers          = layers;

   // Create framebuffer object
   VkFramebuffer framebuffer;
   Profile( vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) )
   if (lastResult[Scheduler.core()] == VK_SUCCESS)
      ptr_dynamic_cast<RenderPassVK, RenderPass>(result)->framebufferHandle = framebuffer;
      
   // TODO: Do clear values !!!!!!!!!
   
   delete [] handles;
   return result;
   }
   
   // Creates render pass which's output goes to window framebuffer
   Ptr<RenderPass> VulkanDevice::create(const Ptr<Texture> framebuffer,
                                        const Ptr<DepthStencilAttachment> depthStencil)
   {
   // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
   }
      
   // Creates render pass which's output is resolved from temporary MSAA target to window framebuffer
   Ptr<RenderPass> VulkanDevice::create(const Ptr<Texture> temporaryMSAA,
                                        const Ptr<Texture> framebuffer,
                                        const Ptr<DepthStencilAttachment> depthStencil)
   {
   // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
   }





      
      
      
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



 





   // Clear Values Setup:
   
    // VkClearColorValue           color;
    // VkClearDepthStencilValue    depthStencil;

//typedef union VkClearColorValue {
//float float32[4];
//int32_t int32[4];
//uint32_t uint32[4];
//} VkClearColorValue;
//
//typedef struct VkClearDepthStencilValue {
//    float       depth;
//    uint32_t    stencil;
//} VkClearDepthStencilValue;
//



 //  Ptr<RenderPass> VulkanDevice::create(uint32 _attachments,
 //                                       const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
 //                                       const Ptr<DepthStencilAttachment> depthStencil)
 //  {
 //  Ptr<RenderPass> result = nullptr;
 //  


 //  // const uint32v2 resolution, // Common attachments resolution
 //  // const uint32   layers)     // Common attachments layers count


 //  Ptr<ColorAttachmentVK> temp = ptr_dynamic_cast<ColorAttachmentVK, ColorAttachment>(color[0]);

 //  
 //  VkImageView attachmentHandle[8];

	//VkAttachmentDescription attachment[2];
	//attachment[0].format = TranslateTextureFormat[ underlyingType((temp->texture[0])->format) ]; //color[0] //colorformat;
	//attachment[0].samples = VK_SAMPLE_COUNT_1_BIT;
	//attachment[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//attachment[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//attachment[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//attachment[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//attachment[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	//attachment[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//attachment[1].format = depthFormat;
	//attachment[1].samples = VK_SAMPLE_COUNT_1_BIT;
	//attachment[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//attachment[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//attachment[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//attachment[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//attachment[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	//attachment[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

 //  VkAttachmentReference colorReference = {};
 //  colorReference.attachment = 0;
 //  colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

 //  VkAttachmentReference depthReference = {};
 //  depthReference.attachment = 1;
 //  depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

 //  VkSubpassDescription subPassInfo = {};
 //  subPassInfo.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
 //  subPassInfo.flags                   = 0;
 //  subPassInfo.inputAttachmentCount    = 0;
 //  subPassInfo.pInputAttachments       = nullptr;
 //  subPassInfo.colorAttachmentCount    = _attachments;
 //  subPassInfo.pColorAttachments       = &colorReference;
 //  subPassInfo.pResolveAttachments = NULL;
 //  subPassInfo.pDepthStencilAttachment = &depthReference;
 //  subPassInfo.preserveAttachmentCount = 0;
 //  subPassInfo.pPreserveAttachments = NULL;

 //  VkRenderPassCreateInfo renderPassInfo = {};
 //  renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
 //  renderPassInfo.pNext           = nullptr;
 //  renderPassInfo.flags           = 0;  // Reserved for future use
 //  renderPassInfo.attachmentCount = _attachments + (depthStencil == nullptr ? 0 : 1);
 //  renderPassInfo.pAttachments    = attachment;
 //  renderPassInfo.subpassCount    = 1;
 //  renderPassInfo.pSubpasses      = &subPassInfo;
 //  renderPassInfo.dependencyCount = 0;
 //  renderPassInfo.pDependencies   = nullptr;



	//renderPassInfo.attachmentCount = 2;
	//renderPassInfo.pAttachments = attachments;
	//renderPassInfo.subpassCount = 1;
	//renderPassInfo.pSubpasses = &subpass;
	//renderPassInfo.dependencyCount = 0;
	//renderPassInfo.pDependencies = NULL;

	//VkResult err;

	//err = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
	//assert(!err);


 //  return result;

 //  }








































   //FramebufferVK::FramebufferVK() :
   //   gpu(nullptr)
   //{
   //}

   //FramebufferVK::~FramebufferVK()
   //{
   //
   //}


   //Ptr<ColorAttachment> VulkanDevice::create(const Format format, const uint32 samples)
   //{
   //return ptr_dynamic_cast<ColorAttachment, ColorAttachmentVK>(new ColorAttachmentVK(format, samples));
   //}

   //   // When binding 3D texture, pass it's plane "depth" through "layer" parameter,
   //   // similarly when binding CubeMap texture, pass it's "face" through "layer".
   //Ptr<ColorAttachment> VulkanDevice::createColorAttachment(const Ptr<Texture> texture,
   //                                       const uint32 mipmap = 0,
   //                                       const uint32 layer = 0,
   //                                       const uint32 layers = 1)
   //{
   //}

   //Ptr<DepthStencilAttachment> VulkanDevice::createDepthStencilAttachment(const Ptr<Texture> depth,
   //                                              const Ptr<Texture> stencil,
   //                                              const uint32 mipmap = 0,
   //                                              const uint32 layer = 0,
   //                                              const uint32 layers = 1)
   //{
   //}

   //   // Creates simple render pass with one color destination
   //Ptr<RenderPass> VulkanDevice::create(const Ptr<ColorAttachment> color,
   //                                  const Ptr<DepthStencilAttachment> depthStencil)
   //{
   //}
   //   
   //   // Creates render pass with Multiple Render Targets
   //Ptr<RenderPass> VulkanDevice::create(const uint32 _attachments,
   //                                  const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
   //                                  const Ptr<DepthStencilAttachment> depthStencil)
   //{
   //}
   //     
   //   // Creates render pass which's output goes to window framebuffer
   //Ptr<RenderPass> VulkanDevice::create(const Ptr<Texture> framebuffer,
   //                                  const Ptr<DepthStencilAttachment> depthStencil)
   //{
   //}
   //   
   //   // Creates render pass which's output is resolved from temporary MSAA target to window framebuffer
   //Ptr<RenderPass> VulkanDevice::create(const Ptr<Texture> temporaryMSAA,
   //                                  const Ptr<Texture> framebuffer,
   //                                  const Ptr<DepthStencilAttachment> depthStencil)
   //{
   //}








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





   // Framebuffer


//   class vkDepthStencilAttachment : public DepthStencilAttachment
//      { 
//      public:
//      VkDepthStencilView id;
//      };
//
//   class vkFramebuffer : public Framebuffer
//      {
//      public:
//      vkFramebuffer id;
//      uint32        colorAttachments;
//      };
//
//   class vkCommandBuffer : public CommandBuffer
//      {
//      public:
//      VkCmdBuffer id;
//
//      void bind(const Ptr<RasterState> raster);
//      void bind(const Ptr<ViewportScissorState> viewportScissor);
//      void bind(const Ptr<DepthStencilState> depthStencil);
//      void bind(const Ptr<BlendState> blend);
//      };
//
//    // VULKAN DEPTH ATTACHMENT
//   //--------------------------------------------------------------------------
//
//   Ptr<DepthStencilAttachment> Create(const Ptr<Texture> depthStencil, 
//                                      const Ptr<Texture> resolveMsaa, 
//                                      const uint32 depthStencilMipMap,
//                                      const uint32 msaaMipMap,
//                                      const uint32 depthStencilLayer,
//                                      const uint32 msaaLayer,
//                                      const uint32 layers)
//   {
//   Ptr<DepthStencilAttachment> result = nullptr;
//
//   Ptr<vkTexture> vkDepthStencil = ptr_dynamic_cast<vkTexture, Texture>(depthStencil);
//   Ptr<vkTexture> vkMsaa         = ptr_dynamic_cast<vkTexture, Texture>(resolveMsaa);
//
//   // Determine depth-stencil attachment type
//   VkImageAspect aspect = VK_IMAGE_ASPECT_DEPTH | VK_IMAGE_ASPECT_STENCIL;
//   if ( (depthStencil->state.format == FormatD_16) ||
//        (depthStencil->state.format == FormatD_24) ||
//        (depthStencil->state.format == FormatD_32) ||
//        (depthStencil->state.format == FormatD_32_f) )
//      aspect = VK_IMAGE_ASPECT_DEPTH;
//
//   VkImageSubresourceRange resourceRange;
//   resourceRange.aspect                    = aspect;
//   resourceRange.baseMipLevel              = msaaMipMap;
//   resourceRange.mipLevels                 = 1;
//   resourceRange.baseArraySlice            = msaaLayer;
//   resourceRange.arraySize                 = layers;
//
//   VkDepthStencilViewCreateInfo depthStencilInfo;
//   depthStencilInfo.sType                  = VK_STRUCTURE_TYPE_DEPTH_STENCIL_VIEW_CREATE_INFO;
//   depthStencilInfo.pNext                  = nullptr;
//   depthStencilInfo.image                  = vkDepthStencil->id;
//   depthStencilInfo.mipLevel               = depthStencilMipMap;
//   depthStencilInfo.baseArraySlice         = depthStencilLayer;
//   depthStencilInfo.arraySize              = layers;
//   depthStencilInfo.msaaResolveImage       = vkMsaa->id;
//   depthStencilInfo.msaaResolveSubResource = resourceRange;
//   depthStencilInfo.flags                  = 0;
//                                          // VK_DEPTH_STENCIL_VIEW_CREATE_FLAGS:
//                                          // VK_DEPTH_STENCIL_VIEW_CREATE_READ_ONLY_DEPTH_BIT
//                                          // VK_DEPTH_STENCIL_VIEW_CREATE_READ_ONLY_STENCIL_BIT  
//
//   VkDepthStencilView depthStencilView;
//   VkResult res = vkCreateDepthStencilView( gpu[i].handle, &depthStencilInfo, &depthStencilView );
//   if (!res)
//      {
//      Ptr<vkDepthStencilAttachment> vkAttachment = new vkDepthStencilAttachment();
//      vkAttachment->id = depthStencilView;
//      result = ptr_dynamic_cast<DepthStencilAttachment, vkDepthStencilAttachment>(vkAttachment);
//      }
//
//   return result;
//   }
//
//   // VULKAN FRAMEBUFFER
//   //-------------------------------------------------------------
//
//   Ptr<Framebuffer> Create(const Ptr<DepthStencilAttachment> depthStencil,
//                           const Ptr<ColorAttachment> color0 );
//
//   Ptr<Framebuffer> Create(const Ptr<DepthStencilAttachment> depthStencil,
//                           const Ptr<ColorAttachment> color0,
//                           const Ptr<ColorAttachment> color1 = nullptr,
//                           const Ptr<ColorAttachment> color2 = nullptr,
//                           const Ptr<ColorAttachment> color3 = nullptr,
//                           const Ptr<ColorAttachment> color4 = nullptr,
//                           const Ptr<ColorAttachment> color5 = nullptr,
//                           const Ptr<ColorAttachment> color6 = nullptr,
//                           const Ptr<ColorAttachment> color7 = nullptr);
//
//   Ptr<Framebuffer> Create(const Ptr<DepthStencilAttachment> depthStencil,
//                           const Ptr<ColorAttachment> color[MaxColorAttachmentsCount]);
//
//   Ptr<Framebuffer> Create(const uint32v2 resolution,
//                           const Ptr<DepthStencilAttachment> depthStencil,
//                           const Ptr<ColorAttachment> color[MaxColorAttachmentsCount])
//   {
//   Ptr<Framebuffer> result;
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
//      colorBindInfo[i].view    = ptr_dynamic_cast<vkColorAttachment, ColorAttachment>(color[i])->id;
//      colorBindInfo[i].layout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//      }
//
//   VkDepthStencilBindInfo depthStencilBindInfo;
//   depthStencilBindInfo.view   = ptr_dynamic_cast<vkDepthStencilAttachment, DepthStencilAttachment>(depthStencil)->id;
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
//      Ptr<vkFramebuffer> vkFbo = new vkFramebuffer();
//      vkFbo->id               = fbo;
//      vkFbo->colorAttachments = colorAttachments;
//      result = ptr_dynamic_cast<Framebuffer, vkFramebuffer>(vkFbo);
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
//   Ptr<RenderPass> VulkanDevice::create(const Ptr<ColorAttachment> color,
//                                        const Ptr<DepthStencilAttachment> depthStencil)
//   {
//   // TODO: !!!
//   }
//
//   Ptr<RenderPass> VulkanDevice::create(uint32 _attachments,
//                                        const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
//                                        const Ptr<DepthStencilAttachment> depthStencil)
//   {
//   Ptr<RenderPass> result = nullptr;
//   
//   assert( _attachments < MaxColorAttachmentsCount );
//
//
//
//// VK_IMAGE_LAYOUT_UNDEFINED = 0,
//// VK_IMAGE_LAYOUT_GENERAL = 1,
//// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2,
//// VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
//// VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
//// VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL = 5,
//// VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL = 6,
//// VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL = 7,
//// VK_IMAGE_LAYOUT_PREINITIALIZED = 8,
//// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR = 1000001002,
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
//      Ptr<RenderPassVK> rpo = new RenderPassVK();
//      rpo->id          = renderpass;
//      rpo->gpu         = this;
//      rpo->attachments = _attachments;
//      result = ptr_dynamic_cast<RenderPass, RenderPassVK>(rpo);
//      }
//
//   delete [] attachment;
//
//   return result;
//   }
//
//   Ptr<RenderPass> VulkanDevice::create(const Ptr<Texture> framebuffer,
//                                        const Ptr<DepthStencilAttachment> depthStencil)
//   {
//   // TODO: !!!
//   }
//
//   Ptr<RenderPass> VulkanDevice::create(const Ptr<Texture> temporaryMSAA,
//                                        const Ptr<Texture> framebuffer,
//                                        const Ptr<DepthStencilAttachment> depthStencil)
//   {
//   // TODO: !!!
//   }
//
//
//
//// typedef void (VKAPI_PTR *PFN_vkCmdBeginRenderPass)(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents);
//// typedef void (VKAPI_PTR *PFN_vkCmdNextSubpass)(VkCommandBuffer commandBuffer, VkSubpassContents contents);
//// typedef void (VKAPI_PTR *PFN_vkCmdEndRenderPass)(VkCommandBuffer commandBuffer);
//
//
//   Ptr<Framebuffer> RenderPassVK::create(const uint32v2 resolution, // Common attachments resolution
//                                         const uint32   layers)     // Common attachments layers count
//   {
//   Ptr<Framebuffer> result = nullptr;
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
//      Ptr<FramebufferVK> fbo = new FramebufferVK();
//      fbo->id  = framebuffer;
//      fbo->gpu = gpu;
//      result = ptr_dynamic_cast<Framebuffer, FramebufferVK>(fbo);
//      }
//
//   return result;
//   }


   // D3D12 IMPLEMENTATION
   //////////////////////////////////////////////////////////////////////////////////////////////////////



   // DX12 Doesn't support Tiled Renderers, and as such, don't have load and store operations.




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
