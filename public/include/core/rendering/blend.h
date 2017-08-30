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
#include "core/utilities/TintrusivePointer.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   {
   #define MaxColorAttachmentsCount 8

   // Color Buffer blend functions
   enum BlendFunction
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

   // Color Buffer blend equations
   enum BlendEquation
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
   // - blending       = false
   // - logicOperation = false (unsupported)
   // - srcRGB         = BlendFunction::One
   // - dstRGB         = BlendFunction::Zero
   // - rgbFunc        = BlendEquation::Add
   // - srcAlpha       = BlendFunction::One
   // - dstAlpha       = BlendFunction::Zero
   // - alphaFunc      = BlendEquation::Add
   // - logic          = NoOperation (unsupported)
   // - writeMask      = ColorMask::ColorMaskAll
   //
   // Logic Operations are currently ignored as they are unsupported in Metal API.
   struct BlendAttachmentInfo
      {
      bool           blending;       // Enablers
      bool           logicOperation;
      BlendFunction  srcRGB;         // Blend parameters
      BlendFunction  dstRGB;
      BlendEquation  rgbFunc;
      BlendFunction  srcAlpha;
      BlendFunction  dstAlpha;
      BlendEquation  alphaFunc;
      LogicOperation logic;          // Logic operation
      ColorMask      writeMask;

      BlendAttachmentInfo();
      };

   class BlendState : public SafeObject<BlendState>
      {
      public:
      virtual ~BlendState() {};                           // Polymorphic deletes require a virtual base destructor
      };
   }
}

#endif
