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

#include "core/rendering/metal/mtlRenderPass.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "utilities/utilities.h" // For underlyingType()
#include "core/rendering/metal/mtlDevice.h"

namespace en
{
   namespace gpu
   {  
   static const MTLLoadAction TranslateLoadOperation[underlyingType(LoadOperation::OperationsCount)] =
      {
      MTLLoadActionDontCare,                    // None
      MTLLoadActionLoad,                        // Load
      MTLLoadActionClear                        // Clear
      };
   
   static const MTLStoreAction TranslateStoreOperation[underlyingType(StoreOperation::OperationsCount)] =
      {
      MTLStoreActionDontCare,                   // Discard
      MTLStoreActionStore,                      // Store
      MTLStoreActionMultisampleResolve,         // ResolveMSAA
      MTLStoreActionMultisampleResolve          // StoreAndResolveMSAA
      };

   ColorAttachmentMTL::ColorAttachmentMTL(const Ptr<Texture> texture, 
      const uint32 mipmap, 
	  const uint32 layer) :
      desc([[MTLRenderPassColorAttachmentDescriptor alloc] init])
   {
   assert( texture );

   Ptr<TextureMTL> target = ptr_dynamic_cast<TextureMTL, Texture>(texture);

   assert( target->state.mipmaps > mipmap );
   assert( target->state.layers > layer );

   desc.texture           = target->handle;
   desc.level             = mipmap;
   desc.slice             = 0;
   desc.depthPlane        = 0;

   if (target->state.type == TextureType::Texture1DArray ||
       target->state.type == TextureType::Texture2DArray ||
       target->state.type == TextureType::TextureCubeMap)
      desc.slice          = layer;
   else
   if (target->state.type == TextureType::Texture3D)
      desc.depthPlane     = layer;
      
   desc.loadAction        = MTLLoadActionLoad;
   desc.storeAction       = MTLStoreActionStore;
   desc.clearColor        = MTLClearColorMake(0.0f,0.0f,0.0f,1.0f);
   }

   ColorAttachmentMTL::~ColorAttachmentMTL()
   {
   [desc release];
   }

   void ColorAttachmentMTL::onLoad(const LoadOperation load, const float4 clearColor)
   {
   desc.loadAction = TranslateLoadOperation[underlyingType(load)];
   desc.clearColor = MTLClearColorMake( static_cast<double>(clearColor.r), 
                                        static_cast<double>(clearColor.g), 
                                        static_cast<double>(clearColor.b), 
                                        static_cast<double>(clearColor.a) );
   }

   void ColorAttachmentMTL::onStore(const StoreOperation store)
   {
   desc.storeAction = TranslateStoreOperation[underlyingType(store)];
   }

