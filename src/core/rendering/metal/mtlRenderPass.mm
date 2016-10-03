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
   static const MTLLoadAction TranslateLoadOperation[underlyingType(LoadOperation::Count)] =
      {
      MTLLoadActionDontCare,                    // None
      MTLLoadActionLoad,                        // Load
      MTLLoadActionClear                        // Clear
      };
   
   static const MTLStoreAction TranslateStoreOperation[underlyingType(StoreOperation::Count)] =
      {
      MTLStoreActionDontCare,                   // Discard
      MTLStoreActionStore                       // Store
      };

#if defined(EN_PLATFORM_IOS)
   static const MTLMultisampleDepthResolveFilter TranslateDepthResolveMode[underlyingType(DepthResolve::Count)] =
      {
      MTLMultisampleDepthResolveFilterSample0,  // Sample0
      MTLMultisampleDepthResolveFilterMin,      // Min
      MTLMultisampleDepthResolveFilterMax       // Max
      };
#endif


   // COLOR ATTACHMENT
   //////////////////////////////////////////////////////////////////////////


   ColorAttachmentMTL::ColorAttachmentMTL(const Ptr<TextureView> source) :
      desc([[MTLRenderPassColorAttachmentDescriptor alloc] init])
   {
   assert( source );

   Ptr<TextureViewMTL> target = ptr_dynamic_cast<TextureViewMTL, TextureView>(source);

   desc.texture           = target->handle;
   desc.level             = 0;  // Texture view will specify mipmap/layer/depthPlane of source texture
   desc.slice             = 0;
   desc.depthPlane        = 0;
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

   void ColorAttachmentMTL::onLoad(const LoadOperation load, const uint32v4 clearColor)
   {
   desc.loadAction = TranslateLoadOperation[underlyingType(load)];
   desc.clearColor = MTLClearColorMake( static_cast<double>(clearColor.x),
                                        static_cast<double>(clearColor.y),
                                        static_cast<double>(clearColor.z),
                                        static_cast<double>(clearColor.w) );
   }
   
   void ColorAttachmentMTL::onLoad(const LoadOperation load, const sint32v4 clearColor)
   {
   desc.loadAction = TranslateLoadOperation[underlyingType(load)];
   desc.clearColor = MTLClearColorMake( static_cast<double>(clearColor.x),
                                        static_cast<double>(clearColor.y),
                                        static_cast<double>(clearColor.z),
                                        static_cast<double>(clearColor.w) );
   }

   void ColorAttachmentMTL::onStore(const StoreOperation store)
   {
   desc.storeAction = TranslateStoreOperation[underlyingType(store)];
   
   // Auto-detect that surface need to be resolved
   if ( (desc.storeAction == MTLStoreActionStore) &&
        (desc.resolveTexture != nil) )
      desc.storeAction = MTLStoreActionStoreAndMultisampleResolve; // MTLStoreActionMultisampleResolve;
   }

   bool ColorAttachmentMTL::resolve(const Ptr<TextureView> destination)
   {
   assert( destination );

   Ptr<TextureViewMTL> resolve = ptr_dynamic_cast<TextureViewMTL, TextureView>(destination);

   // Metal is currently not supporting 
   // Texture2DMultisampleArray, nor TextureCubeMapArray
   assert( resolve->viewType != TextureType::Texture2DMultisampleArray );
   assert( resolve->viewType != TextureType::TextureCubeMapArray );
  
   // HERE: If in debug layer return false
  
   desc.resolveTexture    = resolve->handle;
   desc.resolveLevel      = 0; // Texture view will specify mipmap/layer/depthPlane of source texture
   desc.resolveSlice      = 0;
   desc.resolveDepthPlane = 0; 

   // Correct store action to take into notice resolve
   if (desc.storeAction == MTLStoreActionStore)
      desc.storeAction = MTLStoreActionStoreAndMultisampleResolve; // MTLStoreActionMultisampleResolve;
      
   return true;
   }


   // DEPTH-STENCIL ATTACHMENT
   //////////////////////////////////////////////////////////////////////////

   
   DepthStencilAttachmentMTL::DepthStencilAttachmentMTL(const Ptr<TextureView> depth,
                                                        const Ptr<TextureView> stencil) :
      descDepth([[MTLRenderPassDepthAttachmentDescriptor alloc] init]),
      descStencil([[MTLRenderPassStencilAttachmentDescriptor alloc] init])
   {
   assert( depth );

   Ptr<TextureViewMTL> targetDepth = ptr_dynamic_cast<TextureViewMTL, TextureView>(depth);

   descDepth.texture     = targetDepth->handle;
   descDepth.level       = 0; // Texture view will specify mipmap/layer/depthPlane of source texture
   descDepth.slice       = 0;
   descDepth.depthPlane  = 0;
   descDepth.loadAction  = MTLLoadActionClear;
   descDepth.storeAction = MTLStoreActionStore;
   descDepth.clearDepth  = 1.0f;
   
   // If Depth attachment is Depth-Stencil texture, it needs to be bound to Stencil attachment as well.
   bool depthStencil = false;
   if ( (targetDepth->viewFormat == Format::DS_24_8) ||
        (targetDepth->viewFormat == Format::DS_32_f_8) )
      depthStencil = true;
   
   // TODO: Separate stencil texture can have different mipmap and layer ?
   if (stencil || depthStencil)
      {
      Ptr<TextureViewMTL> targetStencil = stencil ? ptr_dynamic_cast<TextureViewMTL, TextureView>(stencil) :
                                                    ptr_dynamic_cast<TextureViewMTL, TextureView>(depth);

      // Separate Stencil must have the same type as Depth attachment.
      assert( targetStencil->viewType == targetDepth->viewType );

      descStencil.texture      = targetStencil->handle;
      descStencil.level        = 0; // Texture view will specify mipmap/layer/depthPlane of source texture
      descStencil.slice        = 0;
      descStencil.depthPlane   = 0;
      descStencil.loadAction   = MTLLoadActionClear;
      descStencil.storeAction  = MTLStoreActionStore;
      descStencil.clearStencil = 0u;
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
   
   // Auto-detect that surface need to be resolved
   if ( (descDepth.storeAction == MTLStoreActionStore) &&
        (descDepth.resolveTexture != nil) )
      descDepth.storeAction = MTLStoreActionStoreAndMultisampleResolve; // MTLStoreActionMultisampleResolve;;

   if ( (descStencil.storeAction == MTLStoreActionStore) &&
        (descStencil.resolveTexture != nil) )
      descStencil.storeAction = MTLStoreActionStoreAndMultisampleResolve; // MTLStoreActionMultisampleResolve;;
   }
   
   bool DepthStencilAttachmentMTL::resolve(const Ptr<TextureView> destination,
      const DepthResolve mode)
   {
#if defined(EN_PLATFORM_OSX)
   return false;
#endif
#if defined(EN_PLATFORM_IOS)
   assert( destination );

   Ptr<TextureViewMTL> resolveView = ptr_dynamic_cast<TextureViewMTL, TextureView>(destination);

   // Metal is currently not supporting 
   // Texture2DMultisampleArray, nor TextureCubeMapArray
   assert( resolveView->viewType != TextureType::Texture2DMultisampleArray );
   assert( resolveView->viewType != TextureType::TextureCubeMapArray );
  
   // HERE: If in debug layer return false
   
   descDepth.resolveTexture     = resolve->handle;
   descDepth.resolveLevel       = 0; // Texture view will specify mipmap/layer/depthPlane of source texture
   descDepth.resolveSlice       = 0;
   descDepth.resolveDepthPlane  = 0;
   descDepth.depthResolveFilter = TranslateDepthResolveMode[mode];
      
   // Correct store action to take into notice resolve
   if (descDepth.storeAction == MTLStoreActionStore)
      descDepth.storeAction = MTLStoreActionStoreAndMultisampleResolve; // MTLStoreActionMultisampleResolve;;
      
   return true;
#endif
   }
      
   void DepthStencilAttachmentMTL::onStencilLoad(const LoadOperation loadStencil)
   {
   descStencil.loadAction   = TranslateLoadOperation[underlyingType(loadStencil)];
   }
   
   void DepthStencilAttachmentMTL::onStencilStore(const StoreOperation storeStencil)
   {
   descStencil.storeAction = TranslateStoreOperation[underlyingType(storeStencil)];
   }
   
                                  






   Ptr<ColorAttachment> MetalDevice::createColorAttachment(const Ptr<TextureView> source)
   {
   assert( source );

   // Metal is currently not supporting 
   // Texture2DMultisampleArray, nor TextureCubeMapArray
   Ptr<TextureViewMTL> validate = ptr_dynamic_cast<TextureViewMTL, TextureView>(source);
   assert( validate->viewType != TextureType::Texture2DMultisampleArray );
   assert( validate->viewType != TextureType::TextureCubeMapArray );
  
   // HERE: If in debug layer return nullptr
   
   return ptr_dynamic_cast<ColorAttachment, ColorAttachmentMTL>(new ColorAttachmentMTL(source));
   }
   
   Ptr<DepthStencilAttachment> MetalDevice::createDepthStencilAttachment(const Ptr<TextureView> depth,
                                                                         const Ptr<TextureView> stencil)
   {
   assert( depth || stencil );
   
   // Metal is currently not supporting Texture2DMultisampleArray, nor TextureCubeMapArray
   Ptr<TextureViewMTL> validateDepth   = nullptr;
   Ptr<TextureViewMTL> validateStencil = nullptr;
   if (depth)
      {
      validateDepth = ptr_dynamic_cast<TextureViewMTL, TextureView>(depth);
      assert( validateDepth->viewType != TextureType::Texture2DMultisampleArray );
      assert( validateDepth->viewType != TextureType::TextureCubeMapArray );
      }
   if (stencil)
      {
      validateStencil = ptr_dynamic_cast<TextureViewMTL, TextureView>(stencil);
      assert( validateStencil->viewType != TextureType::Texture2DMultisampleArray );
      assert( validateStencil->viewType != TextureType::TextureCubeMapArray );
      }
   if (depth && stencil)
      {
      // Depth and Stencil need to be of the same type
      assert( validateDepth->viewType == validateStencil->viewType );
      }
      
   // HERE: If in debug layer return nullptr
    
   return ptr_dynamic_cast<DepthStencilAttachment, DepthStencilAttachmentMTL>(new DepthStencilAttachmentMTL(depth, stencil));
   }


   // TODO: Use Views, mipma and layer always equal to 0 !
   
   // Metal supports specifying base layer of Render Pass attachment through
   // explicitly setting "slice" or "depthPlane" in MTLRenderPassAttachmentDescriptor.
   //
   // Vulkan supports the same feature through providing new ImageView that
   // starts at given layer offset.
   //
   // Metal is not supporting rendering to more than one layer at the same time.




   RenderPassMTL::RenderPassMTL() :
      desc([[MTLRenderPassDescriptor alloc] init])
   {
   }

   RenderPassMTL::~RenderPassMTL()
   {
   [desc release];
   }


   // Creates simple render pass with one color destination
   Ptr<RenderPass> MetalDevice::createRenderPass(const Ptr<ColorAttachment> color,
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

   Ptr<RenderPass> MetalDevice::createRenderPass(const uint32 _attachments,
                                                 const Ptr<ColorAttachment>* color,
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
   Ptr<RenderPass> MetalDevice::createRenderPass(const Ptr<Texture> framebuffer,
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
   Ptr<RenderPass> MetalDevice::createRenderPass(const Ptr<Texture> temporaryMSAA,
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
   


// declaration - h
// definition  - cpp






   }
}

#endif
