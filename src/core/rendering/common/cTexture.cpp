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

#include "core/rendering/common/texture.h"

// TODO: Rename to texture.cpp and move one level when old interface will be completly removed

namespace en
{
   namespace gpu
   {
   TextureState::TextureState() :
      type(Texture2D),
      format(FormatUnsupported),
      width(1),
      height(1),
      depth(1),
      layers(1),
      samples(1),
      mipmaps(1)
   {
   }     
   
   TextureState::TextureState(const TextureType _type, 
      const TextureFormat _format,
      const uint16 _width,
      const uint16 _height,
      const uint16 _depth,
      const uint16 _layers,
      const uint16 _samples )
   {
   type    = _type;
   format  = _format;
   width   = _width;
   height  = _height;
   depth   = _depth;
   layers  = _layers;
   samples = _samples;
   mipmaps = 1;
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
   static const TextureCompressedBlockInfo TextureCompressionInfo[TextureFormatsCount] = 
      {
      {  0,  0,  0, false },   // FormatUnsupported         
      {  0,  0,  1, false },   // FormatR_8                 
      {  0,  0,  1, false },   // FormatR_8_sn              
      {  0,  0,  1, false },   // FormatR_8_u               
      {  0,  0,  1, false },   // FormatR_8_s               
      {  0,  0,  2, false },   // FormatR_16                
      {  0,  0,  2, false },   // FormatR_16_sn             
      {  0,  0,  2, false },   // FormatR_16_u              
      {  0,  0,  2, false },   // FormatR_16_s              
      {  0,  0,  2, false },   // FormatR_16_hf             
      {  0,  0,  4, false },   // FormatR_32_u              
      {  0,  0,  4, false },   // FormatR_32_s              
      {  0,  0,  4, false },   // FormatR_32_f              
      {  0,  0,  2, false },   // FormatRG_8                
      {  0,  0,  2, false },   // FormatRG_8_sn             
      {  0,  0,  2, false },   // FormatRG_8_u              
      {  0,  0,  2, false },   // FormatRG_8_s              
      {  0,  0,  4, false },   // FormatRG_16               
      {  0,  0,  4, false },   // FormatRG_16_sn            
      {  0,  0,  4, false },   // FormatRG_16_u             
      {  0,  0,  4, false },   // FormatRG_16_s             
      {  0,  0,  4, false },   // FormatRG_16_hf            
      {  0,  0,  8, false },   // FormatRG_32_u             
      {  0,  0,  8, false },   // FormatRG_32_s             
      {  0,  0,  8, false },   // FormatRG_32_f             
      {  0,  0,  3, false },   // FormatRGB_8   
      {  0,  0,  3, false },   // FormatRGB_8_sRGB              
      {  0,  0,  3, false },   // FormatRGB_8_sn            
      {  0,  0,  3, false },   // FormatRGB_8_u             
      {  0,  0,  3, false },   // FormatRGB_8_s                     
      {  0,  0,  6, false },   // FormatRGB_16              
      {  0,  0,  6, false },   // FormatRGB_16_sn           
      {  0,  0,  6, false },   // FormatRGB_16_u            
      {  0,  0,  6, false },   // FormatRGB_16_s            
      {  0,  0,  6, false },   // FormatRGB_16_hf           
      {  0,  0, 12, false },   // FormatRGB_32_u            
      {  0,  0, 12, false },   // FormatRGB_32_s            
      {  0,  0, 12, false },   // FormatRGB_32_f    
      {  0,  0,  4, false },   // FormatRGBA_8                      
      {  0,  0,  4, false },   // FormatRGBA_8_sRGB
      {  0,  0,  4, false },   // FormatRGBA_8_sn           
      {  0,  0,  4, false },   // FormatRGBA_8_u            
      {  0,  0,  4, false },   // FormatRGBA_8_s     
      {  0,  0,  8, false },   // FormatRGBA_16             
      {  0,  0,  8, false },   // FormatRGBA_16_sn          
      {  0,  0,  8, false },   // FormatRGBA_16_u           
      {  0,  0,  8, false },   // FormatRGBA_16_s           
      {  0,  0,  8, false },   // FormatRGBA_16_hf          
      {  0,  0, 16, false },   // FormatRGBA_32_u           
      {  0,  0, 16, false },   // FormatRGBA_32_s           
      {  0,  0, 16, false },   // FormatRGBA_32_f           
      {  0,  0,  2, false },   // FormatD_16                
      {  0,  0,  3, false },   // FormatD_24             // TODO: 3 bytes, or 4 bytes with padding?     
      {  0,  0,  4, false },   // FormatD_32                
      {  0,  0,  4, false },   // FormatD_32_f              
      {  0,  0,  1, false },   // FormatS_8                 
      {  0,  0,  4, false },   // FormatSD_8_24             
      {  0,  0,  8, false },   // FormatSD_8_32_f        // TODO: 5 bytes, or 8 bytes with padding?           
      {  0,  0,  2, false },   // FormatRGB_5_6_5           
      {  0,  0,  2, false },   // FormatBGR_5_6_5           
      {  0,  0,  3, false },   // FormatBGR_8               
      {  0,  0,  4, false },   // FormatBGR_10_11_11_f      
      {  0,  0,  4, false },   // FormatEBGR_5_9_9_9_f      
      {  0,  0,  6, false },   // FormatBGR_16              
      {  0,  0,  2, false },   // FormatRGBA_5_5_5_1        
      {  0,  0,  2, false },   // FormatBGRA_5_5_5_1        
      {  0,  0,  2, false },   // FormatARGB_1_5_5_5        
      {  0,  0,  2, false },   // FormatABGR_1_5_5_5        
      {  0,  0,  4, false },   // FormatABGR_8              
      {  0,  0,  4, false },   // FormatARGB_8              
      {  0,  0,  4, false },   // FormatBGRA_8              
      {  0,  0,  4, false },   // FormatRGBA_10_10_10_2     
      {  0,  0,  4, false },   // FormatRGBA_10_10_10_2_u   
      {  4,  4,  8, true  },   // FormatBC1_RGB             
      {  4,  4,  8, true  },   // FormatBC1_RGB_sRGB        
      {  4,  4,  8, true  },   // FormatBC1_RGBA            
      {  4,  4,  8, true  },   // FormatBC1_RGBA_sRGB       
      {  4,  4, 16, true  },   // FormatBC2_RGBA_pRGB       
      {  4,  4, 16, true  },   // FormatBC2_RGBA            
      {  4,  4, 16, true  },   // FormatBC2_RGBA_sRGB       
      {  4,  4, 16, true  },   // FormatBC3_RGBA_pRGB       
      {  4,  4, 16, true  },   // FormatBC3_RGBA            
      {  4,  4, 16, true  },   // FormatBC3_RGBA_sRGB       
      {  4,  4,  8, true  },   // FormatBC4_R               
      {  4,  4,  8, true  },   // FormatBC4_R_sn            
      {  4,  4, 16, true  },   // FormatBC5_RG              
      {  4,  4, 16, true  },   // FormatBC5_RG_sn           
      {  4,  4, 16, true  },   // FormatBC6H_RGB_f          
      {  4,  4, 16, true  },   // FormatBC6H_RGB_uf         
      {  4,  4, 16, true  },   // FormatBC7_RGBA            
      {  4,  4, 16, true  },   // FormatBC7_RGBA_sRGB       
      {  4,  4,  8, true  },   // FormatETC2_R_11           
      {  4,  4,  8, true  },   // FormatETC2_R_11_sn        
      {  4,  4, 16, true  },   // FormatETC2_RG_11          
      {  4,  4, 16, true  },   // FormatETC2_RG_11_sn       
      {  4,  4,  8, true  },   // FormatETC2_RGB_8          
      {  4,  4,  8, true  },   // FormatETC2_RGB_8_sRGB     
      {  4,  4, 16, true  },   // FormatETC2_RGBA_8         
      {  4,  4, 16, true  },   // FormatETC2_RGBA_8_sRGB    
      {  4,  4,  8, true  },   // FormatETC2_RGB8_A1        
      {  4,  4,  8, true  },   // FormatETC2_RGB8_A1_sRGB   
      {  4,  4,  8, true  },   // FormatPVRTC_RGB_2         
      {  4,  4,  8, true  },   // FormatPVRTC_RGB_2_sRGB    
      {  4,  4,  8, true  },   // FormatPVRTC_RGB_4         
      {  4,  4,  8, true  },   // FormatPVRTC_RGB_4_sRGB    
      {  4,  4,  8, true  },   // FormatPVRTC_RGBA_2        
      {  4,  4,  8, true  },   // FormatPVRTC_RGBA_2_sRGB   
      {  4,  4,  8, true  },   // FormatPVRTC_RGBA_4        
      {  4,  4,  8, true  },   // FormatPVRTC_RGBA_4_sRGB   
      {  4,  4, 16, true  },   // FormatASTC_4x4            
      {  5,  4, 16, true  },   // FormatASTC_5x4            
      {  5,  5, 16, true  },   // FormatASTC_5x5            
      {  6,  5, 16, true  },   // FormatASTC_6x5            
      {  6,  6, 16, true  },   // FormatASTC_6x6            
      {  8,  5, 16, true  },   // FormatASTC_8x5            
      {  8,  6, 16, true  },   // FormatASTC_8x6            
      {  8,  8, 16, true  },   // FormatASTC_8x8            
      { 10,  5, 16, true  },   // FormatASTC_10x5           
      { 10,  6, 16, true  },   // FormatASTC_10x6           
      { 10,  8, 16, true  },   // FormatASTC_10x8           
      { 10, 10, 16, true  },   // FormatASTC_10x10          
      { 12, 10, 16, true  },   // FormatASTC_12x10          
      { 12, 12, 16, true  },   // FormatASTC_12x12          
      {  4,  4, 16, true  },   // FormatASTC_4x4_sRGB       
      {  5,  4, 16, true  },   // FormatASTC_5x4_sRGB       
      {  5,  5, 16, true  },   // FormatASTC_5x5_sRGB       
      {  6,  5, 16, true  },   // FormatASTC_6x5_sRGB       
      {  6,  6, 16, true  },   // FormatASTC_6x6_sRGB       
      {  8,  5, 16, true  },   // FormatASTC_8x5_sRGB       
      {  8,  6, 16, true  },   // FormatASTC_8x6_sRGB       
      {  8,  8, 16, true  },   // FormatASTC_8x8_sRGB       
      { 10,  5, 16, true  },   // FormatASTC_10x5_sRGB      
      { 10,  6, 16, true  },   // FormatASTC_10x6_sRGB      
      { 10,  8, 16, true  },   // FormatASTC_10x8_sRGB      
      { 10, 10, 16, true  },   // FormatASTC_10x10_sRGB     
      { 12, 10, 16, true  },   // FormatASTC_12x10_sRGB     
      { 12, 12, 16, true  }    // FormatASTC_12x12_sRGB     
      };

#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   // API dependent texture information (filled in during API backend creation)
   bool TextureTypeSupported[TextureTypesCount] = 
      {
      false,   // Texture1D                
      false,   // Texture1DArray           
      false,   // Texture2D                
      false,   // Texture2DArray           
      false,   // Texture2DRectangle       
      false,   // Texture2DMultisample     
      false,   // Texture2DMultisampleArray
      false,   // Texture3D                
      false,   // TextureBuffer            
      false,   // TextureCubeMap           
      false    // TextureCubeMapArray      
      };

   TextureInfo TextureCapabilities[TextureFormatsCount] = 
      {
      { false, false },   // FormatUnsupported         
      { false, false },   // FormatR_8                 
      { false, false },   // FormatR_8_sn              
      { false, false },   // FormatR_8_u               
      { false, false },   // FormatR_8_s               
      { false, false },   // FormatR_16                
      { false, false },   // FormatR_16_sn             
      { false, false },   // FormatR_16_u              
      { false, false },   // FormatR_16_s              
      { false, false },   // FormatR_16_hf             
      { false, false },   // FormatR_32_u              
      { false, false },   // FormatR_32_s              
      { false, false },   // FormatR_32_f              
      { false, false },   // FormatRG_8                
      { false, false },   // FormatRG_8_sn             
      { false, false },   // FormatRG_8_u              
      { false, false },   // FormatRG_8_s              
      { false, false },   // FormatRG_16               
      { false, false },   // FormatRG_16_sn            
      { false, false },   // FormatRG_16_u             
      { false, false },   // FormatRG_16_s             
      { false, false },   // FormatRG_16_hf            
      { false, false },   // FormatRG_32_u             
      { false, false },   // FormatRG_32_s             
      { false, false },   // FormatRG_32_f             
      { false, false },   // FormatRGB_8   
      { false, false },   // FormatRGB_8_sRGB              
      { false, false },   // FormatRGB_8_sn            
      { false, false },   // FormatRGB_8_u             
      { false, false },   // FormatRGB_8_s                     
      { false, false },   // FormatRGB_16              
      { false, false },   // FormatRGB_16_sn           
      { false, false },   // FormatRGB_16_u            
      { false, false },   // FormatRGB_16_s            
      { false, false },   // FormatRGB_16_hf           
      { false, false },   // FormatRGB_32_u            
      { false, false },   // FormatRGB_32_s            
      { false, false },   // FormatRGB_32_f    
      { false, false },   // FormatRGBA_8                      
      { false, false },   // FormatRGBA_8_sRGB
      { false, false },   // FormatRGBA_8_sn           
      { false, false },   // FormatRGBA_8_u            
      { false, false },   // FormatRGBA_8_s     
      { false, false },   // FormatRGBA_16             
      { false, false },   // FormatRGBA_16_sn          
      { false, false },   // FormatRGBA_16_u           
      { false, false },   // FormatRGBA_16_s           
      { false, false },   // FormatRGBA_16_hf          
      { false, false },   // FormatRGBA_32_u           
      { false, false },   // FormatRGBA_32_s           
      { false, false },   // FormatRGBA_32_f           
      { false, false },   // FormatD_16                
      { false, false },   // FormatD_24                  
      { false, false },   // FormatD_32                
      { false, false },   // FormatD_32_f              
      { false, false },   // FormatS_8                 
      { false, false },   // FormatSD_8_24             
      { false, false },   // FormatSD_8_32_f                   
      { false, false },   // FormatRGB_5_6_5           
      { false, false },   // FormatBGR_5_6_5           
      { false, false },   // FormatBGR_8               
      { false, false },   // FormatBGR_10_11_11_f      
      { false, false },   // FormatEBGR_5_9_9_9_f      
      { false, false },   // FormatBGR_16              
      { false, false },   // FormatRGBA_5_5_5_1        
      { false, false },   // FormatBGRA_5_5_5_1        
      { false, false },   // FormatARGB_1_5_5_5        
      { false, false },   // FormatABGR_1_5_5_5        
      { false, false },   // FormatABGR_8              
      { false, false },   // FormatARGB_8              
      { false, false },   // FormatBGRA_8              
      { false, false },   // FormatRGBA_10_10_10_2     
      { false, false },   // FormatRGBA_10_10_10_2_u   
      { false, false },   // FormatBC1_RGB             
      { false, false },   // FormatBC1_RGB_sRGB        
      { false, false },   // FormatBC1_RGBA            
      { false, false },   // FormatBC1_RGBA_sRGB       
      { false, false },   // FormatBC2_RGBA_pRGB       
      { false, false },   // FormatBC2_RGBA            
      { false, false },   // FormatBC2_RGBA_sRGB       
      { false, false },   // FormatBC3_RGBA_pRGB       
      { false, false },   // FormatBC3_RGBA            
      { false, false },   // FormatBC3_RGBA_sRGB       
      { false, false },   // FormatBC4_R               
      { false, false },   // FormatBC4_R_sn            
      { false, false },   // FormatBC5_RG              
      { false, false },   // FormatBC5_RG_sn           
      { false, false },   // FormatBC6H_RGB_f          
      { false, false },   // FormatBC6H_RGB_uf         
      { false, false },   // FormatBC7_RGBA            
      { false, false },   // FormatBC7_RGBA_sRGB       
      { false, false },   // FormatETC2_R_11           
      { false, false },   // FormatETC2_R_11_sn        
      { false, false },   // FormatETC2_RG_11          
      { false, false },   // FormatETC2_RG_11_sn       
      { false, false },   // FormatETC2_RGB_8          
      { false, false },   // FormatETC2_RGB_8_sRGB     
      { false, false },   // FormatETC2_RGBA_8         
      { false, false },   // FormatETC2_RGBA_8_sRGB    
      { false, false },   // FormatETC2_RGB8_A1        
      { false, false },   // FormatETC2_RGB8_A1_sRGB   
      { false, false },   // FormatPVRTC_RGB_2         
      { false, false },   // FormatPVRTC_RGB_2_sRGB    
      { false, false },   // FormatPVRTC_RGB_4         
      { false, false },   // FormatPVRTC_RGB_4_sRGB    
      { false, false },   // FormatPVRTC_RGBA_2        
      { false, false },   // FormatPVRTC_RGBA_2_sRGB   
      { false, false },   // FormatPVRTC_RGBA_4        
      { false, false },   // FormatPVRTC_RGBA_4_sRGB   
      { false, false },   // FormatASTC_4x4            
      { false, false },   // FormatASTC_5x4            
      { false, false },   // FormatASTC_5x5            
      { false, false },   // FormatASTC_6x5            
      { false, false },   // FormatASTC_6x6            
      { false, false },   // FormatASTC_8x5            
      { false, false },   // FormatASTC_8x6            
      { false, false },   // FormatASTC_8x8            
      { false, false },   // FormatASTC_10x5           
      { false, false },   // FormatASTC_10x6           
      { false, false },   // FormatASTC_10x8           
      { false, false },   // FormatASTC_10x10          
      { false, false },   // FormatASTC_12x10          
      { false, false },   // FormatASTC_12x12          
      { false, false },   // FormatASTC_4x4_sRGB       
      { false, false },   // FormatASTC_5x4_sRGB       
      { false, false },   // FormatASTC_5x5_sRGB       
      { false, false },   // FormatASTC_6x5_sRGB       
      { false, false },   // FormatASTC_6x6_sRGB       
      { false, false },   // FormatASTC_8x5_sRGB       
      { false, false },   // FormatASTC_8x6_sRGB       
      { false, false },   // FormatASTC_8x8_sRGB       
      { false, false },   // FormatASTC_10x5_sRGB      
      { false, false },   // FormatASTC_10x6_sRGB      
      { false, false },   // FormatASTC_10x8_sRGB      
      { false, false },   // FormatASTC_10x10_sRGB     
      { false, false },   // FormatASTC_12x10_sRGB     
      { false, false }    // FormatASTC_12x12_sRGB     
      };
#endif

   // Calculate texture mipmaps count
   uint32 TextureMipMapCount(const TextureState& state)
   {
   if (state.type == Texture2DRectangle        ||
       state.type == Texture2DMultisample      ||
       state.type == Texture2DMultisampleArray ||
       state.type == TextureBuffer             )
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

   Texture::~Texture()
   {
   }

   TextureCommon::TextureCommon()
   {
   }

   TextureCommon::TextureCommon(const TextureState& _state) :
      state(_state)
   {
   }
   
   TextureCommon::~TextureCommon()
   {
   }

   TextureType TextureCommon::type(void) const
   {
   return state.type;
   }
   
   TextureFormat TextureCommon::format(void) const
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

   TextureCompressedBlockInfo block = TextureCompressionInfo[state.format];
   uint32 mipWidth  = max(1, state.width  >> mipmap);
   uint32 mipHeight = max(1, state.height >> mipmap);

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
   // If mipmap exceeds total mipmaps count, result will be 0
   return uint32v3( state.width >> mipmap, state.height >> mipmap, state.depth >> mipmap );
   }
   
   uint16 TextureCommon::layers(void) const
   {
   return state.layers;
   }
   
   uint16 TextureCommon::samples(void) const
   {
   return state.samples;
   }

   void* TextureCommon::map(const uint8 mipmap, const uint16 surface)
   {
   assert( 0 );
   return nullptr;
   }

   bool TextureCommon::unmap(void)
   {
   assert( 0 );
   return false;
   }

   bool TextureCommon::read(uint8* buffer, const uint8 mipmap, const uint16 surface) const
   {
   assert( 0 );
   return false;
   }

   }
}