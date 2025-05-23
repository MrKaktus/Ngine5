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

#include "core/rendering/d3d12/dx12Blend.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12Device.h"
#include "core/rendering/state.h"

#define MaxColorAttachmentsCount 8

namespace en
{
namespace gpu
{

// Constant Color and Constant Alpha is automaticaly selected from common "Blend Factor" so there is only one enum here.
static const D3D12_BLEND TranslateBlend[underlyingType(BlendFactor::Count)] =
{
    D3D12_BLEND_ZERO,                         // Zero
    D3D12_BLEND_ONE,                          // One
    D3D12_BLEND_SRC_COLOR,                    // Source
    D3D12_BLEND_INV_SRC_COLOR,                // OneMinusSource
    D3D12_BLEND_DEST_COLOR,                   // Destination
    D3D12_BLEND_INV_DEST_COLOR,               // OneMinusDestination
    D3D12_BLEND_SRC_ALPHA,                    // SourceAlpha
    D3D12_BLEND_INV_SRC_ALPHA,                // OneMinusSourceAlpha
    D3D12_BLEND_DEST_ALPHA,                   // DestinationAlpha
    D3D12_BLEND_INV_DEST_ALPHA,               // OneMinusDestinationAlpha
    D3D12_BLEND_BLEND_FACTOR,                 // ConstantColor
    D3D12_BLEND_INV_BLEND_FACTOR,             // OneMinusConstantColor
    D3D12_BLEND_BLEND_FACTOR,                 // ConstantAlpha
    D3D12_BLEND_INV_BLEND_FACTOR,             // OneMinusConstantAlpha
    D3D12_BLEND_SRC_ALPHA_SAT,                // SourceAlphaSaturate
    D3D12_BLEND_SRC1_COLOR,                   // SecondSource
    D3D12_BLEND_INV_SRC1_COLOR,               // OneMinusSecondSource
    D3D12_BLEND_SRC1_ALPHA,                   // SecondSourceAlpha
    D3D12_BLEND_INV_SRC1_ALPHA,               // OneMinusSecondSourceAlpha
};

// Optimisation: This table is not needed. Backend type can be directly cast to D3D12 type by adding 1.
static const D3D12_BLEND_OP TranslateBlendFunc[underlyingType(BlendOperation::Count)] =
{
    D3D12_BLEND_OP_ADD,                       // Add
    D3D12_BLEND_OP_SUBTRACT,                  // Subtract
    D3D12_BLEND_OP_REV_SUBTRACT,              // DestinationMinusSource
    D3D12_BLEND_OP_MIN,                       // Min
    D3D12_BLEND_OP_MAX                        // Max
};

// Logical Operation is alternative to blending (currently not supported)
// Optimisation: This table is not needed. Backend type can be directly cast to D3D12 type.
static const D3D12_LOGIC_OP TranslateLogicOperation[underlyingType(LogicOperation::Count)] = 
{
    D3D12_LOGIC_OP_CLEAR,                     // Clear
    D3D12_LOGIC_OP_SET,                       // Set
    D3D12_LOGIC_OP_COPY,                      // Copy
    D3D12_LOGIC_OP_COPY_INVERTED,             // CopyInverted
    D3D12_LOGIC_OP_NOOP,                      // NoOperation
    D3D12_LOGIC_OP_INVERT,                    // Invert
    D3D12_LOGIC_OP_AND,                       // And
    D3D12_LOGIC_OP_NAND,                      // NAnd
    D3D12_LOGIC_OP_OR,                        // Or
    D3D12_LOGIC_OP_NOR,                       // Nor
    D3D12_LOGIC_OP_XOR,                       // Xor
    D3D12_LOGIC_OP_EQUIV,                     // Equiv
    D3D12_LOGIC_OP_AND_REVERSE,               // AndReverse
    D3D12_LOGIC_OP_AND_INVERTED,              // AndInverted
    D3D12_LOGIC_OP_OR_REVERSE,                // OrReverse
    D3D12_LOGIC_OP_OR_INVERTED                // OrInverted
};

BlendStateD3D12::BlendStateD3D12(const BlendStateInfo& state,
                                 const uint32 attachments, 
                                 const BlendAttachmentInfo* color)
{
    // Pass-Through to Pipeline State Object
    blendColor     = state.blendColor;
    enabledSamples = state.enabledSamples;

    // To create default descriptor: desc = CD3D12_BLEND_DESC(CD3D12_DEFAULT);

    desc.AlphaToCoverageEnable  = false; // Unspecified, patch during Pipeline creation from Multisample State
    desc.IndependentBlendEnable = true;

    for(uint32 i=0; i<min(attachments, MaxColorAttachmentsCount); ++i)
    {
        desc.RenderTarget[i].BlendEnable           = color[i].mode == BlendMode::BlendOperation ? true : false;
        desc.RenderTarget[i].LogicOpEnable         = false;                                  // color[i].logicOperation;
        desc.RenderTarget[i].SrcBlend              = TranslateBlend[underlyingType(color[i].srcRGB)];
        desc.RenderTarget[i].DestBlend             = TranslateBlend[underlyingType(color[i].dstRGB)];
        desc.RenderTarget[i].BlendOp               = static_cast<D3D12_BLEND_OP>(underlyingType(color[i].rgb) + 1);        // Optimisation of: TranslateBlendFunc[color[i].rgb];
        desc.RenderTarget[i].SrcBlendAlpha         = TranslateBlend[underlyingType(color[i].srcAlpha)];
        desc.RenderTarget[i].DestBlendAlpha        = TranslateBlend[underlyingType(color[i].dstAlpha)];
        desc.RenderTarget[i].BlendOpAlpha          = static_cast<D3D12_BLEND_OP>(underlyingType(color[i].alpha) + 1);      // Optimisation of: TranslateBlendFunc[color[i].alpha];
        desc.RenderTarget[i].LogicOp               = TranslateLogicOperation[underlyingType(LogicOperation::NoOperation)]; // TranslateLogicOperation[color[i].logic];
        // Translate Color Write Mask
        desc.RenderTarget[i].RenderTargetWriteMask = static_cast<UINT8>(underlyingType(color[i].writeMask));
        // Optimisation of:
        //if (color[i].writeMask & ColorMaskRed)
        //   desc.RenderTarget[i].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_RED;
        //if (color[i].writeMask & ColorMaskGreen)
        //   desc.RenderTarget[i].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
        //if (color[i].writeMask & ColorMaskBlue)
        //   desc.RenderTarget[i].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
        //if (color[i].writeMask & ColorMaskAlpha)
        //   desc.RenderTarget[i].RenderTargetWriteMask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
        //
        // or for all: 
    }

    // Unused slots need to be set to default values
    for(uint32 i=attachments; i<MaxColorAttachmentsCount; ++i)
    {
        desc.RenderTarget[i].BlendEnable           = FALSE;
        desc.RenderTarget[i].LogicOpEnable         = FALSE;
        desc.RenderTarget[i].SrcBlend              = D3D12_BLEND_ONE;
        desc.RenderTarget[i].DestBlend             = D3D12_BLEND_ZERO;
        desc.RenderTarget[i].BlendOp               = D3D12_BLEND_OP_ADD;
        desc.RenderTarget[i].SrcBlendAlpha         = D3D12_BLEND_ONE;
        desc.RenderTarget[i].DestBlendAlpha        = D3D12_BLEND_ZERO;
        desc.RenderTarget[i].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
        desc.RenderTarget[i].LogicOp               = D3D12_LOGIC_OP_NOOP;
        desc.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    }
}

BlendState* Direct3D12Device::createBlendState(const BlendStateInfo& state,
                                               const uint32 attachments,
                                               const BlendAttachmentInfo* color)
{
    // We don't support Logic Operations for now
    // for(uint32 i=0; i<attachments; ++i)
    //    assert( !(color[0].logicOperation && color[i].blending) );

    return new BlendStateD3D12(state, attachments, color);
}

} // en::gpu
} // en

#endif
