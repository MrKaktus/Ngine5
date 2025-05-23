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

#include "core/rendering/opengl/opengl.h"
#include "core/rendering/opengl/glTexture.h"
#include "core/rendering/opengl/glDevice.h"

#if defined(EN_MODULE_RENDERER_OPENGL)

//#include "core/rendering/rendering.hpp" // OpenGL versions defines
#include "core/rendering/context.h" // Profile

namespace en
{
   namespace gpu
   {
   // Cube map sampling is in left-handed coordinate system (like in Direct3D)
   const uint16 TranslateTextureFace[TextureFacesCount] = 
      { 
      GL_TEXTURE_CUBE_MAP_POSITIVE_X,   // Right
      GL_TEXTURE_CUBE_MAP_NEGATIVE_X,   // Left
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y,   // Top
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,   // Bottom
      GL_TEXTURE_CUBE_MAP_POSITIVE_Z,   // Front
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    // Back
      };


#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME

#ifdef EN_DISCRETE_GPU

   static Nversion TextureTypeSupportedGL[underlyingType(TextureType::Count)] =
      {
      OpenGL_1_1                     ,   // Texture1D                 
      OpenGL_3_0                     ,   // Texture1DArray            
      OpenGL_1_0                     ,   // Texture2D                 
      OpenGL_3_0                     ,   // Texture2DArray            
    //OpenGL_3_1                     ,   // Texture2DRectangle
      OpenGL_3_2                     ,   // Texture2DMultisample      
      OpenGL_3_2                     ,   // Texture2DMultisampleArray 
      OpenGL_1_2                     ,   // Texture3D                 
    //OpenGL_3_1                     ,   // TextureBuffer
      OpenGL_1_3                     ,   // TextureCubeMap            
      OpenGL_4_0                         // TextureCubeMapArray       
      };

#elif EN_MOBILE_GPU

   static Nversion TextureTypeSupportedGL[underlyingType(TextureType::Count)] =
      {
      OpenGL_ES_Unsupported          ,   // Texture1D                 
      OpenGL_ES_Unsupported          ,   // Texture1DArray            
      OpenGL_ES_1_0                  ,   // Texture2D                 
      OpenGL_ES_3_0                  ,   // Texture2DArray            
    //OpenGL_ES_Unsupported          ,   // Texture2DRectangle
      OpenGL_ES_3_1                  ,   // Texture2DMultisample      
      OpenGL_ES_3_2                  ,   // Texture2DMultisampleArray
      OpenGL_ES_3_0                  ,   // Texture3D                 
    //OpenGL_ES_3_2                  ,   // TextureBuffer
      OpenGL_ES_2_0                  ,   // TextureCubeMap            
      OpenGL_ES_3_2                      // TextureCubeMapArray
      };

#endif

   struct TextureInfoGL
      {
      Nversion supported;    // Supported as texture
      Nversion rendertarget; // Can be used as rendertarget (since OpenGL 3.0)
      };
     
#ifdef EN_DISCRETE_GPU

   static const MTLPixelFormat TranslateTextureFormat[underlyingType(Format::Count)] =
      { // Sized Internal Format     
  
      MTLPixelFormatBGRA8Unorm            ,   //          
      MTLPixelFormatBGRA8Unorm_sRGB       ,   //           
      MTLPixelFormatInvalid               ,   //      (unsupported)     
      MTLPixelFormatInvalid               ,   //      (unsupported)     
      MTLPixelFormatInvalid               ,   //      (unsupported)     
      MTLPixelFormatInvalid               ,   //      (unsupported)     
      MTLPixelFormatInvalid               ,   //      (unsupported)     
      MTLPixelFormatInvalid               ,   //      (unsupported)       

      };

   // Last verified for OpenGL 4.5

