/*

 Ngine v5.0
 
 Module      : Multisample.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_MULTISAMPLING
#define ENG_CORE_RENDERING_METAL_MULTISAMPLING

#include "core/defines.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/metal.h"
#include "core/rendering/multisampling.h"

namespace en
{
   namespace gpu
   {
   class MultisamplingStateMTL : public MultisamplingState
      {
      public:
      NSUInteger samples;
      BOOL alphaToCoverage;
      BOOL alphaToOne;
      
      MultisamplingStateMTL(const uint32 samples,
                            const bool enableAlphaToCoverage,
                            const bool enableAlphaToOne);
         
      virtual ~MultisamplingStateMTL() {};
      };
   }
}
#endif

#endif
