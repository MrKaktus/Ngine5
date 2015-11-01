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
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   bool TextureFilteringSupported[TextureFilteringMethodsCount] = 
      {
      false, // Nearest
      false, // NearestMipmaped
      false, // NearestMipmapedSmooth
      false, // Linear 
      false, // Bilinear
      false, // Trilinear    
      false, // Anisotropic2x  
      false, // Anisotropic4x  
      false, // Anisotropic8x  
      false, // Anisotropic16x 
      };

   bool TextureWrapingSupported[TextureWrapingMethodsCount] = 
      {
      false, // Clamp         
      false, // Repeat        
      false, // RepeatMirrored
      false, // ClampMirrored 
      false  // ClampToBorder 
      };
#endif

   SamplerState::SamplerState() :
      filtering(Linear),                   // NearestMipmaped will be the fastest one if texture has mip-maps, otherwise Nearest is the fastest
      coordU(Clamp),
      coordV(Clamp),
      coordW(Clamp),
      borderColor(OpaqueBlack),
      depthCompare(LessOrEqual),   
      LODbias(0.0f),
      minLOD(-1000.0f),
      maxLOD(1000.0f) 
   {
   }

   Sampler::~Sampler()
   {
   }

   }
}