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

      BlendStateInfo();
      };

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

   class BlendState : public SafeObject
      {
      public:
      virtual ~BlendState() {};                           // Polymorphic deletes require a virtual base destructor
      };
   }
}

#endif