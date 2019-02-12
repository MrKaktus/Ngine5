/*

 Ngine v5.0
 
 Module      : Pipeline.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_PIPELINE
#define ENG_CORE_RENDERING_PIPELINE

#include <memory>

#include "core/defines.h"
#include "core/types.h"

// For PipelineState 
#include <string>
#include "core/rendering/blend.h"
#include "core/rendering/depthStencil.h"
#include "core/rendering/inputLayout.h"
#include "core/rendering/layout.h"
#include "core/rendering/multisampling.h"
#include "core/rendering/raster.h"
#include "core/rendering/renderPass.h"
#include "core/rendering/shader.h"
#include "core/rendering/viewport.h"

namespace en
{
namespace gpu
{
// Cross API issues:
//
// Logic Operation:
// D3D12  - per Rendertarget
// Vulkan - shared
// Metal  - none
//
// Alpha To Coverage:
// D3D12  - Stored in Blend State
// Vulkan - Stored in Multisample State -> Multisample State choosed
// Metal  - Pipeline State
//
// Rasterization Enabler:
// D3D12  -
// Vulkan - Stored in Raster State
// Metal  - Pipeline State
//
// Samples count:
// D3D12  - Rasterizer State (ForcedSampleCount)
// Vulkan - Stored in Multisample State -> Multisample State choosed
// Metal  - Pipeline State
//
// Multisample Enable:
// D3D12  - Rasterizer Desc
// Vulkan - Multisample State
//
// Depth Bias:
// D3D12  - int
// Vulkan - float
// Metal  - none
//
// D3D12 decided not to support Points Fill Mode ?
// D3D12 has Depth "clip", Vulkan has "clamp"
//
// Add feature flags:
//
// struct Support
//    {
//    bool logicOperation;                  // Supports global logic operation appliced to all Render Targets
//    bool logicOperationPerRenderTarget;   // Supports local logic operation specified per Render Target
//    bool dualSourceBlending;              // Supports Dual-Source blending
//    } support;
//
// PSO Creation can take up to 500-700ms

/// Handle for Pipeline State Object binding specification
class Pipeline
{
    public:
    virtual ~Pipeline() {};
};

class GpuDevice;

/// Helper structure for Pipeline object creation.
struct PipelineState
{
    std::shared_ptr<RenderPass>     renderPass;
    std::shared_ptr<InputLayout>    inputLayout;
    ViewportState*                  viewportState;
    RasterState*                    rasterState;
    MultisamplingState*             multisamplingState;
    DepthStencilState*              depthStencilState;
    BlendState*                     blendState;
    std::shared_ptr<Shader>         shader[5];
    std::string                     function[5];
    std::shared_ptr<PipelineLayout> pipelineLayout;

    PipelineState();
    PipelineState(const PipelineState& src);
    PipelineState(GpuDevice& device);
};

} // en::gpu
} // en

#endif
