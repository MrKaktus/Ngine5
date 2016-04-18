/*

 Ngine v5.0
 
 Module      : Metal Texture.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/defines.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/mtlTexture.h"
#include "utilities/utilities.h"

using namespace en;

namespace en
{
   namespace gpu
   {   
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME

#ifdef EN_DISCRETE_GPU
   static const Nversion TextureTypeSupportedMTL[underlyingType(TextureType::Count)] =
      {
      Metal_OSX_1_0                 ,   // Texture1D                 
      Metal_OSX_1_0                 ,   // Texture1DArray            
      Metal_OSX_1_0                 ,   // Texture2D                 
      Metal_OSX_1_0                 ,   // Texture2DArray            
    //Metal_OSX_Unsupported         ,   // Texture2DRectangle
      Metal_OSX_1_0                 ,   // Texture2DMultisample      
      Metal_OSX_Unsupported         ,   // Texture2DMultisampleArray 
      Metal_OSX_1_0                 ,   // Texture3D                 
    //Metal_OSX_Unsupported         ,   // TextureBuffer
      Metal_OSX_1_0                 ,   // TextureCubeMap            
      Metal_OSX_1_0                     // TextureCubeMapArray       
      };

#elif EN_MOBILE_GPU
   static const Nversion TextureTypeSupportedMTL[TextureTypesCount] =
      {
      Metal_IOS_1_0                 ,   // Texture1D                 
      Metal_IOS_1_0                 ,   // Texture1DArray            
      Metal_IOS_1_0                 ,   // Texture2D                 
      Metal_IOS_1_0                 ,   // Texture2DArray            
      Metal_IOS_Unsupported         ,   // Texture2DRectangle        
      Metal_IOS_1_0                 ,   // Texture2DMultisample      
      Metal_IOS_Unsupported         ,   // Texture2DMultisampleArray 
      Metal_IOS_1_0                 ,   // Texture3D                 
      Metal_IOS_Unsupported         ,   // TextureBuffer             
      Metal_IOS_1_0                 ,   // TextureCubeMap            
      Metal_IOS_Unsupported             // TextureCubeMapArray       
      };
   // TODO: Solve Metal versioning system (capabilities sets?)
   // [gpu family] [version] [os]
   // [os] -> can separate API's like with OpenGL/ES
   // [gpu family] [version] -> this needs to be translated somehow into single increasing version number that can be used for checking support 

   //  MTLFeatureSet_iOS_GPUFamily1_v1 // Metal 1.0 (since iOS 8.0) (Supported by all OSX)
   //  MTLFeatureSet_iOS_GPUFamily2_v1 // Metal 1.0 (since iOS 8.3?) + some additional features (like ASTC ?)
   //  MTLFeatureSet_iOS_GPUFamily1_v2 // Metal 2.0 (since iOS 9.0)
   //  MTLFeatureSet_iOS_GPUFamily2_v2 // Metal 2.0 (since iOS 9.0) + some additional features
   //  MTLFeatureSet_OSX_GPUFamily1_v1 // Metal 1.0 OSX

#endif

   struct TextureInfoMTL
      {
      Nversion supported;    // Supported as texture
      Nversion rendertarget; // Can be used as rendertarget
      };
     
#ifdef EN_DISCRETE_GPU


   // TODO: Populate these fields !
   // https://developer.apple.com/library/ios/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/MetalFeatureSetTables/MetalFeatureSetTables.html
   
   // Last verified for <???>
/*
   // Support of different texture capabilities in Metal specification
   // (this values can be later overriden in general capabilities table by specific extensions used by engine)
   static const TextureInfoMTL TextureCapabilitiesMTL[TextureFormatsCount] =
      {
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatUnsupported         
      OpenGL_3_0,         OpenGL_3_0,           // FormatR_8                 
      OpenGL_3_1,         OpenGL_Unsupported,   // FormatR_8_sn              
      OpenGL_3_0,         OpenGL_3_0,           // FormatR_8_u               
      OpenGL_3_0,         OpenGL_3_0,           // FormatR_8_s               
      OpenGL_3_0,         OpenGL_3_0,           // FormatR_16                
      OpenGL_3_1,         OpenGL_Unsupported,   // FormatR_16_sn             
      OpenGL_3_0,         OpenGL_3_0,           // FormatR_16_u              
      OpenGL_3_0,         OpenGL_3_0,           // FormatR_16_s              
      OpenGL_3_0,         OpenGL_3_0,           // FormatR_16_hf             
      OpenGL_3_0,         OpenGL_3_0,           // FormatR_32_u              
      OpenGL_3_0,         OpenGL_3_0,           // FormatR_32_s              
      OpenGL_3_0,         OpenGL_3_0,           // FormatR_32_f              
      OpenGL_3_0,         OpenGL_3_0,           // FormatRG_8                
      OpenGL_3_1,         OpenGL_Unsupported,   // FormatRG_8_sn             
      OpenGL_3_0,         OpenGL_3_0,           // FormatRG_8_u              
      OpenGL_3_0,         OpenGL_3_0,           // FormatRG_8_s              
      OpenGL_3_0,         OpenGL_3_0,           // FormatRG_16               
      OpenGL_3_1,         OpenGL_Unsupported,   // FormatRG_16_sn            
      OpenGL_3_0,         OpenGL_3_0,           // FormatRG_16_u             
      OpenGL_3_0,         OpenGL_3_0,           // FormatRG_16_s             
      OpenGL_3_0,         OpenGL_3_0,           // FormatRG_16_hf            
      OpenGL_3_0,         OpenGL_3_0,           // FormatRG_32_u             
      OpenGL_3_0,         OpenGL_3_0,           // FormatRG_32_s             
      OpenGL_3_0,         OpenGL_3_0,           // FormatRG_32_f             
      OpenGL_1_0,         OpenGL_Unsupported,   // FormatRGB_8   
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatRGB_8_sRGB              
      OpenGL_3_1,         OpenGL_Unsupported,   // FormatRGB_8_sn            
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatRGB_8_u             
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatRGB_8_s                     
      OpenGL_1_0,         OpenGL_Unsupported,   // FormatRGB_16              
      OpenGL_3_1,         OpenGL_Unsupported,   // FormatRGB_16_sn           
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatRGB_16_u            
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatRGB_16_s            
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatRGB_16_hf           
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatRGB_32_u            
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatRGB_32_s            
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatRGB_32_f    
      OpenGL_1_0,         OpenGL_3_0,           // FormatRGBA_8                      
      OpenGL_3_0,         OpenGL_3_0,           // FormatRGBA_8_sRGB
      OpenGL_3_1,         OpenGL_Unsupported,   // FormatRGBA_8_sn           
      OpenGL_3_0,         OpenGL_3_0,           // FormatRGBA_8_u            
      OpenGL_3_0,         OpenGL_3_0,           // FormatRGBA_8_s     
      OpenGL_1_0,         OpenGL_3_0,           // FormatRGBA_16             
      OpenGL_3_1,         OpenGL_Unsupported,   // FormatRGBA_16_sn          
      OpenGL_3_0,         OpenGL_3_0,           // FormatRGBA_16_u           
      OpenGL_3_0,         OpenGL_3_0,           // FormatRGBA_16_s           
      OpenGL_3_0,         OpenGL_3_0,           // FormatRGBA_16_hf          
      OpenGL_3_0,         OpenGL_3_0,           // FormatRGBA_32_u           
      OpenGL_3_0,         OpenGL_3_0,           // FormatRGBA_32_s           
      OpenGL_3_0,         OpenGL_3_0,           // FormatRGBA_32_f           
      OpenGL_1_4,         OpenGL_3_0,           // FormatD_16                
      OpenGL_1_4,         OpenGL_3_0,           // FormatD_24                  
      OpenGL_1_4,         OpenGL_3_0,           // FormatD_32
      OpenGL_3_0,         OpenGL_3_0,           // FormatD_32_f              
      OpenGL_4_4,         OpenGL_4_3,           // FormatS_8                 
      OpenGL_3_0,         OpenGL_3_0,           // FormatSD_8_24             
      OpenGL_3_0,         OpenGL_3_0,           // FormatSD_8_32_f                   
      OpenGL_1_2,         OpenGL_Unsupported,   // FormatRGB_5_6_5           
      OpenGL_1_2,         OpenGL_Unsupported,   // FormatBGR_5_6_5           
      OpenGL_1_2,         OpenGL_Unsupported,   // FormatBGR_8               
      OpenGL_3_0,         OpenGL_3_0,           // FormatBGR_10_11_11_f      
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatEBGR_5_9_9_9_f      
      OpenGL_1_2,         OpenGL_Unsupported,   // FormatBGR_16              
      OpenGL_1_2,         OpenGL_Unsupported,   // FormatRGBA_5_5_5_1        
      OpenGL_1_2,         OpenGL_Unsupported,   // FormatBGRA_5_5_5_1        
      OpenGL_1_2,         OpenGL_Unsupported,   // FormatARGB_1_5_5_5        
      OpenGL_1_2,         OpenGL_Unsupported,   // FormatABGR_1_5_5_5        
      OpenGL_1_2,         OpenGL_Unsupported,   // FormatABGR_8              
      OpenGL_1_2,         OpenGL_Unsupported,   // FormatARGB_8              
      OpenGL_1_2,         OpenGL_Unsupported,   // FormatBGRA_8              
      OpenGL_1_2,         OpenGL_3_0,           // FormatRGBA_10_10_10_2     
      OpenGL_3_3,         OpenGL_3_3,           // FormatRGBA_10_10_10_2_u   
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatBC1_RGB
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatBC1_RGB_sRGB
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatBC1_RGBA
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatBC1_RGBA_sRGB
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatBC2_RGBA_pRGB       
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatBC2_RGBA
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatBC2_RGBA_sRGB
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatBC3_RGBA_pRGB       
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatBC3_RGBA
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatBC3_RGBA_sRGB       
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatBC4_R               
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatBC4_R_sn            
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatBC5_RG              
      OpenGL_3_0,         OpenGL_Unsupported,   // FormatBC5_RG_sn           
      OpenGL_4_2,         OpenGL_Unsupported,   // FormatBC6H_RGB_f          
      OpenGL_4_2,         OpenGL_Unsupported,   // FormatBC6H_RGB_uf         
      OpenGL_4_2,         OpenGL_Unsupported,   // FormatBC7_RGBA            
      OpenGL_4_2,         OpenGL_Unsupported,   // FormatBC7_RGBA_sRGB       
      OpenGL_4_3,         OpenGL_Unsupported,   // FormatETC2_R_11           
      OpenGL_4_3,         OpenGL_Unsupported,   // FormatETC2_R_11_sn        
      OpenGL_4_3,         OpenGL_Unsupported,   // FormatETC2_RG_11          
      OpenGL_4_3,         OpenGL_Unsupported,   // FormatETC2_RG_11_sn       
      OpenGL_4_3,         OpenGL_Unsupported,   // FormatETC2_RGB_8          
      OpenGL_4_3,         OpenGL_Unsupported,   // FormatETC2_RGB_8_sRGB     
      OpenGL_4_3,         OpenGL_Unsupported,   // FormatETC2_RGBA_8         
      OpenGL_4_3,         OpenGL_Unsupported,   // FormatETC2_RGBA_8_sRGB    
      OpenGL_4_3,         OpenGL_Unsupported,   // FormatETC2_RGB8_A1        
      OpenGL_4_3,         OpenGL_Unsupported,   // FormatETC2_RGB8_A1_sRGB   
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatPVRTC_RGB_2         
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatPVRTC_RGB_2_sRGB    
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatPVRTC_RGB_4         
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatPVRTC_RGB_4_sRGB    
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatPVRTC_RGBA_2        
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatPVRTC_RGBA_2_sRGB   
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatPVRTC_RGBA_4        
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatPVRTC_RGBA_4_sRGB   
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_4x4            
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_5x4            
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_5x5            
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_6x5            
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_6x6            
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_8x5            
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_8x6            
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_8x8            
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_10x5           
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_10x6           
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_10x8           
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_10x10          
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_12x10          
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_12x12          
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_4x4_sRGB       
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_5x4_sRGB       
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_5x5_sRGB       
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_6x5_sRGB       
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_6x6_sRGB       
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_8x5_sRGB       
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_8x6_sRGB       
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_8x8_sRGB       
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_10x5_sRGB      
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_10x6_sRGB      
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_10x8_sRGB      
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_10x10_sRGB     
      OpenGL_Unsupported, OpenGL_Unsupported,   // FormatASTC_12x10_sRGB     
      OpenGL_Unsupported, OpenGL_Unsupported    // FormatASTC_12x12_sRGB     
      };
*/
#elif EN_MOBILE_GPU

   // Last verified for <???>