   // Support of different texture capabilities in OpenGL core specification
   // (this values can be later overriden in general capabilities table by specific extensions used by engine)
   static const TextureInfoGL TextureCapabilitiesGL[underlyingType(Format::Count)] =
      {
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::Unsupported       
      OpenGL_3_0,         OpenGL_3_0,           // Format::R_8        
                                                // Format::R_8_sRGB   
      OpenGL_3_1,         OpenGL_Unsupported,   // Format::R_8_sn            
      OpenGL_3_0,         OpenGL_3_0,           // Format::R_8_u             
      OpenGL_3_0,         OpenGL_3_0,           // Format::R_8_s             
      OpenGL_3_0,         OpenGL_3_0,           // Format::R_16              
      OpenGL_3_1,         OpenGL_Unsupported,   // Format::R_16_sn           
      OpenGL_3_0,         OpenGL_3_0,           // Format::R_16_u            
      OpenGL_3_0,         OpenGL_3_0,           // Format::R_16_s            
      OpenGL_3_0,         OpenGL_3_0,           // Format::R_16_hf           
      OpenGL_3_0,         OpenGL_3_0,           // Format::R_32_u            
      OpenGL_3_0,         OpenGL_3_0,           // Format::R_32_s            
      OpenGL_3_0,         OpenGL_3_0,           // Format::R_32_f            
      OpenGL_3_0,         OpenGL_3_0,           // Format::RG_8     
                                                // Format::RG_8_sRGB
      OpenGL_3_1,         OpenGL_Unsupported,   // Format::RG_8_sn           
      OpenGL_3_0,         OpenGL_3_0,           // Format::RG_8_u            
      OpenGL_3_0,         OpenGL_3_0,           // Format::RG_8_s            
      OpenGL_3_0,         OpenGL_3_0,           // Format::RG_16             
      OpenGL_3_1,         OpenGL_Unsupported,   // Format::RG_16_sn          
      OpenGL_3_0,         OpenGL_3_0,           // Format::RG_16_u           
      OpenGL_3_0,         OpenGL_3_0,           // Format::RG_16_s           
      OpenGL_3_0,         OpenGL_3_0,           // Format::RG_16_hf          
      OpenGL_3_0,         OpenGL_3_0,           // Format::RG_32_u           
      OpenGL_3_0,         OpenGL_3_0,           // Format::RG_32_s           
      OpenGL_3_0,         OpenGL_3_0,           // Format::RG_32_f           
      OpenGL_1_0,         OpenGL_Unsupported,   // Format::RGB_8      
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::RGB_8_sRGB              
      OpenGL_3_1,         OpenGL_Unsupported,   // Format::RGB_8_sn            
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::RGB_8_u             
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::RGB_8_s                     
      OpenGL_1_0,         OpenGL_Unsupported,   // Format::RGB_16              
      OpenGL_3_1,         OpenGL_Unsupported,   // Format::RGB_16_sn           
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::RGB_16_u            
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::RGB_16_s            
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::RGB_16_hf           
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::RGB_32_u            
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::RGB_32_s            
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::RGB_32_f    
      OpenGL_1_0,         OpenGL_3_0,           // Format::RGBA_8                    
      OpenGL_3_0,         OpenGL_3_0,           // Format::RGBA_8_sRGB 
      OpenGL_3_1,         OpenGL_Unsupported,   // Format::RGBA_8_sn         
      OpenGL_3_0,         OpenGL_3_0,           // Format::RGBA_8_u          
      OpenGL_3_0,         OpenGL_3_0,           // Format::RGBA_8_s    
      OpenGL_1_0,         OpenGL_3_0,           // Format::RGBA_16           
      OpenGL_3_1,         OpenGL_Unsupported,   // Format::RGBA_16_sn        
      OpenGL_3_0,         OpenGL_3_0,           // Format::RGBA_16_u         
      OpenGL_3_0,         OpenGL_3_0,           // Format::RGBA_16_s         
      OpenGL_3_0,         OpenGL_3_0,           // Format::RGBA_16_hf        
      OpenGL_3_0,         OpenGL_3_0,           // Format::RGBA_32_u         
      OpenGL_3_0,         OpenGL_3_0,           // Format::RGBA_32_s         
      OpenGL_3_0,         OpenGL_3_0,           // Format::RGBA_32_f         
      OpenGL_1_4,         OpenGL_3_0,           // Format::D_16                       
      OpenGL_1_4,         OpenGL_3_0,           // Format::D_24             
                                                // Format::D_24_8           
      OpenGL_1_4,         OpenGL_3_0, /* ? */   // Format::D_32                       
      OpenGL_3_0,         OpenGL_3_0,           // Format::D_32_f                     
      OpenGL_4_4,         OpenGL_4_3,           // Format::S_8              
                                                // Format::DS_16_8          
      OpenGL_3_0,         OpenGL_3_0,           // Format::DS_24_8                    
      OpenGL_3_0,         OpenGL_3_0,           // Format::DS_32_f_8                          
      OpenGL_1_2,         OpenGL_Unsupported,   // Format::RGB_5_6_5          
      OpenGL_1_2,         OpenGL_Unsupported,   // Format::BGR_5_6_5          
      OpenGL_1_2,         OpenGL_Unsupported,   // Format::BGR_8           
                                                // Format::BGR_8_sRGB      
                                                // Format::BGR_8_sn        
                                                // Format::BGR_8_u         
                                                // Format::BGR_8_s         
      OpenGL_3_0,         OpenGL_3_0,           // Format::RGB_11_11_10_uf    
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::RGBE_9_9_9_5_uf         
      OpenGL_1_2,         OpenGL_Unsupported,   // Format::BGRA_8        
                                                // Format::BGRA_8_sRGB   
                                                // Format::BGRA_8_sn     
                                                // Format::BGRA_8_u      
                                                // Format::BGRA_8_s      
      OpenGL_1_2,         OpenGL_Unsupported,   // Format::BGRA_5_5_5_1  
      OpenGL_1_2,         OpenGL_Unsupported,   // Format::ARGB_1_5_5_5  
      OpenGL_1_2,         OpenGL_Unsupported,   // Format::ABGR_1_5_5_5  
      OpenGL_1_2,         OpenGL_3_0,           // Format::RGBA_10_10_10_2   
      OpenGL_3_3,         OpenGL_3_3,           // Format::RGBA_10_10_10_2_u 
                                                // Format::BGRA_10_10_10_2  
      OpenGL_3_0, /*EXT*/ OpenGL_Unsupported,   // Format::BC1_RGB           
      OpenGL_3_0, /*EXT*/ OpenGL_Unsupported,   // Format::BC1_RGB_sRGB      
      OpenGL_3_0, /*EXT*/ OpenGL_Unsupported,   // Format::BC1_RGBA          
      OpenGL_3_0, /*EXT*/ OpenGL_Unsupported,   // Format::BC1_RGBA_sRGB     
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::BC2_RGBA_pRGB     
      OpenGL_3_0, /*EXT*/ OpenGL_Unsupported,   // Format::BC2_RGBA          
      OpenGL_3_0, /*EXT*/ OpenGL_Unsupported,   // Format::BC2_RGBA_sRGB     
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::BC3_RGBA_pRGB     
      OpenGL_3_0, /*EXT*/ OpenGL_Unsupported,   // Format::BC3_RGBA          
      OpenGL_3_0, /*EXT*/ OpenGL_Unsupported,   // Format::BC3_RGBA_sRGB     
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::BC4_R             
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::BC4_R_sn          
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::BC5_RG            
      OpenGL_3_0,         OpenGL_Unsupported,   // Format::BC5_RG_sn         
      OpenGL_4_2,         OpenGL_Unsupported,   // Format::BC6H_RGB_f        
      OpenGL_4_2,         OpenGL_Unsupported,   // Format::BC6H_RGB_uf       
      OpenGL_4_2,         OpenGL_Unsupported,   // Format::BC7_RGBA          
      OpenGL_4_2,         OpenGL_Unsupported,   // Format::BC7_RGBA_sRGB     
      OpenGL_4_3,         OpenGL_Unsupported,   // Format::ETC2_R_11         
      OpenGL_4_3,         OpenGL_Unsupported,   // Format::ETC2_R_11_sn      
      OpenGL_4_3,         OpenGL_Unsupported,   // Format::ETC2_RG_11        
      OpenGL_4_3,         OpenGL_Unsupported,   // Format::ETC2_RG_11_sn     
      OpenGL_4_3,         OpenGL_Unsupported,   // Format::ETC2_RGB_8        
      OpenGL_4_3,         OpenGL_Unsupported,   // Format::ETC2_RGB_8_sRGB   
      OpenGL_4_3,         OpenGL_Unsupported,   // Format::ETC2_RGBA_8       
      OpenGL_4_3,         OpenGL_Unsupported,   // Format::ETC2_RGBA_8_sRGB  
      OpenGL_4_3,         OpenGL_Unsupported,   // Format::ETC2_RGB8_A1      
      OpenGL_4_3,         OpenGL_Unsupported,   // Format::ETC2_RGB8_A1_sRGB 
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::PVRTC_RGB_2       
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::PVRTC_RGB_2_sRGB  
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::PVRTC_RGB_4       
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::PVRTC_RGB_4_sRGB  
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::PVRTC_RGBA_2      
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::PVRTC_RGBA_2_sRGB 
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::PVRTC_RGBA_4      
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::PVRTC_RGBA_4_sRGB 
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_4x4          
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_5x4          
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_5x5          
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_6x5          
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_6x6          
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_8x5          
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_8x6          
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_8x8          
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_10x5         
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_10x6         
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_10x8         
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_10x10        
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_12x10        
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_12x12        
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_4x4_sRGB     
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_5x4_sRGB     
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_5x5_sRGB     
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_6x5_sRGB     
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_6x6_sRGB     
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_8x5_sRGB     
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_8x6_sRGB     
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_8x8_sRGB     
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_10x5_sRGB    
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_10x6_sRGB    
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_10x8_sRGB    
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_10x10_sRGB   
      OpenGL_Unsupported, OpenGL_Unsupported,   // Format::ASTC_12x10_sRGB   
      OpenGL_Unsupported, OpenGL_Unsupported    // Format::ASTC_12x12_sRGB   
      };
   
#elif EN_MOBILE_GPU

   // Last verified for OpenGL ES 3.1

