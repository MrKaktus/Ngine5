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
#include <assert.h>

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
    layers(1),
    mipmaps(1),
    samples(1)
{
}     

TextureState::TextureState(
        const TextureType _type, 
        const Format _format,
        const TextureUsage _usage,
        const uint32 _width,
        const uint32 _height,
        const uint8 _mipmaps,
        const uint16 _layers,
        const uint8 _samples) :
    type(_type),
    format(_format),
    usage(_usage),
    width(_width),
    height(_height),
    layers(_layers),
    mipmaps(_mipmaps),
    samples(_samples)
{
    assert( width > 0 );
    assert( height > 0 );
    assert( mipmaps > 0 );
    assert( layers > 0 );
    assert( samples > 0 );
   
    // If layers count wasn't specified for Cube texture, autocorrect to 6
    if (type == TextureType::TextureCubeMap && layers == 1)
    {
        layers = 6;
    }

    // Verify that CubeMap Array layers count is multiple of 6
    if (type == TextureType::TextureCubeMapArray)
    {
        assert( layers % 6 == 0 );
    }
      
    assert( layers <= 2048 );
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

uint8 TextureState::planes(void) const
{
    // TODO: Verify how those Depth-Stencil formats are stored by different GPU's
    //       Which GPU's store them separately, and which together.
    //       iOS HW, AMD store them as separate planes, NV packed (confirm), Intel?
    //
    // TODO: How Format::DS_16_8 is stored by most GPU's?
    // TODO: Can Format::DS_24_8 have both planes updated with one blit?
    //
    // TODO: Add support for YCbCr / 422 /420, HDR formats in future
    //
    if ( (format == Format::DS_16_8  ) ||
         (format == Format::DS_24_8  ) ||
         (format == Format::DS_32_f_8) )
    {
        return 2;
    }

    return 1;
}
   
// TODO: Metal YCbCr formats:
//
//   MTLPixelFormatGBGR422
//   MTLPixelFormatBGRG422
//
// TODO: Vulkan YCbCr formats:
//
//   VK_FORMAT_G8B8G8R8_422_UNORM
//   VK_FORMAT_B8G8R8G8_422_UNORM
// 3 VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM
// 2 VK_FORMAT_G8_B8R8_2PLANE_420_UNORM
// 3 VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM
// 2 VK_FORMAT_G8_B8R8_2PLANE_422_UNORM
// 3 VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM
//   VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16
//   VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16
//   VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16
// 3 VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16
// 2 VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16
// 3 VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16
// 2 VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16
// 3 VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16
//   VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16
//   VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16
//   VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16
// 3 VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16
// 2 VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16
// 3 VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16
// 2 VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16
// 3 VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16
//   VK_FORMAT_G16B16G16R16_422_UNORM
//   VK_FORMAT_B16G16R16G16_422_UNORM
// 3 VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM
// 2 VK_FORMAT_G16_B16R16_2PLANE_420_UNORM
// 3 VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM
// 2 VK_FORMAT_G16_B16R16_2PLANE_422_UNORM
// 3 VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM

// TODO: In future handle row size depending on plane for YCbCr formats
uint32 TextureState::rowSize(const uint8 mipmap, const uint8 plane) const
{
    if (mipmap >= mipmaps)
    {
        return 0;
    }

    TextureCompressedBlockInfo block = TextureCompressionInfo[underlyingType(format)];
    uint32 mipWidth = max(1U, static_cast<uint32>(width >> mipmap));

    if (block.compressed)
    {
        return ((mipWidth  + (block.width - 1)) / block.width) * block.blockSize;
    }

    if (plane == 1)
    {
        return mipWidth * samples * block.secondarySize;
    }

    return mipWidth * samples * block.blockSize;
}

// TODO: In future handle row size depending on plane for YCbCr formats
uint32 TextureState::rowsCount(const uint8 mipmap) const
{
    if (mipmap >= mipmaps)
    {
        return 0;
    }

    TextureCompressedBlockInfo block = TextureCompressionInfo[underlyingType(format)];
    uint32 mipHeight = max(1U, static_cast<uint32>(height >> mipmap));

    if (block.compressed)
    {
        return ((mipHeight + (block.height - 1)) / block.height);
    }

    return mipHeight;
}

uint32 TextureState::surfaceSize(const uint8 mipmap,
                                 const uint8 plane) const
{
    if (mipmap >= mipmaps)
    {
        return 0;
    }

    TextureCompressedBlockInfo block = TextureCompressionInfo[underlyingType(format)];
    uint32 mipWidth  = max(1U, static_cast<uint32>(width  >> mipmap));
    uint32 mipHeight = max(1U, static_cast<uint32>(height >> mipmap));

    // TODO: Can MSAA textures be compressed?
    if (block.compressed)
    {
        return ((mipWidth  + (block.width - 1)) / block.width) *
               ((mipHeight + (block.height - 1)) / block.height) *
               block.blockSize;
    }

    if (plane == 1)
    {
        return mipWidth * mipHeight * samples * block.secondarySize;
    }

    return mipWidth * mipHeight * samples * block.blockSize;
}

uint32 TextureState::mipWidth(const uint8 mipmap) const
{
    if (mipmap >= mipmaps)
    {
        return 0;
    }

    return max(1U, static_cast<uint32>(width >> mipmap));
}

uint32 TextureState::mipHeight(const uint8 mipmap) const
{
    if (mipmap >= mipmaps)
    {
        return 0;
    }
     
    return max(1U, static_cast<uint32>(height >> mipmap));
}

uint32 TextureState::mipDepth(const uint8 mipmap) const
{
    if (mipmap >= mipmaps)
    {
        return 0;
    }

    if (type != TextureType::Texture3D)
    {
        return 1;
    }

    return max(1U, static_cast<uint32>(layers >> mipmap));
}

// TODO: Mip resolution may differ for YCbCy surfaces
uint32v2 TextureState::mipResolution(const uint8 mipmap) const
{
    uint32v2 resolution(0,0);
   
    if (mipmap < mipmaps)
    {
        resolution.width  = max(1U, static_cast<uint32>(width >> mipmap));
        resolution.height = max(1U, static_cast<uint32>(height >> mipmap));
    }
      
    return resolution;
}

uint32v3 TextureState::mipVolume(const uint8 mipmap) const
{
    uint32v3 volume(0,0,0);
   
    if (mipmap < mipmaps)
    {
        volume.width  = max(1U, static_cast<uint32>(width >> mipmap));
        volume.height = max(1U, static_cast<uint32>(height >> mipmap));
        volume.depth  = 1;
   
        if (type == TextureType::Texture3D)
        {
            volume.depth = max(1U, static_cast<uint32>(layers >> mipmap));
        }
    }
      
    return volume;
}
   
   // TODO: use generateTextureMemoryLayout instead or other layout
   //
   // Amount of memory needed to store all surfaces of given mip level
   // (includes depth planes, cube faces and array layers).
//   uint32 TextureState::mipSize(const uint8 mipmap,
//                                const SurfacePlanes planes) const
//   {
//   if (mipmap >= mipmaps)
//      return 0;
//
//   uint32 size;
//   uint32 surface = surfaceSize(mipmap, planes);
//   if (type == TextureType::Texture3D)
//      {
//      uint32 depthPlanes = max(1U, static_cast<uint32>(layers >> mipmap));
//      size = surface * depthPlanes;
//      }
//   else
//      size = surface * layers;
//      
////   if (type == TextureType::TextureCubeMap ||
////       type == TextureType::TextureCubeMapArray)
////      size *= 6;
//
//   return size;
//   }

// Direct3D DXGI_FORMAT:    https://msdn.microsoft.com/en-us/library/windows/desktop/bb173059(v=vs.85).aspx
// Metal IOS Pixel Formats: https://developer.apple.com/library/prerelease/ios/documentation/Metal/Reference/MetalConstants_Ref/index.html#//apple_ref/c/tdef/MTLPixelFormat
// Metal OSX Pixel Formats: https://developer.apple.com/library/mac/documentation/Metal/Reference/MetalConstants_Ref/#//apple_ref/c/tdef/MTLPixelFormat
// 
// Direct3D D3DFMT:         https://msdn.microsoft.com/en-us/library/windows/desktop/bb172558(v=vs.85).aspx

// API independent texture compression into
const TextureCompressedBlockInfo TextureCompressionInfo[underlyingType(Format::Count)] =
{  // Width, Height, PaddedSize, PrimarySize, SecondarySize, Compressed
    {  0,  0,  0,  0, 0, false },   // Format::Unsupported
    {  1,  1,  1,  1, 0, false },   // Format::R_8
    {  1,  1,  1,  1, 0, false },   // Format::R_8_sRGB
    {  1,  1,  1,  1, 0, false },   // Format::R_8_sn
    {  1,  1,  1,  1, 0, false },   // Format::R_8_u
    {  1,  1,  1,  1, 0, false },   // Format::R_8_s
    {  1,  1,  2,  2, 0, false },   // Format::R_16
    {  1,  1,  2,  2, 0, false },   // Format::R_16_sn
    {  1,  1,  2,  2, 0, false },   // Format::R_16_u
    {  1,  1,  2,  2, 0, false },   // Format::R_16_s
    {  1,  1,  2,  2, 0, false },   // Format::R_16_hf
    {  1,  1,  4,  4, 0, false },   // Format::R_32_u
    {  1,  1,  4,  4, 0, false },   // Format::R_32_s
    {  1,  1,  4,  4, 0, false },   // Format::R_32_f
    {  1,  1,  2,  2, 0, false },   // Format::RG_8
    {  1,  1,  2,  2, 0, false },   // Format::RG_8_sRGB
    {  1,  1,  2,  2, 0, false },   // Format::RG_8_sn
    {  1,  1,  2,  2, 0, false },   // Format::RG_8_u
    {  1,  1,  2,  2, 0, false },   // Format::RG_8_s
    {  1,  1,  4,  4, 0, false },   // Format::RG_16
    {  1,  1,  4,  4, 0, false },   // Format::RG_16_sn
    {  1,  1,  4,  4, 0, false },   // Format::RG_16_u
    {  1,  1,  4,  4, 0, false },   // Format::RG_16_s
    {  1,  1,  4,  4, 0, false },   // Format::RG_16_hf
    {  1,  1,  8,  8, 0, false },   // Format::RG_32_u
    {  1,  1,  8,  8, 0, false },   // Format::RG_32_s
    {  1,  1,  8,  8, 0, false },   // Format::RG_32_f
    {  1,  1,  4,  3, 0, false },   // Format::RGB_8             (internally padded to 4 bytes)
    {  1,  1,  4,  3, 0, false },   // Format::RGB_8_sRGB        (internally padded to 4 bytes)
    {  1,  1,  4,  3, 0, false },   // Format::RGB_8_sn          (internally padded to 4 bytes)
    {  1,  1,  4,  3, 0, false },   // Format::RGB_8_u           (internally padded to 4 bytes)
    {  1,  1,  4,  3, 0, false },   // Format::RGB_8_s           (internally padded to 4 bytes)
    {  1,  1,  8,  6, 0, false },   // Format::RGB_16            (how is it padded in GPU memory? doest fit 64KB tiles)
    {  1,  1,  8,  6, 0, false },   // Format::RGB_16_sn         (how is it padded in GPU memory? doest fit 64KB tiles)
    {  1,  1,  8,  6, 0, false },   // Format::RGB_16_u          (how is it padded in GPU memory? doest fit 64KB tiles)
    {  1,  1,  8,  6, 0, false },   // Format::RGB_16_s          (how is it padded in GPU memory? doest fit 64KB tiles)
    {  1,  1,  8,  6, 0, false },   // Format::RGB_16_hf         (how is it padded in GPU memory? doest fit 64KB tiles)
    {  1,  1, 16, 12, 0, false },   // Format::RGB_32_u          (how is it padded in GPU memory? doest fit 64KB tiles)
    {  1,  1, 16, 12, 0, false },   // Format::RGB_32_s          (how is it padded in GPU memory? doest fit 64KB tiles)
    {  1,  1, 16, 12, 0, false },   // Format::RGB_32_f          (how is it padded in GPU memory? doest fit 64KB tiles)
    {  1,  1,  4,  4, 0, false },   // Format::RGBA_8
    {  1,  1,  4,  4, 0, false },   // Format::RGBA_8_sRGB
    {  1,  1,  4,  4, 0, false },   // Format::RGBA_8_sn
    {  1,  1,  4,  4, 0, false },   // Format::RGBA_8_u
    {  1,  1,  4,  4, 0, false },   // Format::RGBA_8_s
    {  1,  1,  8,  8, 0, false },   // Format::RGBA_16
    {  1,  1,  8,  8, 0, false },   // Format::RGBA_16_sn
    {  1,  1,  8,  8, 0, false },   // Format::RGBA_16_u
    {  1,  1,  8,  8, 0, false },   // Format::RGBA_16_s
    {  1,  1,  8,  8, 0, false },   // Format::RGBA_16_hf
    {  1,  1, 16, 16, 0, false },   // Format::RGBA_32_u
    {  1,  1, 16, 16, 0, false },   // Format::RGBA_32_s
    {  1,  1, 16, 16, 0, false },   // Format::RGBA_32_f
    {  1,  1,  2,  2, 0, false },   // Format::D_16
    {  1,  1,  4,  3, 0, false },   // Format::D_24              (internally padded to 4 bytes)
    {  1,  1,  4,  3, 1, false },   // Format::D_24_8
    {  1,  1,  4,  4, 0, false },   // Format::D_32
    {  1,  1,  4,  4, 0, false },   // Format::D_32_f
    {  1,  1,  1,  1, 0, false },   // Format::S_8
    {  1,  1,  4,  2, 1, false },   // Format::DS_16_8           (internally padded to 4 bytes, or stored separately?)
    {  1,  1,  4,  3, 1, false },   // Format::DS_24_8
    {  1,  1,  8,  4, 1, false },   // Format::DS_32_f_8         (5 bytes plus 3 bytes padding, may be stored internally as two planes)
    {  1,  1,  2,  2, 0, false },   // Format::RGB_5_6_5
    {  1,  1,  2,  2, 0, false },   // Format::BGR_5_6_5
    {  1,  1,  4,  3, 0, false },   // Format::BGR_8             (internally padded to 4 bytes)
    {  1,  1,  4,  3, 0, false },   // Format::BGR_8_sRGB        (internally padded to 4 bytes)
    {  1,  1,  4,  3, 0, false },   // Format::BGR_8_sn          (internally padded to 4 bytes)
    {  1,  1,  4,  3, 0, false },   // Format::BGR_8_u           (internally padded to 4 bytes)
    {  1,  1,  4,  3, 0, false },   // Format::BGR_8_s           (internally padded to 4 bytes)
    {  1,  1,  4,  4, 0, false },   // Format::RGB_11_11_10_uf
    {  1,  1,  4,  4, 0, false },   // Format::RGBE_9_9_9_5_uf
    {  1,  1,  4,  4, 0, false },   // Format::BGRA_8
    {  1,  1,  4,  4, 0, false },   // Format::BGRA_8_sRGB
    {  1,  1,  4,  4, 0, false },   // Format::BGRA_8_sn
    {  1,  1,  4,  4, 0, false },   // Format::BGRA_8_u
    {  1,  1,  4,  4, 0, false },   // Format::BGRA_8_s
    {  1,  1,  2,  2, 0, false },   // Format::BGRA_5_5_5_1
    {  1,  1,  2,  2, 0, false },   // Format::ARGB_1_5_5_5
    {  1,  1,  2,  2, 0, false },   // Format::ABGR_1_5_5_5
    {  1,  1,  4,  4, 0, false },   // Format::RGBA_10_10_10_2
    {  1,  1,  4,  4, 0, false },   // Format::RGBA_10_10_10_2_u
    {  1,  1,  4,  4, 0, false },   // Format::BGRA_10_10_10_2
    {  4,  4,  8,  8, 0, true  },   // Format::BC1_RGB
    {  4,  4,  8,  8, 0, true  },   // Format::BC1_RGB_sRGB
    {  4,  4,  8,  8, 0, true  },   // Format::BC1_RGBA
    {  4,  4,  8,  8, 0, true  },   // Format::BC1_RGBA_sRGB
    {  4,  4, 16, 16, 0, true  },   // Format::BC2_RGBA_pRGB
    {  4,  4, 16, 16, 0, true  },   // Format::BC2_RGBA
    {  4,  4, 16, 16, 0, true  },   // Format::BC2_RGBA_sRGB
    {  4,  4, 16, 16, 0, true  },   // Format::BC3_RGBA_pRGB
    {  4,  4, 16, 16, 0, true  },   // Format::BC3_RGBA
    {  4,  4, 16, 16, 0, true  },   // Format::BC3_RGBA_sRGB
    {  4,  4,  8,  8, 0, true  },   // Format::BC4_R
    {  4,  4,  8,  8, 0, true  },   // Format::BC4_R_sn
    {  4,  4, 16, 16, 0, true  },   // Format::BC5_RG
    {  4,  4, 16, 16, 0, true  },   // Format::BC5_RG_sn
    {  4,  4, 16, 16, 0, true  },   // Format::BC6H_RGB_f
    {  4,  4, 16, 16, 0, true  },   // Format::BC6H_RGB_uf
    {  4,  4, 16, 16, 0, true  },   // Format::BC7_RGBA
    {  4,  4, 16, 16, 0, true  },   // Format::BC7_RGBA_sRGB
    {  4,  4,  8,  8, 0, true  },   // Format::ETC2_R_11
    {  4,  4,  8,  8, 0, true  },   // Format::ETC2_R_11_sn
    {  4,  4, 16, 16, 0, true  },   // Format::ETC2_RG_11
    {  4,  4, 16, 16, 0, true  },   // Format::ETC2_RG_11_sn
    {  4,  4,  8,  8, 0, true  },   // Format::ETC2_RGB_8
    {  4,  4,  8,  8, 0, true  },   // Format::ETC2_RGB_8_sRGB
    {  4,  4, 16, 16, 0, true  },   // Format::ETC2_RGBA_8
    {  4,  4, 16, 16, 0, true  },   // Format::ETC2_RGBA_8_sRGB
    {  4,  4,  8,  8, 0, true  },   // Format::ETC2_RGB8_A1
    {  4,  4,  8,  8, 0, true  },   // Format::ETC2_RGB8_A1_sRGB
    {  4,  4,  8,  8, 0, true  },   // Format::PVRTC_RGB_2
    {  4,  4,  8,  8, 0, true  },   // Format::PVRTC_RGB_2_sRGB
    {  4,  4,  8,  8, 0, true  },   // Format::PVRTC_RGB_4
    {  4,  4,  8,  8, 0, true  },   // Format::PVRTC_RGB_4_sRGB
    {  4,  4,  8,  8, 0, true  },   // Format::PVRTC_RGBA_2
    {  4,  4,  8,  8, 0, true  },   // Format::PVRTC_RGBA_2_sRGB
    {  4,  4,  8,  8, 0, true  },   // Format::PVRTC_RGBA_4
    {  4,  4,  8,  8, 0, true  },   // Format::PVRTC_RGBA_4_sRGB
    {  4,  4, 16, 16, 0, true  },   // Format::ASTC_4x4
    {  5,  4, 16, 16, 0, true  },   // Format::ASTC_5x4
    {  5,  5, 16, 16, 0, true  },   // Format::ASTC_5x5
    {  6,  5, 16, 16, 0, true  },   // Format::ASTC_6x5
    {  6,  6, 16, 16, 0, true  },   // Format::ASTC_6x6
    {  8,  5, 16, 16, 0, true  },   // Format::ASTC_8x5
    {  8,  6, 16, 16, 0, true  },   // Format::ASTC_8x6
    {  8,  8, 16, 16, 0, true  },   // Format::ASTC_8x8
    { 10,  5, 16, 16, 0, true  },   // Format::ASTC_10x5
    { 10,  6, 16, 16, 0, true  },   // Format::ASTC_10x6
    { 10,  8, 16, 16, 0, true  },   // Format::ASTC_10x8
    { 10, 10, 16, 16, 0, true  },   // Format::ASTC_10x10
    { 12, 10, 16, 16, 0, true  },   // Format::ASTC_12x10
    { 12, 12, 16, 16, 0, true  },   // Format::ASTC_12x12
    {  4,  4, 16, 16, 0, true  },   // Format::ASTC_4x4_sRGB
    {  5,  4, 16, 16, 0, true  },   // Format::ASTC_5x4_sRGB
    {  5,  5, 16, 16, 0, true  },   // Format::ASTC_5x5_sRGB
    {  6,  5, 16, 16, 0, true  },   // Format::ASTC_6x5_sRGB
    {  6,  6, 16, 16, 0, true  },   // Format::ASTC_6x6_sRGB
    {  8,  5, 16, 16, 0, true  },   // Format::ASTC_8x5_sRGB
    {  8,  6, 16, 16, 0, true  },   // Format::ASTC_8x6_sRGB
    {  8,  8, 16, 16, 0, true  },   // Format::ASTC_8x8_sRGB
    { 10,  5, 16, 16, 0, true  },   // Format::ASTC_10x5_sRGB
    { 10,  6, 16, 16, 0, true  },   // Format::ASTC_10x6_sRGB
    { 10,  8, 16, 16, 0, true  },   // Format::ASTC_10x8_sRGB
    { 10, 10, 16, 16, 0, true  },   // Format::ASTC_10x10_sRGB
    { 12, 10, 16, 16, 0, true  },   // Format::ASTC_12x10_sRGB
    { 12, 12, 16, 16, 0, true  }    // Format::ASTC_12x12_sRGB
};

#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
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

void ImageMemoryAlignment::samplesCount(const uint8 samples)
{
    uint32 power;
    assert( samples > 0 );
    assert( powerOfTwo(samples) );
    whichPowerOfTwo(static_cast<uint32>(samples), power);
    samplesCountPower = power;
}

void ImageMemoryAlignment::sampleAlignment(const uint32 alignment)
{
    uint32 power;
    assert( alignment > 0 );
    assert( powerOfTwo(alignment) );
    whichPowerOfTwo(static_cast<uint32>(alignment), power);
    sampleAlignmentPower = power;
}

void ImageMemoryAlignment::texelAlignment(const uint32 alignment)
{
    uint32 power;
    assert( alignment > 0 );
    assert( powerOfTwo(alignment) );
    whichPowerOfTwo(static_cast<uint32>(alignment), power);
    texelAlignmentPower = power;
}

void ImageMemoryAlignment::rowAlignment(const uint32 alignment)
{
    uint32 power;
    assert( alignment > 0 );
    assert( powerOfTwo(alignment) );
    whichPowerOfTwo(static_cast<uint32>(alignment), power);
    rowAlignmentPower = power;
}

void ImageMemoryAlignment::surfaceAlignment(const uint32 alignment)
{
    uint32 power;
    assert( alignment > 0 );
    assert( powerOfTwo(alignment) );
    whichPowerOfTwo(static_cast<uint32>(alignment), power);
    surfaceAlignmentPower = power;
}

uint16v2 texelBlockResolution(const Format format)
{
    assert( format != Format::Unsupported );
   
    // 4 bytes from look-up table
    TextureCompressedBlockInfo blockInfo = TextureCompressionInfo[underlyingType(format)];
   
    return uint16v2(blockInfo.width, blockInfo.height);
}

uint32 texelSize(const Format format, const uint8 plane)
{
    assert( format != Format::Unsupported );
   
    if (plane == 1)
    {
        return TextureCompressionInfo[underlyingType(format)].secondarySize;
    }

    return TextureCompressionInfo[underlyingType(format)].blockSize;
}

// 2D Tile Size look-up table (in texels for 64KB tile)
uint16v2 TranslateTileSize2D[5][5] =
{
    { // Default
        uint16v2(256, 256), // 8 bit/texel
        uint16v2(256, 128), // 16 bit/texel
        uint16v2(128, 128), // 32 bit/texel
        uint16v2(128, 64),  // 64 bit/texel
        uint16v2(64,  64)   // 128 bit/texel
    },
    { // MSAAx2
        uint16v2(128, 256), // 8 bit/sample    16 bit/texel
        uint16v2(128, 128), // 16 bit/sample   32 bit/texel
        uint16v2(64,  128), // 32 bit/sample   64 bit/texel
        uint16v2(64,  64),  // 64 bit/sample   128 bit/texel
        uint16v2(32,  64)   // 128 bit/sample  256 bit/texel
    },
    { // MSAAx4
        uint16v2(128, 128), // 8 bit/sample    32 bit/texel
        uint16v2(128, 64),  // 16 bit/sample   64 bit/texel
        uint16v2(64,  64),  // 32 bit/sample   128 bit/texel
        uint16v2(64,  32),  // 64 bit/sample   256 bit/texel
        uint16v2(32,  32)   // 128 bit/sample  512 bit/texel
    },
    { // MSAAx8
        uint16v2(64, 128),  // 8 bit/sample    64 bit/texel
        uint16v2(64, 64),   // 16 bit/sample   128 bit/texel
        uint16v2(32, 64),   // 32 bit/sample   256 bit/texel
        uint16v2(32, 32),   // 64 bit/sample   512 bit/texel
        uint16v2(16, 32)    // 128 bit/sample  1024 bit/texel
    },
    { // MSAAx16
        uint16v2(64, 64),   // 8 bit/sample    128 bit/texel
        uint16v2(64, 32),   // 16 bit/sample   256 bit/texel
        uint16v2(32, 32),   // 32 bit/sample   512 bit/texel
        uint16v2(32, 16),   // 64 bit/sample   1024 bit/texel
        uint16v2(16, 16)    // 128 bit/sample  2048 bit/texel
    }
};

// Compressed 2D tile size in texels for 64KB:
//
// 512 256 -  64 bit/block - Format::BC1_RGB
// 512 256 -  64 bit/block - Format::BC1_RGB_sRGB
// 512 256 -  64 bit/block - Format::BC1_RGBA
// 512 256 -  64 bit/block - Format::BC1_RGBA_sRGB
// 256 256 - 128 bit/block - Format::BC2_RGBA_pRGB
// 256 256 - 128 bit/block - Format::BC2_RGBA
// 256 256 - 128 bit/block - Format::BC2_RGBA_sRGB
// 256 256 - 128 bit/block - Format::BC3_RGBA_pRGB
// 256 256 - 128 bit/block - Format::BC3_RGBA
// 256 256 - 128 bit/block - Format::BC3_RGBA_sRGB
// 512 256 -  64 bit/block - Format::BC4_R
// 512 256 -  64 bit/block - Format::BC4_R_sn
// 256 256 - 128 bit/block - Format::BC5_RG
// 256 256 - 128 bit/block - Format::BC5_RG_sn
// 256 256 - 128 bit/block - Format::BC6H_RGB_f
// 256 256 - 128 bit/block - Format::BC6H_RGB_uf
// 256 256 - 128 bit/block - Format::BC7_RGBA
// 256 256 - 128 bit/block - Format::BC7_RGBA_sRGB
//
// 512 256 -  64 bit/block - Format::ETC2_R_11
// 512 256 -  64 bit/block - Format::ETC2_R_11_sn
// 256 256 - 128 bit/block - Format::ETC2_RG_11
// 256 256 - 128 bit/block - Format::ETC2_RG_11_sn
// 512 256 -  64 bit/block - Format::ETC2_RGB_8
// 512 256 -  64 bit/block - Format::ETC2_RGB_8_sRGB
// 256 256 - 128 bit/block - Format::ETC2_RGBA_8
// 256 256 - 128 bit/block - Format::ETC2_RGBA_8_sRGB
// 512 256 -  64 bit/block - Format::ETC2_RGB8_A1
// 512 256 -  64 bit/block - Format::ETC2_RGB8_A1_sRGB
// 512 256 -  64 bit/block - Format::PVRTC_RGB_2
// 512 256 -  64 bit/block - Format::PVRTC_RGB_2_sRGB
// 512 256 -  64 bit/block - Format::PVRTC_RGB_4
// 512 256 -  64 bit/block - Format::PVRTC_RGB_4_sRGB
// 512 256 -  64 bit/block - Format::PVRTC_RGBA_2
// 512 256 -  64 bit/block - Format::PVRTC_RGBA_2_sRGB
// 512 256 -  64 bit/block - Format::PVRTC_RGBA_4
// 512 256 -  64 bit/block - Format::PVRTC_RGBA_4_sRGB
//
// 256 256 - 128 bit/block - Format::ASTC_4x4
// 320 256 - 128 bit/block - Format::ASTC_5x4
// 320 320 - 128 bit/block - Format::ASTC_5x5
// 384 320 - 128 bit/block - Format::ASTC_6x5
// 384 384 - 128 bit/block - Format::ASTC_6x6
// 512 320 - 128 bit/block - Format::ASTC_8x5
// 512 384 - 128 bit/block - Format::ASTC_8x6
// 512 512 - 128 bit/block - Format::ASTC_8x8
// 640 320 - 128 bit/block - Format::ASTC_10x5
// 640 384 - 128 bit/block - Format::ASTC_10x6
// 640 512 - 128 bit/block - Format::ASTC_10x8
// 640 640 - 128 bit/block - Format::ASTC_10x10
// 768 640 - 128 bit/block - Format::ASTC_12x10
// 768 768 - 128 bit/block - Format::ASTC_12x12
//
// 256 256 - 128 bit/block - Format::ASTC_4x4_sRGB
// 320 256 - 128 bit/block - Format::ASTC_5x4_sRGB
// 320 320 - 128 bit/block - Format::ASTC_5x5_sRGB
// 384 320 - 128 bit/block - Format::ASTC_6x5_sRGB
// 384 384 - 128 bit/block - Format::ASTC_6x6_sRGB
// 512 320 - 128 bit/block - Format::ASTC_8x5_sRGB
// 512 384 - 128 bit/block - Format::ASTC_8x6_sRGB
// 512 512 - 128 bit/block - Format::ASTC_8x8_sRGB
// 640 320 - 128 bit/block - Format::ASTC_10x5_sRGB
// 640 384 - 128 bit/block - Format::ASTC_10x6_sRGB
// 640 512 - 128 bit/block - Format::ASTC_10x8_sRGB
// 640 640 - 128 bit/block - Format::ASTC_10x10_sRGB
// 768 640 - 128 bit/block - Format::ASTC_12x10_sRGB
// 768 768 - 128 bit/block - Format::ASTC_12x12_sRGB
      
// Below 3D tile size layouts are compatible across D3D12 & Vulkan:
   
// 3D Tile Size look-up table (in texels for 64KB tile)
uint16v4 TranslateTileSize3D[5] =
{
    uint16v4(64, 32, 32, 1), // 8 bit/texel
    uint16v4(32, 32, 32, 1), // 16 bit/texel
    uint16v4(32, 32, 16, 1), // 32 bit/texel
    uint16v4(32, 16, 16, 1), // 64 bit/texel
    uint16v4(16, 16, 16, 1)  // 128 bit/texel
};
      
// Compressed 3D tile size in texels for 64KB:
//
// 128 64  16   -  64 bit/block - Format::BC1_RGB
// 128 64  16   -  64 bit/block - Format::BC1_RGB_sRGB
// 128 64  16   -  64 bit/block - Format::BC1_RGBA
// 128 64  16   -  64 bit/block - Format::BC1_RGBA_sRGB
// 64  64  16   - 128 bit/block - Format::BC2_RGBA_pRGB
// 64  64  16   - 128 bit/block - Format::BC2_RGBA
// 64  64  16   - 128 bit/block - Format::BC2_RGBA_sRGB
// 64  64  16   - 128 bit/block - Format::BC3_RGBA_pRGB
// 64  64  16   - 128 bit/block - Format::BC3_RGBA
// 64  64  16   - 128 bit/block - Format::BC3_RGBA_sRGB
// 128 64  16   -  64 bit/block - Format::BC4_R
// 128 64  16   -  64 bit/block - Format::BC4_R_sn
// 64  64  16   - 128 bit/block - Format::BC5_RG
// 64  64  16   - 128 bit/block - Format::BC5_RG_sn
// 64  64  16   - 128 bit/block - Format::BC6H_RGB_f
// 64  64  16   - 128 bit/block - Format::BC6H_RGB_uf
// 64  64  16   - 128 bit/block - Format::BC7_RGBA
// 64  64  16   - 128 bit/block - Format::BC7_RGBA_sRGB
//
// 128 64  16   -  64 bit/block - Format::ETC2_R_11
// 128 64  16   -  64 bit/block - Format::ETC2_R_11_sn
// 64  64  16   - 128 bit/block - Format::ETC2_RG_11
// 64  64  16   - 128 bit/block - Format::ETC2_RG_11_sn
// 128 64  16   -  64 bit/block - Format::ETC2_RGB_8
// 128 64  16   -  64 bit/block - Format::ETC2_RGB_8_sRGB
// 64  64  16   - 128 bit/block - Format::ETC2_RGBA_8
// 64  64  16   - 128 bit/block - Format::ETC2_RGBA_8_sRGB
// 128 64  16   -  64 bit/block - Format::ETC2_RGB8_A1
// 128 64  16   -  64 bit/block - Format::ETC2_RGB8_A1_sRGB
// 128 64  16   -  64 bit/block - Format::PVRTC_RGB_2
// 128 64  16   -  64 bit/block - Format::PVRTC_RGB_2_sRGB
// 128 64  16   -  64 bit/block - Format::PVRTC_RGB_4
// 128 64  16   -  64 bit/block - Format::PVRTC_RGB_4_sRGB
// 128 64  16   -  64 bit/block - Format::PVRTC_RGBA_2
// 128 64  16   -  64 bit/block - Format::PVRTC_RGBA_2_sRGB
// 128 64  16   -  64 bit/block - Format::PVRTC_RGBA_4
// 128 64  16   -  64 bit/block - Format::PVRTC_RGBA_4_sRGB
//
// 64  64  16   - 128 bit/block - Format::ASTC_4x4
// 80  64  16   - 128 bit/block - Format::ASTC_5x4
// 80  80  16   - 128 bit/block - Format::ASTC_5x5
// 96  80  16   - 128 bit/block - Format::ASTC_6x5
// 96  96  16   - 128 bit/block - Format::ASTC_6x6
// 128 80  16   - 128 bit/block - Format::ASTC_8x5
// 128 96  16   - 128 bit/block - Format::ASTC_8x6
// 128 128 16   - 128 bit/block - Format::ASTC_8x8
// 160 80  16   - 128 bit/block - Format::ASTC_10x5
// 160 96  16   - 128 bit/block - Format::ASTC_10x6
// 160 128 16   - 128 bit/block - Format::ASTC_10x8
// 160 160 16   - 128 bit/block - Format::ASTC_10x10
// 196 160 16   - 128 bit/block - Format::ASTC_12x10
// 196 196 16   - 128 bit/block - Format::ASTC_12x12
//
// 64  64  16   - 128 bit/block - Format::ASTC_4x4_sRGB
// 80  64  16   - 128 bit/block - Format::ASTC_5x4_sRGB
// 80  80  16   - 128 bit/block - Format::ASTC_5x5_sRGB
// 96  80  16   - 128 bit/block - Format::ASTC_6x5_sRGB
// 96  96  16   - 128 bit/block - Format::ASTC_6x6_sRGB
// 128 80  16   - 128 bit/block - Format::ASTC_8x5_sRGB
// 128 96  16   - 128 bit/block - Format::ASTC_8x6_sRGB
// 128 128 16   - 128 bit/block - Format::ASTC_8x8_sRGB
// 160 80  16   - 128 bit/block - Format::ASTC_10x5_sRGB
// 160 96  16   - 128 bit/block - Format::ASTC_10x6_sRGB
// 160 128 16   - 128 bit/block - Format::ASTC_10x8_sRGB
// 160 160 16   - 128 bit/block - Format::ASTC_10x10_sRGB
// 196 160 16   - 128 bit/block - Format::ASTC_12x10_sRGB
// 196 196 16   - 128 bit/block - Format::ASTC_12x12_sRGB
   
#include "utilities/utilities.h"

      // Single tile is always 64KB in size.
      // Engine translates arbitrary region, into (rounded up) subset of tiles,
      // that should be updated, uploaded or downloaded.
      //
      // Render Targets, and MSAA textures are out of scope of resource streamer.
      // ( ? But there need to be a simple way of streaming them back to RAM ? )
      //
      // There is no point in streaming MSAA textures.


uint16v2 tileSize2D(const Format format,
                    const uint32 samples,
                    const uint8  plane)
{
    assert( format != Format::Unsupported );
    assert( powerOfTwo(samples) );
   
    // 4 bytes from look-up table
    TextureCompressedBlockInfo blockInfo = TextureCompressionInfo[underlyingType(format)];
   
    // Size of texel (or compressed block) in linear layout, optimized for GPU's
    // (texels size padded to power of two)
    uint32 texelSize = blockInfo.blockSize;
    if (plane == 1)
    {
        texelSize = blockInfo.secondarySize;
    }

    // Texel should have size that is power of two
    assert( powerOfTwo(texelSize) );
   
    uint32 bppIndex = 0;
    highestBit(texelSize, bppIndex);
      
    uint32 sampleIndex = 0;
    highestBit(samples, sampleIndex);
      
    // Get default size of texel region stored in single 64KB tile
    uint16v2 tileSize = TranslateTileSize2D[sampleIndex][bppIndex];
      
    // Width of single row needs to take into notice block compressed formats
    tileSize.width *= texelSize;

    return tileSize;
}
   
uint16v4 tileSize3D(const Format format)
{
    assert( format != Format::Unsupported );
   
    // 4 bytes from look-up table
    TextureCompressedBlockInfo blockInfo = TextureCompressionInfo[underlyingType(format)];
   
    // Size of texel (or compressed block) in linear layout, optimized for GPU's
    // (texels size padded to power of two)
    uint32 texelSize = blockInfo.blockSize;

    // Texel should have size that is power of two
    assert( powerOfTwo(texelSize) );
   
    uint32 index = 0;
    highestBit(texelSize, index);
      
    // Get default size of texel region stored in single 64KB tile
    uint16v4 tileSize = TranslateTileSize3D[index];
      
    // Width of single row needs to take into notice block compressed formats
    tileSize.width *= texelSize;

    return tileSize;
}

uint16v2 tileResolution2D(const Format format,
                          const uint32 samples,
                          const uint8  plane)
{
    assert( format != Format::Unsupported );
    assert( powerOfTwo(samples) );
   
    // 4 bytes from look-up table
    TextureCompressedBlockInfo blockInfo = TextureCompressionInfo[underlyingType(format)];
   
    // Size of texel (or compressed block) in linear layout, optimized for GPU's
    // (texels size padded to power of two)
    uint32 texelSize = blockInfo.blockSize;
    if (plane == 1)
    {
        texelSize = blockInfo.secondarySize;
    }

    // Texel should have size that is power of two
    assert( powerOfTwo(texelSize) );
   
    uint32 bppIndex = 0;
    highestBit(texelSize, bppIndex);
      
    uint32 sampleIndex = 0;
    highestBit(samples, sampleIndex);
      
    // Get default size of texel region stored in single 64KB tile
    uint16v2 tileSize = TranslateTileSize2D[sampleIndex][bppIndex];
      
    // Take into notice block compressed formats
    tileSize.width  *= blockInfo.width;
    tileSize.height *= blockInfo.height;
   
    return tileSize;
}

uint16v4 tileResolution3D(const Format format)
{
    assert( format != Format::Unsupported );
   
    // 4 bytes from look-up table
    TextureCompressedBlockInfo blockInfo = TextureCompressionInfo[underlyingType(format)];
   
    // Size of texel (or compressed block) in linear layout, optimized for GPU's
    // (texels size padded to power of two)
    uint32 texelSize = blockInfo.blockSize;

    // Texel should have size that is power of two
    assert( powerOfTwo(texelSize) );
   
    uint32 index = 0;
    highestBit(texelSize, index);
      
    // Get default size of texel region stored in single 64KB tile
    uint16v4 tileSize = TranslateTileSize3D[index];
      
    // Take into notice block compressed formats
    tileSize.width  *= blockInfo.width;
    tileSize.height *= blockInfo.height;
   
    return tileSize;
}

bool isDepth(const Format format)
{
    if ( (format == Format::D_16  ) ||   
         (format == Format::D_24  ) ||   
         (format == Format::D_24_8) ||
         (format == Format::D_32  ) ||               
         (format == Format::D_32_f) )
    {
        return true;
    }

    return false;
}

bool isStencil(const Format format)
{
    if (format == Format::S_8)
    {
        return true;
    }

    return false;
}

bool isDepthStencil(const Format format)
{
    if ( (format == Format::DS_16_8  ) ||
         (format == Format::DS_24_8  ) ||
         (format == Format::DS_32_f_8) )
    {
        return true;
    }

    return false;
}

// Calculate texture mipmaps count
uint32 TextureMipMapCount(const TextureState& state)
{
    if (state.type == TextureType::Texture2DMultisample      ||
        state.type == TextureType::Texture2DMultisampleArray )   // also Rectangle & Buffer
    {
        return 1;
    }

    uint32 maxDimmension = state.width > state.height ? state.width : state.height;
   
    if (state.type == TextureType::Texture3D)
    {
        if (maxDimmension < state.layers)
        {
            maxDimmension = state.layers;
        }
    }

    uint32 value = maxDimmension; //nextPowerOfTwo(maxDimmension);
    uint32 mipmaps;
    for(mipmaps = 1; ; ++mipmaps)
    {
        if (value == 1)
        {
            break;
        }

        value = value >> 1;
    }

    return mipmaps;
}

CommonTexture::CommonTexture()
{
}

CommonTexture::CommonTexture(const TextureState& _state) :
    state(_state)
{
}

TextureType CommonTexture::type(void) const
{
    return state.type;
}
   
Format CommonTexture::format(void) const
{
    return state.format;
}

uint32 CommonTexture::mipmaps(void) const
{
    return state.mipmaps;
}

uint32 CommonTexture::size(const uint8 mipmap,
                           const uint8 plane) const
{
    return state.surfaceSize(mipmap, plane);
}

uint32 CommonTexture::width(const uint8 mipmap) const
{
    // If mipmap exceeds total mipmaps count, result will be 0
    return state.mipWidth(mipmap);
}
   
uint32 CommonTexture::height(const uint8 mipmap) const
{
    // If mipmap exceeds total mipmaps count, result will be 0
    return state.mipHeight(mipmap);
}
   
uint32 CommonTexture::depth(const uint8 mipmap) const
{
    // If mipmap exceeds total mipmaps count, result will be 0
    return state.mipDepth(mipmap);
}
   
uint32v2 CommonTexture::resolution(const uint8 mipmap) const
{
    return state.mipResolution(mipmap);
}

uint32v3 CommonTexture::volume(const uint8 mipmap) const
{
    return state.mipVolume(mipmap);
}
      
uint16 CommonTexture::layers(void) const
{
    return state.layers;
}
   
uint16 CommonTexture::samples(void) const
{
    return state.samples;
}
   
TextureView* CommonTexture::view()
{
    // Should be implemented by specialization class.
    // Texture cannot keep pointer to it's "default" view,
    // as then default view would keep strong pointer to the
    // texture, keeping it's refCount always +1 and preventing
    // it from beeing released. Thus default view needs to
    // always be created on the fly.
    assert( 0 );
    return nullptr;
}

CommonTextureView::CommonTextureView(
        const TextureType _type,
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

uint32 CommonDevice::texelSize(const Format format,
                               const uint8  plane) const
{
    if (plane == 1)
    {
        return TextureCompressionInfo[underlyingType(format)].secondarySize;
    }
      
    return TextureCompressionInfo[underlyingType(format)].blockSize;
}

} // en::gpu
} // en