   bool ColorAttachmentMTL::resolve(const Ptr<Texture> texture, const uint32 mipmap, const uint32 layer)
   {
   assert( texture );

   Ptr<TextureMTL> resolve = ptr_dynamic_cast<TextureMTL, Texture>(texture);

   assert( resolve->state.mipmaps > mipmap );
   assert( resolve->state.layers > layer );

   // Metal is currently not supporting 
   // Texture2DMultisampleArray, nor TextureCubeMapArray
   assert( resolve->state.type != TextureType::Texture2DMultisampleArray );
   assert( resolve->state.type != TextureType::TextureCubeMapArray );
  
   // HERE: If in debug layer return false
  
   desc.resolveTexture    = resolve->handle;
   desc.resolveLevel      = mipmap;
   desc.resolveSlice      = 0;
   desc.resolveDepthPlane = 0; 

   if (resolve->state.type == TextureType::Texture1DArray ||
       resolve->state.type == TextureType::Texture2DArray ||
       resolve->state.type == TextureType::TextureCubeMap)
      desc.resolveSlice = layer;
   else
   if (resolve->state.type == TextureType::Texture3D)
      desc.resolveDepthPlane = layer;
      
   return true;
   }



   
   DepthStencilAttachmentMTL::DepthStencilAttachmentMTL(const Ptr<Texture> depth,
      const Ptr<Texture> stencil,
      const uint32 mipmap,
      const uint32 layer) :
      descDepth([[MTLRenderPassDepthAttachmentDescriptor alloc] init]),
      descStencil([[MTLRenderPassStencilAttachmentDescriptor alloc] init])
   {
   assert( depth );

   Ptr<TextureMTL> targetDepth = ptr_dynamic_cast<TextureMTL, Texture>(depth);

   assert( targetDepth->state.mipmaps > mipmap );
   assert( targetDepth->state.layers > layer );

   descDepth.texture           = targetDepth->handle;
   descDepth.level             = mipmap;
   descDepth.slice             = 0;
   descDepth.depthPlane        = 0;

   if (targetDepth->state.type == TextureType::Texture1DArray ||
       targetDepth->state.type == TextureType::Texture2DArray ||
       targetDepth->state.type == TextureType::TextureCubeMap)
      descDepth.slice          = layer;
   else
   if (targetDepth->state.type == TextureType::Texture3D)
      descDepth.depthPlane     = layer;
      
   descDepth.loadAction        = MTLLoadActionClear;
   descDepth.storeAction       = MTLStoreActionStore;
   descDepth.clearDepth        = 1.0f;
   
   // If Depth attachment is Depth-Stencil texture, it needs to be bound to Stencil attachment as well.
   bool depthStencil = false;
   if ( (targetDepth->state.format == Format::DS_24_8) ||
        (targetDepth->state.format == Format::DS_32_f_8) )
      depthStencil = true;
   
   // TODO: Separate stencil texture can have different mipmap and layer ?
   if (stencil || depthStencil)
      {
      Ptr<TextureMTL> targetStencil = stencil ? ptr_dynamic_cast<TextureMTL, Texture>(stencil) :
                                                ptr_dynamic_cast<TextureMTL, Texture>(depth);

      // Separate Stencil must have the same type as Depth attachment.
      assert( targetStencil->state.type == targetDepth->state.type );
      assert( targetStencil->state.mipmaps > mipmap );
      assert( targetStencil->state.layers > layer );

      descStencil.texture           = targetStencil->handle;
      descStencil.level             = mipmap;
      descStencil.slice             = 0;
      descStencil.depthPlane        = 0;

      // TODO: Separate Stencil must have the same type as Depth attachment.
      if (targetDepth->state.type == TextureType::Texture1DArray ||
          targetDepth->state.type == TextureType::Texture2DArray ||
          targetDepth->state.type == TextureType::TextureCubeMap)
         descStencil.slice          = layer;
      else
      if (targetDepth->state.type == TextureType::Texture3D)
         descStencil.depthPlane     = layer;
      
      descStencil.loadAction        = MTLLoadActionClear;
      descStencil.storeAction       = MTLStoreActionStore;
      descStencil.clearStencil      = 0u;
      }
   }

   DepthStencilAttachmentMTL::~DepthStencilAttachmentMTL()
   {
   [descDepth release];
   [descStencil release];
   }
   
   void DepthStencilAttachmentMTL::onLoad(const LoadOperation loadDepthStencil,
      const float clearDepth,
      const uint32 clearStencil)
   {
   descDepth.loadAction = TranslateLoadOperation[underlyingType(loadDepthStencil)];
   descDepth.clearDepth = static_cast<double>(clearDepth);
   descStencil.loadAction = TranslateLoadOperation[underlyingType(loadDepthStencil)];
   descStencil.clearStencil = clearStencil;
   }
   
   void DepthStencilAttachmentMTL::onStore(const StoreOperation storeDepthStencil)
   {
   descDepth.storeAction   = TranslateStoreOperation[underlyingType(storeDepthStencil)];
   descStencil.storeAction = TranslateStoreOperation[underlyingType(storeDepthStencil)];
   }
   
