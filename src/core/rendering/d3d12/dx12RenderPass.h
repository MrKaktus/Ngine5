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

#ifndef ENG_CORE_RENDERING_D3D12_RENDER_PASS
#define ENG_CORE_RENDERING_D3D12_RENDER_PASS

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/renderPass.h"
#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/d3d12/dx12Texture.h"

namespace en
{
   namespace gpu
   {
   struct ColorState
      {
      float4         clearValue;
      DXGI_FORMAT    format;     // Attachment and optional Resolve format (they must match)
      uint32         samples;
      LoadOperation  loadOp;
      StoreOperation storeOp;
      bool           resolve;

      ColorState();
      ColorState(const DXGI_FORMAT format, const uint32 samples);
      };
      
   struct DepthState
      {
      DXGI_FORMAT    format;        // Shared DepthStencil, Depth or Stencil
      uint32         samples;
      D3D12_CLEAR_FLAGS clearFlags;
      StoreOperation storeOp;
      StoreOperation stencilStoreOp;
      float          clearDepth;
      uint32         clearStencil;
      
      DepthState();
      DepthState(const uint32 samples);
      };
      
   class ColorAttachmentD3D12 : public ColorAttachment
      {
      public:
      ColorState state;

      ColorAttachmentD3D12(const Format format, const uint32 samples);

      virtual void onLoad(const LoadOperation load,
         const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 1.0f));
      virtual void onLoad(const LoadOperation load,
         const uint32v4 clearColor = uint32v4(0u, 0u, 0u, 1u));
      virtual void onLoad(const LoadOperation load,
         const sint32v4 clearColor = sint32v4(0, 0, 0, 1));
         
      virtual void onStore(const StoreOperation store);

      virtual ~ColorAttachmentD3D12();
      };

   class DepthStencilAttachmentD3D12 : public DepthStencilAttachment
      {
      public:
      DepthState state;

      DepthStencilAttachmentD3D12(const Format depthFormat,
                                  const Format stencilFormat = Format::Unsupported,
                                  const uint32 samples = 1u);

      virtual void onLoad(const LoadOperation loadDepthStencil,
                          const float  clearDepth = 1.0f,
                          const uint32 clearStencil = 0u);

      // Specify store operation, and Depth resolve method if it's supported by the GPU.
      virtual void onStore(const StoreOperation storeDepthStencil,
                           const DepthResolve resolveMode = DepthResolve::Sample0);

      virtual void onStencilLoad(const LoadOperation loadStencil);

      virtual void onStencilStore(const StoreOperation storeStencil);

      virtual ~DepthStencilAttachmentD3D12();
      };

   class FramebufferD3D12 : public Framebuffer
      {
      public:
      // Keep handles to resources, so that they won't be released
      // when their views are used by RenderPass.
      shared_ptr<TextureViewD3D12> colorHandle[8];
      shared_ptr<TextureViewD3D12> resolveHandle[8];
      shared_ptr<TextureViewD3D12> depthHandle;
      
      D3D12_RENDER_TARGET_VIEW_DESC colorDesc[8];
      D3D12_DEPTH_STENCIL_VIEW_DESC depthDesc;
      uint32v2 resolution;
      uint32   layers;
      
      FramebufferD3D12(const uint32v2 resolution,
                       const uint32 layers);
         
      virtual ~FramebufferD3D12();
      };

   class RenderPassD3D12 : public RenderPass
      {
      public:
      ColorState  colorState[8];
      DepthState  depthState;
      uint32      usedAttachments;  // Bitmask
      bool        resolve;
      bool        depthStencil;
      
      RenderPassD3D12();

      virtual ~RenderPassD3D12();

      virtual shared_ptr<Framebuffer> createFramebuffer(const uint32v2 resolution,
                                                        const uint32   layers,
                                                        const uint32   attachments,
                                                        const shared_ptr<TextureView>* attachment,
                                                        const shared_ptr<TextureView> depthStencil = nullptr,
                                                        const shared_ptr<TextureView> stencil      = nullptr,
                                                        const shared_ptr<TextureView> depthResolve = nullptr);

      // Creates framebuffer using window Swap-Chain surface.
      virtual shared_ptr<Framebuffer> createFramebuffer(const uint32v2 resolution,
                                                        const shared_ptr<TextureView> swapChainSurface,
                                                        const shared_ptr<TextureView> depthStencil = nullptr,
                                                        const shared_ptr<TextureView> stencil      = nullptr);
      
      // Creates framebuffer for rendering to temporary MSAA that is then resolved directly to
      // window Swap-Chain surface.
      virtual shared_ptr<Framebuffer> createFramebuffer(const uint32v2 resolution,
                                                        const shared_ptr<TextureView> temporaryMSAA,
                                                        const shared_ptr<TextureView> swapChainSurface,
                                                        const shared_ptr<TextureView> depthStencil = nullptr,
                                                        const shared_ptr<TextureView> stencil      = nullptr);
      };

   }
}
#endif

#endif
