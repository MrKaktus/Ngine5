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

#include "core/rendering/d3d12/dx12DepthStencil.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   {
   // Optimisation: This table is not needed. Backend type can be directly cast to D3D12 type by adding 1.
   static const D3D12_COMPARISON_FUNC TranslateCompareFunction[CompareMethodsCount] = 
      {
      D3D12_COMPARISON_FUNC_NEVER,         // Never              
      D3D12_COMPARISON_FUNC_LESS,          // Less
      D3D12_COMPARISON_FUNC_EQUAL,         // Equal
      D3D12_COMPARISON_FUNC_LESS_EQUAL,    // LessOrEqual
      D3D12_COMPARISON_FUNC_GREATER,       // Greater
      D3D12_COMPARISON_FUNC_NOT_EQUAL,     // NotEqual
      D3D12_COMPARISON_FUNC_GREATER_EQUAL, // GreaterOrEqual
      D3D12_COMPARISON_FUNC_ALWAYS,        // Always
      };
   
   // Optimisation: This table is not needed. Backend type can be directly cast to D3D12 type by adding 1.
   static const  D3D12_STENCIL_OP TranslateStencilOperation[StencilModificationsCount] =
      {
      D3D12_STENCIL_OP_KEEP,               // Keep
      D3D12_STENCIL_OP_ZERO,               // Clear
      D3D12_STENCIL_OP_REPLACE,            // Reference
      D3D12_STENCIL_OP_INCR_SAT,           // Increase
      D3D12_STENCIL_OP_DECR_SAT,           // Decrease
      D3D12_STENCIL_OP_INVERT,             // InvertBits
      D3D12_STENCIL_OP_INCR,               // IncreaseWrap
      D3D12_STENCIL_OP_DECR                // DecreaseWrap
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

      stencil.StencilFailOp      = static_cast<D3D12_STENCIL_OP>(underlyingType(desc.stencil[i].whenStencilFails) + 1);  // Optimisation: TranslateStencilOperation[desc.stencil[i].whenStencilFails];
      stencil.StencilDepthFailOp = static_cast<D3D12_STENCIL_OP>(underlyingType(desc.stencil[i].whenDepthFails) + 1);    // Optimisation: TranslateStencilOperation[desc.stencil[i].whenDepthFails];
      stencil.StencilPassOp      = static_cast<D3D12_STENCIL_OP>(underlyingType(desc.stencil[i].whenBothPass) + 1);      // Optimisation: TranslateStencilOperation[desc.stencil[i].whenBothPass];
      stencil.StencilFunc        = static_cast<D3D12_COMPARISON_FUNC>(underlyingType(desc.stencil[i].function) + 1);     // Optimisation: TranslateCompareFunction[desc.stencil[i].function];
      }

   reference = static_cast<UINT>(desc.stencil[0].reference);
   }

   Ptr<DepthStencilState> Direct3D12Device::createDepthStencilState(const DepthStencilStateInfo& desc)
   {
   Ptr<DepthStencilStateD3D12> ptr = new DepthStencilStateD3D12(desc);
   return ptr_reinterpret_cast<DepthStencilState>(&ptr);
   }
   
   }
}
#endif
