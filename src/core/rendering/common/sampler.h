/*

 Ngine v5.0
 
 Module      : Sampler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_COMMON_SAMPLER
#define ENG_CORE_RENDERING_COMMON_SAMPLER

#include "core/configuration.h"
#include "core/rendering/sampler.h"

namespace en
{
   namespace gpu
   {
   // Compressed Sampler State used as cache hash
   aligned(1)
   struct SamplerHash
      {
      uint32 mipmap        : 2;
      uint32 magnification : 2;
      uint32 minification  : 2;
      uint32 anisotropy    : 1;
      uint32 coordU        : 3;
      uint32 coordV        : 3;
      uint32 coordW        : 3;
      uint32 borderColor   : 2;
      uint32 depthCompare  : 3;
      uint32               : 11;
      float  bias;
      float  minLOD;
      float  maxLOD;
      };
   aligndefault

//#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
//   extern bool TextureFilteringSupported[TextureFilteringMethodsCount];
//   extern bool TextureWrapingSupported[TextureWrapingMethodsCount];
//#endif
   }
}

#endif