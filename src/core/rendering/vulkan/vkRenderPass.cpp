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

namespace en
{
   namespace gpu
   { 
//
//
//
//
//// typedef struct {
////     VkStructureType                             sType;
////     const void*                                 pNext;
////     VkRenderPass                                renderPass;
////     uint32_t                                    attachmentCount;
////     const VkImageView*                          pAttachments;
////     uint32_t                                    width;
////     uint32_t                                    height;
////     uint32_t                                    layers;
//// } VkFramebufferCreateInfo;
//// 
//// 
//// 
//// typedef struct {
////     uint32_t                                    attachment;
////     VkImageLayout                               layout;
//// } VkAttachmentReference;
//// 
//// typedef struct {
////     VkStructureType                             sType;
////     const void*                                 pNext;
////     VkPipelineBindPoint                         pipelineBindPoint;
////     VkSubpassDescriptionFlags                   flags;
////     uint32_t                                    inputCount;
////     const VkAttachmentReference*                pInputAttachments;
////     uint32_t                                    colorCount;
////     const VkAttachmentReference*                pColorAttachments;
////     const VkAttachmentReference*                pResolveAttachments;
////     VkAttachmentReference                       depthStencilAttachment;
////     uint32_t                                    preserveCount;
////     const VkAttachmentReference*                pPreserveAttachments;
//// } VkSubpassDescription;
//// 
//// typedef struct {
////     VkStructureType                             sType;
////     const void*                                 pNext;
////     uint32_t                                    srcSubpass;
////     uint32_t                                    destSubpass;
////     VkPipelineStageFlags                        srcStageMask;
////     VkPipelineStageFlags                        destStageMask;
////     VkMemoryOutputFlags                         outputMask;
////     VkMemoryInputFlags                          inputMask;
////     VkBool32                                    byRegion;
//// } VkSubpassDependency;
//// 
//// 
//// 
//// VkResult vkCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, VkFramebuffer* pFramebuffer);
//// void vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer);
//// VkResult vkCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, VkRenderPass* pRenderPass);
//// void vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass);
//
//
//
//
//
//
//
//   // COLOR / DEPTH / STENCIL ATTACHMENTS
//   //////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//   enum LoadOperation
//      {
//      DontLoad           = 0,
//      Load                  ,
//      Clear                 ,
//      LoadOperationsCount
//      };
//   
//   enum StoreOperation
//      {
//      Discard            = 0,
//      Store                 ,
//      ResolveMSAA           ,
//      StoreOperationsCount
//      };
//
//   class ColorAttachment : public SafeObject
//      {
//      public:
//      float4         clearColor;
//      Ptr<Texture>   resolve;
//      uint32         mipmap;
//      uint32         layer;
//
//      onLoad(const LoadOperation load, 
//             const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 0.0f)) = 0;
//      onStore(const StoreOperation store) = 0;
//      resolve(const Ptr<Texture> texture, 
//              const uint32 mipmap = 0,
//              const uint32 layer = 0) = 0;
//      };
//
//
//
//
//
//   static const VkAttachmentLoadOp TranslateLoadOperation[LoadOperationsCount] =
//      {
//      VK_ATTACHMENT_LOAD_OP_DONT_CARE,          // DontLoad
//      VK_ATTACHMENT_LOAD_OP_LOAD,               // Load
//      VK_ATTACHMENT_LOAD_OP_CLEAR               // Clear
//      };
//   
//   static const VkAttachmentStoreOp TranslateStoreOperation[StoreOperationsCount] = 
//      {
//      VK_ATTACHMENT_STORE_OP_DONT_CARE,         // DontStore
//      VK_ATTACHMENT_STORE_OP_STORE,             // Store
//      VK_ATTACHMENT_STORE_OP_STORE              // ResolveMSAA
//      };
//
//   class ColorAttachmentVK : public ColorAttachment 
//      {
//      public:
//
//      // Cache state to use by RenderPass
//      VkAttachmentDescription state;
//
//      VkFormat                     format;
//      uint32                       samples;
//      VkAttachmentLoadOp           load;
//      VkAttachmentStoreOp          store;
//      VkAttachmentLoadOp           stencilLoad;
//      VkAttachmentStoreOp          stencilStore;
//      VkImageLayout                initialLayout;
//      VkImageLayout                finalLayout;
//      VkAttachmentDescriptionFlags flags;
//
//      ColorAttachmentVK(const TextureFormat format, 
//                        const uint32 samples);
//      };
//
//   // typedef enum {
//   //     VK_IMAGE_LAYOUT_UNDEFINED = 0,
//   //     VK_IMAGE_LAYOUT_GENERAL = 1,
//   //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2,
//   //     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
//   //     VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
//   //     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL = 5,
//   //     VK_IMAGE_LAYOUT_TRANSFER_SOURCE_OPTIMAL = 6,
//   //     VK_IMAGE_LAYOUT_TRANSFER_DESTINATION_OPTIMAL = 7,
//   //     VK_IMAGE_LAYOUT_PREINITIALIZED = 8,
//   // } VkImageLayout;
//
//   ColorAttachmentVK::ColorAttachmentVK(const TextureFormat format, 
//                                        const uint32 samples)
//   {
//   state.sType          = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION;
//   state.pNext          = nullptr;
//   state.format         = TranslateTextureFormat[format];
//   state.samples        = samples;
//   state.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
//   state.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
//   state.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;
//   state.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
//   state.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//   state.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//   state.flags          = 0; // VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
//   }
//
//
//   // VkColorAttachmentViewCreateInfo desc;
//   // desc.sType            = VK_STRUCTURE_TYPE_COLOR_ATTACHMENT_VIEW_CREATE_INFO;
//   // desc.pNext            = nullptr;
//   // desc.image            = color->id;
//   // desc.format           = TranslateTextureFormat[format];
//   // desc.mipLevel         = mipmap;
//   // desc.baseArraySlice   = layer;
//   // desc.arraySize        = layers;
//   // desc.msaaResolveImage = nullptr;
//   // 
//   // // Vulkan is ignoring "depth" parameter.
//   // // VULKAN TODO: Does vulkan support binding 3D and Cube textures ?
//   // //              Should their depth/face be passed through baseArraySlice ?
//   // 
//   // VkColorAttachmentView colorAttView;
//   // VkResult res = vkCreateColorAttachmentView( gpu[i].handle, &desc, &colorAttView );              // device handle here !!!
//   // if (!res)
//   //    {
//   //    Ptr<ColorAttachmentVK> attachment = new ColorAttachmentVK();
//   //    attachment->id = colorAttView;
//   //    result = ptr_dynamic_cast<ColorAttachment, ColorAttachmentVK>(attachment);
//   //    }
//   // 
//   // return result;
//   // }
//   // 
//   // ColorAttachmentVK::onLoad(const LoadOperation _load, const float4 _clearColor)
//   // {
//   // load       = _load;
//   // clearColor = _clearColor;
//   // }
//   // 
//   // ColorAttachmentVK::onStore(const StoreOperation _store)
//   // {
//   // store = _store;
//   // }
//   // 
//   // ColorAttachmentVK::resolve(const Ptr<Texture> _texture, 
//   //                            const uint32 _mipmap,  
//   //                            const uint32 _layer)
//   // {
//   // assert( texture );
//   // 
//   // texture = _texture;
//   // mipmap  = _mipmap;
//   // layer   = _layer;
//   // }
//
//   //Ptr<vkTexture> vkMsaa  = ptr_dynamic_cast<vkTexture, Texture>(resolveMsaa);
//
//   //VkImageSubresourceRange resourceRange;
//   //resourceRange.aspect                       = VK_IMAGE_ASPECT_COLOR;
//   //resourceRange.baseMipLevel                 = msaaMipMap;
//   //resourceRange.mipLevels                    = 1;
//   //resourceRange.baseArraySlice               = msaaLayer;
//   //resourceRange.arraySize                    = layers;
//
//   //colorAttachmentInfo.msaaResolveImage       = vkMsaa->id;
//   //colorAttachmentInfo.msaaResolveSubResource = resourceRange;
//
//
//   //                            //const Ptr<Texture> resolveMsaa,
//
//
//
//
//
//
//
//
//   // RENDER PASS
//   //////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   class RenderPass : public SafeObject
//      {
//
//      };
//
//   class RenderPassVK : public RenderPass
//      {
//      VkRenderPass state;
//      };
//
//
//   class RenderPassMTL : public RenderPass
//      {
//      MTLRenderPassDescriptor* desc;
//
//      RenderPassMTL();
//     ~RenderPassMTL();
//      };
//
//
//
//
//
//   // DX12 Doesn't support Tiled Renderers, and as such, don't have load and store operations.
//
//   static const MTLLoadAction TranslateLoadOperation[LoadOperationsCount] =
//      {
//      MTLLoadActionDontCare,                    // DontLoad
//      MTLLoadActionLoad,                        // Load
//      MTLLoadActionClear                        // Clear
//      };
//   
//   static const MTLStoreAction TranslateStoreOperation[StoreOperationsCount] = 
//      {
//      MTLStoreActionDontCare,                   // DontStore
//      MTLStoreActionStore,                      // Store
//      MTLStoreActionMultisampleResolve          // ResolveMSAA
//      };
//
//
//   Ptr<RenderPass> Create(const uint32v2 resolution,
//                          const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
//                          const Ptr<DepthAttachment> depth,
//                          const Ptr<StencilAttachment> stencil);
//
//
//   RenderPassVK::RenderPassVK(uint32 attachments,
//                              const Ptr<ColorAttachmentVK>* color)
//   {
//   assert( attachments < MaxColorAttachmentsCount );
//
//   // Gather Attachment states
//   VkAttachmentDescription* attachment = new VkAttachmentDescription[attachments];
//   for(uint32 i=0; i<attachments; ++i)
//      memcpy(&attachment[i], &color[i]->state, sizeof(VkAttachmentDescription));
//
//   VkRenderPassCreateInfo passInfo;
//   passInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//   passInfo.pNext           = nullptr;
//   passInfo.attachmentCount = attachments;
//   passInfo.pAttachments    = attachment;
//    uint32_t                                    subpassCount;
//    const VkSubpassDescription*                 pSubpasses;
//    uint32_t                                    dependencyCount;
//    const VkSubpassDependency*                  pDependencies;
//
//
//   delete [] attachment;
//   }
//
//
//
//
//
//   class ColorAttachment : public SafeObject
//      {
//      // On tiled renderers we can specify additional actions
//      // that should be performed when render target is rebind. 
//      virtual onLoad(const LoadOperation load, 
//                     const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 0.0f)) = 0;
//      virtual onStore(const StoreOperation store) = 0;
//      // When binding 3D texture, pass it's plane "depth" through "layer" parameter,
//      // similarly when binding CubeMap texture, pass it's "face" through "layer".
//      virtual resolve(const Ptr<Texture> texture, 
//                      const uint32 mipmap = 0,
//                      const uint32 layer = 0) = 0;
//      };
//
//   class DepthStencilAttachment : public SafeObject
//      {
//      };
//
//   // When binding 3D texture, pass it's plane "depth" through "layer" parameter,
//   // similarly when binding CubeMap texture, pass it's "face" through "layer".
//   Ptr<ColorAttachment> Create(const Ptr<Texture> texture, 
//                               const TextureFormat format,
//                               const uint32 mipmap = 0,
//                               const uint32 layer = 0,
//                               const uint32 layers = 1);
//
//   // METAL COLOR ATTACHMENT
//   //--------------------------------------------------------------------------
//
//
//   class mtlColorAttachment : public ColorAttachment
//      {
//      private:
//      MTLRenderPassColorAttachmentDescriptor* desc;
//
//      public:
//      mtlColorAttachment();
//     ~mtlColorAttachment();
//      onLoad(const LoadOperation load, 
//             const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 0.0f));
//      onStore(const StoreOperation store);
//      resolve(const Ptr<Texture> texture, 
//              const uint32 mipmap = 0,
//              const uint32 layer = 0);
//      };
//
//   mtlColorAttachment::mtlColorAttachment() :
//      desc([[[MTLRenderPassColorAttachmentDescriptor alloc] init] autorelease])
//   {
//   }
//
//   mtlColorAttachment::~mtlColorAttachment()
//   {
//   [desc release];
//   }
//
//
//   Ptr<ColorAttachment> Create(const Ptr<Texture> texture, 
//                               const TextureFormat format,
//                               const uint32 mipmap,
//                               const uint32 layer,
//                               const uint32 layers)
//   {
//   assert( texture );
//
//   Ptr<ColorAttachmentMTL> attachment = new mtlColorAttachment();
//
//   Ptr<TextureMTL> color = ptr_dynamic_cast<TextureMTL, Texture>(texture);
//
//   attachment->desc.texture       = color->id;
//   attachment->desc.level         = mipmap;
//   attachment->desc.slice         = 0;
//   attachment->desc.depthPlane    = 0;
//
//   // Metal is currently not supporting 
//   // Texture2DMultisampleArray, nor
//   // TextureCubeMapArray
//   if (texture->state.type == Texture1DArray ||
//       texture->state.type == Texture2DArray ||
//       texture->state.type == TextureCubeMap)
//      attachment->desc.slice      = layer;
//   else
//   if (texture->state.type == Texture3D)
//      attachment->desc.depthPlane = layer;
//
//   // Metal is ignoring "layers" parameter as it has no
//   // way to select RT layer due to missing Geometry Shader.
//   // It is also ignoring "format" parameter for now.
//
//   // TODO: Do we need to store "format" for some reason ?
//
//   return ptr_dynamic_cast<ColorAttachment, ColorAttachmentMTL>(attachment);
//   }
//
//
//   ColorAttachmentMTL::onLoad(const LoadOperation load, const float4 clearColor)
//   {
//   desc.loadAction = TranslateLoadOperation[load];
//   desc.clearColor = MTLClearColorMake( static_cast<double>(clearColor.r), 
//                                        static_cast<double>(clearColor.g), 
//                                        static_cast<double>(clearColor.b), 
//                                        static_cast<double>(clearColor.a) );
//   }
//
//   ColorAttachmentMTL::onStore(const StoreOperation store)
//   {
//   desc.storeAction = TranslateStoreOperation[store];
//   }
//
//   ColorAttachmentMTL::resolve(const Ptr<Texture> texture, 
//                               const uint32 mipmap,  
//                               const uint32 layer)
//   {
//   assert( texture );
//
//   Ptr<TextureMTL> resolve = ptr_dynamic_cast<TextureMTL, Texture>(texture);
//
//   desc.resolveTexture    = resolve->id;
//   desc.resolveLevel      = mipmap;
//   desc.resolveSlice      = 0;
//   desc.resolveDepthPlane = 0; 
//
//   // Metal is currently not supporting 
//   // Texture2DMultisampleArray, nor
//   // TextureCubeMapArray
//   if (texture->state.type == Texture1DArray ||
//       texture->state.type == Texture2DArray ||
//       texture->state.type == TextureCubeMap)
//      desc.resolveSlice = layer;
//   else
//   if (texture->state.type == Texture3D)
//      sesc.resolveDepthPlane = layer;
//   }
//
//
//
//
//
//
//
//
//
//
//
//
//
//   class mtlDepthStencilAttachment : public DepthStencilAttachment
//      {
//      private:
//      MTLRenderPassDepthAttachmentDescriptor*   depth;
//      MTLRenderPassStencilAttachmentDescriptor* stencil;
//
//      public:
//      mtlDepthStencilAttachment();
//     ~mtlDepthStencilAttachment();
//      onLoad(const LoadOperation load, 
//             const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 0.0f));
//      onStore(const StoreOperation store);
//      resolve(const Ptr<Texture> color, 
//              const uint32 mipmap = 0,
//              const uint32 depth = 0,
//              const uint32 layer = 0);
//      };
//
//   mtlDepthStencilAttachment::mtlDepthStencilAttachment() :
//      depth([[MTLRenderPassDepthAttachmentDescriptor alloc] autorelease]),
//      stencil([[MTLRenderPassStencilAttachmentDescriptor alloc] autorelease])
//   {
//   }
//
//   mtlDepthStencilAttachment::~mtlDepthStencilAttachment()
//   {
//   [depth   release];
//   [stencil release];
//   }
//
//
//
//
//   Ptr<DepthStencilAttachment> Create(const Ptr<Texture> , 
//                               const TextureFormat format,
//                               const uint32 mipmap,
//                               const uint32 depth,
//                               const uint32 layer,
//                               const uint32 layers
//
//                               const Ptr<Texture> stencil, 
//                               const TextureFormat stencilFormat,
//                               const uint32 stencilMipmap,
//                               const uint32 stencilLayer,
//
//                               const Ptr<Texture> resolveMsaa, 
//                               const uint32 resolveMipmap,
//                               const uint32 resolveLayer,
//)
//   {
//   Ptr<DepthAttachment> result = nullptr;
//
//   Ptr<mtlTexture> mtlDepth = ptr_dynamic_cast<mtlTexture, Texture>(depth);
//   Ptr<mtlTexture> mtlMsaa  = ptr_dynamic_cast<mtlTexture, Texture>(resolveMsaa);
//
//   MTLRenderPassDepthAttachmentDescriptor* depthAttachmentInfo = [[MTLRenderPassDepthAttachmentDescriptor alloc] init];
//   depthAttachmentInfo.texture           = depth ? mtlDepth->id : nil;
//   depthAttachmentInfo.level             = depthMipMap;
//   depthAttachmentInfo.slice             = depthLayer;
//   depthAttachmentInfo.depthPlane        = 0;                   // depth in 3D texture
//   depthAttachmentInfo.loadAction        =
//   depthAttachmentInfo.storeAction       =
//   depthAttachmentInfo.resolveTexture    = resolveMsaa ? mtlMsaa->id : nil;
//   depthAttachmentInfo.resolveLevel      = msaaMipMap;
//   depthAttachmentInfo.resolveSlice      = msaaLayer;
//   depthAttachmentInfo.resolveDepthPlane = 0;                   // depth in 3D texture
//   // Depth Attachment specific
//   depthAttachmentInfo.clearDepth        = clearValue; // double
// 
//
//   }
//
//
//
//
//
//
//
//
//
//
//
//
//
//   RenderPassMTL::RenderPassMTL() :
//      desc([[MTLRenderPassDescriptor alloc] autorelease])
//   {
//   }
//
//   RenderPassMTL::~RenderPassMTL()
//   {
//   [desc release];
//   }
//
//
//   Ptr<RenderPass> Create(const uint32v2 resolution,
//                          const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
//                          const Ptr<DepthAttachment> depth,
//                          const Ptr<StencilAttachment> stencil)
//   {
//   Ptr<RenderPassMTL> pass = new RenderPassMTL();
//
//   pass->desc.visibilityResultBuffer = buffer;
//
//   for(uint32 i=0; i<MaxColorAttachmentsCount; ++i)
//      {
//      MTLRenderPassColorAttachmentDescriptor* colorAttachment = pass->desc.colorAttachments[i];
//      colorAttachment.texture           = drawable.currentTexture;
//      colorAttachment.level             = 0;
//      colorAttachment.slice             = 0;
//      colorAttachment.depthPlane        = 0;
//      colorAttachment.resolveTexture    = nil;
//      colorAttachment.resolveLevel      = 0;
//      colorAttachment.resolveSlice      = 0;
//      colorAttachment.resolveDepthPlane = 0;
//      colorAttachment.loadAction        = MTLLoadActionClear;
//      colorAttachment.storeAction       = MTLStoreActionDontCare;
//      colorAttachment.clearColor        = MTLClearColorMake(0.2f,0.3f,0.4f,1.0f);
//      }
//
//   // Depth
//   Ptr<mtlDepthAttachment> mtlDepth = ptr_dynamic_cast<mtlDepthAttachment, DepthAttachment>(depth);
//   depthAttachment = mtlDepth->desc;
//
//   // Stencil
//   Ptr<mtlStencilAttachment> mtlStencil = ptr_dynamic_cast<mtlStencilAttachment, StencilAttachment>(stencil);
//   stencilAttachment = mtlStencil->desc;
//
//   return ptr_dynamic_cast<RenderPass, mtlRenderPass>(pass);
//   }
//
//
//
//


   }
}