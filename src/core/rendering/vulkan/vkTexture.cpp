

#include "core/rendering/vulkan/vkTexture.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/vulkan/vkDevice.h"

namespace en
{
   namespace gpu
   {
   //BGR_16                 ,  // - Special swizzled format for PNG used on Windows-based systems: http://www.libpng.org/pub/png/book/chapter08.html section 8.5.6

   // Last Verified during Vulkan 1.0 Release
   //
   const VkFormat TranslateTextureFormat[underlyingType(Format::Count)] = 
      { // Sized Internal Format                     
      VK_FORMAT_UNDEFINED                 ,   // Format::Unsupported
      VK_FORMAT_R8_UNORM                  ,   // Format::R_8                    Uncompressed formats:
      VK_FORMAT_R8_SRGB                   ,   // Format::R_8_sRGB        
      VK_FORMAT_R8_SNORM                  ,   // Format::R_8_sn               
      VK_FORMAT_R8_UINT                   ,   // Format::R_8_u                
      VK_FORMAT_R8_SINT                   ,   // Format::R_8_s                
      VK_FORMAT_R16_UNORM                 ,   // Format::R_16                 
      VK_FORMAT_R16_SNORM                 ,   // Format::R_16_sn              
      VK_FORMAT_R16_UINT                  ,   // Format::R_16_u               
      VK_FORMAT_R16_SINT                  ,   // Format::R_16_s               
      VK_FORMAT_R16_SFLOAT                ,   // Format::R_16_hf              
      VK_FORMAT_R32_UINT                  ,   // Format::R_32_u               
      VK_FORMAT_R32_SINT                  ,   // Format::R_32_s               
      VK_FORMAT_R32_SFLOAT                ,   // Format::R_32_f               
      VK_FORMAT_R8G8_UNORM                ,   // Format::RG_8    
      VK_FORMAT_R8G8_SRGB                 ,   // Format::RG_8_sRGB
      VK_FORMAT_R8G8_SNORM                ,   // Format::RG_8_sn              
      VK_FORMAT_R8G8_UINT                 ,   // Format::RG_8_u               
      VK_FORMAT_R8G8_SINT                 ,   // Format::RG_8_s               
      VK_FORMAT_R16G16_UNORM              ,   // Format::RG_16                
      VK_FORMAT_R16G16_SNORM              ,   // Format::RG_16_sn             
      VK_FORMAT_R16G16_UINT               ,   // Format::RG_16_u              
      VK_FORMAT_R16G16_SINT               ,   // Format::RG_16_s              
      VK_FORMAT_R16G16_SFLOAT             ,   // Format::RG_16_hf             
      VK_FORMAT_R32G32_UINT               ,   // Format::RG_32_u              
      VK_FORMAT_R32G32_SINT               ,   // Format::RG_32_s              
      VK_FORMAT_R32G32_SFLOAT             ,   // Format::RG_32_f  
      VK_FORMAT_R8G8B8_UNORM              ,   // Format::RGB_8                  - Not reccomended due to lack of memory aligment
      VK_FORMAT_R8G8B8_SRGB               ,   // Format::RGB_8_sRGB             - Not reccomended due to lack of memory aligment
      VK_FORMAT_R8G8B8_SNORM              ,   // Format::RGB_8_sn               - Not reccomended due to lack of memory aligment
      VK_FORMAT_R8G8B8_UINT               ,   // Format::RGB_8_u                - Not reccomended due to lack of memory aligment
      VK_FORMAT_R8G8B8_SINT               ,   // Format::RGB_8_s                - Not reccomended due to lack of memory aligment
      VK_FORMAT_R16G16B16_UNORM           ,   // Format::RGB_16
      VK_FORMAT_R16G16B16_SNORM           ,   // Format::RGB_16_sn
      VK_FORMAT_R16G16B16_UINT            ,   // Format::RGB_16_u
      VK_FORMAT_R16G16B16_SINT            ,   // Format::RGB_16_s
      VK_FORMAT_R16G16B16_SFLOAT          ,   // Format::RGB_16_hf
      VK_FORMAT_R32G32B32_UINT            ,   // Format::RGB_32_u
      VK_FORMAT_R32G32B32_SINT            ,   // Format::RGB_32_s
      VK_FORMAT_R32G32B32_SFLOAT          ,   // Format::RGB_32_f
      VK_FORMAT_R8G8B8A8_UNORM            ,   // Format::RGBA_8            
      VK_FORMAT_R8G8B8A8_SRGB             ,   // Format::RGBA_8_sRGB       
      VK_FORMAT_R8G8B8A8_SNORM            ,   // Format::RGBA_8_sn         
      VK_FORMAT_R8G8B8A8_UINT             ,   // Format::RGBA_8_u          
      VK_FORMAT_R8G8B8A8_SINT             ,   // Format::RGBA_8_s   
      VK_FORMAT_R16G16B16A16_UNORM        ,   // Format::RGBA_16              
      VK_FORMAT_R16G16B16A16_SNORM        ,   // Format::RGBA_16_sn           
      VK_FORMAT_R16G16B16A16_UINT         ,   // Format::RGBA_16_u            
      VK_FORMAT_R16G16B16A16_SINT         ,   // Format::RGBA_16_s            
      VK_FORMAT_R16G16B16A16_SFLOAT       ,   // Format::RGBA_16_hf           
      VK_FORMAT_R32G32B32A32_UINT         ,   // Format::RGBA_32_u            
      VK_FORMAT_R32G32B32A32_SINT         ,   // Format::RGBA_32_s            
      VK_FORMAT_R32G32B32A32_SFLOAT       ,   // Format::RGBA_32_f        
      VK_FORMAT_D16_UNORM                 ,   // Format::D_16       
      VK_FORMAT_UNDEFINED                 ,   // Format::D_24       
      VK_FORMAT_X8_D24_UNORM_PACK32       ,   // Format::D_24_8                 - 24bit depth with 8 bit padding
      VK_FORMAT_UNDEFINED                 ,   // Format::D_32                   
      VK_FORMAT_D32_SFLOAT                ,   // Format::D_32_f                 
      VK_FORMAT_S8_UINT                   ,   // Format::S_8                    
      VK_FORMAT_D16_UNORM_S8_UINT         ,   // Format::DS_16_8                - Stored in separate D & S channel, order is implementation dependent doesn't matter as this is GPU only texture
      VK_FORMAT_D24_UNORM_S8_UINT         ,   // Format::DS_24_8                - Stored in separate D & S channel, order is implementation dependent doesn't matter as this is GPU only texture
      VK_FORMAT_D32_SFLOAT_S8_UINT        ,   // Format::DS_32_f_8              - Stored in separate D & S channel, order is implementation dependent doesn't matter as this is GPU only texture
      VK_FORMAT_B5G6R5_UNORM_PACK16       ,   // Format::RGB_5_6_5              Packed/special formats:
      VK_FORMAT_R5G6B5_UNORM_PACK16       ,   // Format::BGR_5_6_5              - IOS supported, OSX not (MTLPixelFormatB5G6R5Unorm, IOS only)
      VK_FORMAT_B8G8R8_UNORM              ,   // Format::BGR_8                  - Special swizzled format for PNG used on Windows-based systems: http://www.libpng.org/pub/png/book/chapter08.html section 8.5.6
      VK_FORMAT_B8G8R8_SRGB               ,   // Format::BGR_8_sRGB             
      VK_FORMAT_B8G8R8_SNORM              ,   // Format::BGR_8_sn               
      VK_FORMAT_B8G8R8_UINT               ,   // Format::BGR_8_u                
      VK_FORMAT_B8G8R8_SINT               ,   // Format::BGR_8_s                
      VK_FORMAT_B10G11R11_UFLOAT_PACK32   ,   // Format::RGB_11_11_10_uf        - Packed unsigned float for HDR Textures, UAV's and Render Targets
      VK_FORMAT_E5B9G9R9_UFLOAT_PACK32    ,   // Format::RGBE_9_9_9_5_uf        - Packed unsigned float for HDR Textures only
      VK_FORMAT_B8G8R8A8_UNORM            ,   // Format::BGRA_8                 - (MTLPixelFormatBGRA8Unorm)
      VK_FORMAT_B8G8R8A8_SRGB             ,   // Format::BGRA_8_sRGB            - (MTLPixelFormatBGRA8Unorm_sRGB)
      VK_FORMAT_B8G8R8A8_SNORM            ,   // Format::BGRA_8_sn
      VK_FORMAT_B8G8R8A8_UINT             ,   // Format::BGRA_8_u
      VK_FORMAT_B8G8R8A8_SINT             ,   // Format::BGRA_8_s
      VK_FORMAT_A1R5G5B5_UNORM_PACK16     ,   // Format::BGRA_5_5_5_1
      VK_FORMAT_B5G5R5A1_UNORM_PACK16     ,   // Format::ARGB_1_5_5_5
      VK_FORMAT_R5G5B5A1_UNORM_PACK16     ,   // Format::ABGR_1_5_5_5           - IOS supported, OSX not (MTLPixelFormatA1BGR5Unorm, IOS only)
      VK_FORMAT_A2B10G10R10_UNORM_PACK32  ,   // Format::RGBA_10_10_10_2        - *Less precise than 8bit sRGB compression in dark areas
      VK_FORMAT_A2B10G10R10_UINT_PACK32   ,   // Format::RGBA_10_10_10_2_u    
      VK_FORMAT_A2R10G10B10_UNORM_PACK32  ,   // Format::BGRA_10_10_10_2        - Special swizzled format for 30bpp BMP used by Windows-based systems
      VK_FORMAT_BC1_RGB_UNORM_BLOCK       ,   // Format::BC1_RGB                Compressed formats:
      VK_FORMAT_BC1_RGB_SRGB_BLOCK        ,   // Format::BC1_RGB_sRGB           -S3TC DXT1
      VK_FORMAT_BC1_RGBA_UNORM_BLOCK      ,   // Format::BC1_RGBA               -S3TC DXT1
      VK_FORMAT_BC1_RGBA_SRGB_BLOCK       ,   // Format::BC1_RGBA_sRGB          -S3TC DXT1
      VK_FORMAT_UNDEFINED                 ,   // Format::BC2_RGBA_pRGB          -S3TC DXT2
      VK_FORMAT_BC2_UNORM_BLOCK           ,   // Format::BC2_RGBA               -S3TC DXT3
      VK_FORMAT_BC2_SRGB_BLOCK            ,   // Format::BC2_RGBA_sRGB          -S3TC DXT3
      VK_FORMAT_UNDEFINED                 ,   // Format::BC3_RGBA_pRGB          -S3TC DXT4
      VK_FORMAT_BC3_UNORM_BLOCK           ,   // Format::BC3_RGBA               -S3TC DXT5
      VK_FORMAT_BC3_SRGB_BLOCK            ,   // Format::BC3_RGBA_sRGB          -S3TC DXT5
      VK_FORMAT_BC4_UNORM_BLOCK           ,   // Format::BC4_R                  -RGTC
      VK_FORMAT_BC4_SNORM_BLOCK           ,   // Format::BC4_R_sn               -RGTC
      VK_FORMAT_BC5_UNORM_BLOCK           ,   // Format::BC5_RG                 -RGTC
      VK_FORMAT_BC5_SNORM_BLOCK           ,   // Format::BC5_RG_sn              -RGTC
      VK_FORMAT_BC6H_SFLOAT_BLOCK         ,   // Format::BC6H_RGB_f             -BPTC
      VK_FORMAT_BC6H_UFLOAT_BLOCK         ,   // Format::BC6H_RGB_uf            -BPTC
      VK_FORMAT_BC7_UNORM_BLOCK           ,   // Format::BC7_RGBA               -BPTC
      VK_FORMAT_BC7_SRGB_BLOCK            ,   // Format::BC7_RGBA_sRGB          -BPTC
      VK_FORMAT_EAC_R11_UNORM_BLOCK       ,   // Format::ETC2_R_11              -EAC
      VK_FORMAT_EAC_R11_SNORM_BLOCK       ,   // Format::ETC2_R_11_sn           -EAC
      VK_FORMAT_EAC_R11G11_UNORM_BLOCK    ,   // Format::ETC2_RG_11             -EAC
      VK_FORMAT_EAC_R11G11_SNORM_BLOCK    ,   // Format::ETC2_RG_11_sn          -EAC
      VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK   ,   // Format::ETC2_RGB_8             -ETC1
      VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK    ,   // Format::ETC2_RGB_8_sRGB        
      VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK ,   // Format::ETC2_RGBA_8            -EAC
      VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK  ,   // Format::ETC2_RGBA_8_sRGB       -EAC
      VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK ,   // Format::ETC2_RGB8_A1
      VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK  ,   // Format::ETC2_RGB8_A1_sRGB
      VK_FORMAT_UNDEFINED                 ,   // Format::PVRTC_RGB_2
      VK_FORMAT_UNDEFINED                 ,   // Format::PVRTC_RGB_2_sRGB
      VK_FORMAT_UNDEFINED                 ,   // Format::PVRTC_RGB_4
      VK_FORMAT_UNDEFINED                 ,   // Format::PVRTC_RGB_4_sRGB
      VK_FORMAT_UNDEFINED                 ,   // Format::PVRTC_RGBA_2
      VK_FORMAT_UNDEFINED                 ,   // Format::PVRTC_RGBA_2_sRGB
      VK_FORMAT_UNDEFINED                 ,   // Format::PVRTC_RGBA_4
      VK_FORMAT_UNDEFINED                 ,   // Format::PVRTC_RGBA_4_sRGB
      VK_FORMAT_ASTC_4x4_UNORM_BLOCK      ,   // Format::ASTC_4x4
      VK_FORMAT_ASTC_5x4_UNORM_BLOCK      ,   // Format::ASTC_5x4
      VK_FORMAT_ASTC_5x5_UNORM_BLOCK      ,   // Format::ASTC_5x5
      VK_FORMAT_ASTC_6x5_UNORM_BLOCK      ,   // Format::ASTC_6x5
      VK_FORMAT_ASTC_6x6_UNORM_BLOCK      ,   // Format::ASTC_6x6
      VK_FORMAT_ASTC_8x5_UNORM_BLOCK      ,   // Format::ASTC_8x5
      VK_FORMAT_ASTC_8x6_UNORM_BLOCK      ,   // Format::ASTC_8x6
      VK_FORMAT_ASTC_8x8_UNORM_BLOCK      ,   // Format::ASTC_8x8
      VK_FORMAT_ASTC_10x5_UNORM_BLOCK     ,   // Format::ASTC_10x5
      VK_FORMAT_ASTC_10x6_UNORM_BLOCK     ,   // Format::ASTC_10x6
      VK_FORMAT_ASTC_10x8_UNORM_BLOCK     ,   // Format::ASTC_10x8
      VK_FORMAT_ASTC_10x10_UNORM_BLOCK    ,   // Format::ASTC_10x10
      VK_FORMAT_ASTC_12x10_UNORM_BLOCK    ,   // Format::ASTC_12x10
      VK_FORMAT_ASTC_12x12_UNORM_BLOCK    ,   // Format::ASTC_12x12
      VK_FORMAT_ASTC_4x4_SRGB_BLOCK       ,   // Format::ASTC_4x4_sRGB
      VK_FORMAT_ASTC_5x4_SRGB_BLOCK       ,   // Format::ASTC_5x4_sRGB
      VK_FORMAT_ASTC_5x5_SRGB_BLOCK       ,   // Format::ASTC_5x5_sRGB
      VK_FORMAT_ASTC_6x5_SRGB_BLOCK       ,   // Format::ASTC_6x5_sRGB
      VK_FORMAT_ASTC_6x6_SRGB_BLOCK       ,   // Format::ASTC_6x6_sRGB
      VK_FORMAT_ASTC_8x5_SRGB_BLOCK       ,   // Format::ASTC_8x5_sRGB
      VK_FORMAT_ASTC_8x6_SRGB_BLOCK       ,   // Format::ASTC_8x6_sRGB
      VK_FORMAT_ASTC_8x8_SRGB_BLOCK       ,   // Format::ASTC_8x8_sRGB
      VK_FORMAT_ASTC_10x5_SRGB_BLOCK      ,   // Format::ASTC_10x5_sRGB
      VK_FORMAT_ASTC_10x6_SRGB_BLOCK      ,   // Format::ASTC_10x6_sRGB
      VK_FORMAT_ASTC_10x8_SRGB_BLOCK      ,   // Format::ASTC_10x8_sRGB
      VK_FORMAT_ASTC_10x10_SRGB_BLOCK     ,   // Format::ASTC_10x10_sRGB
      VK_FORMAT_ASTC_12x10_SRGB_BLOCK     ,   // Format::ASTC_12x10_sRGB
      VK_FORMAT_ASTC_12x12_SRGB_BLOCK     ,   // Format::ASTC_12x12_sRGB
      };

