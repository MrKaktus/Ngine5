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

#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"
#include "core/rendering/state.h"

namespace en
{
   namespace gpu
   {
   // Texture Filtering
   enum TextureFiltering
      {
      Nearest                   = 0,
      NearestMipmaped              ,
      NearestMipmapedSmooth        ,
      Linear                       ,
      Bilinear                     ,
      Trilinear                    ,
      Anisotropic2x                ,
      Anisotropic4x                ,
      Anisotropic8x                ,
      Anisotropic16x               ,
      TextureFilteringMethodsCount
      };

   // Texture border mode
   enum TextureWraping
      {
      Clamp                     = 0,
      Repeat                       ,
      RepeatMirrored               ,
      ClampMirrored                ,
      ClampToBorder                ,    
      TextureWrapingMethodsCount
      };

   // Texture border color
   enum TextureBorder
      {
      OpaqueBlack                 = 0,
      OpaqueWhite                    ,
      TransparentBlack               ,
      TextureBorderColorsCount
      };

   struct SamplerState
      {
      TextureFiltering filtering;
      TextureWraping   coordU;
      TextureWraping   coordV;
      TextureWraping   coordW;
      TextureBorder    borderColor;
      CompareMethod    depthCompare;     // For Shadow maps sampling ??
      float            LODbias;          // 0.0f;
      float            minLOD;           // 0.0f;        // Can force sampling of less detailed LOD levels (less mip levels used saves GPU memory)
      float            maxLOD;           // mipmaps.0f;  // Can force sampling of more detailed LOD levels

      SamplerState();
      };

   class Sampler : public SafeObject
      {
      public:   
      // Sampler is immutable after it is created.
        
//      virtual void filtering(const TextureFiltering filtering) = 0;
//      virtual void wraping(const TextureWraping coordU, const TextureWraping coordV, const TextureWraping coordW = Repeat) = 0;
//      virtual void border(const enum TextureBorder color) = 0;
//      virtual bool bias(const float bias) = 0;
//      virtual void minLOD(const float lod) = 0;
//      virtual void minLOD(const float lod) = 0;

      virtual ~Sampler();                              // Polymorphic deletes require a virtual base destructor
      };
   }
}

#endif