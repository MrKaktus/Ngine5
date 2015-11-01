/*

 Ngine v5.0
 
 Module      : Blend State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/defines.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/mtlBlend.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   { 
   // Metal doesn't support double source blending
   static const MTLBlendFactor TranslateBlend[BlendFunctionsCount] =
      {
      MTLBlendFactorZero,                       // Zero
      MTLBlendFactorOne                         // One
      MTLBlendFactorSourceColor,                // Source
      MTLBlendFactorOneMinusSourceColor,        // OneMinusSource
      MTLBlendFactorDestinationColor,           // Destination
      MTLBlendFactorOneMinusDestinationColor,   // OneMinusDestination
      MTLBlendFactorSourceAlpha,                // SourceAlpha
      MTLBlendFactorOneMinusSourceAlpha,        // OneMinusSourceAlpha
      MTLBlendFactorDestinationAlpha,           // DestinationAlpha
      MTLBlendFactorOneMinusDestinationAlpha,   // OneMinusDestinationAlpha
      MTLBlendFactorBlendColor,                 // ConstantColor
      MTLBlendFactorOneMinusBlendColor,         // OneMinusConstantColor
      MTLBlendFactorBlendAlpha,                 // ConstantAlpha
      MTLBlendFactorOneMinusBlendColor,         // OneMinusConstantAlpha
      MTLBlendFactorOneMinusBlendAlpha,         // SourceAlphaSaturate
      MTLBlendFactorZero,                       // SecondSource
      MTLBlendFactorZero,                       // OneMinusSecondSource
      MTLBlendFactorZero,                       // SecondSourceAlpha
      MTLBlendFactorZero                        // OneMinusSecondSourceAlpha
      };

   static const MTLBlendOperation TranslateBlendFunc[BlendEquationsCount] =
      {
      MTLBlendOperationAdd,                     // Add
      MTLBlendOperationSubtract,                // Subtract
      MTLBlendOperationReverseSubtract,         // DestinationMinusSource
      MTLBlendOperationMin,                     // Min
      MTLBlendOperationMax                      // Max
      };

   // Metal is not supporting logical operation at all

   BlendStateMTL::BlendStateMTL(const BlendStateInfo& state,
                                const uint32 attachments, 
                                const BlendAttachmentInfo* color)
   {
   blendColor      = state.blendColor;

   for(uint8 i=0; i<min(attachments, MaxColorAttachmentsCount); ++i)
      {
      //assert( !(color[0].logicOperation && color[i].blending) );

      [blendInfo[i] setBlendingEnabled:             color[i].blending                      ];
    //[blendInfo[i] pixelFormat:                    /* color[i].format ???  */ ];  // Metal TODO: Pixel Format need to match the pixel format of color attachment !!
      [blendInfo[i] setSourceRGBBlendFactor:        TranslateBlend[color[i].srcRGB]        ];
      [blendInfo[i] setDestinationRGBBlendFactor:   TranslateBlend[color[i].dstRGB]        ];
      [blendInfo[i] setSourceAlphaBlendFactor:      TranslateBlendFunc[color[i].srcAlpha]  ];
      [blendInfo[i] setDestinationAlphaBlendFactor: TranslateBlend[color[i].dstAlpha]      ];
      [blendInfo[i] setRgbBlendOperation:           TranslateBlend[color[i].rgbFunc]       ];
      [blendInfo[i] setAlphaBlendOperation:         TranslateBlendFunc[color[i].alphaFunc] ];

      // TODO: What about color mask ????
      }
   }

   }
}
#endif