      // Formats deliberately not supported:
      //
      //   VK_FORMAT_R4G4_UNORM_PACK8          ,   // TextureFormat::GR_4
      //   VK_FORMAT_R4G4B4A4_UNORM_PACK16     ,   // TextureFormat::ABGR_4
      //   VK_FORMAT_B4G4R4A4_UNORM_PACK16     ,   // TextureFormat::ARGB_4
      //
      //   Below ones are Endiannes Dependent, therefore their Endiannes Independent variation is allowed.
      // 
      //   VK_FORMAT_A8B8G8R8_UNORM_PACK32     ,   // TextureFormat::RGBA_8     
      //   VK_FORMAT_A8B8G8R8_SRGB_PACK32      ,   // TextureFormat::RGBA_8_sRGB
      //   VK_FORMAT_A8B8G8R8_SNORM_PACK32     ,   // TextureFormat::RGBA_8_sn  
      //   VK_FORMAT_A8B8G8R8_UINT_PACK32      ,   // TextureFormat::RGBA_8_u   
      //   VK_FORMAT_A8B8G8R8_SINT_PACK32      ,   // TextureFormat::RGBA_8_s   


   // SAMPLER


   // Optimisation: This table is not needed. Backend type can be directly cast to Vulkan type.
   static const VkFilter TranslateSamplerFilter[underlyingType(SamplerFilter::Count)] = 
      {
      VK_FILTER_NEAREST, // Nearest
      VK_FILTER_LINEAR   // Linear
      };

