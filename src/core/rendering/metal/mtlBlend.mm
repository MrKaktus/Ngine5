/*

 Ngine v5.0
 
 Module      : Metal Blend State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlBlend.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/mtlDevice.h"
#include "core/rendering/state.h"
#include "utilities/utilities.h"

namespace en
{
   namespace gpu
   { 
   #define MaxColorAttachmentsCount 8

   // Metal doesn't support double source blending
   static const MTLBlendFactor TranslateBlend[underlyingType(BlendFactor::Count)] =
      {
      MTLBlendFactorZero                     ,  // Zero
      MTLBlendFactorOne                      ,  // One
      MTLBlendFactorSourceColor              ,  // Source
      MTLBlendFactorOneMinusSourceColor      ,  // OneMinusSource
      MTLBlendFactorDestinationColor         ,  // Destination
      MTLBlendFactorOneMinusDestinationColor ,  // OneMinusDestination
      MTLBlendFactorSourceAlpha              ,  // SourceAlpha
      MTLBlendFactorOneMinusSourceAlpha      ,  // OneMinusSourceAlpha
      MTLBlendFactorDestinationAlpha         ,  // DestinationAlpha
      MTLBlendFactorOneMinusDestinationAlpha ,  // OneMinusDestinationAlpha
      MTLBlendFactorBlendColor               ,  // ConstantColor
      MTLBlendFactorOneMinusBlendColor       ,  // OneMinusConstantColor
      MTLBlendFactorBlendAlpha               ,  // ConstantAlpha
      MTLBlendFactorOneMinusBlendColor       ,  // OneMinusConstantAlpha
      MTLBlendFactorOneMinusBlendAlpha       ,  // SourceAlphaSaturate
      MTLBlendFactorZero                     ,  // SecondSource
      MTLBlendFactorZero                     ,  // OneMinusSecondSource
      MTLBlendFactorZero                     ,  // SecondSourceAlpha
      MTLBlendFactorZero                        // OneMinusSecondSourceAlpha
      };

   static const MTLBlendOperation TranslateBlendOp[underlyingType(BlendOperation::Count)] =
      {
      MTLBlendOperationAdd                   ,  // Add
      MTLBlendOperationSubtract              ,  // Subtract
      MTLBlendOperationReverseSubtract       ,  // DestinationMinusSource
      MTLBlendOperationMin                   ,  // Min
      MTLBlendOperationMax                      // Max
      };

   // Metal is not supporting logical operation at all

   BlendStateMTL::BlendStateMTL(const BlendStateInfo& state,
                                const uint32 _attachments,
                                const BlendAttachmentInfo* color)
   {
   blendColor      = state.blendColor;
   attachments     = min(_attachments, static_cast<uint32>(MaxColorAttachmentsCount));
   
   blendInfo       = allocateObjectiveC(MTLRenderPipelineColorAttachmentDescriptorArray);
   for(uint8 i=0; i<attachments; ++i)
      {
      //assert( !(color[0].logicOperation && color[i].blending) );

      MTLRenderPipelineColorAttachmentDescriptor* desc = [blendInfo objectAtIndexedSubscript:i];
         
      [desc setBlendingEnabled:             color[i].mode == BlendMode::BlendOperation ? true : false ];
    //[desc pixelFormat];                   // Pixel Format is patched at Pipeline creation time
      [desc setSourceRGBBlendFactor:        TranslateBlend[underlyingType(color[i].srcRGB)]    ];
      [desc setDestinationRGBBlendFactor:   TranslateBlend[underlyingType(color[i].dstRGB)]    ];
      [desc setSourceAlphaBlendFactor:      TranslateBlend[underlyingType(color[i].srcAlpha)]  ];
      [desc setDestinationAlphaBlendFactor: TranslateBlend[underlyingType(color[i].dstAlpha)]  ];
      [desc setRgbBlendOperation:           TranslateBlendOp[underlyingType(color[i].rgb)]   ];
      [desc setAlphaBlendOperation:         TranslateBlendOp[underlyingType(color[i].alpha)] ];

      // TODO: What about color mask ????
      }
   }

   BlendStateMTL::~BlendStateMTL()
   {
   deallocateObjectiveC(blendInfo);
   }
   
   shared_ptr<BlendState> MetalDevice::createBlendState(const BlendStateInfo& state,
                                                        const uint32 attachments,
                                                        const BlendAttachmentInfo* color)
   {
   // We don't support Logic Operations for now
   // for(uint32 i=0; i<attachments; ++i)
   //    assert( !(color[0].logicOperation && color[i].blending) );
   
   return make_shared<BlendStateMTL>(state, attachments, color);
   }
   
   }
}
#endif
