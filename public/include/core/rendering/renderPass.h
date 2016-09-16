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

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"
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
   
   // Resolving is done through binding resolve texture
   enum class StoreOperation : uint32
      {
      Discard            = 0,
      Store                 ,
      Count
      };

   enum class DepthResolve : uint32
      {
      Sample0            = 0, // Sample 0 value
      Min                   , // Minimum value from all Samples
      Max                   , // Maximum value from all Samples
      Count
      };
      
   class ColorAttachment : public SafeObject<ColorAttachment>
      {
      public:
      // On tiled renderers we can specify additional actions
      // that should be performed when render target is rebind. 
      // We can use the same actions on immediate renderers.
      // By default content of color attachment is cleared to
      // r=0.0f g=0.0f b=0.0f a=1.0f.
      virtual void onLoad(const LoadOperation load,
                          const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 1.0f)) = 0;
      virtual void onLoad(const LoadOperation load,
                          const uint32v4 clearColor = uint32v4(0u, 0u, 0u, 1u)) = 0;
      virtual void onLoad(const LoadOperation load,
                          const sint32v4 clearColor = sint32v4(0, 0, 0, 1)) = 0;
         
      // By default content of color attachment is stored.
      virtual void onStore(const StoreOperation store) = 0;

      // If one color attachment is being resolved as MSAA surface,
      // all color attachments in the Render Pass need to be resolved
      // as well. Content of destination surface for resolve operation
      // is not modified in any way before resolve operation occurs.
      virtual bool resolve(const Ptr<TextureView> destination) = 0;

      virtual ~ColorAttachment() {};                  // Polymorphic deletes require a virtual base destructor
      };

   class DepthStencilAttachment : public SafeObject<DepthStencilAttachment>
      {
      public:
      // By default depth and stencil are cleared on load to 1.0f, and 0.
      virtual void onLoad(const LoadOperation loadDepthStencil,
                          const float  clearDepth = 1.0f,
                          const uint32 clearStencil = 0u) = 0;
         
      virtual void onStore(const StoreOperation storeDepthStencil) = 0;
      
      // Specify destination and method for depth resolve (if it's supported by the GPU)
      virtual bool resolve(const Ptr<TextureView> destination,
                           const DepthResolve mode = DepthResolve::Sample0) = 0;
         
      // Custom load and store actions can be specifid for Stencil.
      // (no matter if it's shared DepthStencil attachment, or
      //  if GPU support separate Depth and Stencil attachments).
      virtual void onStencilLoad(const LoadOperation loadStencil) = 0;

      virtual void onStencilStore(const StoreOperation storeStencil) = 0;
         
      virtual ~DepthStencilAttachment() {};           // Polymorphic deletes require a virtual base destructor
      };

   class RenderPass : public SafeObject<RenderPass>
      {
      public:
      virtual ~RenderPass() {};   // Polymorphic deletes require a virtual base destructor
      };
   }
}

#endif
