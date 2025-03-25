/*

 Ngine v5.0
 
 Module      : Metal Render Pass.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlRenderPass.h"

#if defined(EN_MODULE_RENDERER_METAL)

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
    MTLStoreActionStore,                      // Store
    MTLStoreActionMultisampleResolve,         // Resolve
    MTLStoreActionStoreAndMultisampleResolve  // StoreAndResolve
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


ColorAttachmentMTL::ColorAttachmentMTL(
        const Format _format,
        const uint32 _samples) :
    desc(allocateObjectiveC(MTLRenderPassColorAttachmentDescriptor)),
    format(_format),
    samples(_samples)
{
    desc.texture           = nil; // Bind at RenderPass start. Use Framebuffer surface.
    desc.level             = 0;   // Texture view will specify mipmap/layer/depthPlane of source texture
    desc.slice             = 0;
    desc.depthPlane        = 0;
    desc.loadAction        = MTLLoadActionLoad;
    desc.storeAction       = MTLStoreActionStore;
    desc.clearColor        = MTLClearColorMake(0.0f,0.0f,0.0f,1.0f);
   
    desc.resolveTexture    = nil; // Bind at RenderPass start. Use Framebuffer surface.
    desc.resolveLevel      = 0;   // Texture view will specify mipmap/layer/depthPlane of source texture
    desc.resolveSlice      = 0;
    desc.resolveDepthPlane = 0;
}

ColorAttachmentMTL::~ColorAttachmentMTL()
{
    deallocateObjectiveC(desc);
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
   
    resolve = false;
    if (store == StoreOperation::Resolve ||
        store == StoreOperation::StoreAndResolve)
    {
        resolve = true;
    }
}


// DEPTH-STENCIL ATTACHMENT
//////////////////////////////////////////////////////////////////////////


DepthStencilAttachmentMTL::DepthStencilAttachmentMTL(
        const Format _depthFormat,
        const Format _stencilFormat,
        const uint32 _samples) :
    descDepth(allocateObjectiveC(MTLRenderPassDepthAttachmentDescriptor)),
    descStencil(allocateObjectiveC(MTLRenderPassStencilAttachmentDescriptor)),
    depthFormat(_depthFormat),
    stencilFormat(_stencilFormat),
    samples(_samples)
{
    // Depth-Stencil textures need to be bound to both slots
    if (isDepthStencil(depthFormat) &&
        stencilFormat == Format::Unsupported)
    {
        stencilFormat = depthFormat;
    }
      
    descDepth.texture     = nil; // Bind at RenderPass start. Use Framebuffer surface.
    descDepth.level       = 0;   // Texture view will specify mipmap/layer/depthPlane of source texture
    descDepth.slice       = 0;
    descDepth.depthPlane  = 0;
    descDepth.loadAction  = MTLLoadActionClear;
    descDepth.storeAction = MTLStoreActionStore;
    descDepth.clearDepth  = 1.0f;

    descStencil.texture      = nil; // Bind at RenderPass start. Use Framebuffer surface.
    descStencil.level        = 0;   // Texture view will specify mipmap/layer/depthPlane of source texture
    descStencil.slice        = 0;
    descStencil.depthPlane   = 0;
    descStencil.loadAction   = MTLLoadActionClear;
    descStencil.storeAction  = MTLStoreActionStore;
    descStencil.clearStencil = 0u;
}

DepthStencilAttachmentMTL::~DepthStencilAttachmentMTL()
{
    deallocateObjectiveC(descDepth);
    deallocateObjectiveC(descStencil);
}
   
void DepthStencilAttachmentMTL::onLoad(
    const LoadOperation loadDepthStencil,
    const float clearDepth,
    const uint32 clearStencil)
{
    descDepth.loadAction = TranslateLoadOperation[underlyingType(loadDepthStencil)];
    descDepth.clearDepth = static_cast<double>(clearDepth);
    descStencil.loadAction   = TranslateLoadOperation[underlyingType(loadDepthStencil)];
    descStencil.clearStencil = clearStencil;
}
   
void DepthStencilAttachmentMTL::onStore(
    const StoreOperation storeDepthStencil,
    const DepthResolve resolveMode)
{
    descDepth.storeAction        = TranslateStoreOperation[underlyingType(storeDepthStencil)];
    descStencil.storeAction      = TranslateStoreOperation[underlyingType(storeDepthStencil)];
#if defined(EN_PLATFORM_IOS)
    descDepth.depthResolveFilter = TranslateDepthResolveMode[resolveMode];
#endif

//    // Auto-detect that surface need to be resolved
//    if ( (descDepth.storeAction == MTLStoreActionStore) &&
//         (descDepth.resolveTexture != nil) )
//    {
//        descDepth.storeAction = MTLStoreActionStoreAndMultisampleResolve; // MTLStoreActionMultisampleResolve;
//    }
//
//    if ( (descStencil.storeAction == MTLStoreActionStore) &&
//         (descStencil.resolveTexture != nil) )
//    {
//        descStencil.storeAction = MTLStoreActionStoreAndMultisampleResolve; // MTLStoreActionMultisampleResolve;
//    }
}
   
//   bool DepthStencilAttachmentMTL::resolve(const std::shared_ptr<TextureView> destination,
//      const DepthResolve mode)
//   {
//#if defined(EN_PLATFORM_OSX)
//   return false;
//#endif
//#if defined(EN_PLATFORM_IOS)
//   assert( destination );
//
//   TextureViewMTL* resolveView = reinterpret_cast<TextureViewMTL*>(destination.get());
//
//   // Metal is currently not supporting 
//   // Texture2DMultisampleArray, nor TextureCubeMapArray
//   assert( resolveView->viewType != TextureType::Texture2DMultisampleArray );
//   assert( resolveView->viewType != TextureType::TextureCubeMapArray );
//  
//   // HERE: If in debug layer return false
//   
//   descDepth.resolveTexture     = resolve->handle;
//   descDepth.resolveLevel       = 0; // Texture view will specify mipmap/layer/depthPlane of source texture
//   descDepth.resolveSlice       = 0;
//   descDepth.resolveDepthPlane  = 0;
//   descDepth.depthResolveFilter = TranslateDepthResolveMode[mode];
//      
//   // Correct store action to take into notice resolve
//   if (descDepth.storeAction == MTLStoreActionStore)
//      descDepth.storeAction = MTLStoreActionStoreAndMultisampleResolve; // MTLStoreActionMultisampleResolve;;
//      
//   return true;
//#endif
//   }
      
void DepthStencilAttachmentMTL::onStencilLoad(const LoadOperation loadStencil)
{
    descStencil.loadAction   = TranslateLoadOperation[underlyingType(loadStencil)];
}

void DepthStencilAttachmentMTL::onStencilStore(const StoreOperation storeStencil)
{
    descStencil.storeAction = TranslateStoreOperation[underlyingType(storeStencil)];
}


// FRAMEBUFFER
//////////////////////////////////////////////////////////////////////////


FramebufferMTL::FramebufferMTL(const uint32v2 _resolution, const uint32 _layers) :
    resolution(_resolution),
    layers(_layers)
{
}
   
FramebufferMTL::~FramebufferMTL()
{
    // Auto-release pool to ensure that Metal ARC will flush garbage collector
    @autoreleasepool
    {
        for(uint32 i=0; i<MaxColorAttachmentsCount; ++i)
        {
            deallocateObjectiveC(color[i]);
            deallocateObjectiveC(resolve[i]);
        }
      
        for(uint32 i=0; i<3; ++i)
        {
            deallocateObjectiveC(depthStencil[i]);
        }
    }
}

// RENDER PASS
//////////////////////////////////////////////////////////////////////////


RenderPassMTL::RenderPassMTL() :
    desc(allocateObjectiveC(MTLRenderPassDescriptor)),
    usedAttachments(0u),
    resolve(false)
{
}

RenderPassMTL::~RenderPassMTL()
{
    deallocateObjectiveC(desc);
}

std::shared_ptr<Framebuffer> RenderPassMTL::createFramebuffer(
    const uint32v2 _resolution,
    const uint32 _layers,
    const uint32 surfaces,
    const std::shared_ptr<TextureView>* surface,
    const std::shared_ptr<TextureView> _depthStencil,
    const std::shared_ptr<TextureView> _stencil,
    const std::shared_ptr<TextureView> _depthResolve)
{
    // TODO: If there is only one surface view.
    //       If this view is default, and points to one of the Swap-Chain textures of any created Window.
    //       Get this window pre-created Framebuffer object and return pointer to it.


    uint32 attachments = bitsCount(usedAttachments);

    assert( surface );
    assert( surfaces >= attachments );

    // Create Framebuffer object only if render pass usues any destination surfaces
    // TODO: It shouldn't check Input but RenderPass requirements!
    if (surfaces      == 0u      &&
        _depthStencil == nullptr &&
        _stencil      == nullptr &&
        _depthResolve == nullptr)
    {
        return std::shared_ptr<Framebuffer>(nullptr);
    }

    std::shared_ptr<FramebufferMTL> framebuffer = std::make_shared<FramebufferMTL>(_resolution, _layers);

    // Create patching array
    uint32 index = 0u;
    for(uint32 i=0; i<MaxColorAttachmentsCount; ++i)
    {
        if (checkBit(usedAttachments, i))
        {
            TextureViewMTL* view = reinterpret_cast<TextureViewMTL*>(surface[index].get());
            framebuffer->color[i] = [view->handle retain];
            index++;
        }
        else
        {
            framebuffer->color[i] = nil;
        }
    }
      
    if (resolve)
    {
        assert( surfaces == index * 2 );
      
        for(uint32 i=0; i<MaxColorAttachmentsCount; ++i)
        {
            if (checkBit(usedAttachments, i))
            {
                TextureViewMTL* view = reinterpret_cast<TextureViewMTL*>(surface[index].get());
                framebuffer->resolve[i] = [view->handle retain];
                index++;
            }
            else
            {
                framebuffer->resolve[i] = nil;
            }
        }
    }
      
    for(uint32 i=0; i<3; ++i)
    {
        framebuffer->depthStencil[i] = nil;
    }
      
    if (_depthStencil)
    {
        TextureViewMTL* view = reinterpret_cast<TextureViewMTL*>(_depthStencil.get());
        framebuffer->depthStencil[0u] = [view->handle retain];
      
        // Depth-Stencil textures need to be bound to both slots
        if (!_stencil && isDepthStencil(view->viewFormat))
        {
            framebuffer->depthStencil[1u] = [view->handle retain];
        }
    }
    if (_stencil)
    {
        TextureViewMTL* view = reinterpret_cast<TextureViewMTL*>(_stencil.get());
        framebuffer->depthStencil[1u] = [view->handle retain];
    }
    if (_depthResolve)
    {
        TextureViewMTL* view = reinterpret_cast<TextureViewMTL*>(_depthResolve.get());
        framebuffer->depthStencil[2] = [view->handle retain];
    }
      
    // TODO: This function should only validate that all surfaces have the same
    //       resolution and layers count!
    //       Calculation of common resolution and layers should be done by the app !
 
    return framebuffer;
}
   
std::shared_ptr<Framebuffer> RenderPassMTL::createFramebuffer(
    const uint32v2 _resolution,
    const std::shared_ptr<TextureView> swapChainSurface,
    const std::shared_ptr<TextureView> depthStencil,
    const std::shared_ptr<TextureView> stencil)
{
    uint32 attachments = bitsCount(usedAttachments);

    assert( swapChainSurface );
    assert( attachments == 1 );

    std::shared_ptr<FramebufferMTL> framebuffer = std::make_shared<FramebufferMTL>(_resolution, 1u);

    // Create patching array
    TextureViewMTL* view = reinterpret_cast<TextureViewMTL*>(swapChainSurface.get());
    framebuffer->color[0u] = [view->handle retain];
    framebuffer->resolve[0u] = nil;
   
    for(uint32 i=1; i<MaxColorAttachmentsCount; ++i)
    {
        framebuffer->color[i] = nil;
        framebuffer->resolve[i] = nil;
    }

    for(uint32 i=0; i<3; ++i)
    {
        framebuffer->depthStencil[i] = nil;
    }
      
    if (depthStencil)
    {
        TextureViewMTL* view = reinterpret_cast<TextureViewMTL*>(depthStencil.get());
        framebuffer->depthStencil[0u] = [view->handle retain];
      
        // Depth-Stencil textures need to be bound to both slots
        if (!stencil && isDepthStencil(view->viewFormat))
        {
            framebuffer->depthStencil[1u] = [view->handle retain];
        }
    }
    if (stencil)
    {
        TextureViewMTL* view = reinterpret_cast<TextureViewMTL*>(stencil.get());
        framebuffer->depthStencil[1u] = [view->handle retain];
    }
      
    // TODO: This function should only validate that all surfaces have the same
    //       resolution and layers count!
    //       Calculation of common resolution and layers should be done by the app !
 
    return framebuffer;
}

std::shared_ptr<Framebuffer> RenderPassMTL::createFramebuffer(
    const uint32v2 _resolution,
    const std::shared_ptr<TextureView> temporaryMSAA,
    const std::shared_ptr<TextureView> swapChainSurface,
    const std::shared_ptr<TextureView> depthStencil,
    const std::shared_ptr<TextureView> stencil)
{
    uint32 attachments = bitsCount(usedAttachments);

    assert( temporaryMSAA );
    assert( swapChainSurface );
    assert( attachments == 1 );

    std::shared_ptr<FramebufferMTL> framebuffer = std::make_shared<FramebufferMTL>(_resolution, 1u);

    // Create patching array
    TextureViewMTL* viewA = reinterpret_cast<TextureViewMTL*>(temporaryMSAA.get());
    framebuffer->color[0u] = [viewA->handle retain];

    TextureViewMTL* viewB = reinterpret_cast<TextureViewMTL*>(swapChainSurface.get());
    framebuffer->resolve[0u] = [viewB->handle retain];
   
    for(uint32 i=1; i<MaxColorAttachmentsCount; ++i)
    {
        framebuffer->color[i] = nil;
        framebuffer->resolve[i] = nil;
    }

    for(uint32 i=0; i<3; ++i)
    {
        framebuffer->depthStencil[i] = nil;
    }
   
    if (depthStencil)
    {
        TextureViewMTL* view = reinterpret_cast<TextureViewMTL*>(depthStencil.get());
        framebuffer->depthStencil[0] = [view->handle retain];

        // Depth-Stencil textures need to be bound to both slots
        if (!stencil && isDepthStencil(view->viewFormat))
        {
            framebuffer->depthStencil[1u] = [view->handle retain];
        }
    }
    if (stencil)
    {
        TextureViewMTL* view = reinterpret_cast<TextureViewMTL*>(stencil.get());
        framebuffer->depthStencil[1] = [view->handle retain];
    }
      
    // TODO: This function should only validate that all surfaces have the same
    //       resolution and layers count!
    //       Calculation of common resolution and layers should be done by the app !
 
    return framebuffer;
}

// DEVICE
//////////////////////////////////////////////////////////////////////////


ColorAttachment* MetalDevice::createColorAttachment(
    const Format format,
    const uint32 samples)
{
    assert( format != Format::Unsupported );
    assert( samples > 0u );
   
    return new ColorAttachmentMTL(format, samples);
}
   
DepthStencilAttachment* MetalDevice::createDepthStencilAttachment(
    const Format depthFormat,
    const Format stencilFormat,
    const uint32 samples)
{
    // Needs to be DepthStencil, Depth or Stencil
    assert( isDepthStencil(depthFormat) ||
            isDepth(depthFormat)        ||
            isStencil(stencilFormat) );
    assert( samples > 0u );
   
    // TODO: In Debug mode check if Format is supported at current HW in Real-Time
   
    return new DepthStencilAttachmentMTL(depthFormat, stencilFormat, samples);
}

RenderPass* MetalDevice::createRenderPass(
    const uint32 attachments,
    const std::shared_ptr<ColorAttachment> color[],
    const DepthStencilAttachment* depthStencil)
{
    assert( attachments < MaxColorAttachmentsCount );
   
    // Metal is not supporting Fragment Shaders working only on Side Effect Buffers without classic output ones
    assert( depthStencil || attachments > 0 );

    RenderPassMTL* pass = new RenderPassMTL();

    // pass->desc.visibilityResultBuffer = buffer; // TODO: MTLBuffer for Occlusion Query

    // Optional Color Attachments
    for(uint32 i=0; i<MaxColorAttachmentsCount; ++i)
    {
        if (i >= attachments)
        {
            pass->format[i]  = Format::Unsupported;
            pass->samples[i] = 0u;
            continue;
        }
         
        if (color[i] == nullptr)
        {
            pass->format[i]  = Format::Unsupported;
            pass->samples[i] = 0u;
            continue;
        }
         
        ColorAttachmentMTL* mtlColor = reinterpret_cast<ColorAttachmentMTL*>(color[i].get());
        pass->desc.colorAttachments[i] = mtlColor->desc;
        pass->format[i]  = mtlColor->format;
        pass->samples[i] = mtlColor->samples;
        setBit(pass->usedAttachments, i);
      
        if (mtlColor->resolve)
		{
            pass->resolve = true;
		}
    }

    // Optional Depth-Stencil / Depth / Stencil
    if (depthStencil)
    {
        const DepthStencilAttachmentMTL* mtlDepthStencil = reinterpret_cast<const DepthStencilAttachmentMTL*>(depthStencil);
        pass->desc.depthAttachment   = mtlDepthStencil->descDepth;
        pass->desc.stencilAttachment = mtlDepthStencil->descStencil;
        pass->format[MaxColorAttachmentsCount]   = mtlDepthStencil->depthFormat;
        pass->format[MaxColorAttachmentsCount+1] = mtlDepthStencil->stencilFormat;
        pass->samples[MaxColorAttachmentsCount]  = mtlDepthStencil->samples;
    }
    else
    {
        pass->format[MaxColorAttachmentsCount]   = Format::Unsupported;
        pass->format[MaxColorAttachmentsCount+1] = Format::Unsupported;
        pass->samples[MaxColorAttachmentsCount]  = 0u;
    }

    // Layered Rendering
    pass->desc.renderTargetArrayLength = 0u;  // Set it at RenderPass Start using Framebuffer data
   
    return pass;
}
   
RenderPass* MetalDevice::createRenderPass(
    const ColorAttachment& swapChainSurface,
    const DepthStencilAttachment* depthStencil)
{
    RenderPassMTL* pass = new RenderPassMTL();

    // pass->desc.visibilityResultBuffer = buffer; // TODO: MTLBuffer for Occlusion Query

    // Optional Color Attachments
    for(uint32 i=0; i<MaxColorAttachmentsCount; ++i)
    {
        if (i == 0)
        {
            const ColorAttachmentMTL& mtlColor = reinterpret_cast<const ColorAttachmentMTL&>(swapChainSurface);
            pass->desc.colorAttachments[0] = mtlColor.desc;
            pass->format[0]  = mtlColor.format;
            pass->samples[0] = mtlColor.samples;
            setBit(pass->usedAttachments, 0);
        }
        else
        {
            pass->format[i]  = Format::Unsupported;
            pass->samples[i] = 0u;
        }
    }

    // Optional Depth-Stencil / Depth / Stencil
    if (depthStencil)
    {
        const DepthStencilAttachmentMTL* mtlDepthStencil = reinterpret_cast<const DepthStencilAttachmentMTL*>(depthStencil);
        pass->desc.depthAttachment               = mtlDepthStencil->descDepth;
        pass->desc.stencilAttachment             = mtlDepthStencil->descStencil;
        pass->format[MaxColorAttachmentsCount]   = mtlDepthStencil->depthFormat;
        pass->format[MaxColorAttachmentsCount+1] = mtlDepthStencil->stencilFormat;
        pass->samples[MaxColorAttachmentsCount]  = mtlDepthStencil->samples;
    }
    else
    {
        pass->format[MaxColorAttachmentsCount]   = Format::Unsupported;
        pass->format[MaxColorAttachmentsCount+1] = Format::Unsupported;
        pass->samples[MaxColorAttachmentsCount]  = 0u;
    }

    // Layered Rendering
    pass->desc.renderTargetArrayLength = 0u;  // Set it at RenderPass Start using Framebuffer data
   
    return pass;
}
   
   
     // TODO: Use Views, mipmap and layer always equal to 0 !
   
   // Metal supports specifying base layer of Render Pass attachment through
   // explicitly setting "slice" or "depthPlane" in MTLRenderPassAttachmentDescriptor.
   //
   // Vulkan supports the same feature through providing new ImageView that
   // starts at given layer offset.
   //
   // Metal is not supporting rendering to more than one layer at the same time.




 
   
   
   
   
   
   
   
   
   
   
   
   
//   bool separateStencil = false;
//   if (stencilFormat != Format::Unsupported)
//      separateStencil = true;
//      
//   // If Depth attachment is Depth-Stencil texture, it needs to be bound to Stencil attachment as well.
//
//   // TODO: Separate stencil texture can have different mipmap and layer ?
//   if (separateStencil || isDepthStencil(depthFormat) )
//      {
////      TextureViewMTL* targetStencil = separateStencil ? reinterpret_cast<TextureViewMTL*>(stencil.get()) :
////                                                        reinterpret_cast<TextureViewMTL*>(depth.get());
////
////      // Separate Stencil must have the same type as Depth attachment.
////      assert( targetStencil->viewType == targetDepth->viewType );
//
//
//      }
   
//   // Metal is currently not supporting 
//   // Texture2DMultisampleArray, nor TextureCubeMapArray
//   TextureViewMTL* validate = reinterpret_cast<TextureViewMTL*>(source.get());
//   assert( validate->viewType != TextureType::Texture2DMultisampleArray );
//   assert( validate->viewType != TextureType::TextureCubeMapArray );
//  
//   // HERE: If in debug layer return nullptr

//   // Metal is currently not supporting Texture2DMultisampleArray, nor TextureCubeMapArray
//   TextureViewMTL* validateDepth   = nullptr;
//   TextureViewMTL* validateStencil = nullptr;
//   if (depth)
//      {
//      validateDepth = reinterpret_cast<TextureViewMTL*>(depth.get());
//      assert( validateDepth->viewType != TextureType::Texture2DMultisampleArray );
//      assert( validateDepth->viewType != TextureType::TextureCubeMapArray );
//      }
//   if (stencil)
//      {
//      validateStencil = reinterpret_cast<TextureViewMTL*>(stencil.get());
//      assert( validateStencil->viewType != TextureType::Texture2DMultisampleArray );
//      assert( validateStencil->viewType != TextureType::TextureCubeMapArray );
//      }
//   if (depth && stencil)
//      {
//      // Depth and Stencil need to be of the same type
//      assert( validateDepth->viewType == validateStencil->viewType );
//      }
//      
//   // HERE: If in debug layer return nullptr
//    
//   // Metal is currently not supporting 
//   // Texture2DMultisampleArray, nor TextureCubeMapArray
//   assert( resolve->viewType != TextureType::Texture2DMultisampleArray );
//   assert( resolve->viewType != TextureType::TextureCubeMapArray );
//  
//   // Auto-detect that surface need to be resolved
//   if ( (desc.storeAction == MTLStoreActionStore) &&
//        (desc.resolveTexture != nil) )
//      desc.storeAction = MTLStoreActionStoreAndMultisampleResolve; // MTLStoreActionMultisampleResolve;
      

//   // Creates simple render pass with one color destination
//   std::shared_ptr<RenderPass> MetalDevice::createRenderPass(const std::shared_ptr<ColorAttachment> color,
//                                                 const std::shared_ptr<DepthStencilAttachment> depthStencil)
//   {
//   std::shared_ptr<RenderPassMTL> pass = std::make_shared<RenderPassMTL>();
//
//   pass->desc.visibilityResultBuffer = nil; // TODO: MTLBuffer for Occlusion Query
//
//   // Color
//   ColorAttachmentMTL* mtlColor = reinterpret_cast<ColorAttachmentMTL*>(color.get());
//   pass->desc.colorAttachments[0] = mtlColor->desc;
//   
//   // Depth & Stencil
//   DepthStencilAttachmentMTL* mtlDepthStencil = reinterpret_cast<DepthStencilAttachmentMTL*>(depthStencil.get());
//   pass->desc.depthAttachment   = mtlDepthStencil->descDepth;
//   pass->desc.stencilAttachment = mtlDepthStencil->descStencil;
//
//   // Layered Rendering
//   pass->desc.renderTargetArrayLength = 0;  // TODO: Pick attachment with smallest count of Layers
//  
//   return pass;
//   }

//   // Creates render pass which's output goes to window framebuffer
//   std::shared_ptr<RenderPass> MetalDevice::createRenderPass(const std::shared_ptr<Texture> framebuffer,
//                                                 const std::shared_ptr<DepthStencilAttachment> depthStencil)
//   {
//   std::shared_ptr<RenderPassMTL> pass = std::make_shared<RenderPassMTL>();
//
//   pass->desc.visibilityResultBuffer = nil; // TODO: MTLBuffer for Occlusion Query
//
//   // Color
//   assert( framebuffer );
//   TextureMTL* fbo = reinterpret_cast<TextureMTL*>(framebuffer.get());
//
//   MTLRenderPassColorAttachmentDescriptor* colorAttachment = pass->desc.colorAttachments[0];
//   colorAttachment.texture           = fbo->handle;
//   colorAttachment.level             = 0;
//   colorAttachment.slice             = 0;
//   colorAttachment.depthPlane        = 0;
//   colorAttachment.resolveTexture    = nil;
//   colorAttachment.resolveLevel      = 0;
//   colorAttachment.resolveSlice      = 0;
//   colorAttachment.resolveDepthPlane = 0;
//   colorAttachment.loadAction        = MTLLoadActionClear;
//   colorAttachment.storeAction       = MTLStoreActionStore;
//   colorAttachment.clearColor        = MTLClearColorMake(0.0f,0.0f,0.0f,1.0f);
//   
//   // Depth & Stencil
//   DepthStencilAttachmentMTL* mtlDepthStencil = reinterpret_cast<DepthStencilAttachmentMTL*>(depthStencil.get());
//   pass->desc.depthAttachment   = mtlDepthStencil->descDepth;
//   pass->desc.stencilAttachment = mtlDepthStencil->descStencil;
//
//   // Layered Rendering
//   pass->desc.renderTargetArrayLength = 0;
//   
//   return pass;
//   }
//
//   // Creates render pass which's output is resolved from temporary MSAA target to window framebuffer
//   std::shared_ptr<RenderPass> MetalDevice::createRenderPass(const std::shared_ptr<Texture> temporaryMSAA,
//                                                 const std::shared_ptr<Texture> framebuffer,
//                                                 const std::shared_ptr<DepthStencilAttachment> depthStencil)
//   {
//   assert( temporaryMSAA );
//   assert( framebuffer );
//   
//   std::shared_ptr<RenderPassMTL> pass = std::make_shared<RenderPassMTL>();
//
//   pass->desc.visibilityResultBuffer = nil; // TODO: MTLBuffer for Occlusion Query
//
//   // Color
//   TextureMTL* msaa = reinterpret_cast<TextureMTL*>(temporaryMSAA.get());
//   TextureMTL* fbo  = reinterpret_cast<TextureMTL*>(framebuffer.get());
//
//   MTLRenderPassColorAttachmentDescriptor* colorAttachment = pass->desc.colorAttachments[0];
//   colorAttachment.texture           = msaa->handle;
//   colorAttachment.level             = 0;
//   colorAttachment.slice             = 0;
//   colorAttachment.depthPlane        = 0;
//   colorAttachment.resolveTexture    = fbo->handle;
//   colorAttachment.resolveLevel      = 0;
//   colorAttachment.resolveSlice      = 0;
//   colorAttachment.resolveDepthPlane = 0;
//   colorAttachment.loadAction        = MTLLoadActionClear;
//   colorAttachment.storeAction       = MTLStoreActionMultisampleResolve;
//   colorAttachment.clearColor        = MTLClearColorMake(0.0f,0.0f,0.0f,1.0f);
//   
//   // Depth & Stencil
//   DepthStencilAttachmentMTL* mtlDepthStencil = reinterpret_cast<DepthStencilAttachmentMTL*>(depthStencil.get());
//   pass->desc.depthAttachment   = mtlDepthStencil->descDepth;
//   pass->desc.stencilAttachment = mtlDepthStencil->descStencil;
//
//   // Layered Rendering
//   pass->desc.renderTargetArrayLength = 0;
//   
//   return pass;
//   }


   


// declaration - h
// definition  - cpp






} // en::gpu
} // en

#endif
