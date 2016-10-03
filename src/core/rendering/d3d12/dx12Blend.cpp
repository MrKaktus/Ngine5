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

#include "core/defines.h"

#if defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/d3d12/dx12Blend.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   { 
   // Constant Color and Constant Alpha is automaticaly selected from common "Blend Factor" so there is only one enum here.
   static const D3D12_BLEND TranslateBlend[BlendFunctionsCount] =
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
   static const D3D12_BLEND_OP TranslateBlendFunc[BlendEquationsCount] =
      {
      D3D12_BLEND_OP_ADD,                       // Add
      D3D12_BLEND_OP_SUBTRACT,                  // Subtract
      D3D12_BLEND_OP_REV_SUBTRACT,              // DestinationMinusSource
      D3D12_BLEND_OP_MIN,                       // Min
      D3D12_BLEND_OP_MAX                        // Max
      };

   // Logical Operation is alternative to blending (currently not supported)
   // Optimisation: This table is not needed. Backend type can be directly cast to D3D12 type.
   static const D3D12_LOGIC_OP TranslateLogicOperation[LogicOperationsCount] = 
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
   blendColor = state.blendColor;

   // To create default descriptor: desc = CD3D12_BLEND_DESC(CD3D12_DEFAULT)

   desc.AlphaToCoverageEnable  = false; // Unspecified, patch during Pipeline creation from Multisample State
   desc.IndependentBlendEnable = true;

   for(uint32 i=0; i<min(attachments, MaxColorAttachmentsCount); ++i)
      {
      desc.RenderTarget[i].BlendEnable           = color[i].blending;
      desc.RenderTarget[i].LogicOpEnable         = false;                                  // color[i].logicOperation;
      desc.RenderTarget[i].SrcBlend              = TranslateBlend[color[i].srcRGB];
      desc.RenderTarget[i].DestBlend             = TranslateBlend[color[i].dstRGB];
      desc.RenderTarget[i].BlendOp               = static_cast<D3D12_BLEND_OP>(underlyingType(color[i].rgbFunc) + 1);   // Optimisation of: TranslateBlendFunc[color[i].rgbFunc];
      desc.RenderTarget[i].SrcBlendAlpha         = TranslateBlend[color[i].srcAlpha];
      desc.RenderTarget[i].DestBlendAlpha        = TranslateBlend[color[i].dstAlpha];
      desc.RenderTarget[i].BlendOpAlpha          = static_cast<D3D12_BLEND_OP>(underlyingType(color[i].alphaFunc) + 1); // Optimisation of: TranslateBlendFunc[color[i].alphaFunc];
      desc.RenderTarget[i].LogicOp               = TranslateLogicOperation[NoOperation];   // TranslateLogicOperation[color[i].logic];
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
      // or for all: D3D12_COLOR_WRITE_ENABLE_ALL
      }
   }

   }
}

#endif
