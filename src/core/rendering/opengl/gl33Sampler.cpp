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

#include "core/rendering/opengl/opengl.h"
#include "core/rendering/opengl/glState.h"
#include "core/rendering/opengl/gl33Sampler.h"

#include "core/rendering/context.h"
#include "core/rendering/rendering.h"

namespace en
{
   namespace gpu
   {
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME

#ifdef EN_DISCRETE_GPU

   // Last verified for OpenGL 4.5
   static const Nversion TextureFilteringSupportedGL[TextureFilteringMethodsCount] = 
      {
      OpenGL_1_0              ,   // Nearest
      OpenGL_1_0              ,   // NearestMipmaped
      OpenGL_1_0              ,   // NearestMipmapedSmooth
      OpenGL_1_0              ,   // Linear 
      OpenGL_1_0              ,   // Bilinear
      OpenGL_1_0              ,   // Trilinear   
      OpenGL_Unsupported      ,   // Anisotropic2x  
      OpenGL_Unsupported      ,   // Anisotropic4x  
      OpenGL_Unsupported      ,   // Anisotropic8x  
      OpenGL_Unsupported          // Anisotropic16x 
      };

   static const Nversion TextureWrapingSupportedGL[TextureWrapingMethodsCount] = 
      {
      OpenGL_1_0              ,   // Clamp         
      OpenGL_1_0              ,   // Repeat        
      OpenGL_1_4              ,   // RepeatMirrored
      OpenGL_4_4              ,   // ClampMirrored 
      OpenGL_1_3                  // ClampToBorder 
      };

#elif EN_MOBILE_GPU

   // Last verified for OpenGL ES 3.1
   static const Nversion TextureFilteringSupportedGL[TextureFilteringMethodsCount] = 
      {
      OpenGL_ES_1_0           ,   // Nearest
      OpenGL_ES_1_0           ,   // NearestMipmaped
      OpenGL_ES_1_0           ,   // NearestMipmapedSmooth
      OpenGL_ES_1_0           ,   // Linear 
      OpenGL_ES_1_0           ,   // Bilinear
      OpenGL_ES_1_0           ,   // Trilinear   
      OpenGL_ES_Unsupported   ,   // Anisotropic2x  
      OpenGL_ES_Unsupported   ,   // Anisotropic4x  
      OpenGL_ES_Unsupported   ,   // Anisotropic8x  
      OpenGL_ES_Unsupported       // Anisotropic16x 
      };

   static const Nversion TextureWrapingSupportedGL[TextureWrapingMethodsCount] = 
      {
      OpenGL_ES_1_0           ,   // Clamp         
      OpenGL_ES_1_0           ,   // Repeat        
      OpenGL_ES_2_0           ,   // RepeatMirrored
      OpenGL_ES_Unsupported   ,   // ClampMirrored 
      OpenGL_ES_Unsupported       // ClampToBorder 
      };
#endif

#endif

   static const TextureFilteringTranslation TranslateTextureFiltering[TextureFilteringMethodsCount] = 
      {// magFilter minFilter  mipFilter
      { GL_NEAREST, GL_NEAREST,                1.0f  },   // Nearest
      { GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST, 1.0f  },   // NearestMipmaped
      { GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR,  1.0f  },   // NearestMipmapedSmooth
      { GL_LINEAR,  GL_LINEAR,                 1.0f  },   // Linear 
      { GL_LINEAR,  GL_LINEAR_MIPMAP_NEAREST,  1.0f  },   // Bilinear
      { GL_LINEAR,  GL_LINEAR_MIPMAP_LINEAR,   1.0f  },   // Trilinear     
      { GL_LINEAR,  GL_LINEAR_MIPMAP_LINEAR,   2.0f  },   // Anisotropic2x  
      { GL_LINEAR,  GL_LINEAR_MIPMAP_LINEAR,   4.0f  },   // Anisotropic4x  
      { GL_LINEAR,  GL_LINEAR_MIPMAP_LINEAR,   8.0f  },   // Anisotropic8x  
      { GL_LINEAR,  GL_LINEAR_MIPMAP_LINEAR,   16.0f }    // Anisotropic16x 
      };

#ifdef EN_DISCRETE_GPU
           
   static const uint16 TranslateTextureWraping[TextureWrapingMethodsCount] = 
      {
      GL_CLAMP_TO_EDGE        ,   // Clamp                
      GL_REPEAT               ,   // Repeat               
      GL_MIRRORED_REPEAT      ,   // RepeatMirrored       
      GL_MIRROR_CLAMP_TO_EDGE ,   // ClampMirrored        
      GL_CLAMP_TO_BORDER          // ClampToBorder
      };

#elif EN_MOBILE_GPU
             
   static const uint16 TranslateTextureWraping[TextureWrapingMethodsCount] = 
      {
      GL_CLAMP_TO_EDGE        ,   // Clamp                
      GL_REPEAT               ,   // Repeat               
      GL_MIRRORED_REPEAT      ,   // RepeatMirrored       
      0                       ,   // ClampMirrored        
      0                           // ClampToBorder 
      };

#endif

   const float4 SamplerGL33::borderOpaqueBlack = float4(0.0, 0.0, 0.0, 1.0);
   const float4 SamplerGL33::borderOpaqueWhite = float4(1.0, 1.0, 1.0, 1.0);
   const float4 SamplerGL33::borderTransparentBlack = {0.0, 0.0, 0.0, 0.0};