   bool DepthStencilAttachmentMTL::resolve(const Ptr<Texture> depth,
      const uint32 mipmap,
      const uint32 layer)
   {
   assert( depth );

   Ptr<TextureMTL> resolve = ptr_dynamic_cast<TextureMTL, Texture>(depth);

   // Metal is currently not supporting 
   // Texture2DMultisampleArray, nor TextureCubeMapArray
   assert( resolve->state.type != TextureType::Texture2DMultisampleArray );
   assert( resolve->state.type != TextureType::TextureCubeMapArray );
  
   // HERE: If in debug layer return false
  
   descDepth.resolveTexture    = resolve->handle;
   descDepth.resolveLevel      = mipmap;
   descDepth.resolveSlice      = 0;
   descDepth.resolveDepthPlane = 0;

   if (resolve->state.type == TextureType::Texture1DArray ||
       resolve->state.type == TextureType::Texture2DArray ||
       resolve->state.type == TextureType::TextureCubeMap)
      descDepth.resolveSlice = layer;
   else
   if (resolve->state.type == TextureType::Texture3D)
      descDepth.resolveDepthPlane = layer;
      
   return true;
   }
      
   void DepthStencilAttachmentMTL::onStencilLoad(const LoadOperation loadStencil)
   {
   descStencil.loadAction   = TranslateLoadOperation[underlyingType(loadStencil)];
   }
   
   void DepthStencilAttachmentMTL::onStencilStore(const StoreOperation storeStencil)
   {
   descStencil.storeAction = TranslateStoreOperation[underlyingType(storeStencil)];
   }
   
   bool DepthStencilAttachmentMTL::resolveStencil(const Ptr<Texture> stencil,
      const uint32 mipmap,
      const uint32 layer)
   {
   assert( stencil );
   
   Ptr<TextureMTL> resolve = ptr_dynamic_cast<TextureMTL, Texture>(stencil);

   // Metal is currently not supporting 
   // Texture2DMultisampleArray, nor TextureCubeMapArray
   assert( resolve->state.type != TextureType::Texture2DMultisampleArray );
   assert( resolve->state.type != TextureType::TextureCubeMapArray );
  
   // HERE: If in debug layer return false
  
   descStencil.resolveTexture    = resolve->handle;
   descStencil.resolveLevel      = mipmap;
   descStencil.resolveSlice      = 0;
   descStencil.resolveDepthPlane = 0;

   if (resolve->state.type == TextureType::Texture1DArray ||
       resolve->state.type == TextureType::Texture2DArray ||
       resolve->state.type == TextureType::TextureCubeMap)
      descStencil.resolveSlice = layer;
   else
   if (resolve->state.type == TextureType::Texture3D)
      descStencil.resolveDepthPlane = layer;
      
   return true;
   }
                                  






   Ptr<ColorAttachment> MetalDevice::createColorAttachment(const Ptr<Texture> texture,
      const uint32 mipmap,
      const uint32 layer,
      const uint32 layers)
   {
   assert( texture );

   // Metal is not supporting rendering to more than one layer at the same time.
   assert( layers == 1 );

   // Metal is currently not supporting 
   // Texture2DMultisampleArray, nor TextureCubeMapArray
   Ptr<TextureMTL> validate = ptr_dynamic_cast<TextureMTL, Texture>(texture);
   assert( validate->state.type != TextureType::Texture2DMultisampleArray );
   assert( validate->state.type != TextureType::TextureCubeMapArray );
  
   // HERE: If in debug layer return nullptr
   
   return ptr_dynamic_cast<ColorAttachment, ColorAttachmentMTL>(new ColorAttachmentMTL(texture, mipmap, layer));
   }
   
