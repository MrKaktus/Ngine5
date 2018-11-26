/*

 Ngine v5.0
 
 Module      : Metal Multisampling State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlMultisampling.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/mtlDevice.h"
#include "utilities/utilities.h"

namespace en
{
   namespace gpu
   {
   #define MaxSamples 32u
   
   MultisamplingStateMTL::MultisamplingStateMTL(const uint32 _samples,
                                                const bool enableAlphaToCoverage,
                                                const bool enableAlphaToOne) :
      samples(min(static_cast<uint32>(nextPowerOfTwo(_samples)), static_cast<uint32>(MaxSamples))),  // TODO: Query MaxSamples from current Device in Run-Time
      alphaToCoverage(enableAlphaToCoverage),
      alphaToOne(enableAlphaToOne)
   {
   }
   
   std::shared_ptr<MultisamplingState> MetalDevice::createMultisamplingState(const uint32 samples,
                                                                        const bool enableAlphaToCoverage,
                                                                        const bool enableAlphaToOne)
   {
   return std::make_shared<MultisamplingStateMTL>(samples, enableAlphaToCoverage, enableAlphaToOne);
   }
   
   }
}

#endif
