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

#include "core/rendering/osxSurface.h"
#include "core/rendering/metal/metal.h"
#include "core/rendering/common/texture.h"
#include "core/rendering/common/device.h"
#include "core/rendering/metal/mtlHeap.h"
#include "threading/mutex.h"

namespace en
{
   namespace gpu
   {
   extern const MTLPixelFormat TranslateTextureFormat[underlyingType(Format::Count)];

   class TextureMTL : public CommonTexture
      {
      public:
      id<MTLTexture>        handle;      // Metal texture ID
      Ptr<HeapMTL>          heap;        // Memory backing heap
      Ptr<SharedSurfaceOSX> ioSurface;   // Texture object may own backing, but this backing may be in form of shared IOSurface
      bool                  ownsBacking; // Is this texture container the owner of backing surface (no for Swap-Chain surfaces)
      
      TextureMTL(const id memory,
                 const TextureState& state);
         
      TextureMTL(const id memory,
                 const TextureState& state,
                 const bool allocateBacking);
      
      // Creates Texture backed by IOSurface, that can be shared between processes.
      TextureMTL(const id<MTLDevice> device,
                 const Ptr<SharedSurface> backingSurface);
      
      virtual Ptr<Heap> parent(void) const;
      virtual bool read(uint8* buffer, const uint8 mipmap = 0, const uint16 surface = 0) const; // Reads texture mipmap to given buffer (app needs to allocate it)

      virtual Ptr<TextureView> view(void) const;
      virtual Ptr<TextureView> view(const TextureType type,
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
