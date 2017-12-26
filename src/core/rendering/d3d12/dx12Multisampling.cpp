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
   state.Quality = 0; // D3D12_STANDARD_MULTISAMPLE_PATTERN

   // Multisampling Quality levels for given PixelFormat can be queried using:
   //
   // D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS desc;
   // desc.Format           = ; // Format of RenderTarget
   // desc.SampleCount      = ; // Samples count of RenderTarget
   // desc.Flags            = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE; // D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_TILED_RESOURCE
   // desc.NumQualityLevels = 0;  // Query for this number
   // 
   // Validate( this, CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &desc, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)) )
   }
   
   shared_ptr<MultisamplingState> Direct3D12Device::createMultisamplingState(const uint32 samples,
                                                                      const bool enableAlphaToCoverage,
                                                                      const bool enableAlphaToOne)
   {
   return make_shared<MultisamplingStateD3D12>(samples, enableAlphaToCoverage, enableAlphaToOne);
   }
   
   }
}

#endif