   // Optimisation: This table is not needed. Backend type can be directly cast to Vulkan type.
   static const VkSamplerMipmapMode TranslateSamplerMipMapMode[underlyingType(SamplerMipMapMode::Count)] =
      {
      VK_SAMPLER_MIPMAP_MODE_NEAREST, // Nearest
      VK_SAMPLER_MIPMAP_MODE_LINEAR   // Linear
      };

   // Optimisation: This table is not needed. Backend type can be directly cast to Vulkan type.
   static const VkSamplerAddressMode TranslateSamplerAdressing[underlyingType(SamplerAdressing::Count)] = 
      {
      VK_SAMPLER_ADDRESS_MODE_REPEAT,               // Repeat
      VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,      // RepeatMirrored   
      VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,        // ClampToEdge
      VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,      // ClampToBorder
      VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE  // MirrorClampToEdge
      };

   // Optimisation: This table is not needed. Backend type can be directly cast to Vulkan type.
   static const VkCompareOp TranslateCompareOperation[underlyingType(CompareOperation::Count)] = 
      {
      VK_COMPARE_OP_NEVER,              // Never
      VK_COMPARE_OP_LESS,               // Less
      VK_COMPARE_OP_EQUAL,              // Equal
      VK_COMPARE_OP_LESS_OR_EQUAL,      // LessOrEqual
      VK_COMPARE_OP_GREATER,            // Greater
      VK_COMPARE_OP_NOT_EQUAL,          // NotEqual
      VK_COMPARE_OP_GREATER_OR_EQUAL,   // GreaterOrEqual
      VK_COMPARE_OP_ALWAYS              // Always
      };