   SamplerGL33::SamplerGL33(const SamplerState& state)
   {
   //state = _state;
 
   const float4* color = nullptr;
   if (state.borderColor == OpaqueBlack)
      color = &borderOpaqueBlack;
   if (state.borderColor == OpaqueWhite)
      color = &borderOpaqueWhite;
   if (state.borderColor == TransparentBlack)
      color = &borderTransparentBlack;

   Profile( glGenSamplers(1, &id) )
   Profile( glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, TranslateTextureFiltering[state.filtering].magnification)  )
   Profile( glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, TranslateTextureFiltering[state.filtering].minification)  )
   Profile( glSamplerParameteri(id, GL_TEXTURE_WRAP_S, TranslateTextureWraping[state.coordU]) )
   Profile( glSamplerParameteri(id, GL_TEXTURE_WRAP_T, TranslateTextureWraping[state.coordV]) )
   Profile( glSamplerParameteri(id, GL_TEXTURE_WRAP_R, TranslateTextureWraping[state.coordW]) )
   Profile( glSamplerParameterfv(id, GL_TEXTURE_BORDER_COLOR, reinterpret_cast<const GLfloat*>(color)) )
   Profile( glSamplerParameteri(id, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE) )  // GL_NONE
   Profile( glSamplerParameteri(id, GL_TEXTURE_COMPARE_FUNC, TranslateCompareMethod[state.depthCompare]) )
   Profile( glSamplerParameterf(id, GL_TEXTURE_LOD_BIAS, state.LODbias) )
   Profile( glSamplerParameterf(id, GL_TEXTURE_MIN_LOD, state.minLOD) )
   Profile( glSamplerParameterf(id, GL_TEXTURE_MAX_LOD, state.maxLOD) )

   if (GpuContext.support.extension(EXT_texture_filter_anisotropic))
      Profile( glSamplerParameterf(id, GL_TEXTURE_MAX_ANISOTROPY_EXT, min(TranslateTextureFiltering[state.filtering].anisotropy, GpuContext.support.maxAnisotropy)) )
   }

//   void SamplerGL33::filtering(const TextureFiltering filtering)
//   {
//   state.filtering = filtering;
//
//   Profile( glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, TranslateTextureFiltering[state.filtering].magnification)  )
//   Profile( glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, TranslateTextureFiltering[state.filtering].minification)  )
//   if (GpuContext.support.extension(EXT_texture_filter_anisotropic))
//      Profile( glSamplerParameterf(id, GL_TEXTURE_MAX_ANISOTROPY_EXT, min(TranslateTextureFiltering[state.filtering].anisotropy, GpuContext.support.maxAnisotropy)) )
//   }
//   
//   void SamplerGL33::wraping(const TextureWraping coordU, const TextureWraping coordV, const TextureWraping coordW)
//   {
//#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
//   assert( TextureWrapingSupported[state.coordU] );
//   assert( TextureWrapingSupported[state.coordV] );
//   assert( TextureWrapingSupported[state.coordW] );
//#endif
//
//   state.coordU = coordU;
//   state.coordV = coordV;
//   state.coordW = coordW;
//
//   Profile( glSamplerParameteri(id, GL_TEXTURE_WRAP_S, TranslateTextureWraping[state.coordU]) )
//   Profile( glSamplerParameteri(id, GL_TEXTURE_WRAP_T, TranslateTextureWraping[state.coordV]) )
//   Profile( glSamplerParameteri(id, GL_TEXTURE_WRAP_R, TranslateTextureWraping[state.coordW]) )
//   }
//
//   void SamplerGL33::border(const enum TextureBorder color)
//   {
//   state.borderColor = color;
//
//   float* ptr = nullptr;
//   if (state.borderColor == OpaqueBlack)
//      ptr = &borderOpaqueBlack;
//   if (state.borderColor == OpaqueWhite)
//      ptr = &borderOpaqueWhite;
//   if (state.borderColor == TransparentBlack)
//      ptr = &borderTransparentBlack;
//   Profile( glSamplerParameterfv(id, GL_TEXTURE_BORDER_COLOR, ptr) )
//   }
//
//   bool SamplerGL33::bias(const float bias)
//   {
//   state.LODbias = bias;
//   Profile( glSamplerParameterf(id, GL_TEXTURE_LOD_BIAS, state.LODbias) )
//   return true;
//   }
//
//   void SamplerGL33::minLOD(const float lod)
//   {
//   state.minLOD = lod;
//
//   Profile( glSamplerParameterf(id, GL_TEXTURE_MIN_LOD, state.minLOD) )
//   }
//
//   void SamplerGL33::minLOD(const float lod)
//   {
//   state.maxLOD = lod;
//
//   Profile( glSamplerParameterf(id, GL_TEXTURE_MAX_LOD, state.maxLOD) )
//   }

   SamplerGL33::~SamplerGL33()
   {
   Profile( glDeleteSamplers(1, &id) )
   }

#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   void InitSamplers(void)
   {
   // Init array of currently supported filtering types
   for(uint16 i=0; i<TextureFilteringMethodsCount; ++i)
      {
      if (GpuContext.screen.api.release >= TextureFilteringSupportedGL[i].release)
         TextureFilteringSupported[i] = true;
      else
      if (GpuContext.support.extension(EXT_texture_filter_anisotropic))
         {
         float anisotropy = TranslateTextureFiltering[i].anisotropy;
         if ((anisotropy > 0.0f) && (anisotropy <= GpuContext.support.maxAnisotropy))
            TextureFilteringSupported[i] = true;
         }
      }

   // Init array of currently supported wrapping types
   for(uint16 i=0; i<TextureWrapingMethodsCount; ++i)
      {
      if (GpuContext.screen.api.release >= TextureWrapingSupportedGL[i].release)
         TextureWrapingSupported[i] = true;
      }
   }
#endif

   }
}