/*
   // Support of different texture capabilities in Metal specification
   // (this values can be later overriden in general capabilities table by specific extensions used by engine)
   static const TextureInfoMTL TextureCapabilitiesMTL[TextureFormatsCount] =
      {
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatUnsupported         
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatR_8                 
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatR_8_sn              
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatR_8_u               
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatR_8_s               
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatR_16                
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatR_16_sn             
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatR_16_u              
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatR_16_s              
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatR_16_hf             
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatR_32_u              
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatR_32_s              
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatR_32_f              
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRG_8                
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRG_8_sn             
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRG_8_u              
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRG_8_s              
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatRG_16               
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatRG_16_sn            
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRG_16_u             
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRG_16_s             
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRG_16_hf            
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRG_32_u             
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRG_32_s             
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRG_32_f             
      OpenGL_ES_1_0,         OpenGL_ES_3_0,           // FormatRGB_8   
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRGB_8_sRGB              
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRGB_8_sn            
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRGB_8_u             
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRGB_8_s                     
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatRGB_16              
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatRGB_16_sn           
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRGB_16_u            
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRGB_16_s            
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRGB_16_hf           
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRGB_32_u            
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRGB_32_s            
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRGB_32_f    
      OpenGL_ES_1_0,         OpenGL_ES_3_0,           // FormatRGBA_8                      
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRGBA_8_sRGB
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRGBA_8_sn           
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRGBA_8_u            
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRGBA_8_s     
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatRGBA_16             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatRGBA_16_sn          
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRGBA_16_u           
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRGBA_16_s           
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRGBA_16_hf          
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRGBA_32_u           
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRGBA_32_s           
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatRGBA_32_f           
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatD_16                
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatD_24                  
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatD_32                
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatD_32_f              
      OpenGL_ES_3_1,         OpenGL_ES_Unsupported,   // FormatS_8                 
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatSD_8_24             
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatSD_8_32_f                   
      OpenGL_ES_3_0,         OpenGL_ES_3_0,           // FormatRGB_5_6_5           
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBGR_5_6_5           
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBGR_8               
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatBGR_10_11_11_f      
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatEBGR_5_9_9_9_f      
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBGR_16              
      OpenGL_ES_1_0,         OpenGL_ES_3_0,           // FormatRGBA_5_5_5_1        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBGRA_5_5_5_1        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatARGB_1_5_5_5        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatABGR_1_5_5_5        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatABGR_8              
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatARGB_8              
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBGRA_8              
      OpenGL_ES_Unsupported, OpenGL_ES_3_0,           // FormatRGBA_10_10_10_2      (upload is reversed !!)
      OpenGL_ES_Unsupported, OpenGL_ES_3_0,           // FormatRGBA_10_10_10_2_u    (upload is reversed !!)
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC1_RGB             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC1_RGB_sRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC1_RGBA            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC1_RGBA_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC2_RGBA_pRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC2_RGBA            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC2_RGBA_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC3_RGBA_pRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC3_RGBA            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC3_RGBA_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC4_R               
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC4_R_sn            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC5_RG              
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC5_RG_sn           
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC6H_RGB_f          
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC6H_RGB_uf         
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC7_RGBA            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatBC7_RGBA_sRGB       
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatETC2_R_11           
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatETC2_R_11_sn        
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatETC2_RG_11          
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatETC2_RG_11_sn       
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatETC2_RGB_8          
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatETC2_RGB_8_sRGB     
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatETC2_RGBA_8         
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatETC2_RGBA_8_sRGB    
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatETC2_RGB8_A1        
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // FormatETC2_RGB8_A1_sRGB   
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatPVRTC_RGB_2         
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatPVRTC_RGB_2_sRGB    
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatPVRTC_RGB_4         
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatPVRTC_RGB_4_sRGB    
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatPVRTC_RGBA_2        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatPVRTC_RGBA_2_sRGB   
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatPVRTC_RGBA_4        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatPVRTC_RGBA_4_sRGB   
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_4x4            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_5x4            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_5x5            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_6x5            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_6x6            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_8x5            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_8x6            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_8x8            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_10x5           
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_10x6           
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_10x8           
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_10x10          
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_12x10          
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_12x12          
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_4x4_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_5x4_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_5x5_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_6x5_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_6x6_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_8x5_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_8x6_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_8x8_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_10x5_sRGB      
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_10x6_sRGB      
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_10x8_sRGB      
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_10x10_sRGB     
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // FormatASTC_12x10_sRGB     
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported    // FormatASTC_12x12_sRGB     
      };
*/