   // Support of different texture capabilities in OpenGL core specification
   // (this values can be later overriden in general capabilities table by specific extensions used by engine)
   static TextureInfoGL TextureCapabilitiesGL[underlyingType(Format::Count)] =
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
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC1_RGB              
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC1_RGB_sRGB         
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC1_RGBA             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC1_RGBA_sRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC2_RGBA_pRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC2_RGBA             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC2_RGBA_sRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC3_RGBA_pRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC3_RGBA             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC3_RGBA_sRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC4_R                
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC4_R_sn             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC5_RG               
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC5_RG_sn            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC6H_RGB_f           
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC6H_RGB_uf          
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC7_RGBA             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::BC7_RGBA_sRGB        
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // Format::ETC2_R_11            
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // Format::ETC2_R_11_sn         
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // Format::ETC2_RG_11           
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // Format::ETC2_RG_11_sn        
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // Format::ETC2_RGB_8           
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // Format::ETC2_RGB_8_sRGB      
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // Format::ETC2_RGBA_8          
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // Format::ETC2_RGBA_8_sRGB     
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // Format::ETC2_RGB8_A1         
      OpenGL_ES_3_0,         OpenGL_ES_Unsupported,   // Format::ETC2_RGB8_A1_sRGB    
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::PVRTC_RGB_2          
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::PVRTC_RGB_2_sRGB     
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::PVRTC_RGB_4          
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::PVRTC_RGB_4_sRGB     
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::PVRTC_RGBA_2         
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::PVRTC_RGBA_2_sRGB    
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::PVRTC_RGBA_4         
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::PVRTC_RGBA_4_sRGB    
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_4x4             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_5x4             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_5x5             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_6x5             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_6x6             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_8x5             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_8x6             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_8x8             
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_10x5            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_10x6            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_10x8            
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_10x10           
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_12x10           
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_12x12           
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_4x4_sRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_5x4_sRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_5x5_sRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_6x5_sRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_6x6_sRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_8x5_sRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_8x6_sRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_8x8_sRGB        
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_10x5_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_10x6_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_10x8_sRGB       
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_10x10_sRGB      
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported,   // Format::ASTC_12x10_sRGB      
      OpenGL_ES_Unsupported, OpenGL_ES_Unsupported    // Format::ASTC_12x12_sRGB      
      };

#endif

#endif


#ifdef EN_DISCRETE_GPU

   const uint16 TranslateTextureType[underlyingType(TextureType::Count)] =
      { 
      GL_TEXTURE_1D                   ,   // Texture1D                                            
      GL_TEXTURE_1D_ARRAY             ,   // Texture1DArray                                       
      GL_TEXTURE_2D                   ,   // Texture2D                                            
      GL_TEXTURE_2D_ARRAY             ,   // Texture2DArray                                       
    //GL_TEXTURE_RECTANGLE            ,   // Texture2DRectangle
      GL_TEXTURE_2D_MULTISAMPLE       ,   // Texture2DMultisample                                 
      GL_TEXTURE_2D_MULTISAMPLE_ARRAY ,   // Texture2DMultisampleArray                            
      GL_TEXTURE_3D                   ,   // Texture3D                                            
    //GL_TEXTURE_BUFFER               ,   // TextureBuffer
      GL_TEXTURE_CUBE_MAP             ,   // TextureCubeMap                                       
      GL_TEXTURE_CUBE_MAP_ARRAY           // TextureCubeMapArray                                  
      }; 

#elif EN_MOBILE_GPU

   const uint16 TranslateTextureType[underlyingType(TextureType::Count)] =
      { 
      0                               ,   // Texture1D                 
      0                               ,   // Texture1DArray            
      GL_TEXTURE_2D                   ,   // Texture2D                 
      GL_TEXTURE_2D_ARRAY             ,   // Texture2DArray            
    //0                               ,   // Texture2DRectangle
      GL_TEXTURE_2D_MULTISAMPLE       ,   // Texture2DMultisample      
      0                               ,   // Texture2DMultisampleArray 
      GL_TEXTURE_3D                   ,   // Texture3D                 
    //0                               ,   // TextureBuffer
      GL_TEXTURE_CUBE_MAP             ,   // TextureCubeMap            
      0                                   // TextureCubeMapArray       
      };

#endif

