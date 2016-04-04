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

#include "core/defines.h"
#include "core/types.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/mtlSampler.h"
#include "utilities/Nversion.h"

namespace en
{
   namespace gpu
   {
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME

#ifdef EN_DISCRETE_GPU

   // Last verified for Metal 2.0 and OSX 10.11
   static const Nversion TextureFilteringSupportedMTL[TextureFilteringMethodsCount] = 
      {
      Metal_OSX_1_0           ,   // Nearest
      Metal_OSX_1_0           ,   // NearestMipmaped
      Metal_OSX_1_0           ,   // NearestMipmapedSmooth
      Metal_OSX_1_0           ,   // Linear 
      Metal_OSX_1_0           ,   // Bilinear
      Metal_OSX_1_0           ,   // Trilinear    
      Metal_OSX_1_0           ,   // Anisotropic2x  
      Metal_OSX_1_0           ,   // Anisotropic4x  
      Metal_OSX_1_0           ,   // Anisotropic8x  
      Metal_OSX_1_0               // Anisotropic16x 
      };

   static const Nversion TextureWrapingSupportedMTL[TextureWrapingMethodsCount] = 
      {
      Metal_OSX_1_0           ,   // Clamp         
      Metal_OSX_1_0           ,   // Repeat        
      Metal_OSX_1_0           ,   // RepeatMirrored
      Metal_OSX_Unsupported   ,   // ClampMirrored 
      Metal_OSX_1_0               // ClampToBorder 
      };

#elif EN_MOBILE_GPU

   // Last verified for Metal 2.0 and iOS 9.0
   static const NVersion TextureFilteringSupportedMTL[TextureFilteringMethodsCount] = 
      {
      Metal_IOS_1_0           ,   // Nearest
      Metal_IOS_1_0           ,   // NearestMipmaped
      Metal_IOS_1_0           ,   // NearestMipmapedSmooth
      Metal_IOS_1_0           ,   // Linear 
      Metal_IOS_1_0           ,   // Bilinear
      Metal_IOS_1_0           ,   // Trilinear    
      Metal_IOS_1_0           ,   // Anisotropic2x  
      Metal_IOS_1_0           ,   // Anisotropic4x  
      Metal_IOS_1_0           ,   // Anisotropic8x  
      Metal_IOS_1_0               // Anisotropic16x 
      };

   static const NVersion TextureWrapingSupportedMTL[TextureWrapingMethodsCount] = 
      {
      Metal_IOS_1_0           ,   // Clamp         
      Metal_IOS_1_0           ,   // Repeat        
      Metal_IOS_1_0           ,   // RepeatMirrored
      Metal_IOS_Unsupported   ,   // ClampMirrored 
      Metal_IOS_1_0               // ClampToBorder 
      };
#endif

#endif

   struct TextureFilteringTranslation
      {
      MTLSamplerMinMagFilter filter;     // Metal magnification/minification filtering
      MTLSamplerMipFilter    mipmaping;  // Metal mipmap filtering
      float                  anisotropy; // Anisotropy ratio
      }; 

   static const TextureFilteringTranslation TranslateTextureFiltering[TextureFilteringMethodsCount] = 
      { // magFilter minFilter         mipFilter
      { MTLSamplerMinMagFilterNearest, MTLSamplerMipFilterNotMipmapped, 1.0f  },   // Nearest                                  
      { MTLSamplerMinMagFilterNearest, MTLSamplerMipFilterNearest,      1.0f  },   // NearestMipmaped              
      { MTLSamplerMinMagFilterNearest, MTLSamplerMipFilterLinear,       1.0f  },   // NearestMipmapedSmooth     
      { MTLSamplerMinMagFilterLinear,  MTLSamplerMipFilterNotMipmapped, 1.0f  },   // Linear                                 
      { MTLSamplerMinMagFilterLinear,  MTLSamplerMipFilterNearest,      1.0f  },   // Bilinear                     
      { MTLSamplerMinMagFilterLinear,  MTLSamplerMipFilterLinear,       1.0f  },   // Trilinear                                     
      { MTLSamplerMinMagFilterLinear,  MTLSamplerMipFilterLinear,       2.0f  },   // Anisotropic2x                
      { MTLSamplerMinMagFilterLinear,  MTLSamplerMipFilterLinear,       4.0f  },   // Anisotropic4x                
      { MTLSamplerMinMagFilterLinear,  MTLSamplerMipFilterLinear,       8.0f  },   // Anisotropic8x                
      { MTLSamplerMinMagFilterLinear,  MTLSamplerMipFilterLinear,       16.0f }    // Anisotropic16x               
      };

