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
#include "core/rendering/metal/mtlDevice.h"
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
      private:
      MetalDevice*      gpu;    // GPU owning this texture
      SurfaceDescriptor desc;   // Mapped surface description
      id <MTLTexture>   handle; // Metal sampler ID

      virtual void*    map(const uint8 mipmap, const uint16 surface);
      virtual bool     unmap(void);
         
      TextureMTL(const TextureState& state);
      virtual ~TextureMTL();
      };
   }
}
#endif

#endif