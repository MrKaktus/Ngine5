/*

 Ngine v5.0
 
 Module      : D3D12 Render Pass.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12RenderPass.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12Device.h"
#include "core/rendering/d3d12/dx12CommandBuffer.h"

namespace en
{
   namespace gpu
   {
   // COLOR ATTACHMENT
   //////////////////////////////////////////////////////////////////////////


   ColorState::ColorState() :
      clearValue(),
      format(DXGI_FORMAT_UNKNOWN),
      samples(1u),
      loadOp(LoadOperation::Load),
      storeOp(StoreOperation::Store),
      resolve(false)
   {
   }

   ColorState::ColorState(const DXGI_FORMAT _format, const uint32 _samples) :
      clearValue(),
      format(_format),
      samples(_samples),
      loadOp(LoadOperation::Load),
      storeOp(StoreOperation::Store),
      resolve(false)
   {
   }

   ColorAttachmentD3D12::ColorAttachmentD3D12(const Format format, const uint32 samples) :
      state(TranslateTextureFormat[underlyingType(format)], samples)
   {
   }
   
   ColorAttachmentD3D12::~ColorAttachmentD3D12()
   {
   }

   void ColorAttachmentD3D12::onLoad(const LoadOperation load, const float4 clearValue)
   {
   state.loadOp     = load;
   state.clearValue = clearValue;
   }
   
   void ColorAttachmentD3D12::onLoad(const LoadOperation load, const uint32v4 clearValue)
   {
   state.loadOp       = load;
   state.clearValue.r = static_cast<float>(clearValue.x);
   state.clearValue.g = static_cast<float>(clearValue.y);
   state.clearValue.b = static_cast<float>(clearValue.z);
   state.clearValue.a = static_cast<float>(clearValue.w);
   }
   
   void ColorAttachmentD3D12::onLoad(const LoadOperation load, const sint32v4 clearValue)
   {
   state.loadOp       = load;
   state.clearValue.r = static_cast<float>(clearValue.x);
   state.clearValue.g = static_cast<float>(clearValue.y);
   state.clearValue.b = static_cast<float>(clearValue.z);
   state.clearValue.a = static_cast<float>(clearValue.w);
   }
   
   void ColorAttachmentD3D12::onStore(const StoreOperation store)
   {
   state.storeOp = store;

   if (store == StoreOperation::Resolve ||
       store == StoreOperation::StoreAndResolve)
      {
      // Source surface need to be MSAA to be later resolved
      assert( state.samples > 1 );
      state.resolve = true;
      }
   }


   // DEPTH-STENCIL ATTACHMENT
   //////////////////////////////////////////////////////////////////////////


   DepthState::DepthState() :
      format(DXGI_FORMAT_UNKNOWN),
      samples(1u),
      clearFlags(static_cast<D3D12_CLEAR_FLAGS>(0u)),
      storeOp(StoreOperation::Store),
      stencilStoreOp(StoreOperation::Store),
      clearDepth(1.0f),
      clearStencil(0u)
   {
   }

   DepthState::DepthState(const uint32 _samples) :
      format(DXGI_FORMAT_UNKNOWN),
      samples(_samples),
      clearFlags(static_cast<D3D12_CLEAR_FLAGS>(0u)),
      storeOp(StoreOperation::Store),
      stencilStoreOp(StoreOperation::Store),
      clearDepth(1.0f),
      clearStencil(0u)
   {
   }
   
   // If you don't want to use Depth, nor Stencil surfaces, just don't assign
   // this object at all at Render Pass creation time.
   DepthStencilAttachmentD3D12::DepthStencilAttachmentD3D12(const Format depthFormat,
                                                            const Format stencilFormat,
                                                            const uint32 samples) :
      state(samples)
   {
   // Direct3D12 doesn't support separate Depth and Stencil surfaces at the same time.
   assert( !(depthFormat   != Format::Unsupported && 
             stencilFormat != Format::Unsupported) );
   assert( depthFormat   != Format::Unsupported || 
           stencilFormat != Format::Unsupported );

   // Needs to be DepthStencil, Depth or Stencil
   assert( isDepthStencil(depthFormat) ||
           isDepth(depthFormat)        ||
           isStencil(stencilFormat) );
      
   state.format = depthFormat != Format::Unsupported ? TranslateTextureFormat[underlyingType(depthFormat)] :
                                                       TranslateTextureFormat[underlyingType(stencilFormat)];
   }
   
   DepthStencilAttachmentD3D12::~DepthStencilAttachmentD3D12()
   {
   }

   void DepthStencilAttachmentD3D12::onLoad(const LoadOperation loadDepthStencil,
                                            const float _clearDepth,
                                            const uint32 _clearStencil)
   {
   // DepthStencil load and store operations can be different,
   // but by default we set them to the same operation.
   if (loadDepthStencil == LoadOperation::Clear)
      state.clearFlags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
   else
      state.clearFlags = static_cast<D3D12_CLEAR_FLAGS>(0u);
      
   state.clearDepth   = _clearDepth;
   state.clearStencil = _clearStencil;
   }

   void DepthStencilAttachmentD3D12::onStore(const StoreOperation storeDepthStencil,
                                             const DepthResolve resolveMode)
   {
   // DepthStencil load and store operations can be different,
   // but by default we set them to the same operation.
   state.storeOp        = storeDepthStencil;
   state.stencilStoreOp = storeDepthStencil;

   // Direct3D12 is not supporting Depth/Stencil resolve operations.
   }

   void DepthStencilAttachmentD3D12::onStencilLoad(const LoadOperation loadStencil)
   {
   // Direct3D12 doesn't support separate Depth and Stencil surfaces at the same time.
   if (loadStencil == LoadOperation::Clear)
      state.clearFlags |= D3D12_CLEAR_FLAG_STENCIL;
   else
      clearBitmask(state.clearFlags, D3D12_CLEAR_FLAG_STENCIL);
   }

   void DepthStencilAttachmentD3D12::onStencilStore(const StoreOperation storeStencil)
   {
   // Direct3D12 doesn't support separate Depth and Stencil surfaces at the same time.
   state.stencilStoreOp = storeStencil;
   }


   // FRAMEBUFFER
   //////////////////////////////////////////////////////////////////////////


   FramebufferD3D12::FramebufferD3D12(const uint32v2 _resolution, const uint32 _layers) :
      resolution(_resolution),
      layers(_layers)
   {
   }

   FramebufferD3D12::~FramebufferD3D12()
   {
   for(uint32 i=0; i<8; ++i)
      {
      colorHandle[i] = nullptr;
      resolveHandle[i] = nullptr;
      }

   depthHandle = nullptr;
   }


   // RENDER PASS
   //////////////////////////////////////////////////////////////////////////


   RenderPassD3D12::RenderPassD3D12() :
      usedAttachments(0),
      resolve(false),
      depthStencil(false)
   {
   // By default it asumes that output is carried only by
   // side effects, and there are no output attachments.
   }

   RenderPassD3D12::~RenderPassD3D12()
   {
   }

   void fillRTV(const TextureType viewType,
                const uint32v2 viewMipmaps,
                const uint32v2 viewLayers,
                D3D12_RENDER_TARGET_VIEW_DESC& desc)
   {
   switch(viewType)
      {
      case TextureType::Texture1D:
         desc.Texture1D.MipSlice               = viewMipmaps.base;
         break;

      case TextureType::Texture1DArray:
         desc.Texture1DArray.MipSlice          = viewMipmaps.base;
         desc.Texture1DArray.FirstArraySlice   = viewLayers.base;
         desc.Texture1DArray.ArraySize         = viewLayers.count;
         break;

      case TextureType::Texture2D:
         desc.Texture2D.MipSlice               = viewMipmaps.base;
         desc.Texture2D.PlaneSlice             = 0u;
         break;
            
      case TextureType::Texture2DArray:
         desc.Texture2DArray.MipSlice          = viewMipmaps.base;
         desc.Texture2DArray.FirstArraySlice   = viewLayers.base;
         desc.Texture2DArray.ArraySize         = viewLayers.count;
         desc.Texture2DArray.PlaneSlice        = 0u;
         break;
         
      case TextureType::Texture2DMultisample:
         break;
         
      case TextureType::Texture2DMultisampleArray:
         desc.Texture2DMSArray.FirstArraySlice = viewLayers.base;
         desc.Texture2DMSArray.ArraySize       = viewLayers.count;
         break;
         
      case TextureType::Texture3D:
         desc.Texture3D.MipSlice               = viewMipmaps.base;
         desc.Texture3D.FirstWSlice            = viewLayers.base;
         desc.Texture3D.WSize                  = viewLayers.count;
         break;
            
      default:
         assert( 0 );
         break;
      }
   }
   
   void fillDSV(const TextureType viewType,
                const uint32v2 viewMipmaps,
                const uint32v2 viewLayers,
                D3D12_DEPTH_STENCIL_VIEW_DESC& desc)
   {
   switch(viewType)
      {
      case TextureType::Texture1D:
         desc.Texture1D.MipSlice               = viewMipmaps.base;
         break;

      case TextureType::Texture1DArray:
         desc.Texture1DArray.MipSlice          = viewMipmaps.base;
         desc.Texture1DArray.FirstArraySlice   = viewLayers.base;
         desc.Texture1DArray.ArraySize         = viewLayers.count;
         break;

      case TextureType::Texture2D:
         desc.Texture2D.MipSlice               = viewMipmaps.base;
         break;
               
      case TextureType::Texture2DArray:
         desc.Texture2DArray.MipSlice          = viewMipmaps.base;
         desc.Texture2DArray.FirstArraySlice   = viewLayers.base;
         desc.Texture2DArray.ArraySize         = viewLayers.count;
         break;
            
      case TextureType::Texture2DMultisample:
         break;
            
      case TextureType::Texture2DMultisampleArray:
         desc.Texture2DMSArray.FirstArraySlice = viewLayers.base;
         desc.Texture2DMSArray.ArraySize       = viewLayers.count;
         break;
               
      default:
         assert( 0 );
         break;
      }
   }

   shared_ptr<Framebuffer> RenderPassD3D12::createFramebuffer(const uint32v2 resolution,
      const uint32 layers,
      const uint32 attachments, // TODO: Count of passed attachments, may be smaller than usedAttachments count ?  <-- Currently input attachments array is tightly packed, firct colors, then their resolves
      const shared_ptr<TextureView>* attachment,
      const shared_ptr<TextureView> _depthStencil,
      const shared_ptr<TextureView> _stencil,
      const shared_ptr<TextureView> _depthResolve)
   {
   shared_ptr<FramebufferD3D12> result = nullptr;

   assert( layers >= 1 );
   assert( _depthStencil == nullptr ||   // D3D12 is not supporting separate Depth and Stencil at the same time.
           _stencil      == nullptr );   // (but it supports Depth / DepthStencil or just Stencil setups).
   assert( _depthResolve == nullptr );   // D3D12 is not supporting Depth resolve

   // Create Framebuffer object only if render pass usues any destination surfaces
   if (bitsCount(usedAttachments) == 0u && depthStencil == false)
      return shared_ptr<Framebuffer>(nullptr);

   result = make_shared<FramebufferD3D12>(resolution, layers);
   
   uint32 index = 0;
   for(uint32 i=0; i<8; ++i)
      if (checkBit(usedAttachments, i))
         {
         assert( index < attachments ); // TODO: So what happens if app passes less attachments to creation call ?

         // Keep reference to used view
         shared_ptr<TextureViewD3D12> view = dynamic_pointer_cast<TextureViewD3D12>(attachment[index]);
         result->colorHandle[i] = view;
         
         assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_TEXTURECUBE );
         assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_TEXTURECUBEARRAY );

         // Create Render Target View
         result->colorDesc[i].Format        = view->desc.Format;
         result->colorDesc[i].ViewDimension = static_cast<D3D12_RTV_DIMENSION>(view->desc.ViewDimension);

         // D3D12 is not allowing reinterpretation of resource type
         // when using it for rendering (as there is no ViewDimmension field)
         TextureType type = view->texture->state.type;
         assert( type == view->viewType );
         
         fillRTV(view->viewType,
                 view->mipmaps,
                 view->layers,
                 result->colorDesc[i]);

         ++index;
         }
     
   // Keep references to resolve destinations
   if (resolve)
      for(uint32 i=0; i<8; ++i)
         if (checkBit(usedAttachments, i))
            {
            shared_ptr<TextureViewD3D12> view = dynamic_pointer_cast<TextureViewD3D12>(attachment[index]);
            result->resolveHandle[i] = view;
            ++index;
            }
      
   if (depthStencil)
      {
      shared_ptr<TextureViewD3D12> view = _depthStencil ? dynamic_pointer_cast<TextureViewD3D12>(_depthStencil)
                                                        : dynamic_pointer_cast<TextureViewD3D12>(_stencil);
         
      // Keep reference to used Depth-Stencil resource
      result->depthHandle = view;

      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_UNKNOWN );
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_BUFFER );
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_TEXTURE3D );
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_TEXTURECUBE );
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_TEXTURECUBEARRAY );

      result->depthDesc.Format        = view->desc.Format;
      result->depthDesc.ViewDimension = static_cast<D3D12_DSV_DIMENSION>(static_cast<uint32>(view->desc.ViewDimension) + 1);
      result->depthDesc.Flags         = D3D12_DSV_FLAG_NONE;
      
      // Optimize texture usage
      if (!checkBitmask(underlyingType(view->texture->state.usage), underlyingType(TextureUsage::RenderTargetWrite)))
         {
         Format format = view->texture->state.format;
         
         if (isStencil(format))
            result->depthDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL;
         else
         if (isDepth(format))
            result->depthDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
         else
            result->depthDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH | D3D12_DSV_FLAG_READ_ONLY_STENCIL;
         }
      
      fillDSV(view->viewType,
              view->mipmaps,
              view->layers,
              result->depthDesc);
      }
      
   return result;
   }

   // Creates framebuffer using window Swap-Chain surface.
   shared_ptr<Framebuffer> RenderPassD3D12::createFramebuffer(const uint32v2 resolution,
      const shared_ptr<TextureView> swapChainSurface,
      const shared_ptr<TextureView> _depthStencil,
      const shared_ptr<TextureView> _stencil)
   {
   shared_ptr<FramebufferD3D12> result = nullptr;

   assert( swapChainSurface );
   assert( _depthStencil == nullptr ||   // D3D12 is not supporting separate Depth and Stencil at the same time.
           _stencil      == nullptr );   // (but it supports Depth / DepthStencil or just Stencil setups).

   // Resolution cannot be greater than selected destination size (Swap-Chain surfaces have no mipmaps)
   TextureViewD3D12* ptr = reinterpret_cast<TextureViewD3D12*>(swapChainSurface.get());
   assert( resolution.width  <= ptr->texture->state.width );
   assert( resolution.height <= ptr->texture->state.height );

   // Render Pass should use single Color Attachment #0
   assert( bitsCount(usedAttachments) == 1 );
   assert( checkBit(usedAttachments, 0) );

   result = make_shared<FramebufferD3D12>(resolution, 1);

   // Keep reference to used view
   shared_ptr<TextureViewD3D12> view = dynamic_pointer_cast<TextureViewD3D12>(swapChainSurface);
   result->colorHandle[0] = view;
   
   // It's Swap-Chain surface, thus it needs to be single 2D texture
   assert( view->desc.ViewDimension == D3D12_SRV_DIMENSION_TEXTURE2D );

   // Create Render Target View
   result->colorDesc[0].Format        = view->desc.Format;
   result->colorDesc[0].ViewDimension = static_cast<D3D12_RTV_DIMENSION>(view->desc.ViewDimension);
   
   // D3D12 is not allowing reinterpretation of resource type
   // when using it for rendering (as there is no ViewDimmension field)
   TextureType type = view->texture->state.type;
   assert( type == view->viewType );
   
   fillRTV(view->viewType,
           view->mipmaps,
           view->layers,
           result->colorDesc[0]);

   // There is no MSAA resolve in this Render Pass
   assert( !resolve );

   if (depthStencil)
      {
      shared_ptr<TextureViewD3D12> view = _depthStencil ? dynamic_pointer_cast<TextureViewD3D12>(_depthStencil)
                                                        : dynamic_pointer_cast<TextureViewD3D12>(_stencil);
         
      // Keep reference to used Depth-Stencil resource
      result->depthHandle = view;

      // TODO: Verify those asserts, what about 1D, 1DArray? why forbide 3D, Cube, CubeArray?
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_UNKNOWN );
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_BUFFER );
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_TEXTURE3D );
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_TEXTURECUBE );
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_TEXTURECUBEARRAY );

      result->depthDesc.Format        = view->desc.Format;
      result->depthDesc.ViewDimension = static_cast<D3D12_DSV_DIMENSION>(static_cast<uint32>(view->desc.ViewDimension) + 1);
      result->depthDesc.Flags         = D3D12_DSV_FLAG_NONE;
      
      // Optimize texture usage
      if (!checkBitmask(underlyingType(view->texture->state.usage), underlyingType(TextureUsage::RenderTargetWrite)))
         {
         Format format = view->texture->state.format;
         
         if (isStencil(format))
            result->depthDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL;
         else
         if (isDepth(format))
            result->depthDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
         else
            result->depthDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH | D3D12_DSV_FLAG_READ_ONLY_STENCIL;
         }
      
      fillDSV(view->viewType,
              view->mipmaps,
              view->layers,
              result->depthDesc);
      }

   return result;
   }
   
   // Creates framebuffer for rendering to temporary MSAA that is then resolved directly to
   // window Swap-Chain surface.
   shared_ptr<Framebuffer> RenderPassD3D12::createFramebuffer(const uint32v2 resolution,
      const shared_ptr<TextureView> temporaryMSAA,
      const shared_ptr<TextureView> swapChainSurface,
      const shared_ptr<TextureView> _depthStencil,
      const shared_ptr<TextureView> _stencil)
   {
   shared_ptr<FramebufferD3D12> result = nullptr;

   assert( temporaryMSAA );
   assert( swapChainSurface );
   assert( _depthStencil == nullptr ||   // D3D12 is not supporting separate Depth and Stencil at the same time.
           _stencil      == nullptr );   // (but it supports Depth / DepthStencil or just Stencil setups).

   // Resolution cannot be greater than selected destination size (Swap-Chain surfaces have no mipmaps)
   shared_ptr<TextureViewD3D12> source      = dynamic_pointer_cast<TextureViewD3D12>(temporaryMSAA);
   shared_ptr<TextureViewD3D12> destination = dynamic_pointer_cast<TextureViewD3D12>(swapChainSurface);
   assert( source->texture->state.width  == destination->texture->state.width );
   assert( source->texture->state.height == destination->texture->state.height );
   assert( resolution.width  <= destination->texture->state.width );
   assert( resolution.height <= destination->texture->state.height );

   // Render Pass should use single Color Attachment #0
   assert( bitsCount(usedAttachments) == 1 );
   assert( checkBit(usedAttachments, 0) );

   result = make_shared<FramebufferD3D12>(resolution, 1);

   // Keep reference to used view
   result->colorHandle[0] = source;
   
   // Create Render Target View
   result->colorDesc[0].Format        = source->desc.Format;
   result->colorDesc[0].ViewDimension = static_cast<D3D12_RTV_DIMENSION>(source->desc.ViewDimension);
   
   // D3D12 is not allowing reinterpretation of resource type
   // when using it for rendering (as there is no ViewDimmension field)
   TextureType type = source->texture->state.type;
   assert( type == source->viewType );
   
   fillRTV(source->viewType,
           source->mipmaps,
           source->layers,
           result->colorDesc[0]);

   // It's Swap-Chain surface, thus it needs to be single 2D texture
   assert( destination->desc.ViewDimension == D3D12_SRV_DIMENSION_TEXTURE2D );

   // Keep references to resolve destinations
   assert( resolve );
   result->resolveHandle[0] = destination;

   if (depthStencil)
      {
      shared_ptr<TextureViewD3D12> view = _depthStencil ? dynamic_pointer_cast<TextureViewD3D12>(_depthStencil)
                                                        : dynamic_pointer_cast<TextureViewD3D12>(_stencil);
         
      // Keep reference to used Depth-Stencil resource
      result->depthHandle = view;

      // TODO: Verify those asserts, what about 1D, 1DArray? why forbide 3D, Cube, CubeArray?
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_UNKNOWN );
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_BUFFER );
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_TEXTURE3D );
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_TEXTURECUBE );
      assert( view->desc.ViewDimension != D3D12_SRV_DIMENSION_TEXTURECUBEARRAY );

      result->depthDesc.Format        = view->desc.Format;
      result->depthDesc.ViewDimension = static_cast<D3D12_DSV_DIMENSION>(static_cast<uint32>(view->desc.ViewDimension) + 1);
      result->depthDesc.Flags         = D3D12_DSV_FLAG_NONE;
      
      // Optimize texture usage
      if (!checkBitmask(underlyingType(view->texture->state.usage), underlyingType(TextureUsage::RenderTargetWrite)))
         {
         Format format = view->texture->state.format;
         
         if (isStencil(format))
            result->depthDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL;
         else
         if (isDepth(format))
            result->depthDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
         else
            result->depthDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH | D3D12_DSV_FLAG_READ_ONLY_STENCIL;
         }
      
      fillDSV(view->viewType,
              view->mipmaps,
              view->layers,
              result->depthDesc);
      }

   return result;
   }

   
   // DEVICE
   //////////////////////////////////////////////////////////////////////////


   shared_ptr<ColorAttachment> Direct3D12Device::createColorAttachment(
      const Format format,
      const uint32 samples)
   {
   return make_shared<ColorAttachmentD3D12>(format, samples);
   }

   shared_ptr<DepthStencilAttachment> Direct3D12Device::createDepthStencilAttachment(
      const Format depthFormat,
      const Format stencilFormat,
      const uint32 samples)
   {
   return make_shared<DepthStencilAttachmentD3D12>(depthFormat, stencilFormat, samples);
   }



   // Creates render pass which's output goes to window framebuffer.
   // Swap-Chain surface may be destination of MSAA resolve operation.
   shared_ptr<RenderPass> Direct3D12Device::createRenderPass(
      const ColorAttachment& swapChainSurface,
      const DepthStencilAttachment* depthStencil)
   {
   shared_ptr<RenderPassD3D12> result = nullptr;
   
   // D3D12 doesn't support Render Passes, thus states of Color and
   // Depth-Stencil Attachment objects are stored in Render Pass
   // object, and used to emulate Rende Pass behavior when it is 
   // started and ended on Command Buffer.

   result = make_shared<RenderPassD3D12>();
   assert( result );

   // Single Color Attachment
   result->colorState[0] = reinterpret_cast<const ColorAttachmentD3D12&>(swapChainSurface).state;
   if (result->colorState[0].resolve)
      result->resolve = true;

   setBit(result->usedAttachments, 0);

   // Optional Depth-Stencil / Depth / Stencil
   if (depthStencil)
      {
      result->depthState = reinterpret_cast<const DepthStencilAttachmentD3D12*>(depthStencil)->state;
      result->depthStencil = true;
      }

   return result;
   }

   // Creates render pass. Entries in "color" array, match output
   // color attachment slots in Fragment Shader. Entries in this 
   // array may be set to nullptr, which means that given output
   // color attachment slot has no bound resource descriptor.
   shared_ptr<RenderPass> Direct3D12Device::createRenderPass(
      const uint32 attachments,
      const shared_ptr<ColorAttachment> color[],
      const DepthStencilAttachment* depthStencil)
   {
   shared_ptr<RenderPassD3D12> result = nullptr;
   
   assert( attachments < support.maxColorAttachments );

   // D3D12 doesn't support Render Passes, thus references to Color 
   // and Depth-Stencil Attachment objects are stored in Render Pass 
   // object, and used to emulate Rende Pass behavior when it is 
   // started and ended on Command Buffer.


   // TODO: Does D3D12 support below? (Vulkan does):
   //       Allows pass without any color and depth attachments.
   //       In such case, rasterization is still performed in width x height x layers x sampler space,
   //       but it is expected that results will be outputted as a result of side effects operation.
   //
   // Metal is not supporting that !
   //
   // TODO: Make this method universal and internal. Provide common width/height/layers paarameters.
   //       There should be separate method for creating such RenderPass that accepts only those parameters.
   //

   result = make_shared<RenderPassD3D12>();
   assert( result );

   // Optional Color Attachments
   for(uint32 i=0; i<attachments; ++i)
      {
      // Passed in array of Color Attachment descriptors may have empty slots.
      if (color[i])
         {
         result->colorState[i] = reinterpret_cast<ColorAttachmentD3D12*>(color[i].get())->state;
         if (result->colorState[i].resolve)
            result->resolve = true;

         setBit(result->usedAttachments, i);
         }
      }

   // Optional Depth-Stencil / Depth / Stencil
   if (depthStencil)
      {
      result->depthState = reinterpret_cast<const DepthStencilAttachmentD3D12*>(depthStencil)->state;
      result->depthStencil = true;
      }

   return result;
   }



   }

}
#endif
