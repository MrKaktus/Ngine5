/*

 Ngine v5.0
 
 Module      : OpenGL Texture.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_SAMPLER
#define ENG_CORE_RENDERING_SAMPLER

#include <memory>
using namespace std;

#include "core/types.h"

#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   {
   enum class SamplerFilter : uint32
      {
      Nearest                   = 0, // Nearest texel
      Linear                       , // Linear interpolation between nearest texels (or Bilinear?)
      Count
      };

   enum class SamplerMipMapMode : uint32
      {
      Nearest                   = 0, // Nearest MipMap
      Linear                       , // Linear interpolation between nearest MipMaps
      Count
      };

   enum class SamplerAdressing : uint32
      {
      Repeat                    = 0,
      RepeatMirrored               , 
      ClampToEdge                  , // Outside of texture space, use edge texel values
      ClampToBorder                , // Outside of texture space, use specific "border" values
      MirrorClampToEdge            , // Mirror once, then use edge texel values
      Count
      };

   enum class SamplerBorder : uint32
      {
      TransparentBlack          = 0,
      OpaqueBlack                  ,
      OpaqueWhite                  ,
      Count
      };

   enum class CompareOperation : uint32
      {
      Never                     = 0,
      Less                         ,
      Equal                        ,
      LessOrEqual                  ,
      Greater                      ,
      NotEqual                     ,
      GreaterOrEqual               ,
      Always                       ,
      Count                        ,
      };

   struct SamplerState
      {
      SamplerFilter     minification;  // Filtering during aliasing
      SamplerFilter     magnification; // Filtering during pixelation
      SamplerMipMapMode mipmap;        // Filtering between mipmaps
      float             anisotropy;    // Maximum anisotropy (clamped to maximum supported by the HW). Values smaller than 1.0f means anisotropy is disabled. 
      SamplerAdressing  coordU;
      SamplerAdressing  coordV;
      SamplerAdressing  coordW;
      SamplerBorder     borderColor;
      CompareOperation  compare;       // For Shadow maps sampling ??
      float             LodBias;       // 0.0f;
      float             minLod;        // 0.0f;        // Can force sampling of less detailed LOD levels, when texture is not fully loaded (less mip levels used saves GPU memory)
      float             maxLod;        // mipmaps.0f;  // Can force sampling of more detailed LOD levels

      SamplerState();
      };

   // Sampler is immutable after it is created.
   class Sampler
      {
      public:   
      virtual ~Sampler();                              // Polymorphic deletes require a virtual base destructor
      };
   }
}

#endif
