/*

 Ngine v5.0
 
 Module      : D3D12 Multisampling State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_MULTISAMPLING
#define ENG_CORE_RENDERING_D3D12_MULTISAMPLING

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/multisampling.h"

namespace en
{
   namespace gpu
   {
   class MultisamplingStateD3D12 : public MultisamplingState
      {
      public:
      DXGI_SAMPLE_DESC state;
      bool alphaToCoverage;
      bool alphaToOne;
      
      MultisamplingStateD3D12(const uint32 samples,
                              const bool enableAlphaToCoverage,
                              const bool enableAlphaToOne);
         
      virtual ~MultisamplingStateD3D12() {};
      };
   }
}
#endif

#endif
