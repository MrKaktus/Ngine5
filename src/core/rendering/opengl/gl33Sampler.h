/*

 Ngine v5.0
 
 Module      : OpenGL Sampler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_OPENGL_33_SAMPLER
#define ENG_CORE_RENDERING_OPENGL_33_SAMPLER

#include "core/rendering/common/sampler.h"

#if defined(EN_MODULE_RENDERER_OPENGL)

namespace en
{
   namespace gpu
   {
   class SamplerGL33 : public Sampler
      {
      public:
      uint32 id; // OpenGL handle

      static const float4 borderOpaqueBlack;
      static const float4 borderOpaqueWhite;
      static const float4 borderTransparentBlack;

      SamplerGL33(const SamplerState& state);
      virtual ~SamplerGL33();
   
//      virtual void filtering(const TextureFiltering filtering);
//      virtual void wraping(const TextureWraping coordU, const TextureWraping coordV, const TextureWraping coordW = Repeat);
//      virtual void border(const enum TextureBorder color);
//      virtual bool bias(const float bias);
//      virtual void minLOD(const float lod);
//      virtual void minLOD(const float lod);
      };

   extern const uint16 TranslateSamplerMagnification[underlyingType(SamplerFilter::Count)];

#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   void InitSamplers(void);
#endif
   }
}
#endif

#endif