   const TextureFormatTranslation TranslateTextureFormat[underlyingType(Format::Count)] =
      { // Sized Internal Format                      // Base Internal Format
      { 0,                                            0,                     0                                 },   // Format::Unsupported    
      { GL_R8,                                        GL_RED,                GL_UNSIGNED_BYTE                  },   // Format::R_8        
      { 0,                                            GL_RED,                GL_UNSIGNED_BYTE                  },   // Format::R_8_sRGB   
      { GL_R8_SNORM,                                  GL_RED,                GL_BYTE                           },   // Format::R_8_sn           
      { GL_R8UI,                                      GL_RED_INTEGER,        GL_UNSIGNED_BYTE                  },   // Format::R_8_u            
      { GL_R8I,                                       GL_RED_INTEGER,        GL_BYTE                           },   // Format::R_8_s            
      { GL_R16,                                       GL_RED,                GL_UNSIGNED_SHORT                 },   // Format::R_16             
      { GL_R16_SNORM,                                 GL_RED,                GL_SHORT                          },   // Format::R_16_sn          
      { GL_R16UI,                                     GL_RED_INTEGER,        GL_UNSIGNED_SHORT                 },   // Format::R_16_u           
      { GL_R16I,                                      GL_RED_INTEGER,        GL_SHORT                          },   // Format::R_16_s           
      { GL_R16F,                                      GL_RED,                GL_HALF_FLOAT                     },   // Format::R_16_hf          
      { GL_R32UI,                                     GL_RED_INTEGER,        GL_UNSIGNED_INT                   },   // Format::R_32_u           
      { GL_R32I,                                      GL_RED_INTEGER,        GL_INT                            },   // Format::R_32_s           
      { GL_R32F,                                      GL_RED,                GL_FLOAT                          },   // Format::R_32_f           
      { GL_RG8,                                       GL_RG,                 GL_UNSIGNED_BYTE                  },   // Format::RG_8     
      { 0,                                            GL_RG,                 GL_UNSIGNED_BYTE                  },   // Format::RG_8_sRGB
      { GL_RG8_SNORM,                                 GL_RG,                 GL_BYTE                           },   // Format::RG_8_sn             
      { GL_RG8UI,                                     GL_RG_INTEGER,         GL_UNSIGNED_BYTE                  },   // Format::RG_8_u              
      { GL_RG8I,                                      GL_RG_INTEGER,         GL_BYTE                           },   // Format::RG_8_s              
      { GL_RG16,                                      GL_RG,                 GL_UNSIGNED_SHORT                 },   // Format::RG_16               
      { GL_RG16_SNORM,                                GL_RG,                 GL_SHORT                          },   // Format::RG_16_sn            
      { GL_RG16UI,                                    GL_RG_INTEGER,         GL_UNSIGNED_SHORT                 },   // Format::RG_16_u             
      { GL_RG16I,                                     GL_RG_INTEGER,         GL_SHORT                          },   // Format::RG_16_s             
      { GL_RG16F,                                     GL_RG,                 GL_HALF_FLOAT                     },   // Format::RG_16_hf            
      { GL_RG32UI,                                    GL_RG_INTEGER,         GL_UNSIGNED_INT                   },   // Format::RG_32_u             
      { GL_RG32I,                                     GL_RG_INTEGER,         GL_INT                            },   // Format::RG_32_s             
      { GL_RG32F,                                     GL_RG,                 GL_FLOAT                          },   // Format::RG_32_f       
      { GL_RGB8,                                      GL_RGB,                GL_UNSIGNED_BYTE                  },   // Format::RGB_8      
      { GL_SRGB8,                                     GL_RGB,                GL_UNSIGNED_BYTE                  },   // Format::RGB_8_sRGB             
      { GL_RGB8_SNORM,                                GL_RGB,                GL_BYTE                           },   // Format::RGB_8_sn       
      { GL_RGB8UI,                                    GL_RGB_INTEGER,        GL_UNSIGNED_BYTE                  },   // Format::RGB_8_u               
      { GL_RGB8I,                                     GL_RGB_INTEGER,        GL_BYTE                           },   // Format::RGB_8_s               
      { GL_RGB16,                                     GL_RGB,                GL_UNSIGNED_SHORT                 },   // Format::RGB_16                
      { GL_RGB16_SNORM,                               GL_RGB,                GL_SHORT                          },   // Format::RGB_16_sn             
      { GL_RGB16UI,                                   GL_RGB_INTEGER,        GL_UNSIGNED_SHORT                 },   // Format::RGB_16_u              
      { GL_RGB16I,                                    GL_RGB_INTEGER,        GL_SHORT                          },   // Format::RGB_16_s              
      { GL_RGB16F,                                    GL_RGB,                GL_HALF_FLOAT                     },   // Format::RGB_16_hf             
      { GL_RGB32UI,                                   GL_RGB_INTEGER,        GL_UNSIGNED_INT                   },   // Format::RGB_32_u              
      { GL_RGB32I,                                    GL_RGB_INTEGER,        GL_INT                            },   // Format::RGB_32_s              
      { GL_RGB32F,                                    GL_RGB,                GL_FLOAT                          },   // Format::RGB_32_f    
      { GL_RGBA8,                                     GL_RGBA,               GL_UNSIGNED_BYTE                  },   // Format::RGBA_8                
      { GL_SRGB8_ALPHA8,                              GL_RGBA,               GL_UNSIGNED_BYTE                  },   // Format::RGBA_8_sRGB     
      { GL_RGBA8_SNORM,                               GL_RGBA,               GL_BYTE                           },   // Format::RGBA_8_sn             
      { GL_RGBA8UI,                                   GL_RGBA_INTEGER,       GL_UNSIGNED_BYTE                  },   // Format::RGBA_8_u              
      { GL_RGBA8I,                                    GL_RGBA_INTEGER,       GL_BYTE                           },   // Format::RGBA_8_s        
      { GL_RGBA16,                                    GL_RGBA,               GL_UNSIGNED_SHORT                 },   // Format::RGBA_16               
      { GL_RGBA16_SNORM,                              GL_RGBA,               GL_SHORT                          },   // Format::RGBA_16_sn            
      { GL_RGBA16UI,                                  GL_RGBA_INTEGER,       GL_UNSIGNED_SHORT                 },   // Format::RGBA_16_u             
      { GL_RGBA16I,                                   GL_RGBA_INTEGER,       GL_SHORT                          },   // Format::RGBA_16_s             
      { GL_RGBA16F,                                   GL_RGBA,               GL_HALF_FLOAT                     },   // Format::RGBA_16_hf            
      { GL_RGBA32UI,                                  GL_RGBA_INTEGER,       GL_UNSIGNED_INT                   },   // Format::RGBA_32_u             
      { GL_RGBA32I,                                   GL_RGBA_INTEGER,       GL_INT                            },   // Format::RGBA_32_s             
      { GL_RGBA32F,                                   GL_RGBA,               GL_FLOAT                          },   // Format::RGBA_32_f             
      { GL_DEPTH_COMPONENT16,                         GL_DEPTH_COMPONENT,    GL_UNSIGNED_SHORT                 },   // Format::D_16                 
      { GL_DEPTH_COMPONENT24,                         GL_DEPTH_COMPONENT,    GL_UNSIGNED_INT                   },   // Format::D_24       
      { 0,                                            GL_DEPTH_COMPONENT,    0                                 },   // Format::D_24_8     
      { GL_DEPTH_COMPONENT32,                         GL_DEPTH_COMPONENT,    GL_UNSIGNED_INT                   },   // Format::D_32                 
      { GL_DEPTH_COMPONENT32F,                        GL_DEPTH_COMPONENT,    GL_FLOAT                          },   // Format::D_32_f     
      { GL_STENCIL_INDEX8,                            GL_STENCIL_INDEX,      GL_UNSIGNED_BYTE                  },   // Format::S_8        
      { 0,                                            GL_DEPTH_STENCIL,      0                                 },   // Format::DS_16_8    
      { GL_DEPTH24_STENCIL8,                          GL_DEPTH_STENCIL,      GL_UNSIGNED_INT_24_8              },   // Format::DS_24_8    
      { GL_DEPTH32F_STENCIL8,                         GL_DEPTH_STENCIL,      GL_FLOAT_32_UNSIGNED_INT_24_8_REV },   // Format::DS_32_f_8  
      { GL_RGB565,                                    GL_RGB,                GL_UNSIGNED_SHORT_5_6_5           },   // Format::RGB_5_6_5            
      { GL_RGB565,                                    GL_RGB,                GL_UNSIGNED_SHORT_5_6_5_REV       },   // Format::BGR_5_6_5            
      { GL_RGB8,                                      GL_BGR,                GL_UNSIGNED_BYTE                  },   // Format::BGR_8           
      { GL_SRGB8,                                     GL_BGR,                GL_UNSIGNED_BYTE                  },   // Format::BGR_8_sRGB      
      { GL_RGB8_SNORM,                                GL_BGRA,               GL_BYTE                           },   // Format::BGR_8_sn        
      { GL_RGB8UI,                                    GL_BGRA_INTEGER,       GL_UNSIGNED_BYTE                  },   // Format::BGR_8_u         
      { GL_RGB8I,                                     GL_BGRA_INTEGER,       GL_BYTE                           },   // Format::BGR_8_s         
      { GL_R11F_G11F_B10F,                            GL_RGB,                GL_UNSIGNED_INT_10F_11F_11F_REV   },   // Format::RGB_11_11_10_uf      
      { GL_RGB9_E5,                                   GL_RGBA,               GL_UNSIGNED_INT_5_9_9_9_REV       },   // Format::RGBE_9_9_9_5_uf   
      { GL_RGBA8,                                     GL_BGRA,               GL_UNSIGNED_BYTE                  },   // Format::BGRA_8        
      { GL_SRGB8_ALPHA8,                              GL_BGRA,               GL_UNSIGNED_BYTE                  },   // Format::BGRA_8_sRGB   
      { GL_RGBA8_SNORM,                               GL_BGRA,               GL_BYTE                           },   // Format::BGRA_8_sn     
      { GL_RGBA8UI,                                   GL_BGRA_INTEGER,       GL_UNSIGNED_BYTE                  },   // Format::BGRA_8_u      
      { GL_RGBA8I,                                    GL_BGRA_INTEGER,       GL_BYTE                           },   // Format::BGRA_8_s      
      { GL_RGB5_A1,                                   GL_BGRA,               GL_UNSIGNED_SHORT_1_5_5_5_REV     },   // Format::BGRA_5_5_5_1  
      { GL_RGB5_A1,                                   GL_BGRA,               GL_UNSIGNED_SHORT_5_5_5_1         },   // Format::ARGB_1_5_5_5  
      { GL_RGB5_A1,                                   GL_RGBA,               GL_UNSIGNED_SHORT_5_5_5_1         },   // Format::ABGR_1_5_5_5  
      { GL_RGB10_A2,                                  GL_RGBA,               GL_UNSIGNED_INT_10_10_10_2        },   // Format::RGBA_10_10_10_2      
      { GL_RGB10_A2UI,                                GL_RGBA_INTEGER,       GL_UNSIGNED_INT_10_10_10_2        },   // Format::RGBA_10_10_10_2_u    
      { GL_RGB10_A2,                                  GL_BGRA,               GL_UNSIGNED_INT_10_10_10_2        },   // Format::BGRA_10_10_10_2  
      { GL_COMPRESSED_RGB_S3TC_DXT1_EXT,              GL_RGB,                0                                 },   // Format::BC1_RGB            
      { GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,             GL_RGB,                0                                 },   // Format::BC1_RGB_sRGB       
      { GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,             GL_RGBA,               0                                 },   // Format::BC1_RGBA           
      { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,       GL_RGBA,               0                                 },   // Format::BC1_RGBA_sRGB      
      { 0,                                            0,                     0                                 },   // Format::BC2_RGBA_pRGB      
      { GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,             GL_RGBA,               0                                 },   // Format::BC2_RGBA           
      { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,       GL_RGBA,               0                                 },   // Format::BC2_RGBA_sRGB      
      { 0,                                            0,                     0                                 },   // Format::BC3_RGBA_pRGB      
      { GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,             GL_RGBA,               0                                 },   // Format::BC3_RGBA           
      { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,       GL_RGBA,               0                                 },   // Format::BC3_RGBA_sRGB      
      { GL_COMPRESSED_RED_RGTC1,                      GL_RED,                0                                 },   // Format::BC4_R              
      { GL_COMPRESSED_SIGNED_RED_RGTC1,               GL_RED,                0                                 },   // Format::BC4_R_sn           
      { GL_COMPRESSED_RG_RGTC2,                       GL_RG,                 0                                 },   // Format::BC5_RG             
      { GL_COMPRESSED_SIGNED_RG_RGTC2,                GL_RG,                 0                                 },   // Format::BC5_RG_sn       
#ifndef EN_PLATFORM_OSX
      { GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,          GL_RGB,                0                                 },   // Format::BC6H_RGB_f           
      { GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,        GL_RGB,                0                                 },   // Format::BC6H_RGB_uf          
      { GL_COMPRESSED_RGBA_BPTC_UNORM,                GL_RGBA,               0                                 },   // Format::BC7_RGBA             
      { GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,          GL_RGBA,               0                                 },   // Format::BC7_RGBA_sRGB        
      { GL_COMPRESSED_R11_EAC,                        GL_RED,                0                                 },   // Format::ETC2_R_11            
      { GL_COMPRESSED_SIGNED_R11_EAC,                 GL_RED,                0                                 },   // Format::ETC2_R_11_sn         
      { GL_COMPRESSED_RG11_EAC,                       GL_RG,                 0                                 },   // Format::ETC2_RG_11           
      { GL_COMPRESSED_SIGNED_RG11_EAC,                GL_RG,                 0                                 },   // Format::ETC2_RG_11_sn        
      { GL_COMPRESSED_RGB8_ETC2,                      GL_RGB,                0                                 },   // Format::ETC2_RGB_8           
      { GL_COMPRESSED_SRGB8_ETC2,                     GL_RGB,                0                                 },   // Format::ETC2_RGB_8_sRGB      
      { GL_COMPRESSED_RGBA8_ETC2_EAC,                 GL_RGBA,               0                                 },   // Format::ETC2_RGBA_8          
      { GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,          GL_RGBA,               0                                 },   // Format::ETC2_RGBA_8_sRGB     
      { GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,  GL_RGBA,               0                                 },   // Format::ETC2_RGB8_A1         
      { GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_RGBA,               0                                 },   // Format::ETC2_RGB8_A1_sRGB    
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGB_2          
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGB_2_sRGB     
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGB_4          
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGB_4_sRGB     
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGBA_2         
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGBA_2_sRGB    
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGBA_4         
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGBA_4_sRGB    
      { GL_COMPRESSED_RGBA_ASTC_4x4_KHR,              GL_RGBA,               0                                 },   // Format::ASTC_4x4             
      { GL_COMPRESSED_RGBA_ASTC_5x4_KHR,              GL_RGBA,               0                                 },   // Format::ASTC_5x4             
      { GL_COMPRESSED_RGBA_ASTC_5x5_KHR,              GL_RGBA,               0                                 },   // Format::ASTC_5x5             
      { GL_COMPRESSED_RGBA_ASTC_6x5_KHR,              GL_RGBA,               0                                 },   // Format::ASTC_6x5             
      { GL_COMPRESSED_RGBA_ASTC_6x6_KHR,              GL_RGBA,               0                                 },   // Format::ASTC_6x6             
      { GL_COMPRESSED_RGBA_ASTC_8x5_KHR,              GL_RGBA,               0                                 },   // Format::ASTC_8x5             
      { GL_COMPRESSED_RGBA_ASTC_8x6_KHR,              GL_RGBA,               0                                 },   // Format::ASTC_8x6             
      { GL_COMPRESSED_RGBA_ASTC_8x8_KHR,              GL_RGBA,               0                                 },   // Format::ASTC_8x8             
      { GL_COMPRESSED_RGBA_ASTC_10x5_KHR,             GL_RGBA,               0                                 },   // Format::ASTC_10x5            
      { GL_COMPRESSED_RGBA_ASTC_10x6_KHR,             GL_RGBA,               0                                 },   // Format::ASTC_10x6            
      { GL_COMPRESSED_RGBA_ASTC_10x8_KHR,             GL_RGBA,               0                                 },   // Format::ASTC_10x8            
      { GL_COMPRESSED_RGBA_ASTC_10x10_KHR,            GL_RGBA,               0                                 },   // Format::ASTC_10x10           
      { GL_COMPRESSED_RGBA_ASTC_12x10_KHR,            GL_RGBA,               0                                 },   // Format::ASTC_12x10           
      { GL_COMPRESSED_RGBA_ASTC_12x12_KHR,            GL_RGBA,               0                                 },   // Format::ASTC_12x12           
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,      GL_RGBA,               0                                 },   // Format::ASTC_4x4_sRGB        
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,      GL_RGBA,               0                                 },   // Format::ASTC_5x4_sRGB        
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,      GL_RGBA,               0                                 },   // Format::ASTC_5x5_sRGB        
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,      GL_RGBA,               0                                 },   // Format::ASTC_6x5_sRGB        
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,      GL_RGBA,               0                                 },   // Format::ASTC_6x6_sRGB        
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,      GL_RGBA,               0                                 },   // Format::ASTC_8x5_sRGB        
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,      GL_RGBA,               0                                 },   // Format::ASTC_8x6_sRGB        
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,      GL_RGBA,               0                                 },   // Format::ASTC_8x8_sRGB        
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,     GL_RGBA,               0                                 },   // Format::ASTC_10x5_sRGB       
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,     GL_RGBA,               0                                 },   // Format::ASTC_10x6_sRGB       
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,     GL_RGBA,               0                                 },   // Format::ASTC_10x8_sRGB       
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,    GL_RGBA,               0                                 },   // Format::ASTC_10x10_sRGB      
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,    GL_RGBA,               0                                 },   // Format::ASTC_12x10_sRGB      
      { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,    GL_RGBA,               0                                 }    // Format::ASTC_12x12_sRGB      
