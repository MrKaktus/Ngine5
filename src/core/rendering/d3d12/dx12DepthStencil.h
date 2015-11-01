/*

 Ngine v5.0
 
 Module      : D3D12 Depth-Stencil State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_DEPTH_STENCIL_STATE
#define ENG_CORE_RENDERING_D3D12_DEPTH_STENCIL_STATE

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/depthStencil.h"

namespace en
{
   namespace gpu
   {
   class DepthStencilStateD3D12 : public DepthStencilState
      {
      public:
      D3D12_DEPTH_STENCIL_DESC state;
      UINT reference;

      DepthStencilStateD3D12(const DepthStencilStateInfo& desc);
      };

   }
}

#endif