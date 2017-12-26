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

#ifndef ENG_CORE_RENDERING_BLEND_STATE
#define ENG_CORE_RENDERING_BLEND_STATE

#include "core/defines.h"
#include "core/types.h"

#include <memory>
using namespace std;

#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   {
   // Operation performed by blend stage
   enum class BlendMode : uint32
      {
      Off            = 0,
      BlendOperation = 1,
      LogicOperation = 2   // Currently unsupported (equals to Off), as Metal API doesn't support it
      };
      
   // Color Buffer blend factor
   enum BlendFactor
      {
      Zero                        = 0,
      One                            ,
      Source                         ,
      OneMinusSource                 ,
      Destination                    ,
      OneMinusDestination            ,
      SourceAlpha                    ,
      OneMinusSourceAlpha            ,
      DestinationAlpha               ,
      OneMinusDestinationAlpha       ,
      ConstantColor                  ,
      OneMinusConst                  ,
      ConstantAlpha                  ,
      OneMinusConstantAlpha          ,
      SourceAlphaSaturate            ,
      SecondSource                   ,
      OneMinusSecondSource           ,
      SecondSourceAlpha              ,
      OneMinusSecondSourceAlpha      ,
      BlendFunctionsCount
      };

   // Color Buffer blend operation
   enum BlendOperation
      {
      Add                         = 0,
      Subtract                       ,
      DestinationMinusSource         ,
      Min                            ,
      Max                            ,
      BlendEquationsCount
      };

   // Color Buffer logic operations (currently unsupported)
   enum LogicOperation
      {
      ClearDestination            = 0,
      Set                            ,
      Copy                           ,
      CopyInverted                   ,
      NoOperation                    ,
      Invert                         ,
      And                            ,
      NAnd                           ,
      Or                             ,
      Nor                            ,
      Xor                            ,
      Equiv                          ,
      AndReverse                     ,
      AndInverted                    ,
      OrReverse                      ,
      OrInverted                     ,
      LogicOperationsCount
      };

   // state.h
   enum ColorMask
      {
      ColorMaskRed       = 1,
      ColorMaskGreen     = 2,
      ColorMaskBlue      = 4,
      ColorMaskAlpha     = 8,
      ColorMaskAll       = 15
      };

   struct BlendStateInfo
      {
      float4 blendColor;
      uint32 enabledSamples; // Masks which samples in MSAA RT's should be affected by blending operation.

      BlendStateInfo();
      };

   // Default state:
   //
   // - mode           = BlendMode::Off
   // - srcRGB         = BlendFactor::One
   // - dstRGB         = BlendFactor::Zero
   // - rgbFunc        = BlendOperation::Add
   // - srcAlpha       = BlendFactor::One
   // - dstAlpha       = BlendFactor::Zero
   // - alphaFunc      = BlendOperation::Add
   // - logic          = LogicOperation::NoOperation (ignored, unsupported)
   // - writeMask      = ColorMask::ColorMaskAll
   //
   // Logic Operations are currently ignored as they are unsupported in Metal API.
   struct BlendAttachmentInfo
      {
      BlendMode      mode;
      BlendFactor    srcRGB;         // Blend parameters
      BlendFactor    dstRGB;
      BlendOperation rgb;
      BlendFactor    srcAlpha;
      BlendFactor    dstAlpha;
      BlendOperation alpha;
      LogicOperation logic;          // Logic operation
      ColorMask      writeMask;

      BlendAttachmentInfo();
      };

   class BlendState
      {
      public:
      virtual ~BlendState() {};                           // Polymorphic deletes require a virtual base destructor
      };
   }
}

#endif
