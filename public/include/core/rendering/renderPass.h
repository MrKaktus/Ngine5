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

   class ColorAttachment : public SafeObject<ColorAttachment>
      {
      public:
      // On tiled renderers we can specify additional actions
      // that should be performed when render target is rebind. 
      // We can use the same actions on immediate renderers.
      virtual void onLoad(const LoadOperation load,
                          const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 1.0f)) = 0;

      virtual void onStore(const StoreOperation store) = 0;

      // "layer" parameter can pass specific information, for specific texture types:
      // - for 3D it represents "depth" slice
      // - for CubeMap it represents "face" surface
      // - for CubeMapArray it represents "layer-face" surface
      virtual bool resolve(const Ptr<Texture> texture, 
                           const uint32 mipmap = 0,
                           const uint32 layer = 0) = 0;

      virtual ~ColorAttachment() {};                  // Polymorphic deletes require a virtual base destructor
      };

   class DepthStencilAttachment : public SafeObject<DepthStencilAttachment>
      {
      public:
      virtual void onLoad(const LoadOperation loadDepthStencil,
                          const float  clearDepth = 0.0f,
                          const uint32 clearStencil = 0u) = 0;
         
      virtual void onStore(const StoreOperation storeDepthStencil) = 0;
      
      virtual bool resolve(const Ptr<Texture> depth,
                           const uint32 mipmap = 0u,
                           const uint32 layer = 0u) = 0;
         
      // If GPU supports separate Depth and Stencil atachments,
      // custom load and store actions, and MSAA resolve destination
      // can be specifid for Stencil.

      // DepthStencil shared attachment load and store operations can be different.
      virtual void onStencilLoad(const LoadOperation loadStencil) = 0;

      virtual void onStencilStore(const StoreOperation storeStencil) = 0;
      
      virtual bool resolveStencil(const Ptr<Texture> stencil,
                                  const uint32 mipmap = 0u,
                                  const uint32 layer = 0u) = 0;
         
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
