/*

 Ngine v5.0
 
 Module      : Blend State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/defines.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/mtlMultisampling.h"
#include "utilities/utilities.h"
#include "core/rendering/metal/mtlDevice.h"

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
   
   Ptr<MultisamplingState> MetalDevice::create(const uint32 samples,
                            const bool enableAlphaToCoverage,
                            const bool enableAlphaToOne)
   {
   return ptr_dynamic_cast<MultisamplingState, MultisamplingStateMTL>(Ptr<MultisamplingStateMTL>(new MultisamplingStateMTL(samples, enableAlphaToCoverage, enableAlphaToOne)));
   }
   
   }
}

#endif
