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

namespace en
{
   namespace gpu
   {
   // COLOR ATTACHMENT
   //////////////////////////////////////////////////////////////////////////


   ColorAttachmentD3D12::ColorAttachmentD3D12(const Format _format, const uint32 _samples) :
      clearValue(),
      format(TranslateTextureFormat[underlyingType(_format)]),
      samples(_sampls),
      loadOp(Load),
      storeOp(Store),
      resolve(false)
   {
   }
   
   ColorAttachmentD3D12::~ColorAttachmentD3D12()
   {
   }

   void ColorAttachmentD3D12::onLoad(const LoadOperation load, const float4 _clearValue)
   {
   loadOp     = load;
   clearValue = _clearValue;
   }
   
   void ColorAttachmentD3D12::onLoad(const LoadOperation load, const uint32v4 _clearValue)
   {
   loadOp     = load;
   clearValue.r = reinterpret_cast<float>(_clearValue.x);
   clearValue.g = reinterpret_cast<float>(_clearValue.y);
   clearValue.b = reinterpret_cast<float>(_clearValue.z);
   clearValue.a = reinterpret_cast<float>(_clearValue.w);
   }
   
   void ColorAttachmentD3D12::onLoad(const LoadOperation load, const sint32v4 _clearValue)
   {
   loadOp     = load;
   clearValue.r = reinterpret_cast<float>(_clearValue.x);
   clearValue.g = reinterpret_cast<float>(_clearValue.y);
   clearValue.b = reinterpret_cast<float>(_clearValue.z);
   clearValue.a = reinterpret_cast<float>(_clearValue.w);
   }
   
   void ColorAttachmentD3D12::onStore(const StoreOperation store)
   {
   storeOp = TranslateStoreOperation[underlyingType(store)];

   if (store == StoreOperation::Resolve ||
       store == StoreOperation::StoreAndResolve)
      {
      // Source surface need to be MSAA to be later resolved
      assert( samples > 1 );
      resolve = true;
      }
   }


   // DEPTH-STENCIL ATTACHMENT
   //////////////////////////////////////////////////////////////////////////


   DepthState::DepthState(const uint32 _samples) :
      format(DXGI_FORMAT_UNKNOWN),
      samples(_sampls),
      clearFlags(0u),
      stencilLoadOp(Load),
      stencilStoreOp(Store),
      clearDepth(1.0f),
      clearStencil(0u)
   {
   }
   
   // If you don't want to use Depth, nor Stencil surfaces, just don't assign
   // this object at all at Render Pass creation time.
   DepthStencilAttachmentD3D12::DepthStencilAttachmentD3D12(const Format depthFormat,
                                                            const Format stencilFormat,
                                                            const uint32 _samples) :
      state(_samples)
   {
   // Direct3D12 doesn't support separate Depth and Stencil surfaces at the same time.
   assert( !(depthFormat   != Format::Unsupported && 
             stencilFormat != Format::Unsupported) );
   assert( depthFormat   != Format::Unsupported || 
           stencilFormat != Format::Unsupported );

   // Needs to be DepthStencil, Depth or Stencil
   assert( TextureFormatIsDepthStencil(depthFormat) ||
           TextureFormatIsDepth(depthFormat)        ||
           TextureFormatIsStencil(stencilFormat) );
      
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
      state.clearFlags = 0u;
      
   clearDepth   = _clearDepth;
   clearStencil = _clearStencil;
   }

   void DepthStencilAttachmentD3D12::onStore(const StoreOperation storeDepthStencil,
                                             const DepthResolve resolveMode)
   {
   storeOp = TranslateStoreOperation[underlyingType(storeDepthStencil)];

   // DepthStencil load and store operations can be different,
   // but by default we set them to the same operation.
   stencilStoreOp = storeOp;

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
   stencilStoreOp = TranslateStoreOperation[underlyingType(storeStencil)];
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
   }


   // RENDER PASS
   //////////////////////////////////////////////////////////////////////////


