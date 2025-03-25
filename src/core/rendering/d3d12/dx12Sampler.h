/*

 Ngine v5.0
 
 Module      : D3D12 Sampler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_SAMPLER
#define ENG_CORE_RENDERING_D3D12_SAMPLER

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/sampler.h"

namespace en
{
namespace gpu
{

class SamplerD3D12 : public Sampler
{
    public:
    D3D12_SAMPLER_DESC state;
    D3D12_STATIC_BORDER_COLOR border;
   
    SamplerD3D12(D3D12_SAMPLER_DESC state, D3D12_STATIC_BORDER_COLOR border);
   ~SamplerD3D12();
};

} // en::gpu
} // en
#endif

#endif
