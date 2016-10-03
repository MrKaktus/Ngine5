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

#if defined(EN_PLATFORM_WINDOWS) 
#include "core/rendering/d3d12/dx12Blend.h"
#endif
#if defined(EN_PLATFORM_WINDOWS) 
#include "core/rendering/vulkan/vkBlend.h"
#endif


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
      blendColor(0.0f, 0.0f, 0.0f, 0.0f)
   {
   };

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
   };

#if defined(EN_PLATFORM_WINDOWS)
   Ptr<BlendState> Create(const BlendStateInfo& state,
                          const uint32 attachments, 
                          const BlendAttachmentInfo* color)
   {
// We don't support Logic Operations for now
// for(uint32 i=0; i<attachments; ++i)
//    assert( !(color[0].logicOperation && color[i].blending) );

   return ptr_dynamic_cast<BlendState, BlendStateD3D12>(new BlendStateD3D12(state, attachments, color));

   return ptr_dynamic_cast<BlendState, BlendStateVK>(new BlendStateVK(state, attachments, color));
   }
#endif

   }
}