   Ptr<DepthStencilAttachment> MetalDevice::createDepthStencilAttachment(const Ptr<Texture> depth,
      const Ptr<Texture> stencil,
      const uint32 mipmap,
      const uint32 layer,
      const uint32 layers)
   {
   assert( depth );

   // Metal is not supporting rendering to more than one layer at the same time.
   assert( layers == 1 );
   
   // Metal is currently not supporting 
   // Texture2DMultisampleArray, nor TextureCubeMapArray
   Ptr<TextureMTL> validateDepth = ptr_dynamic_cast<TextureMTL, Texture>(depth);
   assert( validateDepth->state.type != TextureType::Texture2DMultisampleArray );
   assert( validateDepth->state.type != TextureType::TextureCubeMapArray );
   if (stencil)
      {
      Ptr<TextureMTL> validateStencil = ptr_dynamic_cast<TextureMTL, Texture>(stencil);
      assert( validateStencil->state.type != TextureType::Texture2DMultisampleArray );
      assert( validateStencil->state.type != TextureType::TextureCubeMapArray );
      
      // Depth and Stencil need to be of the same type
      assert( validateDepth->state.type == validateStencil->state.type );
      }
      
   // HERE: If in debug layer return nullptr
   
   return ptr_dynamic_cast<DepthStencilAttachment, DepthStencilAttachmentMTL>(new DepthStencilAttachmentMTL(depth, stencil, mipmap, layer));
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
   pass->desc.depthAttachment   = mtlDepthStencil->descDepth;
   pass->desc.stencilAttachment = mtlDepthStencil->descStencil;

   // Layered Rendering
   pass->desc.renderTargetArrayLength = 0;  // TODO: Pick attachment with smallest count of Layers
  
   return ptr_dynamic_cast<RenderPass, RenderPassMTL>(pass);
   }

   Ptr<RenderPass> MetalDevice::create(const uint32 _attachments,
                                       const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
                                       const Ptr<DepthStencilAttachment> depthStencil)
   {
   assert( _attachments < MaxColorAttachmentsCount );

   Ptr<RenderPassMTL> pass = new RenderPassMTL();

   // pass->desc.visibilityResultBuffer = buffer; // TODO: MTLBuffer for Occlusion Query

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
   pass->desc.depthAttachment   = mtlDepthStencil->descDepth;
   pass->desc.stencilAttachment = mtlDepthStencil->descStencil;

   // Layered Rendering
   pass->desc.renderTargetArrayLength = 0;  // TODO: Pick attachment with smallest count of Layers
   
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
   colorAttachment.storeAction       = MTLStoreActionStore;
   colorAttachment.clearColor        = MTLClearColorMake(0.0f,0.0f,0.0f,1.0f);
   
   // Depth & Stencil
   Ptr<DepthStencilAttachmentMTL> mtlDepthStencil = ptr_dynamic_cast<DepthStencilAttachmentMTL, DepthStencilAttachment>(depthStencil);
   pass->desc.depthAttachment   = mtlDepthStencil->descDepth;
   pass->desc.stencilAttachment = mtlDepthStencil->descStencil;

   // Layered Rendering
   pass->desc.renderTargetArrayLength = 0;
   
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
   colorAttachment.storeAction       = MTLStoreActionMultisampleResolve;
   colorAttachment.clearColor        = MTLClearColorMake(0.0f,0.0f,0.0f,1.0f);
   
   // Depth & Stencil
   Ptr<DepthStencilAttachmentMTL> mtlDepthStencil = ptr_dynamic_cast<DepthStencilAttachmentMTL, DepthStencilAttachment>(depthStencil);
   pass->desc.depthAttachment   = mtlDepthStencil->descDepth;
   pass->desc.stencilAttachment = mtlDepthStencil->descStencil;

   // Layered Rendering
   pass->desc.renderTargetArrayLength = 0;
   
   return ptr_dynamic_cast<RenderPass, RenderPassMTL>(pass);
   }
   









   RenderPassMTL::RenderPassMTL() :
      desc([[MTLRenderPassDescriptor alloc] init])
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


// declaration - h
// definition  - cpp






   }
}

#endif