   RenderPassD3D12::RenderPassD3D12(const uint32 _usedAttachments,
                                    const uint32 _surfaces,
                                    const bool   _resolve,
                                    const bool   _depthStencil) :
      usedAttachments(_usedAttachments),
      surfaces(_surfaces),
      resolve(_resolve),
      depthStencil(_depthStencil)
   {

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
         
      case TextureType::TextureType::Texture3D:
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

   Ptr<Framebuffer> RenderPassD3D12::createFramebuffer(const uint32v2 resolution,
      const uint32   layers,
      const uint32   attachments, // Unnecesary ???
      const Ptr<TextureView>* attachment,
      const Ptr<TextureView> depthStencil,
      const Ptr<TextureView> stencil,
      const Ptr<TextureView> depthResolve)
   {
   Ptr<FramebufferD3D12> result = nullptr;

   uint32 attachments = bitsCount(usedAttachments);

   assert( layers >= 1 );
   assert( _depthStencil == nullptr ||   // D3D12 is not supporting separate Depth and Stencil at the same time.
           _stencil      == nullptr );   // (but it supports Depth / DepthStencil or just Stencil setups).
   assert( _depthResolve == nullptr );   // D3D12 is not supporting Depth resolve

   // Create Framebuffer object only if render pass usues any destination surfaces
   if (attachments == 0u && depthStencil == false)
      return Ptr<Framebuffer>(nullptr);

   result = new FramebufferD3D12(resolution, layers);
   
   uint32 index = 0;
   for(uint32 i=0; i<8; ++i)
      if (checkBit(usedAttachments))
         {
         // Keep reference to used view
         Ptr<TextureViewD3D12> view = ptr_reinterpret_cast<TextureViewD3D12>(&attachment[index]);
         result->colorHandle[i] = view;
         
         // Create Render Target View
         result->colorDesc[i].Format        = view->desc.Format;
         result->colorDesc[i].ViewDimension = view->desc.ViewDimension;

         // D3D12 is not allowing reinterpretation of resource type
         // when using it for rendering (as there is no ViewDimmension field)
         TextureType type = view->texture->state.type;
         assert( type == view->viewType );
         
         fillRTV(view->viewType,
                 view->viewMipmaps,
                 view->viewLayers,
                 result->colorDesc[i]);

         ++index;
         }
     
   // Keep references to resolve destinations
   if (resolve)
      for(uint32 i=0; i<8; ++i)
         if (checkBit(usedAttachments))
            {
            Ptr<TextureViewD3D12> view = ptr_reinterpret_cast<TextureViewD3D12>(&attachment[index]);
            result->resolveHandle[i] = view;
            ++index;
            }
      
   if (depthStencil)
      {
      Ptr<TextureViewD3D12> view = depthStencil ? ptr_reinterpret_cast<TextureViewD3D12>(&depthStencil)
                                                : ptr_reinterpret_cast<TextureViewD3D12>(&stencil);
         
      // Keep reference to used Depth-Stencil resource
      result->depthrHandle = view;
         
      result->depthDesc.Format        = view->desc.Format;
      result->depthDesc.ViewDimension = view->desc.ViewDimension;
      result->depthDesc.Flags         = D3D12_DSV_FLAG_NONE;
      
      // Optimize texture usage
      if (!checkBits(view->texture.state.usage, underlyingType(TextureUsage::RenderTargetWrite)))
         {
         TextureFormat format = view->texture.state.format;
         
         if (TextureFormatIsStencil(format))
            result->depthDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL;
         else
         if (TextureFormatIsDepth(state.format))
            result->depthDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
         else
            result->depthDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH | D3D12_DSV_FLAG_READ_ONLY_STENCIL;
         }
      
      fillDSV(view->viewType,
              view->viewMipmaps,
              view->viewLayers,
              result->depthDesc);
      }
      
   return ptr_reinterpret_cast<Framebuffer>(&result);
   }

      // Creates framebuffer using window Swap-Chain surface.
   Ptr<Framebuffer> RenderPassD3D12::createFramebuffer(const uint32v2 resolution,
      const Ptr<TextureView> swapChainSurface,
      const Ptr<TextureView> depthStencil,
      const Ptr<TextureView> stencil)
   {
   }
   
      // Creates framebuffer for rendering to temporary MSAA that is then resolved directly to
      // window Swap-Chain surface.
   Ptr<Framebuffer> RenderPassD3D12::createFramebuffer(const uint32v2 resolution,
      const Ptr<TextureView> temporaryMSAA,
      const Ptr<TextureView> swapChainSurface,
      const Ptr<TextureView> depthStencil,
      const Ptr<TextureView> stencil)
   {
   }

   // TODO: Framebuffer creation methods










   

   bool CommandBufferD3D12::startRenderPass(const Ptr<RenderPass> pass, const Ptr<Framebuffer>_framebuffer)
   {
   if (encoding)
      return false;

   assert( pass );
   assert( _framebuffer );
   
   RenderPassD3D12*  renderPass  = raw_reinterpret_cast<RenderPassD3D12>(&pass);
   FramebufferD3D12* framebuffer = raw_reinterpret_cast<FramebufferD3D12>(&_framebuffer);
   
   // TODO: Bind resource views to descriptors
   checkBit(renderPass->usedAttachments);
   
   // TODO: Init handleRTV[i] and handleDSV at device creation after Heap allocation
   D3D12_CPU_DESCRIPTOR_HANDLE firstHandleRTV = gpu->heapRTV->GetCPUDescriptorHandleForHeapStart();
 
 
   // Create new Views that bind resources to Descriptors on the fly
   uint32 lastUsedIndex = 0;
   uint32 index = 0;
   for(uint32 i=0; i<8; ++i)
      if (checkBit(renderPass->usedAttachments))
         {
         lastUsedIndex = i;
         ProfileNoRet( gpu, CreateRenderTargetView(framebuffer->colorHandle[i]->texture,
                                                  &framebuffer->colorDesc[i],
                                                   handleRTV[i]) )
         ++index;
         }
      else
         {
         // TODO: Clear binding.
         //       Do we need to clear binding, or is it enough that shader won't output anything on this slot?
         }
   
   // Create Depth-Stencil View
   if (renderPass->depthStencil)
      ProfileNoRet( gpu, CreateDepthStencilView(framebuffer->depthHandle->texture,
                                               &framebuffer->depthDesc,
                                                handleDSV[i]) )
   
   // RTV and DSV handles could be sourced from Framebuffer (if preallocated).
   
   // Clear Color Attachments
   // Engine currently doesn't support clearing sub-areas of render targets
   // (there are some uses for it like cinematic mode, but not all API's support it).
   for(uint32 i=0; i<=lastUsedIndex; ++i)
      if (checkBit(renderPass->usedAttachments))
         if (renderPass->colorState[i].loadOp == LoadOperation::Clear)
            ProfileNoRet( handle->ClearRenderTargetView(gpu->handleRTV[i],
                                                        reinterpret_cast<const FLOAT*>(&renderPass->colorState[i].clearValue),
                                                        0, nullptr) )
      
   // Clear Depth-Stencil Attachment
   if (renderPass->depthStencil)
      if (renderPass->depthState.clearFlags > 0)
         ProfileNoRet( handle->)ClearDepthStencilView(gpu->handleDSV,
                                                      renderPass->depthState.clearFlags,
                                                      renderPass->depthState.clearDepth,
                                                      renderPass->depthState.clearStencil,
                                                      0, nullptr) )

   // Always fill up all consecutive descriptors, from first one to
   // the last one that is used, thus TRUE can be passed in third parameter.
   ProfileNoRet( handle->OMSetRenderTargets((lastUsedIndex + 1), gpu->handleRTV, TRUE, gpu->handleDSV) )

   delete [] color;
   }
   

