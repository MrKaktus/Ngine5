#include "core/rendering/vulkan/vkTexture.h"

namespace en
{
   namespace gpu
   {


   //SamplerState::SamplerState() :
   //   magFilter(Linear),
   //   minFilter(Trilinear),        // NearestMipmaped will be the fastest one if texture has mip-maps, otherwise Nearest is the fastest
   //   coordU(Clamp),
   //   coordV(Clamp),
   //   coordW(Clamp),
   //   borderColor(OpaqueBlack),
   //   depthCompare(LessOrEqual),   // ?? which compare is used by default in shadow mapping ? Less or 
   //   LODbias(0.0f),
   //   minLOD(0.0f),
   //   maxLOD(16.0f)                // If counting LOD's from 0, 14th is last LOD for 16Kx16K texture. 
   //{
   //}

   //}


   //static const VkImageType TranslateType[] = 
   //   {
   //   VK_IMAGE_1D, // Texture1D
   //   VK_IMAGE_1D, // Texture1DArray
   //   VK_IMAGE_2D, // Texture2D
   //   VK_IMAGE_2D, // Texture2DArray
   //   VK_IMAGE_2D, // Texture2DRectangle
   //   VK_IMAGE_2D, // Texture2DMultisample
   //   VK_IMAGE_2D, // Texture2DMultisampleArray
   //   VK_IMAGE_3D, // Texture3D
   //   VK_IMAGE_1D, // TextureBuffer
   //   VK_IMAGE_2D, // TextureCubeMap
   //   VK_IMAGE_2D  // TextureCubeMapArray
   //   };
   //
   ////static const VkFormat TranslateFormat[] =
   ////   {
   ////
   ////   };
   //
   //
   ////Ptr<Texture> Create(const TextureState& state)
   ////{
   ////uint32 mipmaps = TextureMipMapCount(state);
   ////
   ////VkExtent3D imageDimmensions;
   ////imageDimmensions.width  = state.width;
   ////imageDimmensions.height = state.height;
   ////imageDimmensions.depth  = state.depth;
   ////
   ////VkImageCreateInfo imageInfo;
   ////imageInfo.sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
   ////imageInfo.pNext     = nullptr;
   ////imageInfo.imageType = TranslateType[state.type];
   ////imageInfo.format    = TranslateFormat[state.format];
   ////imageInfo.extent    = imageDimmensions;
   ////imageInfo.mipLevels = mipmaps;
   ////imageInfo.arraySize = state.layers;
   ////imageInfo.samples   = state.samples;
   ////
   ////
   ////imageInfo.tiling    = XGL_LINEAR_TILING;     // Can be XGL_OPTIMAL_TILING
   ////
   ////                      // This will be classic 2D texture read by shaders
   ////imageInfo.usage;    = XGL_IMAGE_USAGE_SHADER_ACCESS_READ_BIT |
   ////                      XGL_IMAGE_USAGE_TEXTURE_BIT;
   ////
   ////                      // Texture won't be changed (static), and it's format can be 
   ////                      // changed for specific GPU to e.g. gain performance or 
   ////                      // emulate it on older HW.
   ////imageInfo.flags     = XGL_IMAGE_CREATE_INVARIANT_DATA_BIT |     
   ////                      XGL_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
   ////
   ////// XGL_IMAGE_USAGE_FLAGS:
   //////
   //////   XGL_IMAGE_USAGE_GENERAL                                 = 0x00000000,   // no special usage
   //////   XGL_IMAGE_USAGE_SHADER_ACCESS_READ_BIT                  = 0x00000001,   // shader read (e.g. texture, image)
   //////   XGL_IMAGE_USAGE_SHADER_ACCESS_WRITE_BIT                 = 0x00000002,   // shader write (e.g. image)
   //////   XGL_IMAGE_USAGE_SHADER_ACCESS_ATOMIC_BIT                = 0x00000004,   // shader atomic operations (e.g. image)
   //////   XGL_IMAGE_USAGE_TRANSFER_SOURCE_BIT                     = 0x00000008,   // used as a source for copies 
   //////   XGL_IMAGE_USAGE_TRANSFER_DESTINATION_BIT                = 0x00000010,   // used as a destination for copies
   //////   XGL_IMAGE_USAGE_TEXTURE_BIT                             = 0x00000020,   // opaque texture (2d, 3d, etc.)
   //////   XGL_IMAGE_USAGE_IMAGE_BIT                               = 0x00000040,   // opaque image (2d, 3d, etc.)
   //////   XGL_IMAGE_USAGE_COLOR_ATTACHMENT_BIT                    = 0x00000080,   // framebuffer color attachment
   //////   XGL_IMAGE_USAGE_DEPTH_STENCIL_BIT                       = 0x00000100,   // framebuffer depth/stencil 
   ////
   ////// XGL_IMAGE_CREATE_FLAGS:
   //////   XGL_IMAGE_CREATE_INVARIANT_DATA_BIT                     = 0x00000001,
   //////   XGL_IMAGE_CREATE_CLONEABLE_BIT                          = 0x00000002,
   //////   XGL_IMAGE_CREATE_SHAREABLE_BIT                          = 0x00000004,
   //////   XGL_IMAGE_CREATE_SPARSE_BIT                             = 0x00000008,
   //////   XGL_IMAGE_CREATE_MUTABLE_FORMAT_BIT                     = 0x00000010,   // Allows image views to have different format than the base image
   ////
   ////XGL_IMAGE image;
   ////
   ////res = VkCreateImage( gpu[i].handle, &imageInfo, &image);
   ////
   ////res = VkSetFastClearColor( image, static_cast<const float>(float4(0.0f, 0.0f, 0.0f, 1.0f)) ); 
   ////
   ////res = VkSetFastClearDepth( image, 0.0f );
   ////
   ////res = VkGetImageSubresourceInfo( image,
   ////    const XGL_IMAGE_SUBRESOURCE*                pSubresource,
   ////                                  XGL_INFO_TYPE_SUBRESOURCE_LAYOUT
   ////    size_t*                                     pDataSize,
   ////    void*                                       pData);
   ////
   ////
   ////}



