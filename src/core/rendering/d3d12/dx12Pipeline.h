/*

 Ngine v5.0
 
 Module      : D3D12 Pipeline.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_PIPELINE
#define ENG_CORE_RENDERING_D3D12_PIPELINE

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/pipeline.h"
#include "core/rendering/d3d12/dx12Layout.h"

namespace en
{
namespace gpu
{

class Direct3D12Device;

class PipelineD3D12 : public Pipeline
{
    public:
    Direct3D12Device* gpu;
    ID3D12PipelineState* handle;
    const PipelineLayoutD3D12* layout; // Reference to Pipeline Layout

    // Dynamic - Set on CommandBuffer
    FLOAT blendColor[4];
    D3D12_VIEWPORT viewport[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    D3D12_RECT     scissor[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    uint32 viewportsCount;  // Count of enabled states from the beginning of the array
    D3D_PRIMITIVE_TOPOLOGY topology;
    UINT stencilRef;
    uint32 bufferStride[MaxInputLayoutAttributesCount]; // Element size per bound buffer (passed to Pipeline, and cached on CommandBuffer when Pipeline is bound)
    uint32 buffersCount;

    PipelineD3D12(Direct3D12Device* gpu, ID3D12PipelineState* handle, const PipelineLayoutD3D12* layout);
    virtual ~PipelineD3D12();
};

} // en::gpu
} // en

#endif
#endif