   // DEVICE
   //////////////////////////////////////////////////////////////////////////


   Ptr<ColorAttachment> Direct3D12Device::createColorAttachment(const Format format, const uint32 samples)
   {
   Ptr<ColorAttachmentDirect3D12> result = new ColorAttachmentVK(format, samples);
   return ptr_reinterpret_cast<ColorAttachment>(&result);
   }

   Ptr<DepthStencilAttachment> Direct3D12Device::createDepthStencilAttachment(const Format depthFormat,
                                                                              const Format stencilFormat,
                                                                              const uint32 samples)
   {
   Ptr<DepthStencilAttachmentD3D12> result = new DepthStencilAttachmentD3D12(depthFormat, stencilFormat, samples);
   return ptr_reinterpret_cast<DepthStencilAttachment>(&result);
   }

   
   }

}
#endif



// Descriptors:

//   class DescriptorsD3D12
//      {
//      ID3D12DescriptorHeap* handle;
//      
//      DescriptorsD3D12();
//      }
//
//   DescriptorsD3D12::DescriptorsD3D12() :
//      handle(nullptr)
//   {
//   }
//   
//   DescriptorsD3D12::~DescriptorsD3D12()
//   {
//   assert( handle );
//   handle->Release();
//   handle = nullptr;
//   }
//   
//
//   Ptr<Descriptors> Direct3D12Device::createDescriptors(const uint32 count)
//   {
//   Ptr<DescriptorsD3D12> result = nullptr;
//   
//   // Other types:
//   // D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
//   // D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
//   // D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
//   // D3D12_DESCRIPTOR_HEAP_TYPE_DSV
//  
//   D3D12_DESCRIPTOR_HEAP_DESC desc;
//   desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//   desc.NumDescriptors = count;
//   desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // None for RTV and DSV (otherwise D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
//   desc.NodeMask       = 0u;                              // TODO: Set bit to current GPU index
//
//   ID3D12DescriptorHeap* handle = nullptr;
//   Profile( this, CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap), IID_PPV_ARGS(&handle)) )
//   if (SUCCEEDED(lastResult[Scheduler.core()]))
//      result = new DescriptorD3D12(handle);
//      
//   return ptr_reinterpret_cast<Descriptors>(&result);
//   }
//
