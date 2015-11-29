/*

 Ngine v5.0
 
 Module      : Viewport-Scissor State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#if defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/d3d12/dx12Viewport.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   { 
   ViewportStateD3D12::ViewportStateD3D12(const uint32 count, 
                                          const ViewportStateInfo* viewports,
                                          const ScissorStateInfo* scissors)
   {
   // TopLeftX = viewports[0].rect.x;
   // TopLeftY = viewports[0].rect.y;
   // Width    = viewports[0].rect.width;
   // Height   = viewports[0].rect.height;
   // MinDepth = viewports[0].depthRange.x;
   // MaxDepth = viewports[0].depthRange.y;

   assert( count <= 16 );
   memset(&viewport[0], 0, 16 * sizeof(D3D12_VIEWPORT));
   memset(&scissor[0],  0, 16 * sizeof(D3D12_RECT));

   memcpy(&viewport[0], &viewports[0], count * sizeof(ViewportStateInfo));
   memcpy(&scissor[0],  &scissors[0],  count * sizeof(ScissorStateInfo));
   }

   }
}
#endif