   static const VkBorderColor TranslateSamplerBorder[underlyingType(SamplerBorder::Count)] = 
      {
      VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, // TransparentBlack
      VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,      // OpaqueBlack
      VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,      // OpaqueWhite
      };

   // TODO: 
   // VK_BORDER_COLOR_INT_TRANSPARENT_BLACK
   // VK_BORDER_COLOR_INT_OPAQUE_BLACK
   // VK_BORDER_COLOR_INT_OPAQUE_WHITE

   Ptr<Sampler> VulkanDevice::Create(const SamplerState& state)
   {
   Ptr<SamplerVK> sampler = nullptr;
   
   VkSamplerCreateInfo samplerInfo = {};
   samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
   samplerInfo.pNext                   = nullptr;
   samplerInfo.flags                   = 0;                                                               // Reserved for future
   samplerInfo.magFilter               = static_cast<VkFilter>(underlyingType(state.magnification));      // Optimisation: TranslateSamplerFilter[underlyingType(state.magnification)];
   samplerInfo.minFilter               = static_cast<VkFilter>(underlyingType(state.minification));       // Optimisation: TranslateSamplerFilter[underlyingType(state.minification)];
   samplerInfo.mipmapMode              = static_cast<VkSamplerMipmapMode>(underlyingType(state.mipmap));  // Optimisation: TranslateSamplerMipMapMode[underlyingType(state.mipmap)];
   samplerInfo.addressModeU            = static_cast<VkSamplerAddressMode>(underlyingType(state.coordU)); // Optimisation: TranslateSamplerAdressing[underlyingType(state.coordU)];
   samplerInfo.addressModeV            = static_cast<VkSamplerAddressMode>(underlyingType(state.coordV)); // Optimisation: TranslateSamplerAdressing[underlyingType(state.coordV)];
   samplerInfo.addressModeW            = static_cast<VkSamplerAddressMode>(underlyingType(state.coordW)); // Optimisation: TranslateSamplerAdressing[underlyingType(state.coordW)];
   samplerInfo.mipLodBias              = state.LodBias;
   samplerInfo.anisotropyEnable        = (state.anisotropy < 1.0f) ? VK_FALSE : VK_TRUE;
   samplerInfo.maxAnisotropy           = min(state.anisotropy, support.maxAnisotropy);                    // [1.0f - VkPhysicalDeviceLimits::maxSamplerAnisotropy]
   samplerInfo.compareEnable           = state.compare == CompareOperation::Always ? VK_FALSE :  VK_TRUE;
   samplerInfo.compareOp               = static_cast<VkCompareOp>(underlyingType(state.compare));         // Optimisation: TranslateCompareOperation[underlyingType(state.compare)];
   samplerInfo.minLod                  = state.minLod;
   samplerInfo.maxLod                  = state.maxLod;
   samplerInfo.borderColor             = TranslateSamplerBorder[underlyingType(state.borderColor)];
   samplerInfo.unnormalizedCoordinates = VK_FALSE;  // TODO: Unnormalized coordinates are not supported for now. (both supported by Vulkan & Metal)

   VkSampler handle;
   Profile( this, vkCreateSampler(device, &samplerInfo, nullptr, &handle) )
   if ( lastResult[0] >= 0 )  // FIXME !!! Assuming first thread !!
      {
      sampler = new SamplerVK();
      sampler->handle = handle;
      }

   return ptr_dynamic_cast<Sampler, SamplerVK>(sampler);
   };