#endif

#endif

   static const MTLTextureType TranslateTextureType[underlyingType(TextureType::Count)] =
      {
      MTLTextureType1D                    ,   // Texture1D
      MTLTextureType1DArray               ,   // Texture1DArray
      MTLTextureType2D                    ,   // Texture2D
      MTLTextureType2DArray               ,   // Texture2DArray
    //MTLTextureType2D                    ,   // Texture2DRectangle   (emulate with Texture2D)
      MTLTextureType2DMultisample         ,   // Texture2DMultisample
      (MTLTextureType)0xFFFF              ,   // Texture2DMultisampleArray
      MTLTextureType3D                    ,   // Texture3D
    //(MTLTextureType)0xFFFF              ,   // TextureBuffer
      MTLTextureTypeCube                  ,   // TextureCubeMap
      MTLTextureTypeCubeArray                 // TextureCubeMapArray
      };

#if defined(EN_PLATFORM_OSX)
   // Metal OSX Pixel Formats:
   // https://developer.apple.com/library/mac/documentation/Metal/Reference/MetalConstants_Ref/#//apple_ref/c/tdef/MTLPixelFormat
   // (last verified for Metal on OSX 10.11)
   static const MTLPixelFormat TranslateTextureFormat[underlyingType(Format::Count)] =
      { // Sized Internal Format     
      MTLPixelFormatInvalid               ,   // Format::Unsupported
      MTLPixelFormatR8Unorm               ,   // Format::R_8                    
      MTLPixelFormatInvalid               ,   // Format::R_8_sRGB           (unsupported)            
      MTLPixelFormatR8Snorm               ,   // Format::R_8_sn                 
      MTLPixelFormatR8Uint                ,   // Format::R_8_u                  
      MTLPixelFormatR8Sint                ,   // Format::R_8_s                  
      MTLPixelFormatR16Unorm              ,   // Format::R_16                   
      MTLPixelFormatR16Snorm              ,   // Format::R_16_sn                
      MTLPixelFormatR16Uint               ,   // Format::R_16_u                 
      MTLPixelFormatR16Sint               ,   // Format::R_16_s                 
      MTLPixelFormatR16Float              ,   // Format::R_16_hf                
      MTLPixelFormatR32Uint               ,   // Format::R_32_u                 
      MTLPixelFormatR32Sint               ,   // Format::R_32_s                 
      MTLPixelFormatR32Float              ,   // Format::R_32_f                 
      MTLPixelFormatRG8Unorm              ,   // Format::RG_8                   
      MTLPixelFormatInvalid               ,   // Format::RG_8_sRGB          (unsupported)           
      MTLPixelFormatRG8Snorm              ,   // Format::RG_8_sn                
      MTLPixelFormatRG8Uint               ,   // Format::RG_8_u                 
      MTLPixelFormatRG8Sint               ,   // Format::RG_8_s                 
      MTLPixelFormatRG16Unorm             ,   // Format::RG_16                  
      MTLPixelFormatRG16Snorm             ,   // Format::RG_16_sn               
      MTLPixelFormatRG16Uint              ,   // Format::RG_16_u                
      MTLPixelFormatRG16Sint              ,   // Format::RG_16_s                
      MTLPixelFormatRG16Float             ,   // Format::RG_16_hf               
      MTLPixelFormatRG32Uint              ,   // Format::RG_32_u                
      MTLPixelFormatRG32Sint              ,   // Format::RG_32_s                
      MTLPixelFormatRG32Float             ,   // Format::RG_32_f                
      MTLPixelFormatInvalid               ,   // Format::RGB_8              (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_8_sRGB         (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_8_sn           (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_8_u            (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_8_s            (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_16             (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_16_sn          (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_16_u           (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_16_s           (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_16_hf          (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_32_u           (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_32_s           (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_32_f           (unsupported)          
      MTLPixelFormatRGBA8Unorm            ,   // Format::RGBA_8                 
      MTLPixelFormatRGBA8Unorm_sRGB       ,   // Format::RGBA_8_sRGB            
      MTLPixelFormatRGBA8Snorm            ,   // Format::RGBA_8_sn              
      MTLPixelFormatRGBA8Uint             ,   // Format::RGBA_8_u               
      MTLPixelFormatRGBA8Sint             ,   // Format::RGBA_8_s               
      MTLPixelFormatRGBA16Unorm           ,   // Format::RGBA_16                
      MTLPixelFormatRGBA16Snorm           ,   // Format::RGBA_16_sn             
      MTLPixelFormatRGBA16Uint            ,   // Format::RGBA_16_u              
      MTLPixelFormatRGBA16Sint            ,   // Format::RGBA_16_s              
      MTLPixelFormatRGBA16Float           ,   // Format::RGBA_16_hf             
      MTLPixelFormatRGBA32Uint            ,   // Format::RGBA_32_u              
      MTLPixelFormatRGBA32Sint            ,   // Format::RGBA_32_s              
      MTLPixelFormatRGBA32Float           ,   // Format::RGBA_32_f              
      MTLPixelFormatInvalid               ,   // Format::D_16               (unsupported)            
      MTLPixelFormatInvalid               ,   // Format::D_24               (unsupported)            
      MTLPixelFormatInvalid               ,   // Format::D_24_8             (unsupported)            
      MTLPixelFormatInvalid               ,   // Format::D_32               (unsupported)            
      MTLPixelFormatDepth32Float          ,   // Format::D_32_f                       
      MTLPixelFormatStencil8              ,   // Format::S_8                           
      MTLPixelFormatInvalid               ,   // Format::DS_16_8            (unsupported)            
      MTLPixelFormatDepth24Unorm_Stencil8 ,   // Format::DS_24_8            ( query support through [MTLDevice depth24Stencil8PixelFormatSupported] )         
      MTLPixelFormatDepth32Float_Stencil8 ,   // Format::DS_32_f_8              
      MTLPixelFormatInvalid               ,   // Format::RGB_5_6_5          (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGR_5_6_5          (unsupported)        
      MTLPixelFormatInvalid               ,   // Format::BGR_8              (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGR_8_sRGB         (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGR_8_sn           (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGR_8_u            (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGR_8_s            (unsupported)     
      MTLPixelFormatRG11B10Float          ,   // Format::RGB_11_11_10_uf        
      MTLPixelFormatRGB9E5Float           ,   // Format::RGBE_9_9_9_5_uf        
      MTLPixelFormatBGRA8Unorm            ,   // Format::BGRA_8                 
      MTLPixelFormatBGRA8Unorm_sRGB       ,   // Format::BGRA_8_sRGB             
      MTLPixelFormatInvalid               ,   // Format::BGRA_8_sn          (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGRA_8_u           (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGRA_8_s           (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGRA_5_5_5_1       (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::ARGB_1_5_5_5       (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::ABGR_1_5_5_5       (unsupported)       
      MTLPixelFormatRGB10A2Unorm          ,   // Format::RGBA_10_10_10_2        
      MTLPixelFormatRGB10A2Uint           ,   // Format::RGBA_10_10_10_2_u      
      MTLPixelFormatInvalid               ,   // Format::BGRA_10_10_10_2    (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC1_RGB            (unsupported)      
      MTLPixelFormatInvalid               ,   // Format::BC1_RGB_sRGB       (unsupported)      
      MTLPixelFormatBC1_RGBA              ,   // Format::BC1_RGBA               
      MTLPixelFormatBC1_RGBA_sRGB         ,   // Format::BC1_RGBA_sRGB          
      MTLPixelFormatInvalid               ,   // Format::BC2_RGBA_pRGB      (unsupported)     
      MTLPixelFormatBC2_RGBA              ,   // Format::BC2_RGBA               
      MTLPixelFormatBC2_RGBA_sRGB         ,   // Format::BC2_RGBA_sRGB          
      MTLPixelFormatInvalid               ,   // Format::BC3_RGBA_pRGB      (unsupported)         
      MTLPixelFormatBC3_RGBA              ,   // Format::BC3_RGBA               
      MTLPixelFormatBC3_RGBA_sRGB         ,   // Format::BC3_RGBA_sRGB          
      MTLPixelFormatBC4_RUnorm            ,   // Format::BC4_R                  
      MTLPixelFormatBC4_RSnorm            ,   // Format::BC4_R_sn               
      MTLPixelFormatBC5_RGUnorm           ,   // Format::BC5_RG                 
      MTLPixelFormatBC5_RGSnorm           ,   // Format::BC5_RG_sn              
      MTLPixelFormatBC6H_RGBFloat         ,   // Format::BC6H_RGB_f             
      MTLPixelFormatBC6H_RGBUfloat        ,   // Format::BC6H_RGB_uf            
      MTLPixelFormatBC7_RGBAUnorm         ,   // Format::BC7_RGBA               
      MTLPixelFormatBC7_RGBAUnorm_sRGB    ,   // Format::BC7_RGBA_sRGB          
      MTLPixelFormatInvalid               ,   // Format::ETC2_R_11          (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ETC2_R_11_sn       (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ETC2_RG_11         (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ETC2_RG_11_sn      (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ETC2_RGB_8         (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ETC2_RGB_8_sRGB    (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ETC2_RGBA_8        (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ETC2_RGBA_8_sRGB   (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ETC2_RGB8_A1       (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ETC2_RGB8_A1_sRGB  (unsupported)
      MTLPixelFormatInvalid               ,   // Format::PVRTC_RGB_2        (unsupported)
      MTLPixelFormatInvalid               ,   // Format::PVRTC_RGB_2_sRGB   (unsupported)
      MTLPixelFormatInvalid               ,   // Format::PVRTC_RGB_4        (unsupported)
      MTLPixelFormatInvalid               ,   // Format::PVRTC_RGB_4_sRGB   (unsupported)
      MTLPixelFormatInvalid               ,   // Format::PVRTC_RGBA_2       (unsupported)
      MTLPixelFormatInvalid               ,   // Format::PVRTC_RGBA_2_sRGB  (unsupported)
      MTLPixelFormatInvalid               ,   // Format::PVRTC_RGBA_4       (unsupported)
      MTLPixelFormatInvalid               ,   // Format::PVRTC_RGBA_4_sRGB  (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_4x4           (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_5x4           (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_5x5           (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_6x5           (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_6x6           (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_8x5           (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_8x6           (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_8x8           (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_10x5          (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_10x6          (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_10x8          (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_10x10         (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_12x10         (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_12x12         (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_4x4_sRGB      (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_5x4_sRGB      (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_5x5_sRGB      (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_6x5_sRGB      (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_6x6_sRGB      (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_8x5_sRGB      (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_8x6_sRGB      (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_8x8_sRGB      (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_10x5_sRGB     (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_10x6_sRGB     (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_10x8_sRGB     (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_10x10_sRGB    (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_12x10_sRGB    (unsupported)
      MTLPixelFormatInvalid               ,   // Format::ASTC_12x12_sRGB    (unsupported) 
      };
