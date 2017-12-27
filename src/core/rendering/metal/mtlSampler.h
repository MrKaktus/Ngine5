/*

 Ngine v5.0
 
 Module      : Metal Sampler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_SAMPLER
#define ENG_CORE_RENDERING_METAL_SAMPLER

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/common/sampler.h"

namespace en
{
   namespace gpu
   {
   class MetalDevice;
   
   class SamplerMTL : public Sampler
      {
      public:
      id <MTLSamplerState> handle; // Metal handle
   
      SamplerMTL(const MetalDevice* gpu, const SamplerState& state);
      virtual ~SamplerMTL();
      };
   }
}
#endif

#endif