   // TEXTURE


   // Binding order:
   //
   // Pipeline <- Image View <- Image <- Memory

   // Creation hierarchy:
   //
   // Device -> Heap
   //           Heap -> Buffer
   //           Heap -> Image
   //                   Image   -> View
   //           Heap -> Texture
   //                   Texture -> View

   static const VkImageType TranslateImageType[underlyingType(TextureType::Count)] = 
      {
      VK_IMAGE_TYPE_1D,             // Texture1D
      VK_IMAGE_TYPE_1D,             // Texture1DArray
      VK_IMAGE_TYPE_2D,             // Texture2D
      VK_IMAGE_TYPE_2D,             // Texture2DArray
      VK_IMAGE_TYPE_2D,             // Texture2DMultisample
      VK_IMAGE_TYPE_2D,             // Texture2DMultisampleArray
      VK_IMAGE_TYPE_3D,             // Texture3D
      VK_IMAGE_TYPE_2D,             // TextureCubeMap
      VK_IMAGE_TYPE_2D              // TextureCubeMapArray
      };

   static const VkImageViewType TranslateViewType[underlyingType(TextureType::Count)] = 
      {
      VK_IMAGE_VIEW_TYPE_1D,        // Texture1D
      VK_IMAGE_VIEW_TYPE_1D_ARRAY,  // Texture1DArray
      VK_IMAGE_VIEW_TYPE_2D,        // Texture2D
      VK_IMAGE_VIEW_TYPE_2D_ARRAY,  // Texture2DArray
      VK_IMAGE_VIEW_TYPE_2D,        // Texture2DMultisample
      VK_IMAGE_VIEW_TYPE_2D_ARRAY,  // Texture2DMultisampleArray
      VK_IMAGE_VIEW_TYPE_3D,        // Texture3D
      VK_IMAGE_VIEW_TYPE_CUBE,      // TextureCubeMap
      VK_IMAGE_VIEW_TYPE_CUBE_ARRAY // TextureCubeMapArray
      };

   // Optional: Can be replaced with min(nextPowerOfTwo(samples), 64u)
   VkSampleCountFlagBits TranslateSamplesCount(const uint32 samples)
   {
   if (samples == 2u)  return VK_SAMPLE_COUNT_2_BIT;
   if (samples == 4u)  return VK_SAMPLE_COUNT_4_BIT;
   if (samples == 8u)  return VK_SAMPLE_COUNT_8_BIT;
   if (samples == 16u) return VK_SAMPLE_COUNT_16_BIT;
   if (samples == 32u) return VK_SAMPLE_COUNT_32_BIT;
   if (samples == 64u) return VK_SAMPLE_COUNT_64_BIT;

   return VK_SAMPLE_COUNT_1_BIT;
   }

