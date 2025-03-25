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

extern const MTLTextureType TranslateTextureType[underlyingType(TextureType::Count)];
extern const MTLPixelFormat TranslateTextureFormat[underlyingType(Format::Count)];

class TextureMTL : public CommonTexture
{
    public:
    id<MTLTexture> handle;      // Metal texture ID
    HeapMTL*       heap;        // Memory backing heap
    std::shared_ptr<SharedSurfaceOSX> ioSurface;   // Texture object may own backing, but this backing may be in form of shared IOSurface
    bool           ownsBacking; // Is this texture container the owner of backing surface (no for Swap-Chain surfaces)
    
    TextureMTL(const id<MTLHeap> heap,
               const TextureState& state);
       
    TextureMTL(const id<MTLHeap> heap,
               const TextureState& state,
               const bool allocateBacking);
    
    // Creates Texture backed by IOSurface, that can be shared between processes.
    TextureMTL(const id<MTLDevice> device,
               const std::shared_ptr<SharedSurface> backingSurface);
    
    virtual Heap*        parent(void) const;
    virtual TextureView* view(void);
    virtual TextureView* view(const TextureType type,
                         const Format format,
                         const uint32v2 mipmaps,
                         const uint32v2 layers);
                          
    virtual ~TextureMTL();
};
      
class TextureViewMTL : public CommonTextureView
{
    public:
    TextureMTL&    texture; // Parent texture
    id<MTLTexture> handle;  // Vulkan Image View ID

    TextureViewMTL(TextureMTL&       parent,
                   id<MTLTexture>    view,
                   const TextureType type,
                   const Format      format,
                   const uint32v2    mipmaps,
                   const uint32v2    layers);
       
    Texture& parent(void) const;
 
    virtual ~TextureViewMTL();
};

} // en::gpu
} // en
#endif

#endif
