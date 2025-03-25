/*

 Ngine v5.0
 
 Module      : D3D12 Blend State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_BLEND_STATE
#define ENG_CORE_RENDERING_D3D12_BLEND_STATE

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/blend.h"

namespace en
{
namespace gpu
{

class BlendStateD3D12 : public BlendState
{
    public:
    float4 blendColor;     // Dynamic - Set on CommandBuffer
    uint32 enabledSamples; // Static  - Set in PSO
    D3D12_BLEND_DESC desc;
    
    BlendStateD3D12(const BlendStateInfo& state,
                    const uint32 attachments, 
                    const BlendAttachmentInfo* color);
};

} // en::gpu
} // en
#endif

#endif