#else
      { 0,                                            GL_RGB,                0                                 },   // Format::BC6H_RGB_f           
      { 0,                                            GL_RGB,                0                                 },   // Format::BC6H_RGB_uf          
      { 0,                                            GL_RGBA,               0                                 },   // Format::BC7_RGBA             
      { 0,                                            GL_RGBA,               0                                 },   // Format::BC7_RGBA_sRGB        
      { 0,                                            GL_RED,                0                                 },   // Format::ETC2_R_11            
      { 0,                                            GL_RED,                0                                 },   // Format::ETC2_R_11_sn         
      { 0,                                            GL_RG,                 0                                 },   // Format::ETC2_RG_11           
      { 0,                                            GL_RG,                 0                                 },   // Format::ETC2_RG_11_sn        
      { 0,                                            GL_RGB,                0                                 },   // Format::ETC2_RGB_8           
      { 0,                                            GL_RGB,                0                                 },   // Format::ETC2_RGB_8_sRGB      
      { 0,                                            GL_RGBA,               0                                 },   // Format::ETC2_RGBA_8          
      { 0,                                            GL_RGBA,               0                                 },   // Format::ETC2_RGBA_8_sRGB     
      { 0,                                            GL_RGBA,               0                                 },   // Format::ETC2_RGB8_A1         
      { 0,                                            GL_RGBA,               0                                 },   // Format::ETC2_RGB8_A1_sRGB    
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGB_2          
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGB_2_sRGB     
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGB_4          
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGB_4_sRGB     
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGBA_2         
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGBA_2_sRGB    
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGBA_4         
      { 0,                                            0,                     0                                 },   // Format::PVRTC_RGBA_4_sRGB    
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_4x4             
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_5x4             
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_5x5             
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_6x5             
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_6x6             
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_8x5             
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_8x6             
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_8x8             
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_10x5            
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_10x6            
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_10x8            
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_10x10           
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_12x10           
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_12x12           
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_4x4_sRGB        
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_5x4_sRGB        
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_5x5_sRGB        
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_6x5_sRGB        
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_6x6_sRGB        
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_8x5_sRGB        
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_8x6_sRGB        
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_8x8_sRGB        
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_10x5_sRGB       
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_10x6_sRGB       
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_10x8_sRGB       
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_10x10_sRGB      
      { 0,                                            GL_RGBA,               0                                 },   // Format::ASTC_12x10_sRGB      
      { 0,                                            GL_RGBA,               0                                 }    // Format::ASTC_12x12_sRGB      
