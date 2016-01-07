/*

 Ngine v5.0
 
 Module      : Input Assembler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/mtlRenderPass.h"

namespace en
{
   namespace gpu
   {  
   static const MTLLoadAction TranslateLoadOperation[LoadOperationsCount] =
      {
      MTLLoadActionDontCare,                    // None
      MTLLoadActionLoad,                        // Load
      MTLLoadActionClear                        // Clear
      };
   
   static const MTLStoreAction TranslateStoreOperation[StoreOperationsCount] = 
      {
      MTLStoreActionDontCare,                   // Discard
      MTLStoreActionStore,                      // Store
      MTLStoreActionMultisampleResolve,         // ResolveMSAA
      0                                         // StoreAndResolveMSAA
      };

   ColorAttachmentMTL::ColorAttachmentMTL(const Ptr<Texture> texture, const uint32 mipmap, const uint32 layer) :
      desc([[[MTLRenderPassColorAttachmentDescriptor alloc] init] autorelease])
   {
   assert( texture );

   Ptr<TextureMTL> target = ptr_dynamic_cast<TextureMTL, Texture>(texture);

   desc.texture           = target->id;
   desc.level             = mipmap;
   desc.slice             = 0;
   desc.depthPlane        = 0;
   
   // Metal is currently not supporting 
   // Texture2DMultisampleArray, nor
   // TextureCubeMapArray
   if (resolve->state.type == Texture1DArray ||
       resolve->state.type == Texture2DArray ||
       resolve->state.type == TextureCubeMap)
      desc.slice          = layer;
   else
   if (resolve->state.type == Texture3D)
      desc.depthPlane     = layer;
      
   desc.loadAction        = MTLLoadActionClear;
   desc.storeAction       = MTLStoreActionStore;
   desc.clearColor        = MTLClearColorMake(0.0f,0.0f,0.0f,1.0f);
   }

   ColorAttachmentMTL::~ColorAttachmentMTL()
   {
   [desc release];
   }

   ColorAttachmentMTL::onLoad(const LoadOperation load, const float4 clearColor)
   {
   desc.loadAction = TranslateLoadOperation[load];
   desc.clearColor = MTLClearColorMake( static_cast<double>(clearColor.r), 
                                        static_cast<double>(clearColor.g), 
                                        static_cast<double>(clearColor.b), 
                                        static_cast<double>(clearColor.a) );
   }

   ColorAttachmentMTL::onStore(const StoreOperation store)
   {
   desc.storeAction = TranslateStoreOperation[store];
   }

   ColorAttachmentMTL::resolve(const Ptr<Texture> texture, const uint32 mipmap, const uint32 layer)
   {
   assert( texture );

   Ptr<TextureMTL> resolve = ptr_dynamic_cast<TextureMTL, Texture>(texture);

   desc.resolveTexture    = resolve->id;
   desc.resolveLevel      = mipmap;
   desc.resolveSlice      = 0;
   desc.resolveDepthPlane = 0; 

   // Metal is currently not supporting 
   // Texture2DMultisampleArray, nor
   // TextureCubeMapArray
   if (resolve->state.type == Texture1DArray ||
       resolve->state.type == Texture2DArray ||
       resolve->state.type == TextureCubeMap)
      desc.resolveSlice = layer;
   else
   if (resolve->state.type == Texture3D)
      desc.resolveDepthPlane = layer;
   }


   RenderPassMTL::RenderPassMTL() :
      desc([[MTLRenderPassDescriptor alloc] autorelease])
   {
   }

   RenderPassMTL::~RenderPassMTL()
   {
   [desc release];
   }

//   Ptr<RenderPass> MetalDevice::create(const uint32 _attachments,
//                                       const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
//                                       const Ptr<DepthStencilAttachment> depthstencil)
//   {
//   Ptr<RenderPass> result = nullptr;
//   
//
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


   Ptr<ColorAttachment> MetalDevice::createColorAttachment(const Ptr<Texture> texture,
                                          const uint32 mipmap = 0,
                                          const uint32 layer = 0,
                                          const uint32 layers = 1)
   {
   assert( texture );

   // Metal is not supporting rendering to more than one layer at the same time.
   assert( layers == 1 );
   
   return ptr_dynamic_cast<ColorAttachment, ColorAttachmentMTL>(new ColorAttachmentMTL(texture, mipmap, layer));
   }

   Ptr<DepthStencilAttachment> MetalDevice::createDepthStencilAttachment(const Ptr<Texture> depth, const Ptr<Texture> stencil,
                                                 const uint32 mipmap = 0,
                                                 const uint32 layer = 0,
                                                 const uint32 layers = 1)
   {
   assert( texture );

   // Metal is not supporting rendering to more than one layer at the same time.
   assert( layers == 1 );
   
   return ptr_dynamic_cast<DepthStecilAttachment, DepthStecilAttachmentMTL>(new DepthStecilAttachmentMTL(depth, stencil, mipmap, layer));
   }





   // Creates simple render pass with one color destination
   Ptr<RenderPass> MetalDevice::create(const Ptr<ColorAttachment> color,
                                       const Ptr<DepthStencilAttachment> depthStencil)
   {
   Ptr<RenderPassMTL> pass = new RenderPassMTL();

   pass->desc.visibilityResultBuffer = nil; // TODO: MTLBuffer for Occlusion Query

   // Color
   Ptr<ColorAttachmentMTL> mtlColor = ptr_dynamic_cast<ColorAttachmentMTL, ColorAttachment>(color);
   pass->desc.colorAttachments[0] = mtlColor->desc;
   
   // Depth & Stencil
   Ptr<DepthStencilAttachmentMTL> mtlDepthStencil = ptr_dynamic_cast<DepthStencilAttachmentMTL, DepthStencilAttachment>(depthStencil);
   pass->desc.depthAttachment   = mtlDepthStencil->depth;
   pass->desc.stencilAttachment = mtlDepthStencil->stencil;

   return ptr_dynamic_cast<RenderPass, RenderPassMTL>(pass);
   }

   Ptr<RenderPass> MetalDevice::create(const uint32 _attachments,
                                       const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
                                       const Ptr<DepthStencilAttachment> depthStencil)
   {
   assert( _attachments < MaxColorAttachmentsCount );

   Ptr<RenderPassMTL> pass = new RenderPassMTL();

   pass->desc.visibilityResultBuffer = buffer; // TODO: MTLBuffer for Occlusion Query

   // Color
   for(uint32 i=0; i<_attachments; ++i)
      {
      if (color[i] == nullptr)
         continue;
         
      Ptr<ColorAttachmentMTL> mtlColor = ptr_dynamic_cast<ColorAttachmentMTL, ColorAttachment>(color[i]);
      pass->desc.colorAttachments[i] = mtlColor->desc;
      }

   // Depth & Stencil
   Ptr<DepthStencilAttachmentMTL> mtlDepthStencil = ptr_dynamic_cast<DepthStencilAttachmentMTL, DepthStencilAttachment>(depthStencil);
   pass->desc.depthAttachment   = mtlDepthStencil->depth;
   pass->desc.stencilAttachment = mtlDepthStencil->stencil;

   return ptr_dynamic_cast<RenderPass, RenderPassMTL>(pass);
   }

   // Creates render pass which's output goes to window framebuffer
   Ptr<RenderPass> MetalDevice::create(const Ptr<Texture> framebuffer,
                                       const Ptr<DepthStencilAttachment> depthStencil)
   {
   Ptr<RenderPassMTL> pass = new RenderPassMTL();

   pass->desc.visibilityResultBuffer = nil; // TODO: MTLBuffer for Occlusion Query

   // Color
   assert( framebuffer );
   Ptr<TextureMTL> fbo  = ptr_dynamic_cast<TextureMTL, Texture>(framebuffer);

   MTLRenderPassColorAttachmentDescriptor* colorAttachment = pass->desc.colorAttachments[0];
   colorAttachment.texture           = fbo->handle;
   colorAttachment.level             = 0;
   colorAttachment.slice             = 0;
   colorAttachment.depthPlane        = 0;
   colorAttachment.resolveTexture    = nil;
   colorAttachment.resolveLevel      = 0;
   colorAttachment.resolveSlice      = 0;
   colorAttachment.resolveDepthPlane = 0;
   colorAttachment.loadAction        = MTLLoadActionClear;
   colorAttachment.storeAction       = MTLStoreActionDontCare;
   colorAttachment.clearColor        = MTLClearColorMake(0.0f,0.0f,0.0f,1.0f);
   
   // Depth & Stencil
   Ptr<DepthStencilAttachmentMTL> mtlDepthStencil = ptr_dynamic_cast<DepthStencilAttachmentMTL, DepthStencilAttachment>(depthStencil);
   pass->desc.depthAttachment   = mtlDepthStencil->depth;
   pass->desc.stencilAttachment = mtlDepthStencil->stencil;

   return ptr_dynamic_cast<RenderPass, RenderPassMTL>(pass);
   }

   // Creates render pass which's output is resolved from temporary MSAA target to window framebuffer
   Ptr<RenderPass> MetalDevice::create(const Ptr<Texture> temporaryMSAA,
                                       const Ptr<Texture> framebuffer,
                                       const Ptr<DepthStencilAttachment> depthStencil)
   {
   assert( temporaryMSAA );
   assert( framebuffer );
   
   Ptr<RenderPassMTL> pass = new RenderPassMTL();

   pass->desc.visibilityResultBuffer = nil; // TODO: MTLBuffer for Occlusion Query

   // Color
   Ptr<TextureMTL> msaa = ptr_dynamic_cast<TextureMTL, Texture>(temporaryMSAA);
   Ptr<TextureMTL> fbo  = ptr_dynamic_cast<TextureMTL, Texture>(framebuffer);

   MTLRenderPassColorAttachmentDescriptor* colorAttachment = pass->desc.colorAttachments[0];
   colorAttachment.texture           = msaa->handle;
   colorAttachment.level             = 0;
   colorAttachment.slice             = 0;
   colorAttachment.depthPlane        = 0;
   colorAttachment.resolveTexture    = fbo->handle;
   colorAttachment.resolveLevel      = 0;
   colorAttachment.resolveSlice      = 0;
   colorAttachment.resolveDepthPlane = 0;
   colorAttachment.loadAction        = MTLLoadActionClear;
   colorAttachment.storeAction       = MTLStoreActionDontCare;
   colorAttachment.clearColor        = MTLClearColorMake(0.0f,0.0f,0.0f,1.0f);
   
   // Depth & Stencil
   Ptr<DepthStencilAttachmentMTL> mtlDepthStencil = ptr_dynamic_cast<DepthStencilAttachmentMTL, DepthStencilAttachment>(depthStencil);
   pass->desc.depthAttachment   = mtlDepthStencil->depth;
   pass->desc.stencilAttachment = mtlDepthStencil->stencil;

   return ptr_dynamic_cast<RenderPass, RenderPassMTL>(pass);
   }
      
      












   }
}

#endif