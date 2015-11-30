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
   static const Nversion TextureTypeSupportedMTL[TextureTypesCount] =
      {
      Metal_OSX_1_0                 ,   // Texture1D                 
      Metal_OSX_1_0                 ,   // Texture1DArray            
      Metal_OSX_1_0                 ,   // Texture2D                 
      Metal_OSX_1_0                 ,   // Texture2DArray            
      Metal_OSX_Unsupported         ,   // Texture2DRectangle        
      Metal_OSX_1_0                 ,   // Texture2DMultisample      
      Metal_OSX_Unsupported         ,   // Texture2DMultisampleArray 
      Metal_OSX_1_0                 ,   // Texture3D                 
      Metal_OSX_Unsupported         ,   // TextureBuffer             
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

   static const MTLTextureType TranslateTextureType[TextureTypesCount] =
      {
      MTLTextureType1D                    ,   // Texture1D
      MTLTextureType1DArray               ,   // Texture1DArray
      MTLTextureType2D                    ,   // Texture2D
      MTLTextureType2DArray               ,   // Texture2DArray
      (MTLTextureType)0xFFFF              ,   // Texture2DRectangle   (could emulate with 2D)
      MTLTextureType2DMultisample         ,   // Texture2DMultisample
      (MTLTextureType)0xFFFF              ,   // Texture2DMultisampleArray
      MTLTextureType3D                    ,   // Texture3D
      (MTLTextureType)0xFFFF              ,   // TextureBuffer
      MTLTextureTypeCube                  ,   // TextureCubeMap
      MTLTextureTypeCubeArray                 // TextureCubeMapArray
      };

   static const MTLPixelFormat TranslateTextureFormat[TextureFormatsCount] = 
      { // Sized Internal Format                     
      MTLPixelFormatInvalid               ,   // FormatUnsupported
      MTLPixelFormatR8Unorm               ,   // FormatR_8                  
      MTLPixelFormatR8Snorm               ,   // FormatR_8_sn               
      MTLPixelFormatR8Uint                ,   // FormatR_8_u                
      MTLPixelFormatR8Sint                ,   // FormatR_8_s                
      MTLPixelFormatR16Unorm              ,   // FormatR_16                 
      MTLPixelFormatR16Snorm              ,   // FormatR_16_sn              
      MTLPixelFormatR16Uint               ,   // FormatR_16_u               
      MTLPixelFormatR16Sint               ,   // FormatR_16_s               
      MTLPixelFormatR16Float              ,   // FormatR_16_hf              
      MTLPixelFormatR32Uint               ,   // FormatR_32_u               
      MTLPixelFormatR32Sint               ,   // FormatR_32_s               
      MTLPixelFormatR32Float              ,   // FormatR_32_f               
      MTLPixelFormatRG8Unorm              ,   // FormatRG_8                 
      MTLPixelFormatRG8Snorm              ,   // FormatRG_8_sn              
      MTLPixelFormatRG8Uint               ,   // FormatRG_8_u               
      MTLPixelFormatRG8Sint               ,   // FormatRG_8_s               
      MTLPixelFormatRG16Unorm             ,   // FormatRG_16                
      MTLPixelFormatRG16Snorm             ,   // FormatRG_16_sn             
      MTLPixelFormatRG16Uint              ,   // FormatRG_16_u              
      MTLPixelFormatRG16Sint              ,   // FormatRG_16_s              
      MTLPixelFormatRG16Float             ,   // FormatRG_16_hf             
      MTLPixelFormatRG32Uint              ,   // FormatRG_32_u              
      MTLPixelFormatRG32Sint              ,   // FormatRG_32_s              
      MTLPixelFormatRG32Float             ,   // FormatRG_32_f  
      (MTLPixelFormat)0                   ,   // FormatRGB_8
      (MTLPixelFormat)0                   ,   // FormatRGB_8_sRGB
      (MTLPixelFormat)0                   ,   // FormatRGB_8_sn
      (MTLPixelFormat)0                   ,   // FormatRGB_8_u
      (MTLPixelFormat)0                   ,   // FormatRGB_8_s
      (MTLPixelFormat)0                   ,   // FormatRGB_16
      (MTLPixelFormat)0                   ,   // FormatRGB_16_sn
      (MTLPixelFormat)0                   ,   // FormatRGB_16_u
      (MTLPixelFormat)0                   ,   // FormatRGB_16_s
      (MTLPixelFormat)0                   ,   // FormatRGB_16_hf
      (MTLPixelFormat)0                   ,   // FormatRGB_32_u
      (MTLPixelFormat)0                   ,   // FormatRGB_32_s
      (MTLPixelFormat)0                   ,   // FormatRGB_32_f
      MTLPixelFormatRGBA8Unorm            ,   // FormatRGBA_8               
      MTLPixelFormatRGBA8Unorm_sRGB       ,   // FormatRGBA_8_sRGB          
      MTLPixelFormatRGBA8Snorm            ,   // FormatRGBA_8_sn            
      MTLPixelFormatRGBA8Uint             ,   // FormatRGBA_8_u             
      MTLPixelFormatRGBA8Sint             ,   // FormatRGBA_8_s             
      MTLPixelFormatRGBA16Unorm           ,   // FormatRGBA_16              
      MTLPixelFormatRGBA16Snorm           ,   // FormatRGBA_16_sn           
      MTLPixelFormatRGBA16Uint            ,   // FormatRGBA_16_u            
      MTLPixelFormatRGBA16Sint            ,   // FormatRGBA_16_s            
      MTLPixelFormatRGBA16Float           ,   // FormatRGBA_16_hf           
      MTLPixelFormatRGBA32Uint            ,   // FormatRGBA_32_u            
      MTLPixelFormatRGBA32Sint            ,   // FormatRGBA_32_s            
      MTLPixelFormatRGBA32Float           ,   // FormatRGBA_32_f            
      (MTLPixelFormat)0                   ,   // FormatD_16
      (MTLPixelFormat)0                   ,   // FormatD_24
      (MTLPixelFormat)0                   ,   // FormatD_32
      MTLPixelFormatDepth32Float          ,   // FormatD_32_f  
      MTLPixelFormatStencil8              ,   // FormatS_8  
      MTLPixelFormatDepth24Unorm_Stencil8 ,   // FormatSD_8_24   
      MTLPixelFormatDepth32Float_Stencil8 ,   // FormatSD_8_32_f 
      (MTLPixelFormat)0                   ,   // FormatRGB_5_6_5
#if defined(EN_PLATFORM_IOS)
      MTLPixelFormatB5G6R5Unorm           ,   // FormatBGR_5_6_5
#elif defined(EN_PLATFORM_OSX)
      (MTLPixelFormat)0                   ,   // FormatBGR_5_6_5
#endif
      (MTLPixelFormat)0                   ,   // FormatBGR_8
      MTLPixelFormatRG11B10Float          ,   // FormatBGR_10_11_11_f       
      MTLPixelFormatRGB9E5Float           ,   // FormatEBGR_5_9_9_9_f       
      (MTLPixelFormat)0                   ,   // FormatBGR_16
      (MTLPixelFormat)0                   ,   // FormatRGBA_5_5_5_1
      (MTLPixelFormat)0                   ,   // FormatBGRA_5_5_5_1
      (MTLPixelFormat)0                   ,   // FormatARGB_1_5_5_5
#if defined(EN_PLATFORM_IOS)
      MTLPixelFormatA1BGR5Unorm           ,   // FormatABGR_1_5_5_5
#elif defined(EN_PLATFORM_OSX)
      (MTLPixelFormat)0                   ,   // FormatABGR_1_5_5_5
#endif
      (MTLPixelFormat)0                   ,   // FormatABGR_8
      (MTLPixelFormat)0                   ,   // FormatARGB_8
      MTLPixelFormatBGRA8Unorm            ,   // FormatBGRA_8               
      MTLPixelFormatRGB10A2Unorm          ,   // FormatRGBA_10_10_10_2      
      MTLPixelFormatRGB10A2Uint           ,   // FormatRGBA_10_10_10_2_u    
      (MTLPixelFormat)0                   ,   // FormatBC1_RGB
      (MTLPixelFormat)0                   ,   // FormatBC1_RGB_sRGB
      MTLPixelFormatBC1_RGBA              ,   // FormatBC1_RGBA             
      MTLPixelFormatBC1_RGBA_sRGB         ,   // FormatBC1_RGBA_sRGB        
      (MTLPixelFormat)0                   ,   // FormatBC2_RGBA_pRGB
      MTLPixelFormatBC2_RGBA              ,   // FormatBC2_RGBA             
      MTLPixelFormatBC2_RGBA_sRGB         ,   // FormatBC2_RGBA_sRGB        
      (MTLPixelFormat)0                   ,   // FormatBC3_RGBA_pRGB
      MTLPixelFormatBC3_RGBA              ,   // FormatBC3_RGBA             
      MTLPixelFormatBC3_RGBA_sRGB         ,   // FormatBC3_RGBA_sRGB        
      MTLPixelFormatBC4_RUnorm            ,   // FormatBC4_R                
      MTLPixelFormatBC4_RSnorm            ,   // FormatBC4_R_sn             
      MTLPixelFormatBC5_RGUnorm           ,   // FormatBC5_RG               
      MTLPixelFormatBC5_RGSnorm           ,   // FormatBC5_RG_sn            
      MTLPixelFormatBC6H_RGBFloat         ,   // FormatBC6H_RGB_f           
      MTLPixelFormatBC6H_RGBUfloat        ,   // FormatBC6H_RGB_uf          
      MTLPixelFormatBC7_RGBAUnorm         ,   // FormatBC7_RGBA             
      MTLPixelFormatBC7_RGBAUnorm_sRGB    ,   // FormatBC7_RGBA_sRGB
#if defined(EN_PLATFORM_IOS)
      MTLPixelFormatEAC_R11Unorm          ,   // FormatETC2_R_11            
      MTLPixelFormatEAC_R11Snorm          ,   // FormatETC2_R_11_sn         
      MTLPixelFormatEAC_RG11Unorm         ,   // FormatETC2_RG_11           
      MTLPixelFormatEAC_RG11Snorm         ,   // FormatETC2_RG_11_sn        
      MTLPixelFormatETC2_RGB8             ,   // FormatETC2_RGB_8           
      MTLPixelFormatETC2_RGB8_sRGB        ,   // FormatETC2_RGB_8_sRGB      
      MTLPixelFormatEAC_RGBA8             ,   // FormatETC2_RGBA_8
      MTLPixelFormatEAC_RGBA8_sRGB        ,   // FormatETC2_RGBA_8_sRGB
      MTLPixelFormatETC2_RGB8A1           ,   // FormatETC2_RGB8_A1
      MTLPixelFormatETC2_RGB8A1_sRGB      ,   // FormatETC2_RGB8_A1_sRGB
      MTLPixelFormatPVRTC_RGB_2BPP        ,   // FormatPVRTC_RGB_2
      MTLPixelFormatPVRTC_RGB_2BPP_sRGB   ,   // FormatPVRTC_RGB_2_sRGB
      MTLPixelFormatPVRTC_RGB_4BPP        ,   // FormatPVRTC_RGB_4
      MTLPixelFormatPVRTC_RGB_4BPP_sRGB   ,   // FormatPVRTC_RGB_4_sRGB
      MTLPixelFormatPVRTC_RGBA_2BPP       ,   // FormatPVRTC_RGBA_2
      MTLPixelFormatPVRTC_RGBA_2BPP_sRGB  ,   // FormatPVRTC_RGBA_2_sRGB
      MTLPixelFormatPVRTC_RGBA_4BPP       ,   // FormatPVRTC_RGBA_4
      MTLPixelFormatPVRTC_RGBA_4BPP_sRGB  ,   // FormatPVRTC_RGBA_4_sRGB
      MTLPixelFormatASTC_4x4_LDR          ,   // FormatASTC_4x4
      MTLPixelFormatASTC_5x4_LDR          ,   // FormatASTC_5x4
      MTLPixelFormatASTC_5x5_LDR          ,   // FormatASTC_5x5
      MTLPixelFormatASTC_6x5_LDR          ,   // FormatASTC_6x5
      MTLPixelFormatASTC_6x6_LDR          ,   // FormatASTC_6x6
      MTLPixelFormatASTC_8x5_LDR          ,   // FormatASTC_8x5
      MTLPixelFormatASTC_8x6_LDR          ,   // FormatASTC_8x6
      MTLPixelFormatASTC_8x8_LDR          ,   // FormatASTC_8x8
      MTLPixelFormatASTC_10x5_LDR         ,   // FormatASTC_10x5
      MTLPixelFormatASTC_10x6_LDR         ,   // FormatASTC_10x6
      MTLPixelFormatASTC_10x8_LDR         ,   // FormatASTC_10x8
      MTLPixelFormatASTC_10x10_LDR        ,   // FormatASTC_10x10
      MTLPixelFormatASTC_12x10_LDR        ,   // FormatASTC_12x10
      MTLPixelFormatASTC_12x12_LDR        ,   // FormatASTC_12x12
      MTLPixelFormatASTC_4x4_sRGB         ,   // FormatASTC_4x4_sRGB
      MTLPixelFormatASTC_5x4_sRGB         ,   // FormatASTC_5x4_sRGB
      MTLPixelFormatASTC_5x5_sRGB         ,   // FormatASTC_5x5_sRGB
      MTLPixelFormatASTC_6x5_sRGB         ,   // FormatASTC_6x5_sRGB
      MTLPixelFormatASTC_6x6_sRGB         ,   // FormatASTC_6x6_sRGB
      MTLPixelFormatASTC_8x5_sRGB         ,   // FormatASTC_8x5_sRGB
      MTLPixelFormatASTC_8x6_sRGB         ,   // FormatASTC_8x6_sRGB
      MTLPixelFormatASTC_8x8_sRGB         ,   // FormatASTC_8x8_sRGB
      MTLPixelFormatASTC_10x5_sRGB        ,   // FormatASTC_10x5_sRGB
      MTLPixelFormatASTC_10x6_sRGB        ,   // FormatASTC_10x6_sRGB
      MTLPixelFormatASTC_10x8_sRGB        ,   // FormatASTC_10x8_sRGB
      MTLPixelFormatASTC_10x10_sRGB       ,   // FormatASTC_10x10_sRGB
      MTLPixelFormatASTC_12x10_sRGB       ,   // FormatASTC_12x10_sRGB
      MTLPixelFormatASTC_12x12_sRGB       ,   // FormatASTC_12x12_sRGB
#elif defined(EN_PLATFORM_OSX)
      (MTLPixelFormat)0                   ,   // FormatETC2_R_11
      (MTLPixelFormat)0                   ,   // FormatETC2_R_11_sn
      (MTLPixelFormat)0                   ,   // FormatETC2_RG_11
      (MTLPixelFormat)0                   ,   // FormatETC2_RG_11_sn
      (MTLPixelFormat)0                   ,   // FormatETC2_RGB_8
      (MTLPixelFormat)0                   ,   // FormatETC2_RGB_8_sRGB
      (MTLPixelFormat)0                   ,   // FormatETC2_RGBA_8
      (MTLPixelFormat)0                   ,   // FormatETC2_RGBA_8_sRGB
      (MTLPixelFormat)0                   ,   // FormatETC2_RGB8_A1
      (MTLPixelFormat)0                   ,   // FormatETC2_RGB8_A1_sRGB
      (MTLPixelFormat)0                   ,   // FormatPVRTC_RGB_2
      (MTLPixelFormat)0                   ,   // FormatPVRTC_RGB_2_sRGB
      (MTLPixelFormat)0                   ,   // FormatPVRTC_RGB_4
      (MTLPixelFormat)0                   ,   // FormatPVRTC_RGB_4_sRGB
      (MTLPixelFormat)0                   ,   // FormatPVRTC_RGBA_2
      (MTLPixelFormat)0                   ,   // FormatPVRTC_RGBA_2_sRGB
      (MTLPixelFormat)0                   ,   // FormatPVRTC_RGBA_4
      (MTLPixelFormat)0                   ,   // FormatPVRTC_RGBA_4_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_4x4
      (MTLPixelFormat)0                   ,   // FormatASTC_5x4
      (MTLPixelFormat)0                   ,   // FormatASTC_5x5
      (MTLPixelFormat)0                   ,   // FormatASTC_6x5
      (MTLPixelFormat)0                   ,   // FormatASTC_6x6
      (MTLPixelFormat)0                   ,   // FormatASTC_8x5
      (MTLPixelFormat)0                   ,   // FormatASTC_8x6
      (MTLPixelFormat)0                   ,   // FormatASTC_8x8
      (MTLPixelFormat)0                   ,   // FormatASTC_10x5
      (MTLPixelFormat)0                   ,   // FormatASTC_10x6
      (MTLPixelFormat)0                   ,   // FormatASTC_10x8
      (MTLPixelFormat)0                   ,   // FormatASTC_10x10
      (MTLPixelFormat)0                   ,   // FormatASTC_12x10
      (MTLPixelFormat)0                   ,   // FormatASTC_12x12
      (MTLPixelFormat)0                   ,   // FormatASTC_4x4_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_5x4_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_5x5_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_6x5_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_6x6_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_8x5_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_8x6_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_8x8_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_10x5_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_10x6_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_10x8_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_10x10_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_12x10_sRGB
      (MTLPixelFormat)0                   ,   // FormatASTC_12x12_sRGB
#endif
      };

   SurfaceDescriptor::SurfaceDescriptor() :
      mipmap(0),
      layer(0),
      ptr(nullptr)
   {
   }

   TextureMTL::TextureMTL(const TextureState& _state)
   {
   state = _state;

   // Calculate amount of mipmaps texture will have
   state.mipmaps = TextureMipMapCount(state);

   // All validation was done in interface, parameters should be correct
   MTLTextureDescriptor* desc = [[MTLTextureDescriptor alloc] init];
   desc.textureType      = TranslateTextureType[state.type];
   desc.pixelFormat      = TranslateTextureFormat[state.format];
   desc.width            = state.width;
   desc.height           = state.height;
   desc.depth            = state.depth;
   desc.mipmapLevelCount = state.mipmaps;
   desc.sampleCount      = state.samples;
   desc.arrayLength      = state.layers;                // [1..2048]
   desc.cpuCacheMode     = MTLCPUCacheModeDefaultCache; // or MTLCPUCacheModeWriteCombined

   handle = [gpu->device newTextureWithDescriptor:desc];
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
   if (state.type == Texture3D)
      {
      if (state.depth <= surface)
         return nullptr;
      }
   else
   if (state.type == TextureCubeMap)
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
   delete [] desc.ptr;
   desc.ptr = nullptr;
   desc.lock.unlock();
   return true;
   }
 
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   void InitTextures(const CommonDevice* gpu)
   {
   // Init array of currently supported texture types
   for(uint16 i=0; i<TextureTypesCount; ++i)
      {
      if (gpu->api.release >= TextureTypeSupportedMTL[i].release)
         TextureTypeSupported[i] = true;
      }

   // Init array of texture capabilities
   for(uint16 i=0; i<TextureFormatsCount; ++i)
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