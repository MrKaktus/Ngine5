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

namespace en
{
   namespace gpu
   {
   enum LoadOperation
      {
      DontLoad           = 0,
      Load                  ,
      Clear                 ,
      LoadOperationsCount
      };
   
   enum StoreOperation
      {
      Discard            = 0,
      Store                 ,
      ResolveMSAA           ,
      StoreOperationsCount
      };

   class ColorAttachment : public SafeObject
      {
      public:
      // On tiled renderers we can specify additional actions
      // that should be performed when render target is rebind. 
      // We can use the same actions on immediate renderers.
      virtual void onLoad(const LoadOperation load, 
                          const float4 clearColor = float4(0.0f, 0.0f, 0.0f, 0.0f)) = 0;

      virtual void onStore(const StoreOperation store) = 0;

      // "layer" parameter can pass specific information, for specific texture types:
      // - for 3D it represents "depth" slice
      // - for CubeMap it represents "face" surface
      // - for CubeMapArray it represents "layer-face" surface
      virtual bool resolve(const Ptr<Texture> texture, 
                           const uint32 mipmap = 0,
                           const uint32 layer = 0) = 0;

      virtual ~ColorAttachment();                     // Polymorphic deletes require a virtual base destructor
      };

   class DepthStencilAttachment : public SafeObject
      {
      public:
      virtual ~DepthStencilAttachment();              // Polymorphic deletes require a virtual base destructor
      };

   // When binding 3D texture, pass it's plane "depth" through "layer" parameter,
   // similarly when binding CubeMap texture, pass it's "face" through "layer".
   Ptr<ColorAttachment> Create(const Ptr<Texture> texture, 
                               const TextureFormat format,
                               const uint32 mipmap = 0,
                               const uint32 layer = 0,
                               const uint32 layers = 1);


   // Links texture resources with Render Pass attachment declarations
   class Framebuffer : public SafeObject
      {
      bool attach(const Ptr<Texture> texture, 
                  const uint32 mipmap = 0,
                  const uint32 layer = 0,
                  const uint32 layers = 1)
      };

   class RenderPass : public SafeObject
      {
      Ptr<Framebuffer> create(const uint32v2 resolution,
                              const uint32   layers = 1) = 0;          // Create resources binding group
      bool attach(Ptr<Framebuffer> fbo) = 0;  // Attach resources to use
      };

   Ptr<ColorAttachment> VulkanDevice::create(const TextureFormat format = FormatUnsupported, 
                                             const uint32 samples = 1);

   Ptr<RenderPass> VulkanDevice::create(const Ptr<ColorAttachment> color[MaxColorAttachmentsCount],
                                        const Ptr<DepthStencilAttachment> depthStencil);




   class RasterState : public SafeObject
      {
      public:
      virtual ~RasterState();                              // Polymorphic deletes require a virtual base destructor
      };
   }
}

#endif