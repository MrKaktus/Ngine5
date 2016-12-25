/*

 Ngine v5.0
 
 Module      : D3D12 Input Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_INPUT_LAYOUT
#define ENG_CORE_RENDERING_D3D12_INPUT_LAYOUT

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/common/inputLayout.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   {
   class InputLayoutD3D12 : public InputLayout
      {
      public:
      D3D12_INPUT_LAYOUT_DESC state;
      DrawableType primitive;
      uint32 points;

      InputLayoutD3D12(const DrawableType primitiveType,
                          const uint32 controlPoints, 
                          const uint32 usedAttributes, 
                          const uint32 usedBuffers, 
                          const AttributeDesc* attributes,  
                          const BufferDesc* buffers);

      virtual ~InputLayoutD3D12();
      };

   }
}
#endif

#endif
