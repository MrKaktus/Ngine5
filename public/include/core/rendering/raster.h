/*

 Ngine v5.0
 
 Module      : Raster State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/


#ifndef ENG_CORE_RENDERING_RASTER_STATE
#define ENG_CORE_RENDERING_RASTER_STATE

#include "core/defines.h"
#include "core/types.h"

#include "core/rendering/state.h"

namespace en
{
namespace gpu
{

/// Default state:
///
/// - enableCulling                   = false
/// - enableDepthBias                 = false
/// - enableDepthClamp                = false
/// - disableRasterizer               = false
/// - fillMode                        = Solid
/// - cullFace                        = Face::Back
/// - frontFace                       = ClockWise
/// - depthBiasConstantFactor         = 0.0f
/// - depthBiasClamp                  = 0.0f
/// - depthBiasSlopeFactor            = 0.0f
///
/// Enabling Depth Clamping (for e.g. for Stencil Shadows), is equivalent to
/// disabling fragment Depth Clipping in Z, so fragments outside 0 <= z <= w
/// range won't be discarded.
///
/// Currently not exposed features:
/// - Conservative Rasterization - supported only by D3D12 (needs adoption)
/// - lineWidth                  - supported only by Vulkan (deprecated)
/// - pointFadeThreshold         - old Fixed Function state (deprecated)
///
/// Supported in Shading Language:
/// - pointSize                  - D3D12 HLSL "PSIZE"
///                                Vulkan SPIR-V "PointSize"
///                                Metal SL [[point_size]] (0.125 to 255.0)
///
struct RasterStateInfo
{
    bool  enableCulling;                ///< Enables culling of primitives oriented towards camera with given face.
    bool  enableDepthBias;
    bool  enableDepthClamp;             ///< Disables cliping fragments outside 0 <= z <= w range.
    bool  disableRasterizer;
    FillMode fillMode;
    Face  cullFace;
    NormalCalculationMethod frontFace; ///< Method used to calculate Front Face Normal vector
    float depthBiasConstantFactor;     ///< Depth value added to given pixel's depth (prevents z-fighting on decals).
    float depthBiasClamp;              ///< More about depth bias and depth slopes:
    float depthBiasSlopeFactor;        ///< https://msdn.microsoft.com/en-us/library/windows/desktop/cc308048(v=vs.85).aspx

    RasterStateInfo();
};

class RasterState
{
    public:
    virtual ~RasterState() {};
};

} // en::gpu
} // en

#endif
