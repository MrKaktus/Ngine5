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

// Default blend states:
//
// Direct3D12:
//
// AlphaToCoverageEnable  - false
// IndependentBlendEnable - false
//
// D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT:
// 
//    blending - false
//    logicOperation - false
//    srcRGB    - One
//    dstRGB    - Zero
//    rgbFunc   - Add
//    srcAlpha  - One
//    dstAlpha  - Zero
//    alphaFunc - Add
//    logic     - NoOperation
//    writeMask - ColorMaskAll

// TODO: Rename to blend.cpp and move one level up when old interface will be completly removed

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
      blending(false),
    //logicOperation(false),
      srcRGB(One),
      dstRGB(Zero),
      rgbFunc(Add),
      srcAlpha(One),
      dstAlpha(Zero),
      alphaFunc(Add),
    //logic(NoOperation),
      writeMask(ColorMaskAll)
   {
   }

   }
}