#endif

#if defined(EN_PLATFORM_IOS)
   // Metal IOS Pixel Formats:
   // https://developer.apple.com/library/ios/documentation/Metal/Reference/MetalConstants_Ref/#//apple_ref/c/tdef/MTLPixelFormat
   // (last verified for Metal on IOS 9.0)
   static const MTLPixelFormat TranslateTextureFormat[underlyingType(Format::Count)] =
      { // Sized Internal Format     
      MTLPixelFormatInvalid               ,   // Format::Unsupported
      MTLPixelFormatR8Unorm               ,   // Format::R_8                    
      MTLPixelFormatR8Unorm_sRGB          ,   // Format::R_8_sRGB                       
      MTLPixelFormatR8Snorm               ,   // Format::R_8_sn                 
      MTLPixelFormatR8Uint                ,   // Format::R_8_u                  
      MTLPixelFormatR8Sint                ,   // Format::R_8_s                  
      MTLPixelFormatR16Unorm              ,   // Format::R_16                   
      MTLPixelFormatR16Snorm              ,   // Format::R_16_sn                
      MTLPixelFormatR16Uint               ,   // Format::R_16_u                 
      MTLPixelFormatR16Sint               ,   // Format::R_16_s                 
      MTLPixelFormatR16Float              ,   // Format::R_16_hf                
      MTLPixelFormatR32Uint               ,   // Format::R_32_u                 
      MTLPixelFormatR32Sint               ,   // Format::R_32_s                 
      MTLPixelFormatR32Float              ,   // Format::R_32_f                 
      MTLPixelFormatRG8Unorm              ,   // Format::RG_8                   
      MTLPixelFormatRG8Unorm_sRGB         ,   // Format::RG_8_sRGB                     
      MTLPixelFormatRG8Snorm              ,   // Format::RG_8_sn                
      MTLPixelFormatRG8Uint               ,   // Format::RG_8_u                 
      MTLPixelFormatRG8Sint               ,   // Format::RG_8_s                 
      MTLPixelFormatRG16Unorm             ,   // Format::RG_16                  
      MTLPixelFormatRG16Snorm             ,   // Format::RG_16_sn               
      MTLPixelFormatRG16Uint              ,   // Format::RG_16_u                
      MTLPixelFormatRG16Sint              ,   // Format::RG_16_s                
      MTLPixelFormatRG16Float             ,   // Format::RG_16_hf               
      MTLPixelFormatRG32Uint              ,   // Format::RG_32_u                
      MTLPixelFormatRG32Sint              ,   // Format::RG_32_s                
      MTLPixelFormatRG32Float             ,   // Format::RG_32_f                
      MTLPixelFormatInvalid               ,   // Format::RGB_8              (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_8_sRGB         (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_8_sn           (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_8_u            (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_8_s            (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_16             (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_16_sn          (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_16_u           (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_16_s           (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_16_hf          (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_32_u           (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_32_s           (unsupported)          
      MTLPixelFormatInvalid               ,   // Format::RGB_32_f           (unsupported)          
      MTLPixelFormatRGBA8Unorm            ,   // Format::RGBA_8                 
      MTLPixelFormatRGBA8Unorm_sRGB       ,   // Format::RGBA_8_sRGB            
      MTLPixelFormatRGBA8Snorm            ,   // Format::RGBA_8_sn              
      MTLPixelFormatRGBA8Uint             ,   // Format::RGBA_8_u               
      MTLPixelFormatRGBA8Sint             ,   // Format::RGBA_8_s               
      MTLPixelFormatRGBA16Unorm           ,   // Format::RGBA_16                
      MTLPixelFormatRGBA16Snorm           ,   // Format::RGBA_16_sn             
      MTLPixelFormatRGBA16Uint            ,   // Format::RGBA_16_u              
      MTLPixelFormatRGBA16Sint            ,   // Format::RGBA_16_s              
      MTLPixelFormatRGBA16Float           ,   // Format::RGBA_16_hf             
      MTLPixelFormatRGBA32Uint            ,   // Format::RGBA_32_u              
      MTLPixelFormatRGBA32Sint            ,   // Format::RGBA_32_s              
      MTLPixelFormatRGBA32Float           ,   // Format::RGBA_32_f              
      MTLPixelFormatInvalid               ,   // Format::D_16               (unsupported)            
      MTLPixelFormatInvalid               ,   // Format::D_24               (unsupported)            
      MTLPixelFormatInvalid               ,   // Format::D_24_8             (unsupported)            
      MTLPixelFormatInvalid               ,   // Format::D_32               (unsupported)            
      MTLPixelFormatDepth32Float          ,   // Format::D_32_f                       
      MTLPixelFormatStencil8              ,   // Format::S_8            
      MTLPixelFormatInvalid               ,   // Format::DS_16_8            (unsupported)            
      MTLPixelFormatInvalid               ,   // Format::DS_24_8            (unsupported)
      MTLPixelFormatDepth32Float_Stencil8 ,   // Format::DS_32_f_8          (IOS 9.0)             
      MTLPixelFormatInvalid               ,   // Format::RGB_5_6_5          (unsupported)     
      MTLPixelFormatB5G6R5Unorm           ,   // Format::BGR_5_6_5              
      MTLPixelFormatInvalid               ,   // Format::BGR_8              (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGR_8_sRGB         (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGR_8_sn           (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGR_8_u            (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGR_8_s            (unsupported)     
      MTLPixelFormatRG11B10Float          ,   // Format::RGB_11_11_10_uf        
      MTLPixelFormatRGB9E5Float           ,   // Format::RGBE_9_9_9_5_uf        
      MTLPixelFormatBGRA8Unorm            ,   // Format::BGRA_8                 
      MTLPixelFormatBGRA8Unorm_sRGB       ,   // Format::BGRA_8_sRGB      
      MTLPixelFormatInvalid               ,   // Format::BGRA_8_sn          (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGRA_8_u           (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BGRA_8_s           (unsupported)     
      MTLPixelFormatBGR5A1Unorm           ,   // Format::BGRA_5_5_5_1     
      MTLPixelFormatInvalid               ,   // Format::ARGB_1_5_5_5       (unsupported)     
      MTLPixelFormatA1BGR5Unorm           ,   // Format::ABGR_1_5_5_5           
      MTLPixelFormatRGB10A2Unorm          ,   // Format::RGBA_10_10_10_2        
      MTLPixelFormatRGB10A2Uint           ,   // Format::RGBA_10_10_10_2_u      
      MTLPixelFormatInvalid               ,   // Format::BGRA_10_10_10_2    (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC1_RGB            (unsupported)      
      MTLPixelFormatInvalid               ,   // Format::BC1_RGB_sRGB       (unsupported)      
      MTLPixelFormatInvalid               ,   // Format::BC1_RGBA           (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC1_RGBA_sRGB      (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC2_RGBA_pRGB      (unsupported)     
      MTLPixelFormatInvalid               ,   // Format::BC2_RGBA           (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC2_RGBA_sRGB      (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC3_RGBA_pRGB      (unsupported)         
      MTLPixelFormatInvalid               ,   // Format::BC3_RGBA           (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC3_RGBA_sRGB      (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC4_R              (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC4_R_sn           (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC5_RG             (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC5_RG_sn          (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC6H_RGB_f         (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC6H_RGB_uf        (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC7_RGBA           (unsupported)    
      MTLPixelFormatInvalid               ,   // Format::BC7_RGBA_sRGB      (unsupported)       
      MTLPixelFormatEAC_R11Unorm          ,   // Format::ETC2_R_11              
      MTLPixelFormatEAC_R11Snorm          ,   // Format::ETC2_R_11_sn           
      MTLPixelFormatEAC_RG11Unorm         ,   // Format::ETC2_RG_11             
      MTLPixelFormatEAC_RG11Snorm         ,   // Format::ETC2_RG_11_sn          
      MTLPixelFormatETC2_RGB8             ,   // Format::ETC2_RGB_8             
      MTLPixelFormatETC2_RGB8_sRGB        ,   // Format::ETC2_RGB_8_sRGB        
      MTLPixelFormatEAC_RGBA8             ,   // Format::ETC2_RGBA_8            
      MTLPixelFormatEAC_RGBA8_sRGB        ,   // Format::ETC2_RGBA_8_sRGB       
      MTLPixelFormatETC2_RGB8A1           ,   // Format::ETC2_RGB8_A1           
      MTLPixelFormatETC2_RGB8A1_sRGB      ,   // Format::ETC2_RGB8_A1_sRGB      
      MTLPixelFormatPVRTC_RGB_2BPP        ,   // Format::PVRTC_RGB_2            
      MTLPixelFormatPVRTC_RGB_2BPP_sRGB   ,   // Format::PVRTC_RGB_2_sRGB       
      MTLPixelFormatPVRTC_RGB_4BPP        ,   // Format::PVRTC_RGB_4            
      MTLPixelFormatPVRTC_RGB_4BPP_sRGB   ,   // Format::PVRTC_RGB_4_sRGB       
      MTLPixelFormatPVRTC_RGBA_2BPP       ,   // Format::PVRTC_RGBA_2           
      MTLPixelFormatPVRTC_RGBA_2BPP_sRGB  ,   // Format::PVRTC_RGBA_2_sRGB      
      MTLPixelFormatPVRTC_RGBA_4BPP       ,   // Format::PVRTC_RGBA_4           
      MTLPixelFormatPVRTC_RGBA_4BPP_sRGB  ,   // Format::PVRTC_RGBA_4_sRGB      
      MTLPixelFormatASTC_4x4_LDR          ,   // Format::ASTC_4x4               
      MTLPixelFormatASTC_5x4_LDR          ,   // Format::ASTC_5x4               
      MTLPixelFormatASTC_5x5_LDR          ,   // Format::ASTC_5x5               
      MTLPixelFormatASTC_6x5_LDR          ,   // Format::ASTC_6x5               
      MTLPixelFormatASTC_6x6_LDR          ,   // Format::ASTC_6x6               
      MTLPixelFormatASTC_8x5_LDR          ,   // Format::ASTC_8x5               
      MTLPixelFormatASTC_8x6_LDR          ,   // Format::ASTC_8x6               
      MTLPixelFormatASTC_8x8_LDR          ,   // Format::ASTC_8x8               
      MTLPixelFormatASTC_10x5_LDR         ,   // Format::ASTC_10x5              
      MTLPixelFormatASTC_10x6_LDR         ,   // Format::ASTC_10x6              
      MTLPixelFormatASTC_10x8_LDR         ,   // Format::ASTC_10x8              
      MTLPixelFormatASTC_10x10_LDR        ,   // Format::ASTC_10x10             
      MTLPixelFormatASTC_12x10_LDR        ,   // Format::ASTC_12x10             
      MTLPixelFormatASTC_12x12_LDR        ,   // Format::ASTC_12x12             
      MTLPixelFormatASTC_4x4_sRGB         ,   // Format::ASTC_4x4_sRGB          
      MTLPixelFormatASTC_5x4_sRGB         ,   // Format::ASTC_5x4_sRGB          
      MTLPixelFormatASTC_5x5_sRGB         ,   // Format::ASTC_5x5_sRGB          
      MTLPixelFormatASTC_6x5_sRGB         ,   // Format::ASTC_6x5_sRGB          
      MTLPixelFormatASTC_6x6_sRGB         ,   // Format::ASTC_6x6_sRGB          
      MTLPixelFormatASTC_8x5_sRGB         ,   // Format::ASTC_8x5_sRGB          
      MTLPixelFormatASTC_8x6_sRGB         ,   // Format::ASTC_8x6_sRGB          
      MTLPixelFormatASTC_8x8_sRGB         ,   // Format::ASTC_8x8_sRGB          
      MTLPixelFormatASTC_10x5_sRGB        ,   // Format::ASTC_10x5_sRGB         
      MTLPixelFormatASTC_10x6_sRGB        ,   // Format::ASTC_10x6_sRGB         
      MTLPixelFormatASTC_10x8_sRGB        ,   // Format::ASTC_10x8_sRGB         
      MTLPixelFormatASTC_10x10_sRGB       ,   // Format::ASTC_10x10_sRGB        
      MTLPixelFormatASTC_12x10_sRGB       ,   // Format::ASTC_12x10_sRGB        
      MTLPixelFormatASTC_12x12_sRGB       ,   // Format::ASTC_12x12_sRGB        
      };
