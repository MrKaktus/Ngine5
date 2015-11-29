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

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/common/sampler.h"
#include "core/rendering/metal/metal.h"

namespace en
{
   namespace gpu
   {
   class SamplerMTL : public Sampler
      {
      public:
      id <MTLSamplerState> handle; // Metal handle
   
      SamplerMTL::SamplerMTL(const SamplerState& state);
      virtual ~SamplerMTL();
      };

#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   void InitSamplers(void);
#endif
   }
}
#endif

#endif