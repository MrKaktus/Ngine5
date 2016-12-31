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

#include "core/rendering/d3d12/dx12Multisampling.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12Device.h"
#include "utilities/utilities.h"

namespace en
{
   namespace gpu
   {
   #define MaxSamples 32u
   
   MultisamplingStateD3D12::MultisamplingStateD3D12(const uint32 _samples,
                                                    const bool enableAlphaToCoverage,
                                                    const bool enableAlphaToOne) :
      alphaToCoverage(enableAlphaToCoverage),
      alphaToOne(enableAlphaToOne)
   {
   // TODO: Query MaxSamples from current Device in Run-Time
   // More about Quality levels:
   // https://msdn.microsoft.com/en-us/library/windows/desktop/ff476218(v=vs.85).aspx
   // Quality level can be also used to enable CSAA.
   state.Count   = min(static_cast<uint32>(nextPowerOfTwo(_samples)), static_cast<uint32>(MaxSamples));
   state.Quality = D3D12_STANDARD_MULTISAMPLE_PATTERN;
   }
   
   Ptr<MultisamplingState> Direct3D12Device::createMultisamplingState(const uint32 samples,
                                                                      const bool enableAlphaToCoverage,
                                                                      const bool enableAlphaToOne)
   {
   Ptr<MultisamplingStateD3D12> ptr = new MultisamplingStateD3D12(samples, enableAlphaToCoverage, enableAlphaToOne);
   return ptr_reinterpret_cast<MultisamplingState>(&ptr);
   }
   
   }
}

#endif
