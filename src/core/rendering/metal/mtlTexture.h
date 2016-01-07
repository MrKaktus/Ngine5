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

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/metal.h"
#include "core/rendering/common/texture.h"
#include "core/rendering/common/device.h"
#include "threading/mutex.h"

namespace en
{
   namespace gpu
   {
   struct SurfaceDescriptor
      {
      Nmutex lock;    // Locks this texture instance, to prevent it from beeing modified by other thread while it is mapped  
      uint16 mipmap;  // MipMap level
      uint16 layer;   // MipMap depth slice / CubeMap face / Array layer / CubeMapArray face-layer
      void*  ptr;     // Pointer to local memory
      
      SurfaceDescriptor();
      };

   class TextureMTL : public TextureCommon
      {
      public:
      id<MTLDevice>     device; // GPU owning this texture
      SurfaceDescriptor desc;   // Mapped surface description
      id <MTLTexture>   handle; // Metal sampler ID

      virtual void*    map(const uint8 mipmap, const uint16 surface);
      virtual bool     unmap(void);
      virtual bool     read(uint8* buffer, const uint8 mipmap = 0, const uint16 surface = 0) const; // Reads texture mipmap to given buffer (app needs to allocate it)

      TextureMTL(const id<MTLDevice> _device, const TextureState& state);
      TextureMTL(const id<MTLDevice> _device, const TextureState& state, const bool allocateBacking);
      virtual ~TextureMTL();
      };
   }
}
#endif

#endif