   VkImageAspectFlags TranslateImageAspect(const Format format)
   {
   if ( (format == Format::D_16   ) ||   
        (format == Format::D_24   ) ||   
        (format == Format::D_24_8 ) ||
        (format == Format::D_32   ) ||               
        (format == Format::D_32_f ) )
      return VK_IMAGE_ASPECT_DEPTH_BIT;
   else
   if (format == Format::S_8)
      return VK_IMAGE_ASPECT_STENCIL_BIT;
   else
   if ( (format == Format::DS_16_8  ) ||
        (format == Format::DS_24_8  ) ||
        (format == Format::DS_32_f_8) )
      return ( VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT );

   return VK_IMAGE_ASPECT_COLOR_BIT;
   }

   //bool VulkanDevice::getMemoryType(uint32 allowedTypes, VkFlags properties, uint32* memoryType)
   //{
   //for(uint32 i=0; i<memory.memoryTypeCount; ++i)
   //   if (checkBit(allowedTypes, i))
   //      if ((memory.memoryTypes[i].propertyFlags & properties) == properties)
   //         {
   //         *memoryType = i;
   //         return true;
   //         }

   //return false;
   //}

   //VkDeviceMemory VulkanDevice::allocMemory(VkMemoryRequirements requirements, VkFlags properties)
   //{
   //VkDeviceMemory handle;

   //// Find Memory Type that best suits required allocation
   //uint32 index = VK_MAX_MEMORY_TYPES;
   //for(uint32 i=0; i<memory.memoryTypeCount; ++i)
   //   {
   //   // Memory Type needs to be able to support requested allocation
   //   if (checkBit(requirements.memoryTypeBits, (i+1)))
   //      // Memory Type needs to support at least requested sub-set of memory properties
   //      if ((memory.memoryTypes[i].propertyFlags & properties) == properties)
   //         index = i;

   //   // If this memory type cannot be used to allocate requested resource, continue search
   //   if (index == VK_MAX_MEMORY_TYPES)
   //      continue;

   //   // Try to allocate memory on Heap supporting this memory type
   //   VkMemoryAllocateInfo allocInfo;
   //   allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
   //   allocInfo.pNext           = nullptr;
   //   allocInfo.allocationSize  = requirements.size;
   //   allocInfo.memoryTypeIndex = index;
   //   
   //   Profile( this, vkAllocateMemory(device, &allocInfo, &defaultAllocCallbacks, &handle) )
   //   if (lastResult[0] == VK_SUCCESS)  // FIXME!! First thread assumed!
   //      return handle;
   //   }

   //// FAIL
   //return handle;
   //}




   // Use Cases:

   // 1) Depth/Stencil buffer for classic Depth Test (Write Only)
   //    TextureUsage::RenderTargetWrite
   //
   //    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
   //    On mobile we may want to add: VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT
   //
   // 2) Depth/Stencil buffer for Depth Test, later reused (for eg. as part of GBuffer)
   //    TextureUsage::Read
   //    TextureUsage::RenderTargetWrite
   //
   //    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
   //    VK_IMAGE_USAGE_SAMPLED_BIT
   //
   // 3) Temporary Color Attachment (for eg. part of Post Process)
   //    TextureUsage::Read
   //    TextureUsage::RenderTargetWrite
   //
   //    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
   //    On mobile we may want to add: VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT  - It may be completly stored in Tile Cache between passes
   //    TextureUsage::RenderTargetRead
   //                                  VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT      - and be input to next Post Process pass
   //
   // 4) GPU Private texture, used for Sampling only (VRAM ReadOnly)
   //    TextureUsage::Read
   //
   //    VK_IMAGE_USAGE_TRANSFER_DST_BIT - To first populate with data from CPU RAM
   //    VK_IMAGE_USAGE_SAMPLED_BIT
   //
   // 5) Temporary Linear(Staging Buffer backed) Texture for Data Upload to GPU Private Textures
   //    VK_IMAGE_USAGE_TRANSFER_SRC_BIT
   //
   // 6) ShadowMap (first rendered, then sampled)
   //    TextureUsage::Read
   //    TextureUsage::RenderTargetWrite
   //
   //    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
   //    VK_IMAGE_USAGE_SAMPLED_BIT
   //
   // Q) Are Load/Store/Atomic operations treated as Transfer Operations ? No. Only Blits.


   // VK_IMAGE_USAGE_TRANSFER_SRC_BIT             - It can be read from (for e.g. source of texture blit)
   // VK_IMAGE_USAGE_TRANSFER_DST_BIT             - It is written to (for e.g. ShadowMap)
   // VK_IMAGE_USAGE_SAMPLED_BIT                  - To Create View, and allow Sampling by the Shader -> Texture
   // VK_IMAGE_USAGE_STORAGE_BIT                  - To Create View, and allow Load/Store/Atomics by the Shader -> Image
   // VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT         - To Create View, Color or Resolve
   // VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT - To Create View, DepthStencil Attachment
   // VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT     - For Memoryless/Lazili allocated Color/Depth/Stencil Attachments
   // VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT         - Rasterizer Input Attachment





