/*

 Ngine v5.0
 
 Module      : D3D12 Viewport-Scissor State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_VIEWPORT_STENCIL_STATE
#define ENG_CORE_RENDERING_D3D12_VIEWPORT_STENCIL_STATE

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/viewport.h"

namespace en
{
   namespace gpu
   {
   // In D3D12 this state is dynamic and allows setting up to 16 different viewports and scissor rects.
   // Used calls:
   //
   // ID3D12GraphicsCommandList::RSSetViewports
   // ID3D12GraphicsCommandList::RSSetScissorRects
   //
   class ViewportStateD3D12 : public ViewportState
      {
      public:
      D3D12_VIEWPORT viewport[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
      D3D12_RECT     scissor[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

      ViewportStateD3D12(const uint32 count, 
                         const ViewportStateInfo* viewports,
                         const ScissorStateInfo* scissors);
      };
   }
}
#endif

#endif