   static const MTLSamplerAddressMode TranslateTextureWraping[TextureWrapingMethodsCount] = 
      {
      MTLSamplerAddressModeClampToEdge ,  // Clamp
      MTLSamplerAddressModeRepeat      ,  // Repeat
      MTLSamplerAddressModeMirrorRepeat,  // RepeatMirrored               
      (MTLSamplerAddressMode)0         ,  // ClampMirrored
      MTLSamplerAddressModeClampToZero    // ClampToBorder    (for Metal border is always zero - "OpaqueBlack")
      };

   static const MTLCompareFunction TranslateCompareMethod[CompareMethodsCount] =
      {
      MTLCompareFunctionNever        ,   // Never
      MTLCompareFunctionAlways       ,   // Always
      MTLCompareFunctionLess         ,   // Less
      MTLCompareFunctionLessEqual    ,   // LessOrEqual
      MTLCompareFunctionEqual        ,   // Equal
      MTLCompareFunctionGreaterEqual ,   // GreaterOrEqual
      MTLCompareFunctionGreater      ,   // Greater
      MTLCompareFunctionNotEqual         // NotEqual
      };
      
   SamplerMTL::SamplerMTL(const MetalDevice* gpu, const SamplerState& _state)
   {
   //state = _state;

   MTLSamplerDescriptor* samplerInfo = [[MTLSamplerDescriptor alloc] init];
   samplerInfo.magFilter             = TranslateTextureFiltering[_state.filtering].filter;     //TranslateTextureFiltering[state.magFilter].filter;
   samplerInfo.minFilter             = TranslateTextureFiltering[_state.filtering].filter;     //TranslateTextureFiltering[state.minFilter].filter;
   samplerInfo.mipFilter             = TranslateTextureFiltering[_state.filtering].mipmaping;  //TranslateTextureFiltering[state.minFilter].mipmaping;
   samplerInfo.rAddressMode          = TranslateTextureWraping[_state.coordU];
   samplerInfo.sAddressMode          = TranslateTextureWraping[_state.coordV];
   samplerInfo.tAddressMode          = TranslateTextureWraping[_state.coordW];
   samplerInfo.maxAnisotropy         = min(TranslateTextureFiltering[_state.filtering].anisotropy, gpu->support.maxAnisotropy);
   samplerInfo.lodMinClamp           = _state.minLOD;
   samplerInfo.lodMaxClamp           = _state.maxLOD;
   samplerInfo.compareFunction       = TranslateCompareMethod[_state.depthCompare]; // IOS 9.0+ !
   samplerInfo.normalizedCoordinates = TRUE;

   handle = [gpu->device newSamplerStateWithDescriptor:samplerInfo];       // or getDevice()
   [samplerInfo release];

   // Metal vs OpenGL/Vulkan diff:
   //
   // - mipLodBias
   // - borderColor / Type
   // + normalizedCoordinates, effectively always TRUE

   assert( handle != nil );
   }

   SamplerMTL::~SamplerMTL()
   {
   [handle release];
   }

#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   void InitSamplers(const CommonDevice* gpu)
   {
   // Init array of currently supported filtering types
   for(uint16 i=0; i<TextureFilteringMethodsCount; ++i)
      {
      if (gpu->api.release >= TextureFilteringSupportedMTL[i].release)
         TextureFilteringSupported[i] = true;
      }

   // Init array of currently supported wrapping types
   for(uint16 i=0; i<TextureWrapingMethodsCount; ++i)
      {
      if (gpu->api.release >= TextureWrapingSupportedMTL[i].release)
         TextureWrapingSupported[i] = true;
      }
   }
#endif

   }
}

#endif