   //struct TranslateSampler
   //   {
   //   VkTexFilter      filter;
   //   VkTexMipmapMode  mipmaping;
   //   uint32           anisotropy;
   //   };
   //
   //static const TranslateSampler TranslateTextureFiltering[] = {
   //   { VK_TEX_FILTER_NEAREST, VK_TEX_MIPMAP_BASE,    1.0f  },   // Nearest               ( no mipmaps, no filtering )                   
   //   { VK_TEX_FILTER_NEAREST, VK_TEX_MIPMAP_NEAREST, 1.0f  },   // NearestMipmaped              
   //   { VK_TEX_FILTER_NEAREST, VK_TEX_MIPMAP_LINEAR,  1.0f  },   // NearestMipmapedSmooth ( linear between nearest samples from 2 mipmaps )     
   //   { VK_TEX_FILTER_LINEAR,  VK_TEX_MIPMAP_BASE,    1.0f  },   // Linear                ( no mipmaps, filtering )                       
   //   { VK_TEX_FILTER_LINEAR,  VK_TEX_MIPMAP_NEAREST, 1.0f  },   // Bilinear                     
   //   { VK_TEX_FILTER_LINEAR,  VK_TEX_MIPMAP_LINEAR,  1.0f  },   // Trilinear                                     
   //   { VK_TEX_FILTER_LINEAR,  VK_TEX_MIPMAP_LINEAR,  2.0f  },   // Anisotropic2x                
   //   { VK_TEX_FILTER_LINEAR,  VK_TEX_MIPMAP_LINEAR,  4.0f  },   // Anisotropic4x                
   //   { VK_TEX_FILTER_LINEAR,  VK_TEX_MIPMAP_LINEAR,  8.0f  },   // Anisotropic8x                
   //   { VK_TEX_FILTER_LINEAR,  VK_TEX_MIPMAP_LINEAR,  16.0f } }; // Anisotropic16x               
   //
   //static const VkTexAddress TranslateTextureWraping[] = 
   //   {
   //   VK_TEX_ADDRESS_CLAMP,       // Clamp
   //   VK_TEX_ADDRESS_WRAP,        // Repeat
   //   VK_TEX_ADDRESS_MIRROR,      // RepeatMirrored               
   //   VK_TEX_ADDRESS_MIRROR_ONCE, // ClampMirrored
   //   VK_TEX_ADDRESS_CLAMP_BORDER // ClampToBorder
   //   };
   //
   //static const VkCompareFunc TranslateCompareFunction[] = 
   //   {
   //   VK_COMPARE_NEVER,         // Never                      
   //   VK_COMPARE_ALWAYS,        // Always                         
   //   VK_COMPARE_LESS,          // Less                          
   //   VK_COMPARE_LESS_EQUAL,    // LessOrEqual                    
   //   VK_COMPARE_EQUAL,         // Equal                          
   //   VK_COMPARE_GREATER,       // Greater                        
   //   VK_COMPARE_GREATER_EQUAL, // GreaterOrEqual                 
   //   VK_COMPARE_NOT_EQUAL      // NotEqual                       
   //   };
   //
   //static const VkBorderColorType TranslateBorderColor[] =
   //   {
   //   VK_BORDER_COLOR_OPAQUE_BLACK,      // OpaqueBlack
   //   VK_BORDER_COLOR_OPAQUE_WHITE,      // OpaqueWhite
   //   VK_BORDER_COLOR_TRANSPARENT_BLACK  // TransparentBlack
   //   };

   ////Ptr<Sampler> Create(const SamplerState& state)
   ////{
   ////Ptr<Sampler> sampler = nullptr;
   ////
   ////VkSamplerCreateInfo samplerInfo;
   ////samplerInfo.sType           = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
   ////samplerInfo.pNext           = nullptr;
   ////samplerInfo.magFilter       = TranslateTextureFiltering[state.magFilter].filter;     
   ////samplerInfo.minFilter       = TranslateTextureFiltering[state.minFilter].filter;
   ////samplerInfo.mipMode         = TranslateTextureFiltering[state.minFilter].mipmaping;
   ////samplerInfo.addressU        = TranslateTextureWraping[state.coordU];
   ////samplerInfo.addressV        = TranslateTextureWraping[state.coordV];
   ////samplerInfo.addressW        = TranslateTextureWraping[state.coordW];
   ////samplerInfo.mipLodBias      = state.LODbias;
   ////samplerInfo.maxAnisotropy   = TranslateTextureFiltering[state.magFilter].anisotropy;
   ////samplerInfo.compareFunc     = TranslateCompareFunction[state.depthCompare]; 
   ////samplerInfo.minLod          = state.minLOD; 
   ////samplerInfo.maxLod          = state.maxLOD; 
   ////samplerInfo.borderColorType = TranslateBorderColor[state.borderColor];
   ////
   ////VkSampler id;
   ////
   ////res = VkCreateSampler( gpu[i].handle, &samplerInfo, &id );
   ////if ( res >= 0 )
   ////   {
   ////   // TODO: Here create sampler instance 
   ////   }
   ////
   ////return sampler;
   //}

   }
}