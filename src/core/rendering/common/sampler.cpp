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

#include "core/rendering/common/sampler.h"

namespace en
{
   namespace gpu
   {
//#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
//   bool TextureWrapingSupported[TextureWrapingMethodsCount] = 
//      {
//      false, // Clamp         
//      false, // Repeat        
//      false, // RepeatMirrored
//      false, // ClampMirrored 
//      false  // ClampToBorder 
//      };
//#endif

   // TODO: What are the defaults in API's ?
   SamplerState::SamplerState() :
      minification(SamplerFilter::Linear),
      magnification(SamplerFilter::Linear),
      mipmap(SamplerMipMapMode::Linear),
      anisotropy(1),
      coordU(SamplerAdressing::ClampToEdge),
      coordV(SamplerAdressing::ClampToEdge),
      coordW(SamplerAdressing::ClampToEdge),
      borderColor(SamplerBorder::OpaqueBlack),
      compare(CompareOperation::Always),   // Disabled depth test, "LessOrEqual" typical depth test
      LodBias(0.0f),
      minLod(-1000.0f),
      maxLod(1000.0f) 
   {
   }

   Sampler::~Sampler()
   {
   }

   }
}