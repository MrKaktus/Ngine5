/*

 Ngine v5.0
 
 Module      : D3D12 Raster State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12Raster.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/state.h"
#include "core/rendering/d3d12/dx12Device.h"

namespace en
{
namespace gpu
{ 

static const D3D12_FILL_MODE TranslateFillMode[FillModesCount] =
{
    static_cast<const D3D12_FILL_MODE>(1), // Vertices
    D3D12_FILL_MODE_WIREFRAME,             // Wireframe
    D3D12_FILL_MODE_SOLID                  // Solid
};

static const D3D12_CULL_MODE TranslateCullingMethod[underlyingType(Face::Count)] = 
{      
    D3D12_CULL_MODE_FRONT,       // Front
    D3D12_CULL_MODE_BACK,        // Back
};

RasterStateD3D12::RasterStateD3D12(const RasterStateInfo& desc)
{
    // To create default descriptor: state = CD3D12_RASTERIZER_DESC(CD3D12_DEFAULT);

    state.FillMode              = TranslateFillMode[desc.fillMode];
    state.CullMode              = desc.enableCulling ? TranslateCullingMethod[underlyingType(desc.cullFace)] : D3D12_CULL_MODE_NONE;
    state.FrontCounterClockwise = desc.frontFace == CounterClockWise ? TRUE : FALSE;
    state.DepthBias             = static_cast<sint32>(desc.depthBiasConstantFactor);
    state.DepthBiasClamp        = desc.depthBiasClamp; 
    state.SlopeScaledDepthBias  = desc.depthBiasSlopeFactor;
    state.DepthClipEnable       = desc.enableDepthClamp ? FALSE : TRUE; // Enable Clamping by disabling Clipping.
    state.MultisampleEnable     = FALSE; // Set by Multisampling state.
    state.AntialiasedLineEnable = FALSE; // Should be set by Multisampling state (currently unsupported by engine)
    state.ForcedSampleCount     = 0u;    // Render MSAA without MSAA target, for e.g. for:
                                         // https://developer.nvidia.com/content/basics-gpu-voxelization
                                         // (currently unsupported by engine)

    // Conservative rasterization is currently unsupported.
  //state.ConservativeRaster    = desc.enableConservativeRasterization ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    state.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;   

    // Pass disableRasterizer down, to moment when Pipeline object is created
    disableRasterizer = desc.disableRasterizer;
}

RasterState* Direct3D12Device::createRasterState(const RasterStateInfo& state)
{
    return new RasterStateD3D12(state);
}
   
} // en::gpu
} // en

#endif
