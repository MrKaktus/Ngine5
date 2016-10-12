/*

 Ngine v5.0
 
 Module      : Metal Texture.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_TEXTURE
#define ENG_CORE_RENDERING_METAL_TEXTURE

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/common/texture.h"
#include "core/rendering/common/device.h"
#include "threading/mutex.h"

namespace en
{
   namespace gpu
   {
   class TextureMTL : public CommonTexture
      {
      public:
      id<MTLTexture> handle;  // Metal texture ID
      id<MTLBuffer>  staging; // Staging buffer to which texture surface data can be written
      Nmutex         lock;    // Locks this texture instance, to prevent it from beeing modified by other thread while it is mapped
      uint16         mipmap;  // Mapped mipmap
      uint16         layer;   // Mapped depth slice / cube face / array layer / cube array face-layer

      virtual bool     read(uint8* buffer, const uint8 mipmap = 0, const uint16 surface = 0) const; // Reads texture mipmap to given buffer (app needs to allocate it)

      TextureMTL(const id memory, const TextureState& state);
      TextureMTL(const id memory, const TextureState& state, const bool allocateBacking);

      Ptr<TextureView> view(const TextureType type,
                            const Format format,
                            const uint32v2 mipmaps,         
                            const uint32v2 layers) const;
                            
      virtual ~TextureMTL();
      };
      
   class TextureViewMTL : public CommonTextureView
      {
      public:
      Ptr<TextureMTL> texture; // Parent texture
      id<MTLTexture>  handle;  // Vulkan Image View ID

      TextureViewMTL(Ptr<TextureMTL>   parent,
                     id<MTLTexture>    view,
                     const TextureType type,
                     const Format      format,
                     const uint32v2    mipmaps,
                     const uint32v2    layers);
         
      Ptr<Texture> parent(void) const;
   
      virtual ~TextureViewMTL();
      };
   }
}
#endif

#endif