#endif

   SurfaceDescriptor::SurfaceDescriptor() :
      mipmap(0),
      layer(0),
      ptr(nullptr)
   {
   }

   TextureMTL::TextureMTL(const id<MTLDevice> _device, const TextureState& _state) :
      TextureMTL(_device, _state, true)
   {
   }
   
   TextureMTL::TextureMTL(const id<MTLDevice> _device, const TextureState& _state, const bool allocateBacking) :
      TextureCommon(_state)
   {
   // Calculate amount of mipmaps texture will have
   state.mipmaps = TextureMipMapCount(state);

   // Keep handle to parent device
   device = _device;
   
   // We may not want to allocate backing memory if texture
   // object is used as container for surface returned by
   // environment (like HMD or Window framebuffer handle).
   if (!allocateBacking)
      return;
      
   // All validation was done in interface, parameters should be correct
   MTLTextureDescriptor* desc = [[MTLTextureDescriptor alloc] init];
   desc.textureType      = TranslateTextureType[underlyingType(state.type)];
   desc.pixelFormat      = TranslateTextureFormat[underlyingType(state.format)];
   desc.width            = state.width;
   desc.height           = state.height;
   desc.depth            = state.depth;
   desc.mipmapLevelCount = state.mipmaps;
   desc.sampleCount      = state.samples;
   desc.arrayLength      = state.layers;                // [1..2048]
   desc.cpuCacheMode     = MTLCPUCacheModeDefaultCache; // or MTLCPUCacheModeWriteCombined
   desc.storageMode      = MTLStorageModePrivate;       // We try to keep all textures in GPU memory
   
   handle = [device newTextureWithDescriptor:desc];
   [desc release];

   assert( handle != nil );
   }

   TextureMTL::~TextureMTL()
   {
   [handle release];
   }

   void* TextureMTL::map(const uint8 mipmap, const uint16 surface)
   {
   // Check if specified mipmap and layer are correct
   if (state.mipmaps <= mipmap)
      return nullptr;
   if (state.type == TextureType::Texture3D)
      {
      if (state.depth <= surface)
         return nullptr;
      }
   else
   if (state.type == TextureType::TextureCubeMap)
      {
      if (surface >= 6)
         return nullptr;
      }
   else
      {
      if (state.layers <= surface)
         return nullptr;
      }

   // Once texture is created we know it's type is valid on
   // current architecture and supported by used API, so we 
   // don't need to validate it here.

   // Lock this texture instance for surface upload.
   if (!desc.lock.tryLock())
      return nullptr;

   // In Metal texture is uploaded synchronously, by passing
   // client memory pointer to the API. Therefore API storage
   // needs to be emulated using local memory.
   desc.ptr = new uint8[size(mipmap)];
   if (desc.ptr == nullptr)
      {
      desc.lock.unlock();
      return nullptr;
      }

   // Save mapped surface properties
   desc.mipmap = mipmap;
   desc.layer  = surface; 
 
   return desc.ptr;
   }

   bool TextureMTL::unmap(void)
   {
   // Check if this texture is locked for upload.
   if (!desc.lock.isLocked())
      return false;

   // Update given layer mipmap
   NSUInteger rowSize   = size(desc.mipmap) / max(1U, height(desc.mipmap));
   NSUInteger imageSize = size(desc.mipmap);

   [handle replaceRegion:MTLRegionMake2D(0, 0, max(1U, width(desc.mipmap)), max(1U, height(desc.mipmap)))
      mipmapLevel:desc.mipmap slice:desc.layer withBytes:desc.ptr
      bytesPerRow:rowSize bytesPerImage:imageSize];

   // Free surface buffer
   delete [] reinterpret_cast<uint8*>(desc.ptr);
   desc.ptr = nullptr;
   desc.lock.unlock();
   return true;
   }
 
   bool TextureMTL::read(uint8* buffer, const uint8 mipmap, const uint16 surface) const
   {
   // Check if specified mipmap and layer are correct
   if (state.mipmaps <= mipmap)
      return false;
   if (state.type == TextureType::Texture3D)
      {
      if (state.depth <= surface)
         return false;
      }
   else
   if (state.type == TextureType::TextureCubeMap)
      {
      if (surface >= 6)
         return false;
      }
   else
      {
      if (state.layers <= surface)
         return false;
      }

   // TODO: Read back texture content !!!
   return false;
   }
   
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   void InitTextures(const CommonDevice* gpu)
   {
   // Init array of currently supported texture types
   for(uint16 i=0; i<underlyingType(TextureType::Count); ++i)
      {
      if (gpu->api.release >= TextureTypeSupportedMTL[i].release)
         TextureTypeSupported[i] = true;
      }

   // Init array of texture capabilities
   for(uint16 i=0; i<underlyingType(Format::Count); ++i)
      {
      // TODO: Populate tables above !!
      // https://developer.apple.com/library/ios/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/MetalFeatureSetTables/MetalFeatureSetTables.html
      //
      // if (gpu->api.release >= TextureCapabilitiesMTL[i].supported.release)
      //    TextureCapabilities[i].supported = true;
      // if (gpu->api.release >= TextureCapabilitiesMTL[i].rendertarget.release)
      //    TextureCapabilities[i].rendertarget = true;
      }

   // TODO: Here add support of ASTC if A8 / GPU Family v2
   }
#endif

   }
}
#endif