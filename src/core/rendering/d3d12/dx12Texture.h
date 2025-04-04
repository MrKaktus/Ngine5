/*

 Ngine v5.0
 
 Module      : D3D12 Texture.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_TEXTURE
#define ENG_CORE_RENDERING_D3D12_TEXTURE

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/sampler.h"
#include "core/rendering/common/texture.h"
#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/d3d12/dx12Heap.h"

namespace en
{
namespace gpu
{

extern const DXGI_FORMAT TranslateTextureFormat[underlyingType(Format::Count)];

extern UINT D3D12CalcSubresource(UINT MipSlice, UINT ArraySlice, UINT PlaneSlice, UINT MipLevels, UINT ArraySize);

class TextureD3D12 : public CommonTexture
{
    public:
    ID3D12Resource* handle;      // Vulkan Image ID
    HeapD3D12*      heap;        // Memory backing heap
    uint64          offset;      // Offset in the heap
    uint64          textureSize; // Texture total size in memory (all mips and layers)
    
    TextureD3D12(HeapD3D12* heap,
                 ID3D12Resource* handle,
                 uint64 offset,
                 uint64 size,
                 const TextureState& state);
                 
    TextureD3D12(Direct3D12Device* gpu,
                 const TextureState& state); // Create texture interface for texture that already exists
    
    virtual Heap* parent(void) const;
    virtual TextureView* view(void);
    virtual TextureView* view(const TextureType type,
                              const Format format,
                              const uint32v2 mipmaps,
                              const uint32v2 layers);
       
    virtual ~TextureD3D12();
};
      
class TextureViewD3D12 : public CommonTextureView
{
    public:
    TextureD3D12& texture;                // Parent texture
    D3D12_SHADER_RESOURCE_VIEW_DESC desc; // View descriptor
    
    TextureViewD3D12(TextureD3D12& parent,
                     const TextureType _type,
                     const Format _format,
                     const uint32v2 _mipmaps,
                     const uint32v2 _layers);
    
    Texture& parent(void) const;
    
    virtual ~TextureViewD3D12();
};

} // en::gpu
} // en

#endif
#endif