   Ptr<Texture> createTexture(VulkanDevice* gpu, const TextureState& state)
   {
   Ptr<TextureVK> texture = nullptr;

   VkImageCreateInfo textureInfo = {};
   textureInfo.sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
   textureInfo.pNext       = nullptr;
   textureInfo.flags       = 0;

   // Cube-Maps
   if (state.type == TextureType::TextureCubeMap ||
       state.type == TextureType::TextureCubeMapArray)
      textureInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

   // Multiple Views
   if (checkBits(underlyingType(state.usage), underlyingType(TextureUsage::MultipleViews)))
      {
      // If app will create different Views from this texture, we
      // assume that those Views can map different texel Formats.
      textureInfo.flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;

      // If app will create different Views from this texture, and
      // it's 2DArray, it's possible that app will want to create
      // CubeMap or CubeMapArray Views.
      if (state.type == TextureType::Texture2DArray)
         textureInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
      }

   // Sparse Textures
   if (checkBits(underlyingType(state.usage), underlyingType(TextureUsage::Sparse)))
      {
      textureInfo.flags |= VK_IMAGE_CREATE_SPARSE_BINDING_BIT;

      // TODO: Decide which of these flags combinations should be used:
      //
      // VK_IMAGE_CREATE_SPARSE_BINDING_BIT    // Uses Sparse Memory Binding
      // VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT  // Partially uses Sparse Memory Binding
      // VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT | VK_IMAGE_CREATE_SPARSE_ALIASED_BIT    // Uses Sparse Memory Binding, can Alias itself and others (require VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT) - useful for Sparse ShadowMaps ?
      }

   textureInfo.imageType   = TranslateImageType[underlyingType(state.type)];
   textureInfo.format      = TranslateTextureFormat[underlyingType(state.format)];
   textureInfo.extent      = { state.width, state.height, state.depth };
   textureInfo.mipLevels   = state.mipmaps;    // Starting from mipmap 0 (original image)
   textureInfo.arrayLayers = state.layers;
   textureInfo.samples     = static_cast<VkSampleCountFlagBits>(min(nextPowerOfTwo(state.samples), 64u));   // Optional: TranslateSamplesCount(state.samples);
   textureInfo.tiling      = VK_IMAGE_TILING_OPTIMAL;      // VK_IMAGE_TILING_LINEAR - For Linear Textures with optional padding, and SSBO's

   VkFlags properties = 0;
   bool canBeMemoryless = true;
   bool isMemoryless    = false;

   // Streamed or Static
   if (checkBits(underlyingType(state.usage), underlyingType(TextureUsage::Streamed)))
      {
      // TODO: Optimize it to specify Source/Destination only.
      textureInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT; // Streaming Source? (stream from GPU, GPU generated content?)
      textureInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT; // Definitely destination on Streaming

      // VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT  - can be mapped using vkMapMemory
      // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT - memory is always correct (in-sync) on both Host and Device
      // VK_MEMORY_PROPERTY_HOST_CACHED_BIT   - memory need to be manually synced after it's modified on one of Host or Device copies using  vkFlushMappedMemoryRanges and vkInvalidateMappedMemoryRanges 
      properties |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
      properties |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;

      canBeMemoryless = false;
      }
   else
      {
      // Read only textures need to be populated first using blit
      if (checkBits(underlyingType(state.usage), underlyingType(TextureUsage::Read)))
         textureInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

      properties |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      }

   if (checkBits(underlyingType(state.usage), underlyingType(TextureUsage::Read)))
      {
      textureInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
      canBeMemoryless   = false;
      }

   if (checkBits(underlyingType(state.usage), underlyingType(TextureUsage::Atomic)))
      {
      textureInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
      canBeMemoryless   = false;
      }

   // Render Target Attachment
   if ( checkBits(underlyingType(state.usage), underlyingType(TextureUsage::RenderTargetRead)) ||
        checkBits(underlyingType(state.usage), underlyingType(TextureUsage::RenderTargetWrite)) )
      {
      // Destination for Render Operations
      if (checkBits(underlyingType(state.usage), underlyingType(TextureUsage::RenderTargetWrite)))
         {
         if ( TextureFormatIsDepth(state.format)   ||
              TextureFormatIsStencil(state.format) ||
              TextureFormatIsDepthStencil(state.format) )
            textureInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
         else
            textureInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
      
         // TODO: In future use TextureCapabilities[].rendertarget to verify if Format is correct.
         }
      
      // Can be sourced in next Post Process Pass
      if (checkBits(underlyingType(state.usage), underlyingType(TextureUsage::RenderTargetRead)))
         {
         textureInfo.usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
         }

      // On mobile try to use Tile Cache instead of Memory Backing
      if (canBeMemoryless)
         {
         textureInfo.usage |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;

         properties |= VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;

         isMemoryless = true;
         }
      }

// It's possible that current GPU driver will distinguish different Queue Families
// for different use. For example it can have 10 Rendering Queues in one Family and
// only 1 Transfer Queue in other family. In such case, depending on resource
// usage, it may be required to share it between Families.
// if (queueFamiliesCount > 1)
//    {
//    textureInfo.sharingMode           = VK_SHARING_MODE_CONCURRENT; // Sharing between multiple Queue Families
//    textureInfo.queueFamilyIndexCount = queueFamiliesCount;         // Count of Queue Families (for eg. GPU has 40 Rendering and 10 Compute Queues grouped into 2 Families)
//    textureInfo.pQueueFamilyIndices   = queueFamilyIndices;         // pQueueFamilyIndices is a list of queue families that will access this image
//    }
// else
      {
      textureInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;  // No sharing between multiple Queue Families
      textureInfo.queueFamilyIndexCount = 0;
      textureInfo.pQueueFamilyIndices   = nullptr;
      }

   textureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // VK_IMAGE_LAYOUT_PREINITIALIZED allows application to init Texture contents but Texture needs to be Linear

   // Query additional info about Format required for this Texture to validate it
   VkImageFormatProperties formatInfo;

   Profile( gpu, vkGetPhysicalDeviceImageFormatProperties(gpu->handle, 
      textureInfo.format,
      textureInfo.imageType,
      textureInfo.tiling,
      textureInfo.usage,
      textureInfo.flags,
      &formatInfo) )

   uint32 threadId = Scheduler.core();

   assert( gpu->lastResult[threadId] != VK_ERROR_FORMAT_NOT_SUPPORTED );
   assert( textureInfo.extent.width  <= formatInfo.maxExtent.width );
   assert( textureInfo.extent.height <= formatInfo.maxExtent.height );
   assert( textureInfo.extent.depth  <= formatInfo.maxExtent.depth );
   assert( textureInfo.mipLevels     <= formatInfo.maxMipLevels );
   assert( textureInfo.arrayLayers   <= formatInfo.maxArrayLayers );
   assert( textureInfo.samples & formatInfo.sampleCounts );
   // There is no way to validate against formatInfo.maxResourceSize until texture is created.

   VkImage handle;
   Profile( gpu, vkCreateImage(gpu->device, &textureInfo, nullptr, &handle) )
   if (gpu->lastResult[threadId] >= 0)  
      {
      texture = new TextureVK(gpu, state);
      texture->handle = handle;

      // Query image requirements and validate if it can be created on this device
      ProfileNoRet( gpu, vkGetImageMemoryRequirements(gpu->device, handle, &texture->memoryRequirements) )
      assert( texture->memoryRequirements.size <= formatInfo.maxResourceSize );

      // Default view is not swizzling anything
      VkComponentMapping components = {};
      components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

      // Create default Texture View that will be used to access it
      VkImageViewCreateInfo viewInfo = {};
      viewInfo.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      viewInfo.pNext      = nullptr;
      viewInfo.flags      = 0; // Reserved
      viewInfo.image      = texture->handle;
      viewInfo.viewType   = TranslateViewType[underlyingType(state.type)];
      viewInfo.format     = textureInfo.format;
      viewInfo.components = components;  // Note: Set for clarity. It should default to 0's which corresponds to no swizzling.

      // Default view is representing whole resource
      viewInfo.subresourceRange.aspectMask     = TranslateImageAspect(state.format);
      viewInfo.subresourceRange.baseMipLevel   = 0;
      viewInfo.subresourceRange.levelCount     = state.mipmaps;
      viewInfo.subresourceRange.baseArrayLayer = 0;
      viewInfo.subresourceRange.layerCount     = state.layers;

      Profile( gpu, vkCreateImageView(gpu->device, &viewInfo, nullptr, &texture->view) )
      assert( !gpu->lastResult[threadId] );
      }

   return ptr_dynamic_cast<Texture, TextureVK>(texture);
   }

