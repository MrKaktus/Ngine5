/*

 Ngine v5.0
 
 Module      : Depth-Stencil State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#if defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/d3d12/dx12DepthStencil.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   { 
   static const D3D12_COMPARISON_FUNC TranslateCompareFunction[CompareMethodsCount] = 
      {
      D3D12_COMPARISON_FUNC_NEVER,         // Never              
      D3D12_COMPARISON_FUNC_ALWAYS,        // Always
      D3D12_COMPARISON_FUNC_LESS,          // Less
      D3D12_COMPARISON_FUNC_LESS_EQUAL,    // LessOrEqual
      D3D12_COMPARISON_FUNC_EQUAL,         // Equal
      D3D12_COMPARISON_FUNC_GREATER,       // Greater
      D3D12_COMPARISON_FUNC_GREATER_EQUAL, // GreaterOrEqual
      D3D12_COMPARISON_FUNC_NOT_EQUAL      // NotEqual
      };
   
   static const  D3D12_STENCIL_OP TranslateStencilOperation[StencilModificationsCount] =
      {
      D3D12_STENCIL_OP_KEEP,               // Keep
      D3D12_STENCIL_OP_ZERO,               // Clear
      D3D12_STENCIL_OP_REPLACE,            // Reference
      D3D12_STENCIL_OP_INCR_SAT,           // Increase
      D3D12_STENCIL_OP_INCR,               // IncreaseWrap
      D3D12_STENCIL_OP_DECR_SAT,           // Decrease
      D3D12_STENCIL_OP_DECR,               // DecreaseWrap
      D3D12_STENCIL_OP_INVERT              // InvertBits
      };

   DepthStencilStateD3D12::DepthStencilStateD3D12(const DepthStencilStateInfo& desc)
   {
   state.DepthEnable      = desc.enableDepthTest;
   state.DepthWriteMask   = desc.enableDepthWrite  ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
   state.DepthFunc        = TranslateCompareFunction[desc.depthFunction];
   state.StencilEnable    = desc.enableStencilTest;
   state.StencilReadMask  = desc.stencil[0].readMask;
   state.StencilWriteMask = desc.stencil[0].writeMask;
   for(uint8 i=0; i<2; ++i)
      {
      D3D12_DEPTH_STENCILOP_DESC& stencil = (i == 0) ? state.FrontFace : state.BackFace;

      stencil.StencilFailOp      = TranslateStencilOperation[desc.stencil[i].whenStencilFails];
      stencil.StencilDepthFailOp = TranslateStencilOperation[desc.stencil[i].whenDepthFails]; 
      stencil.StencilPassOp      = TranslateStencilOperation[desc.stencil[i].whenBothPass];
      stencil.StencilFunc        = TranslateCompareFunction[desc.stencil[i].function];
      }

   reference = static_cast<UINT>(desc.stencil[0].reference);
   }

   }
}
#endif