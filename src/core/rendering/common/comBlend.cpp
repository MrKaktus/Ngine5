/*

 Ngine v5.0
 
 Module      : Common Blend.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/blend.h"

// TODO: Rename to blend.cpp (or add com prefix to all common rendering files)

// Default blend states:
//
// Direct3D12:
//
// AlphaToCoverageEnable  - false
// IndependentBlendEnable - false
//
// D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT:
// 
//    blending       - false
//    logicOperation - false
//    srcRGB         - One
//    dstRGB         - Zero
//    rgbFunc        - Add
//    srcAlpha       - One
//    dstAlpha       - Zero
//    alphaFunc      - Add
//    logic          - NoOperation
//    writeMask      - ColorMaskAll
//
//
// Metal default state:
//
//    blendingEnabled             - NO
//    [Metal is not exposing Logic Operations]
//    sourceRGBBlendFactor        - MTLBlendFactorOne
//    destinationRGBBlendFactor   - MTLBlendFactorZero
//    rgbBlendOperation           - MTLBlendOperationAdd
//    sourceAlphaBlendFactor      - MTLBlendFactorOne
//    destinationAlphaBlendFactor - MTLBlendFactorZero
//    alphaBlendOperation         - MTLBlendOperationAdd
//    [Metal is not exposing Logic Operations]
//    writeMask                   - MTLColorWriteMaskAll
//
// Vulkan has no default state, it's specified at creation time.

namespace en
{
namespace gpu
{

BlendStateInfo::BlendStateInfo() :
    blendColor(0.0f, 0.0f, 0.0f, 0.0f),
    enabledSamples(0xFFFFFFFF)
{
}

// TODO: Compare with OpenGL, Vulkan, Metal default states
BlendAttachmentInfo::BlendAttachmentInfo() :
    mode(BlendMode::Off),
    srcRGB(BlendFactor::One),
    dstRGB(BlendFactor::Zero),
    rgb(BlendOperation::Add),
    srcAlpha(BlendFactor::One),
    dstAlpha(BlendFactor::Zero),
    alpha(BlendOperation::Add),
    logic(LogicOperation::NoOperation),
    writeMask(ColorMask::All)
{
}

} // en::gpu
} // en
