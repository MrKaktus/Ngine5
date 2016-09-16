/*

 Ngine v5.0
 
 Module      : Common Texture.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/common/device.h"
#include "core/rendering/common/texture.h"
#include <string.h> // memcmp

// TODO: Rename to texture.cpp and move one level when old interface will be completly removed

namespace en
{
   namespace gpu
   {
   TextureState::TextureState() :
      type(TextureType::Texture2D),
      format(Format::Unsupported),
      usage(TextureUsage::Read),
      width(1),
      height(1),
      depth(1),
      layers(1),
      samples(1),
      mipmaps(1)
   {
   }     
   
   TextureState::TextureState(const TextureType _type, 
      const Format _format,
      const TextureUsage _usage,
      const uint16 _width,
      const uint16 _height,
      const uint16 _depth,
      const uint16 _layers,
      const uint16 _samples,
      const uint16 _mipmaps ) :
      type(_type),
      format(_format),
      usage(_usage),
      width(_width),
      height(_height),
      depth(_depth),
      layers(_layers),
      samples(_samples),
      mipmaps(_mipmaps)
   {
   }

   bool TextureState::operator !=(const TextureState& b)
   {
   // TextureState structure can be aligned to
   // specified memory size. Therefore it is possible 
   // that there will be unused bytes at its end. To
   // perform proper memory comparison, only used
   // bytes need to be compared.
   return memcmp(this, &b, ((uint64)&mipmaps - (uint64)this + sizeof(mipmaps)) ) & 1;
   }

   // API independent texture compression into
   const TextureCompressedBlockInfo TextureCompressionInfo[underlyingType(Format::Count)] =
      {
      {  0,  0,  0, false },   // Format::Unsupported         
      {  0,  0,  1, false },   // Format::R_8                  
      {  0,  0,  1, false },   // Format::R_8_sRGB            
      {  0,  0,  1, false },   // Format::R_8_sn              
      {  0,  0,  1, false },   // Format::R_8_u               
      {  0,  0,  1, false },   // Format::R_8_s               
      {  0,  0,  2, false },   // Format::R_16                
      {  0,  0,  2, false },   // Format::R_16_sn             
      {  0,  0,  2, false },   // Format::R_16_u              
      {  0,  0,  2, false },   // Format::R_16_s              
      {  0,  0,  2, false },   // Format::R_16_hf             
      {  0,  0,  4, false },   // Format::R_32_u              
      {  0,  0,  4, false },   // Format::R_32_s              
      {  0,  0,  4, false },   // Format::R_32_f              
      {  0,  0,  2, false },   // Format::RG_8                
      {  0,  0,  2, false },   // Format::RG_8_sRGB           
      {  0,  0,  2, false },   // Format::RG_8_sn             
      {  0,  0,  2, false },   // Format::RG_8_u              
      {  0,  0,  2, false },   // Format::RG_8_s              
      {  0,  0,  4, false },   // Format::RG_16               
      {  0,  0,  4, false },   // Format::RG_16_sn            
      {  0,  0,  4, false },   // Format::RG_16_u             
      {  0,  0,  4, false },   // Format::RG_16_s             
      {  0,  0,  4, false },   // Format::RG_16_hf            
      {  0,  0,  8, false },   // Format::RG_32_u             
      {  0,  0,  8, false },   // Format::RG_32_s             
      {  0,  0,  8, false },   // Format::RG_32_f             
      {  0,  0,  3, false },   // Format::RGB_8             (may be padded internally to 4 bytes)            
      {  0,  0,  3, false },   // Format::RGB_8_sRGB        (may be padded internally to 4 bytes)    
      {  0,  0,  3, false },   // Format::RGB_8_sn          (may be padded internally to 4 bytes)    
      {  0,  0,  3, false },   // Format::RGB_8_u           (may be padded internally to 4 bytes)    
      {  0,  0,  3, false },   // Format::RGB_8_s           (may be padded internally to 4 bytes)    
      {  0,  0,  6, false },   // Format::RGB_16            (may be padded internally to 8 bytes)    
      {  0,  0,  6, false },   // Format::RGB_16_sn         (may be padded internally to 8 bytes)    
      {  0,  0,  6, false },   // Format::RGB_16_u          (may be padded internally to 8 bytes)    
      {  0,  0,  6, false },   // Format::RGB_16_s          (may be padded internally to 8 bytes)    
      {  0,  0,  6, false },   // Format::RGB_16_hf         (may be padded internally to 8 bytes)    
      {  0,  0, 12, false },   // Format::RGB_32_u            
      {  0,  0, 12, false },   // Format::RGB_32_s            
      {  0,  0, 12, false },   // Format::RGB_32_f            
      {  0,  0,  4, false },   // Format::RGBA_8              
      {  0,  0,  4, false },   // Format::RGBA_8_sRGB         
      {  0,  0,  4, false },   // Format::RGBA_8_sn           
      {  0,  0,  4, false },   // Format::RGBA_8_u            
      {  0,  0,  4, false },   // Format::RGBA_8_s            
      {  0,  0,  8, false },   // Format::RGBA_16             
      {  0,  0,  8, false },   // Format::RGBA_16_sn          
      {  0,  0,  8, false },   // Format::RGBA_16_u           
      {  0,  0,  8, false },   // Format::RGBA_16_s           
      {  0,  0,  8, false },   // Format::RGBA_16_hf          
      {  0,  0, 16, false },   // Format::RGBA_32_u           
      {  0,  0, 16, false },   // Format::RGBA_32_s           
      {  0,  0, 16, false },   // Format::RGBA_32_f           
      {  0,  0,  2, false },   // Format::D_16                
      {  0,  0,  3, false },   // Format::D_24              (may be padded internally to 4 bytes)        
      {  0,  0,  4, false },   // Format::D_24_8               
      {  0,  0,  4, false },   // Format::D_32                 
      {  0,  0,  4, false },   // Format::D_32_f               
      {  0,  0,  1, false },   // Format::S_8                  
      {  0,  0,  3, false },   // Format::DS_16_8              
      {  0,  0,  4, false },   // Format::DS_24_8              
      {  0,  0,  8, false },   // Format::DS_32_f_8         (5 bytes plus 3 bytes padding)            
      {  0,  0,  2, false },   // Format::RGB_5_6_5            
      {  0,  0,  2, false },   // Format::BGR_5_6_5            
      {  0,  0,  3, false },   // Format::BGR_8             (may be padded internally to 4 bytes)   
      {  0,  0,  3, false },   // Format::BGR_8_sRGB        (may be padded internally to 4 bytes)   
      {  0,  0,  3, false },   // Format::BGR_8_sn          (may be padded internally to 4 bytes)   
      {  0,  0,  3, false },   // Format::BGR_8_u           (may be padded internally to 4 bytes)   
      {  0,  0,  3, false },   // Format::BGR_8_s           (may be padded internally to 4 bytes) 
      {  0,  0,  4, false },   // Format::RGB_11_11_10_uf      
      {  0,  0,  4, false },   // Format::RGBE_9_9_9_5_uf      
      {  0,  0,  4, false },   // Format::BGRA_8               
      {  0,  0,  4, false },   // Format::BGRA_8_sRGB          
      {  0,  0,  4, false },   // Format::BGRA_8_sn           
      {  0,  0,  4, false },   // Format::BGRA_8_u            
      {  0,  0,  4, false },   // Format::BGRA_8_s            
      {  0,  0,  2, false },   // Format::BGRA_5_5_5_1        
      {  0,  0,  2, false },   // Format::ARGB_1_5_5_5        
      {  0,  0,  2, false },   // Format::ABGR_1_5_5_5         
      {  0,  0,  4, false },   // Format::RGBA_10_10_10_2      
      {  0,  0,  4, false },   // Format::RGBA_10_10_10_2_u   
      {  0,  0,  4, false },   // Format::BGRA_10_10_10_2      
      {  4,  4,  8, true  },   // Format::BC1_RGB              
      {  4,  4,  8, true  },   // Format::BC1_RGB_sRGB         
      {  4,  4,  8, true  },   // Format::BC1_RGBA             
      {  4,  4,  8, true  },   // Format::BC1_RGBA_sRGB        
      {  4,  4, 16, true  },   // Format::BC2_RGBA_pRGB        
      {  4,  4, 16, true  },   // Format::BC2_RGBA             
      {  4,  4, 16, true  },   // Format::BC2_RGBA_sRGB        
      {  4,  4, 16, true  },   // Format::BC3_RGBA_pRGB        
      {  4,  4, 16, true  },   // Format::BC3_RGBA             
      {  4,  4, 16, true  },   // Format::BC3_RGBA_sRGB        
      {  4,  4,  8, true  },   // Format::BC4_R                
      {  4,  4,  8, true  },   // Format::BC4_R_sn             
      {  4,  4, 16, true  },   // Format::BC5_RG               
      {  4,  4, 16, true  },   // Format::BC5_RG_sn           
      {  4,  4, 16, true  },   // Format::BC6H_RGB_f           
      {  4,  4, 16, true  },   // Format::BC6H_RGB_uf          
      {  4,  4, 16, true  },   // Format::BC7_RGBA             
      {  4,  4, 16, true  },   // Format::BC7_RGBA_sRGB        
      {  4,  4,  8, true  },   // Format::ETC2_R_11            
      {  4,  4,  8, true  },   // Format::ETC2_R_11_sn         
      {  4,  4, 16, true  },   // Format::ETC2_RG_11           
      {  4,  4, 16, true  },   // Format::ETC2_RG_11_sn        
      {  4,  4,  8, true  },   // Format::ETC2_RGB_8           
      {  4,  4,  8, true  },   // Format::ETC2_RGB_8_sRGB      
      {  4,  4, 16, true  },   // Format::ETC2_RGBA_8          
      {  4,  4, 16, true  },   // Format::ETC2_RGBA_8_sRGB     
      {  4,  4,  8, true  },   // Format::ETC2_RGB8_A1        
      {  4,  4,  8, true  },   // Format::ETC2_RGB8_A1_sRGB   
      {  4,  4,  8, true  },   // Format::PVRTC_RGB_2         
      {  4,  4,  8, true  },   // Format::PVRTC_RGB_2_sRGB    
      {  4,  4,  8, true  },   // Format::PVRTC_RGB_4         
      {  4,  4,  8, true  },   // Format::PVRTC_RGB_4_sRGB    
      {  4,  4,  8, true  },   // Format::PVRTC_RGBA_2        
      {  4,  4,  8, true  },   // Format::PVRTC_RGBA_2_sRGB   
      {  4,  4,  8, true  },   // Format::PVRTC_RGBA_4        
      {  4,  4,  8, true  },   // Format::PVRTC_RGBA_4_sRGB   
      {  4,  4, 16, true  },   // Format::ASTC_4x4            
      {  5,  4, 16, true  },   // Format::ASTC_5x4            
      {  5,  5, 16, true  },   // Format::ASTC_5x5            
      {  6,  5, 16, true  },   // Format::ASTC_6x5            
      {  6,  6, 16, true  },   // Format::ASTC_6x6            
      {  8,  5, 16, true  },   // Format::ASTC_8x5            
      {  8,  6, 16, true  },   // Format::ASTC_8x6            
      {  8,  8, 16, true  },   // Format::ASTC_8x8            
      { 10,  5, 16, true  },   // Format::ASTC_10x5           
      { 10,  6, 16, true  },   // Format::ASTC_10x6           
      { 10,  8, 16, true  },   // Format::ASTC_10x8           
      { 10, 10, 16, true  },   // Format::ASTC_10x10          
      { 12, 10, 16, true  },   // Format::ASTC_12x10          
      { 12, 12, 16, true  },   // Format::ASTC_12x12          
      {  4,  4, 16, true  },   // Format::ASTC_4x4_sRGB       
      {  5,  4, 16, true  },   // Format::ASTC_5x4_sRGB       
      {  5,  5, 16, true  },   // Format::ASTC_5x5_sRGB       
      {  6,  5, 16, true  },   // Format::ASTC_6x5_sRGB       
      {  6,  6, 16, true  },   // Format::ASTC_6x6_sRGB       
      {  8,  5, 16, true  },   // Format::ASTC_8x5_sRGB       
      {  8,  6, 16, true  },   // Format::ASTC_8x6_sRGB       
      {  8,  8, 16, true  },   // Format::ASTC_8x8_sRGB       
      { 10,  5, 16, true  },   // Format::ASTC_10x5_sRGB      
      { 10,  6, 16, true  },   // Format::ASTC_10x6_sRGB      
      { 10,  8, 16, true  },   // Format::ASTC_10x8_sRGB      
      { 10, 10, 16, true  },   // Format::ASTC_10x10_sRGB     
      { 12, 10, 16, true  },   // Format::ASTC_12x10_sRGB     
      { 12, 12, 16, true  }    // Format::ASTC_12x12_sRGB     
      };

#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   // API dependent texture information (filled in during API backend creation)
   bool TextureTypeSupported[underlyingType(TextureType::Count)] =
      {
      false,   // Texture1D                
      false,   // Texture1DArray           
      false,   // Texture2D                
      false,   // Texture2DArray
      false,   // Texture2DMultisample     
      false,   // Texture2DMultisampleArray
      false,   // Texture3D
      false,   // TextureCubeMap           
      false    // TextureCubeMapArray      
      };

   TextureInfo TextureCapabilities[underlyingType(Format::Count)] =
      {
      { false, false },   // Format::Unsupported         
      { false, false },   // Format::R_8                  
      { false, false },   // Format::R_8_sRGB            
      { false, false },   // Format::R_8_sn              
      { false, false },   // Format::R_8_u               
      { false, false },   // Format::R_8_s               
      { false, false },   // Format::R_16                
      { false, false },   // Format::R_16_sn             
      { false, false },   // Format::R_16_u              
      { false, false },   // Format::R_16_s              
      { false, false },   // Format::R_16_hf             
      { false, false },   // Format::R_32_u              
      { false, false },   // Format::R_32_s              
      { false, false },   // Format::R_32_f              
      { false, false },   // Format::RG_8                
      { false, false },   // Format::RG_8_sRGB           
      { false, false },   // Format::RG_8_sn             
      { false, false },   // Format::RG_8_u              
      { false, false },   // Format::RG_8_s              
      { false, false },   // Format::RG_16               
      { false, false },   // Format::RG_16_sn            
      { false, false },   // Format::RG_16_u             
      { false, false },   // Format::RG_16_s             
      { false, false },   // Format::RG_16_hf            
      { false, false },   // Format::RG_32_u             
      { false, false },   // Format::RG_32_s             
      { false, false },   // Format::RG_32_f             
      { false, false },   // Format::RGB_8               
      { false, false },   // Format::RGB_8_sRGB          
      { false, false },   // Format::RGB_8_sn            
      { false, false },   // Format::RGB_8_u             
      { false, false },   // Format::RGB_8_s             
      { false, false },   // Format::RGB_16              
      { false, false },   // Format::RGB_16_sn           
      { false, false },   // Format::RGB_16_u            
      { false, false },   // Format::RGB_16_s            
      { false, false },   // Format::RGB_16_hf           
      { false, false },   // Format::RGB_32_u            
      { false, false },   // Format::RGB_32_s            
      { false, false },   // Format::RGB_32_f            
      { false, false },   // Format::RGBA_8              
      { false, false },   // Format::RGBA_8_sRGB         
      { false, false },   // Format::RGBA_8_sn           
      { false, false },   // Format::RGBA_8_u            
      { false, false },   // Format::RGBA_8_s            
      { false, false },   // Format::RGBA_16             
      { false, false },   // Format::RGBA_16_sn          
      { false, false },   // Format::RGBA_16_u           
      { false, false },   // Format::RGBA_16_s           
      { false, false },   // Format::RGBA_16_hf          
      { false, false },   // Format::RGBA_32_u           
      { false, false },   // Format::RGBA_32_s           
      { false, false },   // Format::RGBA_32_f           
      { false, false },   // Format::D_16                
      { false, false },   // Format::D_24                
      { false, false },   // Format::D_24_8               
      { false, false },   // Format::D_32                 
      { false, false },   // Format::D_32_f               
      { false, false },   // Format::S_8                  
      { false, false },   // Format::DS_16_8              
      { false, false },   // Format::DS_24_8              
      { false, false },   // Format::DS_32_f_8            
      { false, false },   // Format::RGB_5_6_5            
      { false, false },   // Format::BGR_5_6_5            
      { false, false },   // Format::BGR_8                
      { false, false },   // Format::BGR_8_sRGB           
      { false, false },   // Format::BGR_8_sn             
      { false, false },   // Format::BGR_8_u              
      { false, false },   // Format::BGR_8_s              
      { false, false },   // Format::RGB_11_11_10_uf      
      { false, false },   // Format::RGBE_9_9_9_5_uf      
      { false, false },   // Format::BGRA_8               
      { false, false },   // Format::BGRA_8_sRGB          
      { false, false },   // Format::BGRA_8_sn           
      { false, false },   // Format::BGRA_8_u            
      { false, false },   // Format::BGRA_8_s            
      { false, false },   // Format::BGRA_5_5_5_1        
      { false, false },   // Format::ARGB_1_5_5_5        
      { false, false },   // Format::ABGR_1_5_5_5         
      { false, false },   // Format::RGBA_10_10_10_2      
      { false, false },   // Format::RGBA_10_10_10_2_u   
      { false, false },   // Format::BGRA_10_10_10_2      
      { false, false },   // Format::BC1_RGB              
      { false, false },   // Format::BC1_RGB_sRGB         
      { false, false },   // Format::BC1_RGBA             
      { false, false },   // Format::BC1_RGBA_sRGB        
      { false, false },   // Format::BC2_RGBA_pRGB        
      { false, false },   // Format::BC2_RGBA             
      { false, false },   // Format::BC2_RGBA_sRGB        
      { false, false },   // Format::BC3_RGBA_pRGB        
      { false, false },   // Format::BC3_RGBA             
      { false, false },   // Format::BC3_RGBA_sRGB        
      { false, false },   // Format::BC4_R                
      { false, false },   // Format::BC4_R_sn             
      { false, false },   // Format::BC5_RG               
      { false, false },   // Format::BC5_RG_sn           
      { false, false },   // Format::BC6H_RGB_f           
      { false, false },   // Format::BC6H_RGB_uf          
      { false, false },   // Format::BC7_RGBA             
      { false, false },   // Format::BC7_RGBA_sRGB        
      { false, false },   // Format::ETC2_R_11            
      { false, false },   // Format::ETC2_R_11_sn         
      { false, false },   // Format::ETC2_RG_11           
      { false, false },   // Format::ETC2_RG_11_sn        
      { false, false },   // Format::ETC2_RGB_8           
      { false, false },   // Format::ETC2_RGB_8_sRGB      
      { false, false },   // Format::ETC2_RGBA_8          
      { false, false },   // Format::ETC2_RGBA_8_sRGB     
      { false, false },   // Format::ETC2_RGB8_A1        
      { false, false },   // Format::ETC2_RGB8_A1_sRGB   
      { false, false },   // Format::PVRTC_RGB_2         
      { false, false },   // Format::PVRTC_RGB_2_sRGB    
      { false, false },   // Format::PVRTC_RGB_4         
      { false, false },   // Format::PVRTC_RGB_4_sRGB    
      { false, false },   // Format::PVRTC_RGBA_2        
      { false, false },   // Format::PVRTC_RGBA_2_sRGB   
      { false, false },   // Format::PVRTC_RGBA_4        
      { false, false },   // Format::PVRTC_RGBA_4_sRGB   
      { false, false },   // Format::ASTC_4x4            
      { false, false },   // Format::ASTC_5x4            
      { false, false },   // Format::ASTC_5x5            
      { false, false },   // Format::ASTC_6x5            
      { false, false },   // Format::ASTC_6x6            
      { false, false },   // Format::ASTC_8x5            
      { false, false },   // Format::ASTC_8x6            
      { false, false },   // Format::ASTC_8x8            
      { false, false },   // Format::ASTC_10x5           
      { false, false },   // Format::ASTC_10x6           
      { false, false },   // Format::ASTC_10x8           
      { false, false },   // Format::ASTC_10x10          
      { false, false },   // Format::ASTC_12x10          
      { false, false },   // Format::ASTC_12x12          
      { false, false },   // Format::ASTC_4x4_sRGB       
      { false, false },   // Format::ASTC_5x4_sRGB       
      { false, false },   // Format::ASTC_5x5_sRGB       
      { false, false },   // Format::ASTC_6x5_sRGB       
      { false, false },   // Format::ASTC_6x6_sRGB       
      { false, false },   // Format::ASTC_8x5_sRGB       
      { false, false },   // Format::ASTC_8x6_sRGB       
      { false, false },   // Format::ASTC_8x8_sRGB       
      { false, false },   // Format::ASTC_10x5_sRGB      
      { false, false },   // Format::ASTC_10x6_sRGB      
      { false, false },   // Format::ASTC_10x8_sRGB      
      { false, false },   // Format::ASTC_10x10_sRGB     
      { false, false },   // Format::ASTC_12x10_sRGB     
      { false, false }    // Format::ASTC_12x12_sRGB   
      };
#endif

   bool TextureFormatIsDepth(const Format format)
   {
   if ( (format == Format::D_16  ) ||   
        (format == Format::D_24  ) ||   
        (format == Format::D_24_8) ||
        (format == Format::D_32  ) ||               
        (format == Format::D_32_f) )
      return true;

   return false;
   }

   bool TextureFormatIsStencil(const Format format)
   {
   if (format == Format::S_8)
      return true;

   return false;
   }

   bool TextureFormatIsDepthStencil(const Format format)
   {
   if ( (format == Format::DS_16_8  ) ||
        (format == Format::DS_24_8  ) ||
        (format == Format::DS_32_f_8) )
      return true;

   return false;
   }

   // Calculate texture mipmaps count
   uint32 TextureMipMapCount(const TextureState& state)
   {
   if (state.type == TextureType::Texture2DMultisample      ||
       state.type == TextureType::Texture2DMultisampleArray )   // also Rectangle & Buffer
      return 1;

   uint32 maxDimmension = state.width > state.height ? state.width : state.height;
   if (maxDimmension < state.depth)
      maxDimmension = state.depth;
   
   uint32 value = maxDimmension; //nextPowerOfTwo(maxDimmension);
   uint32 mipmaps;
   for(mipmaps = 1; ; ++mipmaps)
      {
      if (value == 1)
         break;
      value = value >> 1;
      }

   return mipmaps;
   }

   TextureCommon::TextureCommon()
   {
   }

   TextureCommon::TextureCommon(const TextureState& _state) :
      state(_state)
   {
   }

   TextureType TextureCommon::type(void) const
   {
   return state.type;
   }
   
   Format TextureCommon::format(void) const
   {
   return state.format;
   }

   uint32 TextureCommon::mipmaps(void) const
   {
   return state.mipmaps;
   }
   
   uint32 TextureCommon::size(const uint8 mipmap) const
   {
   if (mipmap > state.mipmaps)
      return 0;

   TextureCompressedBlockInfo block = TextureCompressionInfo[underlyingType(state.format)];
   uint32 mipWidth  = max(1U, static_cast<uint32>(state.width  >> mipmap));
   uint32 mipHeight = max(1U, static_cast<uint32>(state.height >> mipmap));

   if (block.compressed)
      return ((mipWidth  + (block.width - 1)) / block.width) * 
             ((mipHeight + (block.height - 1)) / block.height) *
             block.size;

   return mipWidth * mipHeight * block.size;
   }

   uint32 TextureCommon::width(const uint8 mipmap) const
   {
   // If mipmap exceeds total mipmaps count, result will be 0
   return state.width >> mipmap;
   }
   
   uint32 TextureCommon::height(const uint8 mipmap) const
   {
   // If mipmap exceeds total mipmaps count, result will be 0
   return state.height >> mipmap;
   }
   
   uint32 TextureCommon::depth(const uint8 mipmap) const
   {
   // If mipmap exceeds total mipmaps count, result will be 0
   return state.depth >> mipmap;
   }
   
   uint32v3 TextureCommon::resolution(const uint8 mipmap) const
   {
   uint32 width  = state.width >> mipmap;
   uint32 height = state.height >> mipmap;
   uint32 depth  = state.depth >> mipmap;
   
   // If mipmap exceeds total mipmaps count, result will be all 0's
   // otherwise other dimensions are properly clamped to 1's.
   if ((width > 0) || (height > 0) || (depth > 0))
      {
      if (width  == 0) width  = 1;
      if (height == 0) height = 1;
      if (depth  == 0) depth  = 1;
      }

   return uint32v3(width, height, depth);
   }
   
   uint16 TextureCommon::layers(void) const
   {
   return state.layers;
   }
   
   uint16 TextureCommon::samples(void) const
   {
   return state.samples;
   }

   bool TextureCommon::read(uint8* buffer, const uint8 mipmap, const uint16 surface) const
   {
   assert( 0 );
   return false;
   }
   
   Ptr<TextureView> TextureCommon::view() const
   {
   return ptr_dynamic_cast<TextureView, CommonTextureView>(Ptr<CommonTextureView>(textureView));
   }

   CommonTextureView::CommonTextureView(const TextureType _type,
         const Format   _format,
         const uint32v2 _mipmaps,
         const uint32v2 _layers) :
      viewType(_type),
      viewFormat(_format),
      mipmaps(_mipmaps),
      layers(_layers)
   {
   }
   
   TextureType CommonTextureView::type(void) const
   {
   return viewType;
   }
   
   Format CommonTextureView::format(void) const
   {
   return viewFormat;
   }
      
   uint32v2 CommonTextureView::parentMipmaps(void) const
   {
   return mipmaps;
   }
   
   uint32v2 CommonTextureView::parentLayers(void) const
   {
   return layers;
   }

   uint32 CommonDevice::texelSize(const Format format)
   {
   return TextureCompressionInfo[underlyingType(format)].size;
   }

   }
}
