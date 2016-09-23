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

#include "core/defines.h"
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

   void ColorAttachmentVK::onLoad(const LoadOperation load, const float4 _clearValue)
   {
   state[Color].loadOp = TranslateLoadOperation[underlyingType(load)];
   clearValue.color.float32[0] = _clearValue.r;
   clearValue.color.float32[1] = _clearValue.g;
   clearValue.color.float32[2] = _clearValue.b;
   clearValue.color.float32[3] = _clearValue.a;
   }
   
   void ColorAttachmentVK::onLoad(const LoadOperation load, const uint32v4 _clearValue)
   {
   state[Color].loadOp = TranslateLoadOperation[underlyingType(load)];
   clearValue.color.uint32[0]  = _clearValue.x;
   clearValue.color.uint32[1]  = _clearValue.y;
   clearValue.color.uint32[2]  = _clearValue.z;
   clearValue.color.uint32[3]  = _clearValue.w;
   }
   
   void ColorAttachmentVK::onLoad(const LoadOperation load, const sint32v4 _clearValue)
   {
   state[Color].loadOp = TranslateLoadOperation[underlyingType(load)];
   clearValue.color.int32[0] = _clearValue.x;
   clearValue.color.int32[1] = _clearValue.y;
   clearValue.color.int32[2] = _clearValue.z;
   clearValue.color.int32[3] = _clearValue.w;
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
      view(nullptr),
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

   bool DepthStencilAttachmentVK::resolve(const Ptr<TextureView> destination, const DepthResolve mode)
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
      resolution(),
      attachments(_attachments),
      clearValues(_attachments ? new VkClearValue[attachments] : nullptr)
   {
   // Init clear values array
   if (attachments)
      memset(clearValues, 0, sizeof(float) * 4 * attachments);
   }

   RenderPassVK::~RenderPassVK()
   {
   delete [] clearValues;
   ProfileNoRet( gpu, vkDestroyFramebuffer(gpu->device, framebufferHandle, nullptr) )
   ProfileNoRet( gpu, vkDestroyRenderPass(gpu->device, passHandle, nullptr) )
   }

   Ptr<ColorAttachment> VulkanDevice::createColorAttachment(const Ptr<TextureView> texture)
   {
   // TODO: Layers is unused!
   return ptr_dynamic_cast<ColorAttachment, ColorAttachmentVK>(Ptr<ColorAttachmentVK>(new ColorAttachmentVK(texture)));
   }

   Ptr<DepthStencilAttachment> VulkanDevice::createDepthStencilAttachment(const Ptr<TextureView> depth,
      const Ptr<TextureView> stencil)
   {
   // TODO: Layers is unused!
   return ptr_dynamic_cast<DepthStencilAttachment, DepthStencilAttachmentVK>(Ptr<DepthStencilAttachmentVK>(new DepthStencilAttachmentVK(depth, stencil)));
   }
   

   // CREATE RENDER PASS
   //////////////////////////////////////////////////////////////////////////


   Ptr<RenderPass> VulkanDevice::create(const Ptr<ColorAttachment> color,
                                        const Ptr<DepthStencilAttachment> depthStencil)
   {
   // At least one of the two needs to be present
   assert( color || depthStencil );

   // Calculate resolution of mipmap used as rendering destination,
   // and count of available layers. Validate that they are identical
   // between all used surfaces.
   uint32v2 resolution;
   uint32 layers = 1;
   bool selected = false;
   if (color)
      {
      Ptr<ColorAttachmentVK> ptr = ptr_dynamic_cast<ColorAttachmentVK, ColorAttachment>(color);
      Ptr<TextureViewVK> view = ptr->view[Color];
      uint32 mipmap = view->mipmaps.base;
      resolution = view->texture->resolution(mipmap);
      layers     = view->layers.count;
      selected   = true;
      }
   else
   if (depthStencil)
      {
      Ptr<DepthStencilAttachmentVK> ptr = ptr_dynamic_cast<DepthStencilAttachmentVK, DepthStencilAttachment>(depthStencil);
      Ptr<TextureViewVK> view = ptr->view;
      uint32 mipmap = view->mipmaps.base;

      if (!selected)
         {
         resolution = view->texture->resolution(mipmap);
         layers     = view->layers.count;
         selected = true;
         }
      else
         {
         assert( resolution == view->texture->resolution(mipmap) );
         assert( layers     == view->layers.count );
         }
      }
   assert( selected );

   return createRenderPass(1, &color, depthStencil, resolution, layers);
   }
   
      
   Ptr<RenderPass> VulkanDevice::create(uint32 _attachments,
                                        const Ptr<ColorAttachment>* color,
                                        const Ptr<DepthStencilAttachment> depthStencil)
   {
   // At least one type of the two needs to be present
   assert( _attachments > 0 || depthStencil );
   
   // Calculate resolution of mipmap used as rendering destination,
   // and count of available layers. Validate that they are identical
   // between all used surfaces.
   uint32v2 resolution;
   uint32   layers = 1;
   bool     selected = false;
   if (_attachments)
      {
      for(uint32 i=0; i<_attachments; ++i)
         if (color[i]) // Allow empty entries in the input array.
            {
            Ptr<ColorAttachmentVK> ptr = ptr_dynamic_cast<ColorAttachmentVK, ColorAttachment>(color[i]);
            Ptr<TextureViewVK> view = ptr->view[Color];
            uint32 mipmap = view->mipmaps.base;

            if (!selected)
               {
               resolution = view->texture->resolution(mipmap);
               layers     = view->layers.count;
               selected = true;
               }
            else
               {
               assert( resolution == view->texture->resolution(mipmap) );
               assert( layers     == view->layers.count );
               }
            }
      }
   if (depthStencil)
      {
      Ptr<DepthStencilAttachmentVK> ptr = ptr_dynamic_cast<DepthStencilAttachmentVK, DepthStencilAttachment>(depthStencil);
      Ptr<TextureViewVK> view = ptr->view;
      uint32 mipmap = view->mipmaps.base;

      if (!selected)
         {
         resolution = view->texture->resolution(mipmap);
         layers     = view->layers.count;
         selected = true;
         }
      else
         {
         assert( resolution == view->texture->resolution(mipmap) );
         assert( layers     == view->layers.count );
         }
      }
   assert( selected );

   return createRenderPass(_attachments, color, depthStencil, resolution, layers);



   //// Calculate smallest common resolution and layers count of passed attachments,
   //// or use explicitly pased ones if this Render Pass is not rendering anything
   //// (has no attachments, uses shader side effects).
   //uint32v2 resolution = explicitResolution;
   //uint32   layers     = explicitLayers;
   //if (surfaces)
   //   {
   //   for(uint32 i=0; i<attachments; ++i)
   //      {
   //      Ptr<ColorAttachmentVK> ptr = ptr_dynamic_cast<ColorAttachmentVK, ColorAttachment>(color[i]);
   //      resolution.width  = min(resolution.width,  ptr->view[Color]->texture->state.width);
   //      resolution.height = min(resolution.height, ptr->view[Color]->texture->state.height);
   //      layers            = min(layers,            ptr->view[Color]->layers.count);
   //      if (resolve)
   //         {
   //         resolution.width  = min(resolution.width,  ptr->view[Resolve]->texture->state.width);
   //         resolution.height = min(resolution.height, ptr->view[Resolve]->texture->state.height);
   //         layers            = min(layers,            ptr->view[Resolve]->layers.count);
   //         }
   //      }
   //      
   //   if (depthStencil)
   //      {
   //      Ptr<DepthStencilAttachmentVK> ptr = ptr_dynamic_cast<DepthStencilAttachmentVK, DepthStencilAttachment>(depthStencil);
   //      resolution.width  = min(resolution.width,  ptr->view->texture->state.width);
   //      resolution.height = min(resolution.height, ptr->view->texture->state.height);
   //      layers            = min(layers,            ptr->view->layers.count);
   //      }
   //   }


   //// Calculate resolution of mipmap used as rendering destination,
   //// and count of available layers.


   //   Ptr<ColorAttachmentVK> ptr = ptr_dynamic_cast<ColorAttachmentVK, ColorAttachment>(color);
   //   Ptr<TextureViewVK> view = ptr->view[Color];
   //   uint32 mipmap = view->mipmaps.base;
   //   resolution = view->texture->resolution(mipmap);
   //   layers = view->layers.count;
   //   }
   //else
   //if (depthStencil)
   //   {
   //   Ptr<DepthStencilAttachmentVK> ptr = ptr_dynamic_cast<DepthStencilAttachmentVK, DepthStencilAttachment>(depthStencil);
   //   Ptr<TextureViewVK> view = ptr->view;
   //   uint32 mipmap = view->mipmaps.base;
   //   resolution = view->texture->resolution(mipmap);
   //   layers = view->layers.count;
   //   }

   }


   // Internal universal method to create Render Pass
   Ptr<RenderPass> VulkanDevice::createRenderPass(const uint32 attachments,
                                                  const Ptr<ColorAttachment>* color,
                                                  const Ptr<DepthStencilAttachment> depthStencil,
                                                  const uint32v2 explicitResolution,
                                                  const uint32   explicitLayers)
   {
   Ptr<RenderPassVK> result = nullptr;
   
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

      if (ptr_dynamic_cast<ColorAttachmentVK, ColorAttachment>(color[0])->view[Resolve])
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
         Ptr<TextureViewVK> view = ptr_dynamic_cast<ColorAttachmentVK, ColorAttachment>(color[i])->view[Resolve];
         assert( view );
         
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
      result = new RenderPassVK(this, renderpass, surfaces);

   delete [] attColor;
   delete [] attResolve;
   delete [] attachment;

   // If Render Pass creation failed, early out
   if (!result)
      {
      delete [] handles;
      return Ptr<RenderPass>(nullptr);
      }

   // TODO: This function should only validate that all surfaces have the same
   //       resolution and layers count!
   //       Calculation of common resolution and layers should be done by the app !
   
   
   // Describe Framebuffer
   VkFramebufferCreateInfo framebufferInfo;
   framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
   framebufferInfo.pNext           = nullptr;
   framebufferInfo.flags           = 0; // Reserved for future.
   framebufferInfo.renderPass      = renderpass;
   framebufferInfo.attachmentCount = surfaces;
   framebufferInfo.pAttachments    = handles;
   framebufferInfo.width           = explicitResolution.width;
   framebufferInfo.height          = explicitResolution.height;
   framebufferInfo.layers          = explicitLayers;

   // Create framebuffer object
   VkFramebuffer framebuffer;
   Profile( this, vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) )
   if (lastResult[Scheduler.core()] == VK_SUCCESS)
      result->framebufferHandle = framebuffer;
      
   // Those values will be used at beginning of RenderPass
   index = 0;
   for(uint32 i=0; i<attachments; ++i)
      {
      Ptr<ColorAttachmentVK> ptr = ptr_dynamic_cast<ColorAttachmentVK, ColorAttachment>(color[i]);
      result->clearValues[index] = ptr->clearValue;
      index++;
      }
      
   // Were not specifying clear values for MSAA resolve destinations (but need keep entries)
   if (resolve)
      index += attachments;

   if (depthStencil)
      {
      Ptr<DepthStencilAttachmentVK> ptr = ptr_dynamic_cast<DepthStencilAttachmentVK, DepthStencilAttachment>(depthStencil);
      result->clearValues[index].depthStencil.depth   = ptr->clearDepth;
      result->clearValues[index].depthStencil.stencil = ptr->clearStencil;
      }
      
   // Resolution we render in on RenderPass start
   result->resolution = explicitResolution;
   
   delete [] handles;
   return ptr_dynamic_cast<RenderPass, RenderPassVK>(result);
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



 






      









































   //Ptr<ColorAttachment> VulkanDevice::create(const Format format, const uint32 samples)
   //{
   //return ptr_dynamic_cast<ColorAttachment, ColorAttachmentVK>(new ColorAttachmentVK(format, samples));
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