#endif
      };

   SurfaceDescriptorGL::SurfaceDescriptorGL() :
      mipmap(0),
      layer(0),
      ptr(nullptr),
      pbo(0)
   {
   }

   TextureGL::TextureGL() :
      CommonTexture()
   {
   }

   TextureGL::TextureGL(const TextureState& _state) :
      CommonTexture(_state)
   {
   // Calculate amount of mipmaps texture will have
   state.mipmaps = TextureMipMapCount(state);

   // Calculate amount of layers texture will have
   if ( state.layers == 1 &&
        ( state.type == TextureType::TextureCubeMap ||
          state.type == TextureType::TextureCubeMapArray ) )
      state.layers = 6;

   // Gather API specific texture state 
   uint16 glType           = TranslateTextureType[underlyingType(state.type)];
   uint16 glInternalFormat = TranslateTextureFormat[underlyingType(state.format)].dstFormat;
   uint16 glSourceFormat   = TranslateTextureFormat[underlyingType(state.format)].srcFormat;
   uint16 glSourceType     = TranslateTextureFormat[underlyingType(state.format)].srcType;
   uint32 mipmapsCount     = mipmaps();
   uint32 layers           = state.layers;
   uint32 samples          = state.samples;

   // Generate texture descriptor with default state
   Validate( glGenTextures(1, (GLuint*)&id) );
   Validate( glBindTexture(glType, id) );
   if ( (state.type != TextureType::Texture2DMultisample) &&
        (state.type != TextureType::Texture2DMultisampleArray) )
      {
    //if (state.type != TextureType::Texture2DRectangle)
         {
         Validate( glTexParameteri(glType, GL_TEXTURE_WRAP_S, GL_REPEAT) );
         Validate( glTexParameteri(glType, GL_TEXTURE_WRAP_T, GL_REPEAT) );
         Validate( glTexParameteri(glType, GL_TEXTURE_WRAP_R, GL_REPEAT) );
         }
      Validate( glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
      Validate( glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
      }

   if (GpuContext.screen.support(OpenGL_2_1))
      Validate( glGenBuffers(1, &desc.pbo) );

   // Reserve memory for texture completness (OpenGL legacy way)   
   if (TextureCompressionInfo[underlyingType(state.format)].compressed)
      {
      switch(state.type)
         {
         case TextureType::Texture1D:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               Validate( glCompressedTexImage1D(glType, mipmap, glInternalFormat, width, 0, size(mipmap), nullptr) );
               }
            break;
      
         case TextureType::Texture1DArray:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               Validate( glCompressedTexImage2D(glType, mipmap, glInternalFormat, width, layers, 0, size(mipmap), nullptr) );
               }
            break;
      
         case TextureType::Texture2D:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               uint16 height = max(1U, this->height(mipmap));
               Validate( glCompressedTexImage2D(glType, mipmap, glInternalFormat, width, height, 0, size(mipmap), nullptr) );
               }
            break;
      
         case TextureType::Texture2DArray:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               uint16 height = max(1U, this->height(mipmap));
               Validate( glCompressedTexImage3D(glType, mipmap, glInternalFormat, width, height, layers, 0, size(mipmap), nullptr) );
               }
            break;
       
         case TextureType::Texture3D:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               uint16 height = max(1U, this->height(mipmap));
               uint16 depth  = max(1U, this->depth(mipmap));
               Validate( glTexImage3D(glType, mipmap, glInternalFormat, width, height, depth, 0, glSourceFormat, glSourceType, nullptr) );
               }
            break;
      
         case TextureType::TextureCubeMap:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               Validate( glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipmap, glInternalFormat, width, width, 0, size(mipmap), nullptr) );
               Validate( glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipmap, glInternalFormat, width, width, 0, size(mipmap), nullptr) );
               Validate( glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipmap, glInternalFormat, width, width, 0, size(mipmap), nullptr) );
               Validate( glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipmap, glInternalFormat, width, width, 0, size(mipmap), nullptr) );
               Validate( glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipmap, glInternalFormat, width, width, 0, size(mipmap), nullptr) );
               Validate( glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipmap, glInternalFormat, width, width, 0, size(mipmap), nullptr) );
               }
            break;
      
         case TextureType::TextureCubeMapArray:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               Validate( glCompressedTexImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipmap, glInternalFormat, width, width, layers, 0, size(mipmap), nullptr) );
               Validate( glCompressedTexImage3D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipmap, glInternalFormat, width, width, layers, 0, size(mipmap), nullptr) );
               Validate( glCompressedTexImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipmap, glInternalFormat, width, width, layers, 0, size(mipmap), nullptr) );
               Validate( glCompressedTexImage3D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipmap, glInternalFormat, width, width, layers, 0, size(mipmap), nullptr) );
               Validate( glCompressedTexImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipmap, glInternalFormat, width, width, layers, 0, size(mipmap), nullptr) );
               Validate( glCompressedTexImage3D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipmap, glInternalFormat, width, width, layers, 0, size(mipmap), nullptr) );
               }
            break;
      
       //case TextureType::Texture2DRectangle:
         case TextureType::Texture2DMultisample:
         case TextureType::Texture2DMultisampleArray:
       //case TextureType::TextureBuffer:
            // These texture types don't support compression
         default:
            // How did we get here?
            assert( 0 );
            break;
         };
      }
   else
      {
      switch(state.type)
         {
         case TextureType::Texture1D:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               Validate( glTexImage1D(glType, mipmap, glInternalFormat, width, 0, glSourceFormat, glSourceType, nullptr) );
               }
            break;
      
         case TextureType::Texture1DArray:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               Validate( glTexImage2D(glType, mipmap, glInternalFormat, width, layers, 0, glSourceFormat, glSourceType, nullptr) );
               }
            break;
      
         case TextureType::Texture2D:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               uint16 height = max(1U, this->height(mipmap));
               Validate( glTexImage2D(glType, mipmap, glInternalFormat, width, height, 0, glSourceFormat, glSourceType, nullptr) );
               }
            break;
      
         case TextureType::Texture2DArray:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               uint16 height = max(1U, this->height(mipmap));
               Validate( glTexImage3D(glType, mipmap, glInternalFormat, width, height, layers, 0, glSourceFormat, glSourceType, nullptr) );
               }
            break;
      
       //case TextureType::Texture2DRectangle:
       //   Validate( glTexImage2D(glType, 0, glInternalFormat, width(), height(), 0, glSourceFormat, glSourceType, nullptr) );
       //   break;
      
         case TextureType::Texture2DMultisample:
            // TODO: Check samples count (depth/color/integer)???
            Validate( glTexImage2DMultisample(glType, samples, glInternalFormat, width(), height(), GL_TRUE) );
            break;
         
         case TextureType::Texture2DMultisampleArray:
            // TODO: Check samples count (depth/color/integer)???
            Validate( glTexImage3DMultisample(glType, samples, glInternalFormat, width(), height(), layers, GL_TRUE) );
            break;
      
       //case TextureType::TextureBuffer:
            // Buffer textures don't have their own storage
            break;
      
         case TextureType::Texture3D:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               uint16 height = max(1U, this->height(mipmap));
               uint16 depth  = max(1U, this->depth(mipmap));
               Validate( glTexImage3D(glType, mipmap, glInternalFormat, width, height, depth, 0, glSourceFormat, glSourceType, nullptr) );
               }
            break;
      
         case TextureType::TextureCubeMap:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               Validate( glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipmap, glInternalFormat, width, width, 0, glSourceFormat, glSourceType, nullptr) );
               Validate( glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipmap, glInternalFormat, width, width, 0, glSourceFormat, glSourceType, nullptr) );
               Validate( glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipmap, glInternalFormat, width, width, 0, glSourceFormat, glSourceType, nullptr) );
               Validate( glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipmap, glInternalFormat, width, width, 0, glSourceFormat, glSourceType, nullptr) );
               Validate( glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipmap, glInternalFormat, width, width, 0, glSourceFormat, glSourceType, nullptr) );
               Validate( glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipmap, glInternalFormat, width, width, 0, glSourceFormat, glSourceType, nullptr) );
               }
            break;
      
         case TextureType::TextureCubeMapArray:
            for(uint8 mipmap=0; mipmap<mipmapsCount; ++mipmap)
               {
               uint16 width  = max(1U, this->width(mipmap));
               Validate( glTexImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipmap, glInternalFormat, width, width, layers, 0, glSourceFormat, glSourceType, nullptr) );
               Validate( glTexImage3D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipmap, glInternalFormat, width, width, layers, 0, glSourceFormat, glSourceType, nullptr) );
               Validate( glTexImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipmap, glInternalFormat, width, width, layers, 0, glSourceFormat, glSourceType, nullptr) );
               Validate( glTexImage3D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipmap, glInternalFormat, width, width, layers, 0, glSourceFormat, glSourceType, nullptr) );
               Validate( glTexImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipmap, glInternalFormat, width, width, layers, 0, glSourceFormat, glSourceType, nullptr) );
               Validate( glTexImage3D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipmap, glInternalFormat, width, width, layers, 0, glSourceFormat, glSourceType, nullptr) );
               }
            break;
      
         default:
            // How did we get here?
            assert( 0 );
            break;
         };
      }
   }

   TextureGL::TextureGL(const TextureState& _state, const uint32 _id) :
      id(_id),
      CommonTexture(_state)
   {
   }

   TextureGL::~TextureGL()
   {
   Validate( glDeleteTextures(1, &id) );
   if (GpuContext.screen.support(OpenGL_2_1))
      Validate( glDeleteBuffers(1, &desc.pbo) );
   }

   void* TextureGL::map(const uint8 mipmap, const uint16 surface)
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

   if (GpuContext.screen.support(OpenGL_2_1))
      {
      // Lock PBO mechanism for surface upload.
      // Only one thread can upload data using PBO at the same time.
      if (!GpuContext.textureUpload.tryLock())
         return nullptr;

      // Discard previous PBO data to prevent CPU-GPU synchronization.
      Validate( glBindBuffer( GL_PIXEL_UNPACK_BUFFER, desc.pbo) );
      Validate( glBufferData( GL_PIXEL_UNPACK_BUFFER, size(mipmap), 0, GL_STREAM_DRAW) );

      // Map PBO memory for upload to GPU.
      Validate( desc.ptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY) );
      }
   else
      {
      // Lock this texture instance for surface upload.
      if (!desc.lock.tryLock())
         return nullptr;

      // In OpenGL 2.0 and older there is no PBO 
      // implementation so it needs to be emulated. 
      // Instead of using PBO, local memory is allocated.
      desc.ptr = new uint8[size(mipmap)];
      if (desc.ptr == nullptr)
         {
         desc.lock.unlock();
         return nullptr;
         }
      }

   // Save mapped surface properties
   desc.mipmap = mipmap;
   desc.layer  = surface; 
 
   return desc.ptr;
   }

   bool TextureGL::unmap(void)
   {
   if (GpuContext.screen.support(OpenGL_2_1))
      {
      // Check if we're in the process od PBO upload.
      if (!GpuContext.textureUpload.isLocked())
         return false;

      // Unmap PBO after all data is written to it.
      Validate( glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER) )
      desc.ptr = nullptr;
      }
   else
      {
      // Check if this texture is locked for upload.
      if (!desc.lock.isLocked())
         return false;
      }

   // Gather texture surface parameters
   bool   compressed       = TextureCompressionInfo[underlyingType(state.format)].compressed;
   uint16 glType           = TranslateTextureType[underlyingType(state.type)];
   uint16 glInternalFormat = TranslateTextureFormat[underlyingType(state.format)].dstFormat;
   uint16 srcFormat        = TranslateTextureFormat[underlyingType(state.format)].srcFormat;
   uint16 srcType          = TranslateTextureFormat[underlyingType(state.format)].srcType;
   uint32 size             = this->size(desc.mipmap);
   uint16 width            = max(1U, this->width(desc.mipmap));
   uint16 height           = max(1U, this->height(desc.mipmap));
   uint16 depth            = desc.layer;
   uint16 layer            = desc.layer;
   uint16 mipmap           = desc.mipmap;
   void*  ptr              = desc.ptr;

   // Send texture to GPU. If PBO is supported, texture data 
   // will be transferred using DMA. Otherwise it will be 
   // copied from client memory stalling CPU. 
   Validate( glBindTexture(glType, id) )

   // This switch could be avoided if there would be typed texture objects.
   // Because wrapper design decision was to have unified texture object,
   // this tradeoff is required.
   if (compressed)
      {
      switch(state.type)
         {
         case TextureType::Texture1D:
            Validate( glCompressedTexSubImage1D(GL_TEXTURE_1D, mipmap, 0, width, glInternalFormat, size, ptr) )
            break;

         case TextureType::Texture1DArray:
            Validate( glCompressedTexSubImage2D(GL_TEXTURE_1D_ARRAY, mipmap, 0, 0, width, layer, glInternalFormat, size, ptr) )
            break;

         case TextureType::Texture2D:
            Validate( glCompressedTexSubImage2D(GL_TEXTURE_2D, mipmap, 0, 0, width, height, glInternalFormat, size, ptr) )
            break;

         case TextureType::Texture2DArray:
            Validate( glCompressedTexSubImage3D(GL_TEXTURE_2D_ARRAY, mipmap, 0, 0, layer, width, height, 1, glInternalFormat, size, ptr) )
            break;

         case TextureType::Texture3D:
            Validate( glCompressedTexSubImage3D(GL_TEXTURE_3D, mipmap, 0, 0, depth, width, height, 1, glInternalFormat, size, ptr) )
            break;

         case TextureType::TextureCubeMap:
            uint16 glFace;
            glFace = TranslateTextureFace[desc.layer];
            Validate( glCompressedTexSubImage2D(glFace, mipmap, 0, 0, width, width, glInternalFormat, size, ptr) )
            break;

         case TextureType::TextureCubeMapArray:
            Validate( glCompressedTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, mipmap, 0, 0, layer, width, height, 1, glInternalFormat, size, ptr) )
            break;

       //case TextureType::Texture2DRectangle:        // Rectangle compressed textures are not supported
         case TextureType::Texture2DMultisample:      // Only valid as rendertarget, cannot upload it's content
         case TextureType::Texture2DMultisampleArray: // Only valid as rendertarget, cannot upload it's content
       //case TextureType::TextureBuffer:             // Texture Buffer is a proxy bind point for buffers
         default:
            // How did we get here?
            assert( 0 );
            break;
         };
      }
   else
      {
      switch(state.type)
         {
         case TextureType::Texture1D:
            Validate( glTexSubImage1D(GL_TEXTURE_1D, mipmap, 0, width, srcFormat, srcType, ptr) )
            break;

         case TextureType::Texture1DArray:
            Validate( glTexSubImage2D(GL_TEXTURE_1D_ARRAY, mipmap, 0, 0, width, layer, srcFormat, srcType, ptr) )
            break;

         case TextureType::Texture2D:
            Validate( glTexSubImage2D(GL_TEXTURE_2D, mipmap, 0, 0, width, height, srcFormat, srcType, ptr) )
            break;

         case TextureType::Texture2DArray:
            Validate( glTexSubImage3D(GL_TEXTURE_2D_ARRAY, mipmap, 0, 0, layer, width, height, 1, srcFormat, srcType, ptr) )
            break;

         case TextureType::Texture3D:
            Validate( glTexSubImage3D(GL_TEXTURE_3D, mipmap, 0, 0, depth, width, height, 1, srcFormat, srcType, ptr) )
            break;

         case TextureType::TextureCubeMap:
            uint16 glFace;
            glFace = TranslateTextureFace[desc.layer];
            Validate( glTexSubImage2D(glFace, mipmap, 0, 0, width, width, srcFormat, srcType, ptr) )
            break;

         case TextureType::TextureCubeMapArray:
            Validate( glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, mipmap, 0, 0, layer, width, height, 1, srcFormat, srcType, ptr) )
            break;

       //case TextureType::Texture2DRectangle:        // Rectangle compressed textures are not supported
         case TextureType::Texture2DMultisample:      // Only valid as rendertarget, cannot upload it's content
         case TextureType::Texture2DMultisampleArray: // Only valid as rendertarget, cannot upload it's content
       //case TextureType::TextureBuffer:             // Texture Buffer is a proxy bind point for buffers
         default:
            break;
         };
      }

   // Finish texture upload
   desc.mipmap = 0;
   desc.layer  = 0; 
   if (GpuContext.screen.support(OpenGL_2_1))
      {
      // Unbind PBO restoring standard pixel operations.
      Validate( glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0) )
      GpuContext.textureUpload.unlock();
      }
   else
      {
      // Free surface buffer
      delete [] reinterpret_cast<uint8*>(desc.ptr);
      desc.ptr = nullptr;
      desc.lock.unlock();
      }

   return true;
   }

   bool TextureGL::read(uint8* buffer, const uint8 mipmap, const uint16 surface) const
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

   // Extract information needed for reading texture from OpenGL
   uint16 glType    = TranslateTextureType[underlyingType(state.type)];
   uint16 srcFormat = TranslateTextureFormat[underlyingType(state.format)].srcFormat;
   uint16 srcType   = TranslateTextureFormat[underlyingType(state.format)].srcType;

   if (state.type == TextureType::TextureCubeMap)
      glType = TranslateTextureFace[surface];
   if (state.type == TextureType::TextureCubeMapArray)
      glType = TranslateTextureFace[surface % 6];

   // TODO: How to specify texture array layer ???
   // TODO: Use PBO for streaming through DMA, how to know when streaming is complete ???
   // TODO: Reading formats are limited. If texture type is not supported for reading, should get from user format he wants data to be returned!

   // Read texture content to RAM
   Validate( glBindTexture(glType, id) )
   Validate( glGetTexImage(glType, mipmap, srcFormat, srcType, buffer) )
   return true;
   }

#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   void InitTextureSupport(void)
   {
   // Init array of currently supported texture types
   for(uint16 i=0; i<underlyingType(TextureType::Count); ++i)
      {
      if (GpuContext.screen.api.release >= TextureTypeSupportedGL[i].release)
         TextureTypeSupported[i] = true;
      }

   // Init array of texture capabilities
   for(uint16 i=0; i<underlyingType(Format::Count); ++i)
      {
      if (GpuContext.screen.api.release >= TextureCapabilitiesGL[i].supported.release)
         TextureCapabilities[i].supported = true;
      if (GpuContext.screen.api.release >= TextureCapabilitiesGL[i].rendertarget.release)
         TextureCapabilities[i].rendertarget = true;
      }
   }

   void ClearTextureSupport(void)
   {
   // Clear array of currently supported texture types
   for(uint16 i=0; i<underlyingType(TextureType::Count); ++i)
      TextureTypeSupported[i] = false;

   // Clear array of texture capabilities
   for(uint16 i=0; i<underlyingType(Format::Count); ++i)
      {
      TextureCapabilities[i].supported = false;
      TextureCapabilities[i].rendertarget = false;
      }
   }
#endif

   }
}

#endif
