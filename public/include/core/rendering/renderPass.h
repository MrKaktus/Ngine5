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


#ifndef ENG_CORE_RENDERING_RENDER_PASS
#define ENG_CORE_RENDERING_RENDER_PASS

#include <memory>

#include "core/defines.h"
#include "core/types.h"

#include "core/rendering/state.h"
#include "core/rendering/texture.h"

namespace en
{
   namespace gpu
   {
   enum class LoadOperation : uint32
      {
      None               = 0,
      Load                  ,
      Clear                 ,
      Count
      };

   enum class StoreOperation : uint32
      {
      Discard            = 0,
      Store                 ,
      Resolve               ,
      StoreAndResolve       ,
      Count
      };

   enum class DepthResolve : uint32
      {
      Sample0            = 0, // Sample 0 value
      Min                   , // Minimum value from all Samples
      Max                   , // Maximum value from all Samples
      Count
      };
      
   class ColorAttachment
      {
      public:
      // On tiled renderers we can specify additional actions that should be
      // performed when render target is rebind. We can use the same actions on
      // immediate renderers. By default content of color attachment is not
      // changed. If attachment should be cleared it needs to be explicitly
      // stated by calling onLoad method with proper parameters.
      virtual void onLoad(
         const LoadOperation load,
         const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 1.0f)) = 0;
         
      virtual void onLoad(
         const LoadOperation load,
         const uint32v4 clearColor = uint32v4(0u, 0u, 0u, 1u)) = 0;
         
      virtual void onLoad(
         const LoadOperation load,
         const sint32v4 clearColor = sint32v4(0, 0, 0, 1)) = 0;
         
      // By default content of color attachment is stored.
      virtual void onStore(
         const StoreOperation store) = 0;

      virtual ~ColorAttachment() {};
      };

   // If you don't want to use Depth, nor Stencil surfaces, just
   // don't assign this object at all at Render Pass creation time.
   class DepthStencilAttachment
      {
      public:
      // By default content of depth and stencil is not changed.
      virtual void onLoad(
         const LoadOperation loadDepthStencil,
         const float  clearDepth = 1.0f,
         const uint32 clearStencil = 0u) = 0;
         
      // Specify store operation, and Depth resolve method if it's supported by the GPU.
      virtual void onStore(
         const StoreOperation storeDepthStencil,
         const DepthResolve resolveMode = DepthResolve::Sample0) = 0;
         
      // Custom load and store actions can be specifid for Stencil (no matter if
      // it's shared DepthStencil attachment, or if GPU support separate Depth
      // and Stencil attachments).
      virtual void onStencilLoad(
         const LoadOperation loadStencil) = 0;

      virtual void onStencilStore(
         const StoreOperation storeStencil) = 0;
         
      virtual ~DepthStencilAttachment() {};
      };

   class Framebuffer
      {
      public:
      virtual ~Framebuffer() {};
      };

   class RenderPass
      {
      public:
      
      // If one color attachment is being resolved as MSAA surface, all color
      // attachments in the Render Pass need to be resolved as well. Content of
      // destination surface for resolve operation is not modified in any way
      // before resolve operation occurs.
      virtual std::shared_ptr<Framebuffer> createFramebuffer(
         const uint32v2 resolution,
         const uint32   layers,
         const uint32   attachments,
         const std::shared_ptr<TextureView>* attachment,
         const std::shared_ptr<TextureView> depthStencil = nullptr,
         const std::shared_ptr<TextureView> stencil      = nullptr,
         const std::shared_ptr<TextureView> depthResolve = nullptr) = 0;

      // Creates framebuffer using window Swap-Chain surface.
      virtual std::shared_ptr<Framebuffer> createFramebuffer(
         const uint32v2 resolution,
         const std::shared_ptr<TextureView> swapChainSurface,
         const std::shared_ptr<TextureView> depthStencil = nullptr,
         const std::shared_ptr<TextureView> stencil      = nullptr) = 0;
      
      // Creates framebuffer for rendering to temporary MSAA that is then
      // resolved directly to window Swap-Chain surface. Depth-Stencil can still
      // be nullptr, but that need to be explicitly stated by the application
      // (to prevent ambiguous call).
      virtual std::shared_ptr<Framebuffer> createFramebuffer(
         const uint32v2 resolution,
         const std::shared_ptr<TextureView> temporaryMSAA,
         const std::shared_ptr<TextureView> swapChainSurface,
         const std::shared_ptr<TextureView> depthStencil,
         const std::shared_ptr<TextureView> stencil) = 0;
          
      virtual ~RenderPass() {};
      };
   }
}

#endif