   TextureVK::TextureVK(VulkanDevice* _gpu, const TextureState& state) :
      gpu(_gpu),
      TextureCommon(state)
   {
   }

   TextureVK::~TextureVK()
   {
   // TODO: Deallocate from the Heap (let Heap allocator know that memory region is available again!).
   ProfileNoRet( gpu, vkDestroyImageView(gpu->device, view, nullptr) )
   ProfileNoRet( gpu, vkDestroyImage(gpu->device, handle, nullptr) )
   }
 
   }
}
#endif

   //                      // Texture won't be changed (static), and it's format can be 
   //                      // changed for specific GPU to e.g. gain performance or 
   //                      // emulate it on older HW.
   //imageInfo.flags     = XGL_IMAGE_CREATE_INVARIANT_DATA_BIT |     
   //                      XGL_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
   //
   //// XGL_IMAGE_USAGE_FLAGS:
   ////
   ////   XGL_IMAGE_USAGE_GENERAL                                 = 0x00000000,   // no special usage
   ////   XGL_IMAGE_USAGE_SHADER_ACCESS_READ_BIT                  = 0x00000001,   // shader read (e.g. texture, image)
   ////   XGL_IMAGE_USAGE_SHADER_ACCESS_WRITE_BIT                 = 0x00000002,   // shader write (e.g. image)
   ////   XGL_IMAGE_USAGE_SHADER_ACCESS_ATOMIC_BIT                = 0x00000004,   // shader atomic operations (e.g. image)
   ////   XGL_IMAGE_USAGE_TRANSFER_SOURCE_BIT                     = 0x00000008,   // used as a source for copies 
   ////   XGL_IMAGE_USAGE_TRANSFER_DESTINATION_BIT                = 0x00000010,   // used as a destination for copies
   ////   XGL_IMAGE_USAGE_TEXTURE_BIT                             = 0x00000020,   // opaque texture (2d, 3d, etc.)
   ////   XGL_IMAGE_USAGE_IMAGE_BIT                               = 0x00000040,   // opaque image (2d, 3d, etc.)
   ////   XGL_IMAGE_USAGE_COLOR_ATTACHMENT_BIT                    = 0x00000080,   // framebuffer color attachment
   ////   XGL_IMAGE_USAGE_DEPTH_STENCIL_BIT                       = 0x00000100,   // framebuffer depth/stencil 
 
