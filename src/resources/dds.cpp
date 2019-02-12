/*

 Ngine v5.0
 
 Module      : DDS file support
 Requirements: none
 Description : Supports reading images from *.dds files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/rendering/texture.h"
#include "utilities/utilities.h"
#include "resources/context.h"
#include "resources/dds.h"

#include "core/rendering/device.h"

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
#include "zlib.h"
#endif
using namespace en::gpu;

#include <string>

namespace en
{
   namespace dds
   {
   // DDS_HEADER.dwFlags
   #define DDSD_CAPS                     0x00000001 
   #define DDSD_HEIGHT                   0x00000002 
   #define DDSD_WIDTH                    0x00000004 
   #define DDSD_PITCH                    0x00000008 
   #define DDSD_PIXELFORMAT              0x00001000 
   #define DDSD_MIPMAPCOUNT              0x00020000 
   #define DDSD_LINEARSIZE               0x00080000 
   #define DDSD_DEPTH                    0x00800000 
                                         
   // DDS_HEADER.ddspf.dwFlags           
   #define DDS_ALPHAPIXELS               0x00000001 
   #define DDS_ALPHA                     0x00000002
   #define DDS_FOURCC                    0x00000004 
   #define DDS_INDEXED                   0x00000020 
   #define DDS_RGB                       0x00000040 
   #define DDS_RGBA                      0x00000041
   #define DDS_YUV                       0x00000200 
   #define DDS_LUMINANCE                 0x00020000 
   #define DDS_LUMINANCEA                0x00020001
                                         
   // DDS_HEADER.dwCaps1                 
   #define DDSCAPS_COMPLEX               0x00000008 
   #define DDSCAPS_TEXTURE               0x00001000 
   #define DDSCAPS_MIPMAP                0x00400000 
                                         
   // DDS_HEADER.dwCaps2                 
   #define DDSCAPS2_CUBEMAP              0x00000200 
   #define DDSCAPS2_CUBEMAP_POSITIVEX    0x00000400 
   #define DDSCAPS2_CUBEMAP_NEGATIVEX    0x00000800 
   #define DDSCAPS2_CUBEMAP_POSITIVEY    0x00001000 
   #define DDSCAPS2_CUBEMAP_NEGATIVEY    0x00002000 
   #define DDSCAPS2_CUBEMAP_POSITIVEZ    0x00004000 
   #define DDSCAPS2_CUBEMAP_NEGATIVEZ    0x00008000 
   #define DDSCAPS2_VOLUME               0x00200000 
   
   // DDS_HEADER_DXT10.miscFlag
   #define DDS_RESOURCE_MISC_TEXTURECUBE 0x00000004

#if defined(EN_PLATFORM_OSX)
   #define DWORD uint32
   #define UINT  uint32
#endif

   typedef enum D3D10_RESOURCE_DIMENSION 
      { 
      D3D10_RESOURCE_DIMENSION_UNKNOWN    = 0,
      D3D10_RESOURCE_DIMENSION_BUFFER     = 1,
      D3D10_RESOURCE_DIMENSION_TEXTURE1D  = 2,
      D3D10_RESOURCE_DIMENSION_TEXTURE2D  = 3,
      D3D10_RESOURCE_DIMENSION_TEXTURE3D  = 4
      } D3D10_RESOURCE_DIMENSION;

   typedef enum DXGI_FORMAT 
      { 
      DXGI_FORMAT_UNKNOWN                     = 0,
      DXGI_FORMAT_R32G32B32A32_TYPELESS       = 1,
      DXGI_FORMAT_R32G32B32A32_FLOAT          = 2,
      DXGI_FORMAT_R32G32B32A32_UINT           = 3,
      DXGI_FORMAT_R32G32B32A32_SINT           = 4,
      DXGI_FORMAT_R32G32B32_TYPELESS          = 5,
      DXGI_FORMAT_R32G32B32_FLOAT             = 6,
      DXGI_FORMAT_R32G32B32_UINT              = 7,
      DXGI_FORMAT_R32G32B32_SINT              = 8,
      DXGI_FORMAT_R16G16B16A16_TYPELESS       = 9,
      DXGI_FORMAT_R16G16B16A16_FLOAT          = 10,
      DXGI_FORMAT_R16G16B16A16_UNORM          = 11,
      DXGI_FORMAT_R16G16B16A16_UINT           = 12,
      DXGI_FORMAT_R16G16B16A16_SNORM          = 13,
      DXGI_FORMAT_R16G16B16A16_SINT           = 14,
      DXGI_FORMAT_R32G32_TYPELESS             = 15,
      DXGI_FORMAT_R32G32_FLOAT                = 16,
      DXGI_FORMAT_R32G32_UINT                 = 17,
      DXGI_FORMAT_R32G32_SINT                 = 18,
      DXGI_FORMAT_R32G8X24_TYPELESS           = 19,
      DXGI_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
      DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
      DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
      DXGI_FORMAT_R10G10B10A2_TYPELESS        = 23,
      DXGI_FORMAT_R10G10B10A2_UNORM           = 24,
      DXGI_FORMAT_R10G10B10A2_UINT            = 25,
      DXGI_FORMAT_R11G11B10_FLOAT             = 26,
      DXGI_FORMAT_R8G8B8A8_TYPELESS           = 27,
      DXGI_FORMAT_R8G8B8A8_UNORM              = 28,
      DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
      DXGI_FORMAT_R8G8B8A8_UINT               = 30,
      DXGI_FORMAT_R8G8B8A8_SNORM              = 31,
      DXGI_FORMAT_R8G8B8A8_SINT               = 32,
      DXGI_FORMAT_R16G16_TYPELESS             = 33,
      DXGI_FORMAT_R16G16_FLOAT                = 34,
      DXGI_FORMAT_R16G16_UNORM                = 35,
      DXGI_FORMAT_R16G16_UINT                 = 36,
      DXGI_FORMAT_R16G16_SNORM                = 37,
      DXGI_FORMAT_R16G16_SINT                 = 38,
      DXGI_FORMAT_R32_TYPELESS                = 39,
      DXGI_FORMAT_D32_FLOAT                   = 40,
      DXGI_FORMAT_R32_FLOAT                   = 41,
      DXGI_FORMAT_R32_UINT                    = 42,
      DXGI_FORMAT_R32_SINT                    = 43,
      DXGI_FORMAT_R24G8_TYPELESS              = 44,
      DXGI_FORMAT_D24_UNORM_S8_UINT           = 45,
      DXGI_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
      DXGI_FORMAT_X24_TYPELESS_G8_UINT        = 47,
      DXGI_FORMAT_R8G8_TYPELESS               = 48,
      DXGI_FORMAT_R8G8_UNORM                  = 49,
      DXGI_FORMAT_R8G8_UINT                   = 50,
      DXGI_FORMAT_R8G8_SNORM                  = 51,
      DXGI_FORMAT_R8G8_SINT                   = 52,
      DXGI_FORMAT_R16_TYPELESS                = 53,
      DXGI_FORMAT_R16_FLOAT                   = 54,
      DXGI_FORMAT_D16_UNORM                   = 55,
      DXGI_FORMAT_R16_UNORM                   = 56,
      DXGI_FORMAT_R16_UINT                    = 57,
      DXGI_FORMAT_R16_SNORM                   = 58,
      DXGI_FORMAT_R16_SINT                    = 59,
      DXGI_FORMAT_R8_TYPELESS                 = 60,
      DXGI_FORMAT_R8_UNORM                    = 61,
      DXGI_FORMAT_R8_UINT                     = 62,
      DXGI_FORMAT_R8_SNORM                    = 63,
      DXGI_FORMAT_R8_SINT                     = 64,
      DXGI_FORMAT_A8_UNORM                    = 65,
      DXGI_FORMAT_R1_UNORM                    = 66,
      DXGI_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
      DXGI_FORMAT_R8G8_B8G8_UNORM             = 68,
      DXGI_FORMAT_G8R8_G8B8_UNORM             = 69,
      DXGI_FORMAT_BC1_TYPELESS                = 70,
      DXGI_FORMAT_BC1_UNORM                   = 71,
      DXGI_FORMAT_BC1_UNORM_SRGB              = 72,
      DXGI_FORMAT_BC2_TYPELESS                = 73,
      DXGI_FORMAT_BC2_UNORM                   = 74,
      DXGI_FORMAT_BC2_UNORM_SRGB              = 75,
      DXGI_FORMAT_BC3_TYPELESS                = 76,
      DXGI_FORMAT_BC3_UNORM                   = 77,
      DXGI_FORMAT_BC3_UNORM_SRGB              = 78,
      DXGI_FORMAT_BC4_TYPELESS                = 79,
      DXGI_FORMAT_BC4_UNORM                   = 80,
      DXGI_FORMAT_BC4_SNORM                   = 81,
      DXGI_FORMAT_BC5_TYPELESS                = 82,
      DXGI_FORMAT_BC5_UNORM                   = 83,
      DXGI_FORMAT_BC5_SNORM                   = 84,
      DXGI_FORMAT_B5G6R5_UNORM                = 85,
      DXGI_FORMAT_B5G5R5A1_UNORM              = 86,
      DXGI_FORMAT_B8G8R8A8_UNORM              = 87,
      DXGI_FORMAT_B8G8R8X8_UNORM              = 88,
      DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
      DXGI_FORMAT_B8G8R8A8_TYPELESS           = 90,
      DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
      DXGI_FORMAT_B8G8R8X8_TYPELESS           = 92,
      DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
      DXGI_FORMAT_BC6H_TYPELESS               = 94,
      DXGI_FORMAT_BC6H_UF16                   = 95,
      DXGI_FORMAT_BC6H_SF16                   = 96,
      DXGI_FORMAT_BC7_TYPELESS                = 97,
      DXGI_FORMAT_BC7_UNORM                   = 98,
      DXGI_FORMAT_BC7_UNORM_SRGB              = 99,
      DXGI_FORMAT_AYUV                        = 100,
      DXGI_FORMAT_Y410                        = 101,
      DXGI_FORMAT_Y416                        = 102,
      DXGI_FORMAT_NV12                        = 103,
      DXGI_FORMAT_P010                        = 104,
      DXGI_FORMAT_P016                        = 105,
      DXGI_FORMAT_420_OPAQUE                  = 106,
      DXGI_FORMAT_YUY2                        = 107,
      DXGI_FORMAT_Y210                        = 108,
      DXGI_FORMAT_Y216                        = 109,
      DXGI_FORMAT_NV11                        = 110,
      DXGI_FORMAT_AI44                        = 111,
      DXGI_FORMAT_IA44                        = 112,
      DXGI_FORMAT_P8                          = 113,
      DXGI_FORMAT_A8P8                        = 114,
      DXGI_FORMAT_B4G4R4A4_UNORM              = 115,
      DXGI_FORMAT_P208                        = 130,
      DXGI_FORMAT_V208                        = 131,
      DXGI_FORMAT_V408                        = 132,
      DXGI_FORMAT_ASTC_4X4_TYPELESS           = 133,
      DXGI_FORMAT_ASTC_4X4_UNORM              = 134,
      DXGI_FORMAT_ASTC_4X4_UNORM_SRGB         = 135,
      DXGI_FORMAT_ASTC_5X4_TYPELESS           = 137,
      DXGI_FORMAT_ASTC_5X4_UNORM              = 138,
      DXGI_FORMAT_ASTC_5X4_UNORM_SRGB         = 139,
      DXGI_FORMAT_ASTC_5X5_TYPELESS           = 141,
      DXGI_FORMAT_ASTC_5X5_UNORM              = 142,
      DXGI_FORMAT_ASTC_5X5_UNORM_SRGB         = 143,
      DXGI_FORMAT_ASTC_6X5_TYPELESS           = 145,
      DXGI_FORMAT_ASTC_6X5_UNORM              = 146,
      DXGI_FORMAT_ASTC_6X5_UNORM_SRGB         = 147,
      DXGI_FORMAT_ASTC_6X6_TYPELESS           = 149,
      DXGI_FORMAT_ASTC_6X6_UNORM              = 150,
      DXGI_FORMAT_ASTC_6X6_UNORM_SRGB         = 151,
      DXGI_FORMAT_ASTC_8X5_TYPELESS           = 153,
      DXGI_FORMAT_ASTC_8X5_UNORM              = 154,
      DXGI_FORMAT_ASTC_8X5_UNORM_SRGB         = 155,
      DXGI_FORMAT_ASTC_8X6_TYPELESS           = 157,
      DXGI_FORMAT_ASTC_8X6_UNORM              = 158,
      DXGI_FORMAT_ASTC_8X6_UNORM_SRGB         = 159,
      DXGI_FORMAT_ASTC_8X8_TYPELESS           = 161,
      DXGI_FORMAT_ASTC_8X8_UNORM              = 162,
      DXGI_FORMAT_ASTC_8X8_UNORM_SRGB         = 163,
      DXGI_FORMAT_ASTC_10X5_TYPELESS          = 165,
      DXGI_FORMAT_ASTC_10X5_UNORM             = 166,
      DXGI_FORMAT_ASTC_10X5_UNORM_SRGB        = 167,
      DXGI_FORMAT_ASTC_10X6_TYPELESS          = 169,
      DXGI_FORMAT_ASTC_10X6_UNORM             = 170,
      DXGI_FORMAT_ASTC_10X6_UNORM_SRGB        = 171,
      DXGI_FORMAT_ASTC_10X8_TYPELESS          = 173,
      DXGI_FORMAT_ASTC_10X8_UNORM             = 174,
      DXGI_FORMAT_ASTC_10X8_UNORM_SRGB        = 175,
      DXGI_FORMAT_ASTC_10X10_TYPELESS         = 177,
      DXGI_FORMAT_ASTC_10X10_UNORM            = 178,
      DXGI_FORMAT_ASTC_10X10_UNORM_SRGB       = 179,
      DXGI_FORMAT_ASTC_12X10_TYPELESS         = 181,
      DXGI_FORMAT_ASTC_12X10_UNORM            = 182,
      DXGI_FORMAT_ASTC_12X10_UNORM_SRGB       = 183,
      DXGI_FORMAT_ASTC_12X12_TYPELESS         = 185,
      DXGI_FORMAT_ASTC_12X12_UNORM            = 186,
      DXGI_FORMAT_ASTC_12X12_UNORM_SRGB       = 187,
      DXGI_FORMAT_FORCE_UINT                  = 0xffffffff
      } DXGI_FORMAT;

   alignTo(1) 
   // 32 bytes
   struct DDS_PIXELFORMAT 
      {
      DWORD dwSize;
      DWORD dwFlags;
      DWORD dwFourCC;
      DWORD dwRGBBitCount;
      DWORD dwRBitMask;
      DWORD dwGBitMask;
      DWORD dwBBitMask;
      DWORD dwABitMask;
      };

   // 124 bytes in total
   typedef struct 
      {
      DWORD           dwSize;
      DWORD           dwFlags;
      DWORD           dwHeight;
      DWORD           dwWidth;
      DWORD           dwPitchOrLinearSize;
      DWORD           dwDepth;
      DWORD           dwMipMapCount;
      DWORD           dwReserved1[11];
      DDS_PIXELFORMAT ddspf;
      DWORD           dwCaps;
      DWORD           dwCaps2;
      DWORD           dwCaps3;
      DWORD           dwCaps4;
      DWORD           dwReserved2;
      } DDS_HEADER;
   
   // 20 bytes
   typedef struct 
      {
      DXGI_FORMAT              dxgiFormat;
      D3D10_RESOURCE_DIMENSION resourceDimension;
      UINT                     miscFlag;
      UINT                     arraySize;
      UINT                     miscFlags2;
      } DDS_HEADER_DXT10;
   alignToDefault

   bool DetermineTextureType(DDS_HEADER& header, DDS_HEADER_DXT10* header10, gpu::TextureType& type)
   {
   using namespace en::gpu;

   if (header10)
      {
      if (header10->resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE1D)
         {
         if (header10->arraySize > 1)
            {
            type = TextureType::Texture1DArray;
            return true;
            }

         type = TextureType::Texture1D;
         return true;
         }
      if (header10->resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE2D)
         {
         if (header10->miscFlag == DDS_RESOURCE_MISC_TEXTURECUBE)
            {
            if (header10->arraySize > 1)
               {
               type = TextureType::TextureCubeMapArray;
               return true;
               }
            type = TextureType::TextureCubeMap;
            return true;
            }
         if (header10->arraySize > 1)
            {
            type = TextureType::Texture2DArray;
            return true;
            }
         type = TextureType::Texture2D;
         return true;
         }
      if (header10->resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE3D)
         {
         type = TextureType::Texture3D;
         return true;
         }
      }

   // Determine texture type the old way
   if (header.dwCaps2 & DDSCAPS2_CUBEMAP)
      {
      type = TextureType::TextureCubeMap;
      return true;
      }

   if (header.dwCaps2 & DDSCAPS2_VOLUME)
      {
      type = TextureType::Texture3D;
      return true;
      }

   if ( (header.dwFlags & DDSD_HEIGHT) &&
        (header.dwHeight == 1) )
      {
      type = TextureType::Texture1D;
      return true;
      }

   if ( (header.dwFlags & DDSD_WIDTH & DDSD_HEIGHT) &&
        (header.dwWidth  > 0) &&
        (header.dwHeight > 0) )
      {
      type = TextureType::Texture2D;
      return true;
      }

   return false;
   }

   bool DetectTextureFormat(DDS_HEADER& header, DDS_HEADER_DXT10* header10, Format& format)
   {
   if (header10)
      {                                                                   // LO -> HI order in memory all formats
      if (header10->dxgiFormat == DXGI_FORMAT_UNKNOWN                   ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_R32G32B32A32_TYPELESS     ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT        ) { format = Format::RGBA_32_f;         return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32G32B32A32_UINT         ) { format = Format::RGBA_32_u;         return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32G32B32A32_SINT         ) { format = Format::RGBA_32_s;         return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32G32B32_TYPELESS        ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_R32G32B32_FLOAT           ) { format = Format::RGB_32_f;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32G32B32_UINT            ) { format = Format::RGB_32_u;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32G32B32_SINT            ) { format = Format::RGB_32_s;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R16G16B16A16_TYPELESS     ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT        ) { format = Format::RGBA_16_hf;        return true; } // ???? do they expect type conversion f32 -> f16 on load ???
      if (header10->dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM        ) { format = Format::RGBA_16;           return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R16G16B16A16_UINT         ) { format = Format::RGBA_16_u;         return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R16G16B16A16_SNORM        ) { format = Format::RGBA_16_sn;        return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R16G16B16A16_SINT         ) { format = Format::RGBA_16_s;         return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32G32_TYPELESS           ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_R32G32_FLOAT              ) { format = Format::RG_32_f;           return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32G32_UINT               ) { format = Format::RG_32_u;           return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32G32_SINT               ) { format = Format::RG_32_s;           return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32G8X24_TYPELESS         ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_D32_FLOAT_S8X24_UINT      ) { format = Format::DS_32_f_8;         return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS  ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_X32_TYPELESS_G8X24_UINT   ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_R10G10B10A2_TYPELESS      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM         ) { format = Format::RGBA_10_10_10_2;   return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R10G10B10A2_UINT          ) { format = Format::RGBA_10_10_10_2_u; return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R11G11B10_FLOAT           ) { format = Format::RGB_11_11_10_uf;   return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R8G8B8A8_TYPELESS         ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM            ) { format = Format::RGBA_8;            return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB       ) { format = Format::RGBA_8_sRGB;       return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R8G8B8A8_UINT             ) { format = Format::RGBA_8_u;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R8G8B8A8_SNORM            ) { format = Format::RGBA_8_sn;         return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R8G8B8A8_SINT             ) { format = Format::RGBA_8_s;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R16G16_TYPELESS           ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_R16G16_FLOAT              ) { format = Format::RG_16_hf;          return true; } // ???? do they expect type conversion f32 -> f16 on load ???
      if (header10->dxgiFormat == DXGI_FORMAT_R16G16_UNORM              ) { format = Format::RG_16;             return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R16G16_UINT               ) { format = Format::RG_16_u;           return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R16G16_SNORM              ) { format = Format::RG_16_sn;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R16G16_SINT               ) { format = Format::RG_16_s;           return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32_TYPELESS              ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_D32_FLOAT                 ) { format = Format::D_32_f;            return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32_FLOAT                 ) { format = Format::R_32_f;            return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32_UINT                  ) { format = Format::R_32_u;            return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R32_SINT                  ) { format = Format::R_32_s;            return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R24G8_TYPELESS            ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_D24_UNORM_S8_UINT         ) { format = Format::DS_24_8;           return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R24_UNORM_X8_TYPELESS     ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_X24_TYPELESS_G8_UINT      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_R8G8_TYPELESS             ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_R8G8_UNORM                ) { format = Format::RG_8;              return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R8G8_UINT                 ) { format = Format::RG_8_u;            return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R8G8_SNORM                ) { format = Format::RG_8_sn;           return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R8G8_SINT                 ) { format = Format::RG_8_s;            return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R16_TYPELESS              ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_R16_FLOAT                 ) { format = Format::R_16_hf;           return true; } /// ???? do they expect type conversion f32 -> f16 on load ???
      if (header10->dxgiFormat == DXGI_FORMAT_D16_UNORM                 ) { format = Format::D_16;              return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R16_UNORM                 ) { format = Format::R_16;              return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R16_UINT                  ) { format = Format::R_16_u;            return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R16_SNORM                 ) { format = Format::R_16_sn;           return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R16_SINT                  ) { format = Format::R_16_s;            return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R8_TYPELESS               ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_R8_UNORM                  ) { format = Format::R_8;               return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R8_UINT                   ) { format = Format::R_8_u;             return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R8_SNORM                  ) { format = Format::R_8_sn;            return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R8_SINT                   ) { format = Format::R_8_s;             return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_A8_UNORM                  ) { format = Format::R_8;               return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R1_UNORM                  ) return false;  // ????  FormatR_1; ?????
      if (header10->dxgiFormat == DXGI_FORMAT_R9G9B9E5_SHAREDEXP        ) { format = Format::RGBE_9_9_9_5_uf;   return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_R8G8_B8G8_UNORM           ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_G8R8_G8B8_UNORM           ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_BC1_TYPELESS              ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_BC1_UNORM                 ) { format = Format::BC1_RGB;           return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_BC1_UNORM_SRGB            ) { format = Format::BC1_RGB_sRGB;      return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_BC2_TYPELESS              ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_BC2_UNORM                 ) { format = Format::BC2_RGBA;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_BC2_UNORM_SRGB            ) { format = Format::BC2_RGBA_sRGB;     return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_BC3_TYPELESS              ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_BC3_UNORM                 ) { format = Format::BC3_RGBA;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_BC3_UNORM_SRGB            ) { format = Format::BC3_RGBA_sRGB;     return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_BC4_TYPELESS              ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_BC4_UNORM                 ) { format = Format::BC4_R;             return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_BC4_SNORM                 ) { format = Format::BC4_R_sn;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_BC5_TYPELESS              ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_BC5_UNORM                 ) { format = Format::BC5_RG;            return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_BC5_SNORM                 ) { format = Format::BC5_RG_sn;         return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM              ) { format = Format::BGR_5_6_5;         return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM            ) { format = Format::BGRA_5_5_5_1;      return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM            ) { format = Format::BGRA_8;            return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM            ) { format = Format::BGRA_8;            return true; } // X is just put in A channel
      if (header10->dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return false; // like FormatRGBA_10_10_10_2 but Alpha is not present so cannot emulate
      if (header10->dxgiFormat == DXGI_FORMAT_B8G8R8A8_TYPELESS         ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB       ) return false; // FormatBGRA_sRGB; but we only have RGBA_sRGB; ???
      if (header10->dxgiFormat == DXGI_FORMAT_B8G8R8X8_TYPELESS         ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB       ) return false; // FormatBGRA_sRGB ??? 
      if (header10->dxgiFormat == DXGI_FORMAT_BC6H_TYPELESS             ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_BC6H_UF16                 ) { format = Format::BC6H_RGB_uf;       return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_BC6H_SF16                 ) { format = Format::BC6H_RGB_f;        return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_BC7_TYPELESS              ) return false;                   
      if (header10->dxgiFormat == DXGI_FORMAT_BC7_UNORM                 ) { format = Format::BC7_RGBA;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_BC7_UNORM_SRGB            ) { format = Format::BC7_RGBA_sRGB;     return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_AYUV                      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_Y410                      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_Y416                      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_NV12                      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_P010                      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_P016                      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_420_OPAQUE                ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_YUY2                      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_Y210                      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_Y216                      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_NV11                      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_AI44                      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_IA44                      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_P8                        ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_A8P8                      ) return false;
      if (header10->dxgiFormat == DXGI_FORMAT_B4G4R4A4_UNORM            ) return false;
      // 116 - 129 are missing here                                        
      if (header10->dxgiFormat == DXGI_FORMAT_P208                      ) return false; 
      if (header10->dxgiFormat == DXGI_FORMAT_V208                      ) return false; 
      if (header10->dxgiFormat == DXGI_FORMAT_V408                      ) return false; 
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_4X4_TYPELESS         ) return false; 
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_4X4_UNORM            ) { format = Format::ASTC_4x4;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_4X4_UNORM_SRGB       ) { format = Format::ASTC_4x4_sRGB;     return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_5X4_TYPELESS         ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_5X4_UNORM            ) { format = Format::ASTC_5x4;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_5X4_UNORM_SRGB       ) { format = Format::ASTC_5x4_sRGB;     return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_5X5_TYPELESS         ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_5X5_UNORM            ) { format = Format::ASTC_5x5;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_5X5_UNORM_SRGB       ) { format = Format::ASTC_5x5_sRGB;     return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_6X5_TYPELESS         ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_6X5_UNORM            ) { format = Format::ASTC_6x5;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_6X5_UNORM_SRGB       ) { format = Format::ASTC_6x5_sRGB;     return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_6X6_TYPELESS         ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_6X6_UNORM            ) { format = Format::ASTC_6x6;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_6X6_UNORM_SRGB       ) { format = Format::ASTC_6x6_sRGB;     return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_8X5_TYPELESS         ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_8X5_UNORM            ) { format = Format::ASTC_8x5;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_8X5_UNORM_SRGB       ) { format = Format::ASTC_8x5_sRGB;     return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_8X6_TYPELESS         ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_8X6_UNORM            ) { format = Format::ASTC_8x6;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_8X6_UNORM_SRGB       ) { format = Format::ASTC_8x6_sRGB;     return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_8X8_TYPELESS         ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_8X8_UNORM            ) { format = Format::ASTC_8x8;          return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_8X8_UNORM_SRGB       ) { format = Format::ASTC_8x8_sRGB;     return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_10X5_TYPELESS        ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_10X5_UNORM           ) { format = Format::ASTC_10x5;         return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_10X5_UNORM_SRGB      ) { format = Format::ASTC_10x5_sRGB;    return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_10X6_TYPELESS        ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_10X6_UNORM           ) { format = Format::ASTC_10x6;         return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_10X6_UNORM_SRGB      ) { format = Format::ASTC_10x6_sRGB;    return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_10X8_TYPELESS        ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_10X8_UNORM           ) { format = Format::ASTC_10x8;         return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_10X8_UNORM_SRGB      ) { format = Format::ASTC_10x8_sRGB;    return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_10X10_TYPELESS       ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_10X10_UNORM          ) { format = Format::ASTC_10x10;        return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_10X10_UNORM_SRGB     ) { format = Format::ASTC_10x10_sRGB;   return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_12X10_TYPELESS       ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_12X10_UNORM          ) { format = Format::ASTC_12x10;        return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_12X10_UNORM_SRGB     ) { format = Format::ASTC_12x10_sRGB;   return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_12X12_TYPELESS       ) return false;                         
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_12X12_UNORM          ) { format = Format::ASTC_12x12;        return true; }
      if (header10->dxgiFormat == DXGI_FORMAT_ASTC_12X12_UNORM_SRGB     ) { format = Format::ASTC_12x12_sRGB;   return true; }
      }                                                                   
   else
   if (header.dwFlags & DDS_FOURCC)
      {
      // Pre DX10 DDS file format is not supporting sRGB color spaces (it's Hi -> LO)
      if (header.ddspf.dwFourCC == 36)         { format = Format::RGBA_16;       return true; } 
      if (header.ddspf.dwFourCC == 110)        { format = Format::RGBA_16_sn;    return true; } // Hi to LO
      if (header.ddspf.dwFourCC == 111)        { format = Format::R_16_hf;       return true; } 
      if (header.ddspf.dwFourCC == 112)        { format = Format::RG_16_hf;      return true; } // Hi to LO
      if (header.ddspf.dwFourCC == 113)        { format = Format::RGBA_16_hf;    return true; } // Hi to LO
      if (header.ddspf.dwFourCC == 114)        { format = Format::R_32_f;        return true; } 
      if (header.ddspf.dwFourCC == 115)        { format = Format::RG_32_f;       return true; } 
      if (header.ddspf.dwFourCC == 116)        { format = Format::RGBA_32_f;     return true; } 
      if (header.ddspf.dwFourCC == 0x31545844) { format = Format::BC1_RGBA;      return true; } // 'DXT1' -> 0x31545844
      if (header.ddspf.dwFourCC == 0x32545844) { format = Format::BC2_RGBA_pRGB; return true; } // 'DXT2' -> 0x32545844
      if (header.ddspf.dwFourCC == 0x33545844) { format = Format::BC2_RGBA;      return true; } // 'DXT3' -> 0x33545844
      if (header.ddspf.dwFourCC == 0x34545844) { format = Format::BC3_RGBA_pRGB; return true; } // 'DXT4' -> 0x34545844
      if (header.ddspf.dwFourCC == 0x35545844) { format = Format::BC3_RGBA;      return true; } // 'DXT5' -> 0x35545844
      if (header.ddspf.dwFourCC == 0x55344342) { format = Format::BC4_R;         return true; } // 'BC4U' -> 0x55344342
      if (header.ddspf.dwFourCC == 0x53344342) { format = Format::BC4_R_sn;      return true; } // 'BC4S' -> 0x53344342
      if (header.ddspf.dwFourCC == 0x32495441) { format = Format::BC5_RG;        return true; } // 'ATI2' -> 0x32495441
      if (header.ddspf.dwFourCC == 0x53354342) { format = Format::BC5_RG_sn;     return true; } // 'BC5S' -> 0x53354342
                                                                      
      // 'RGBG'  DXGI_FORMAT_R8G8_B8G8_UNORM   D3DFMT_R8G8_B8G8
      // 'GRGB'  DXGI_FORMAT_G8R8_G8B8_UNORM   D3DFMT_G8R8_G8B8
      // 'UYVY'  D3DFMT_UYVY
      // 'YUY2'  D3DFMT_YUY2
      // 117     D3DFMT_CxV8U8
      }

   if (header.dwFlags & DDS_RGBA)
      {
      if ( (header.ddspf.dwRGBBitCount == 32) &&
           (header.ddspf.dwRBitMask    == 0x000000FF) &&
           (header.ddspf.dwGBitMask    == 0x0000FF00) &&
           (header.ddspf.dwBBitMask    == 0x00FF0000) &&
           (header.ddspf.dwABitMask    == 0xFF000000) )
         { format = Format::RGBA_8;   return true; }   
   
      if ( (header.ddspf.dwRGBBitCount == 32) &&
           (header.ddspf.dwRBitMask    == 0x0000FFFF) &&
           (header.ddspf.dwGBitMask    == 0xFFFF0000) )
         { format = Format::RG_16_hf; return true; }   
      }

   return false;
   }

   std::shared_ptr<gpu::Texture> load(const std::string& filename)
   {
   using namespace en::storage;
   using namespace en::gpu;
   //assert( Gpu.screen.created() );

   uint64 offset = 0;

   // Open file
   std::shared_ptr<File> file = Storage->open(filename);
   if (!file)
      {
      file = Storage->open(en::ResourcesContext.path.textures + filename);
      if (!file)
         {
         Log << en::ResourcesContext.path.textures + filename << std::endl;
         Log << "ERROR: There is no such file!\n";
         return std::shared_ptr<gpu::Texture>(nullptr);
         }
      }
 
   // Verify minimum file size
   if (file->size() < 128)
      {
      Log << "ERROR: DDS file size is incorrect, file corrupted!\n";
      file = nullptr;
      return std::shared_ptr<gpu::Texture>(nullptr);
      }

   // Verify DDS file signature 'DDS ' -> 0x20534444
   uint32 signature;
   file->read(offset, 4, &signature);
   if (signature != 0x20534444)
      {
      Log << "ERROR: DDS file header signature is incorrect!\n";
      file = nullptr;
      return std::shared_ptr<gpu::Texture>(nullptr);
      }   

   // Verify file header struct size
   offset += 4;
   uint32 headerSize;
   file->read(offset, 4, &headerSize);
   if (headerSize != 124)
      {
      Log << "ERROR: DDS file header size is incorrect!\n";
      file = nullptr;
      return std::shared_ptr<gpu::Texture>(nullptr);
      }

   // Read default file header, and check if additional DX10/DX11 
   // extended file header is present (for support of texture arrays)
   DDS_HEADER header;
   DDS_HEADER_DXT10 header10;
   bool supportArrays = false;
   file->read(offset, 124, &header);
   offset += 124;
   if (header.ddspf.dwFlags & DDS_FOURCC)
      if (header.ddspf.dwFourCC == 0x30315844) // 'DX10' -> 0x30315844
         {
         // Verify minimum file size to store both headers
         if (file->size() < 148)
            {
            Log << "ERROR: DDS file size is incorrect, file corrupted!\n";
            file = nullptr;
            return std::shared_ptr<gpu::Texture>(nullptr);
            }

         file->read(offset, 20, &header10);
         offset += 20;
         supportArrays = true;
         }

   // Determine texture type stored in DDS
   TextureType type;
   if (!DetermineTextureType(header, (supportArrays ? &header10 : nullptr), type))
      {
      Log << "ERROR: DDS texture type unsupported!\n";
      file = nullptr;
      return std::shared_ptr<gpu::Texture>(nullptr);
      }

   // Determine texture format stored in DDS
   Format format;
   if (!DetectTextureFormat(header, (supportArrays ? &header10 : nullptr), format))
      {
      Log << "ERROR: DDS texture format unsupported!\n";
      file = nullptr;
      return std::shared_ptr<gpu::Texture>(nullptr);
      }

   // Determine texture state
   TextureState settings;
   settings.type   = type;
   settings.format = format;
   settings.width  = static_cast<uint16>(header.dwWidth);

   // Calculate texture height
   if (header.dwFlags & DDSD_HEIGHT)
      settings.height = static_cast<uint16>(header.dwHeight);

   // Calculate texture depth
   if (settings.type == TextureType::Texture3D)
      settings.layers = static_cast<uint16>(header.dwDepth);

   // Calculate amount of mipmaps
   if (header.dwCaps & DDSCAPS_COMPLEX & DDSCAPS_TEXTURE)
      settings.mipmaps = static_cast<uint8>(header.dwMipMapCount);

   // Calculate amount of texture layers
   if (supportArrays)
      settings.layers = header10.arraySize;
   if ( (settings.type == TextureType::TextureCubeMap) ||
        (settings.type == TextureType::TextureCubeMapArray) )
      settings.layers *= 6;

   // Create texture in GPU
   std::unique_ptr<gpu::Texture> texture(en::ResourcesContext.defaults.enHeapTextures->createTexture(settings));
   if (!texture)
      {
      Log << "ERROR: Cannot create texture in GPU!\n";
      file = nullptr;
      return std::shared_ptr<gpu::Texture>(nullptr);
      }

   // Load layers and mipmaps of texture to GPU
   for(uint32 layer=0; layer<settings.layers; ++layer)
      {
      uint32 mipDepth = settings.type == TextureType::Texture3D ? settings.layers : 1;
      for(uint32 mipmap=0; mipmap<settings.mipmaps; ++mipmap)
         {
         for(uint32 slice=0; slice<mipDepth; ++slice)
            {
            // Stream given surface from file to GPU memory
            uint64 surfaceSize = texture->size(mipmap);
            
            // Create staging buffer
            std::unique_ptr<gpu::Buffer> staging(en::ResourcesContext.defaults.enStagingHeap->createBuffer(gpu::BufferType::Transfer, static_cast<uint32>(surfaceSize)));
            if (!staging)
               {
               Log << "ERROR: Cannot create staging buffer!\n";
               file = nullptr;
               return texture;
               }

            // Read texture to temporary buffer
            volatile void* ptr = staging->map();
            file->read(offset, static_cast<uint32>(surfaceSize), ptr);
            staging->unmap();
          
            // TODO: In future distribute transfers to different queues in the same queue type family
            gpu::QueueType type = gpu::QueueType::Universal;
            if (Graphics->primaryDevice()->queues(gpu::QueueType::Transfer) > 0u)
               type = gpu::QueueType::Transfer;

            // Copy data from staging buffer to final texture
            std::shared_ptr<gpu::CommandBuffer> command = Graphics->primaryDevice()->createCommandBuffer(type);
            command->start();
            command->copy(*staging, 0u, settings.rowSize(mipmap), *texture, mipmap, slice);
            command->commit();
            
            // TODO:
            // here return completion handler callback !!! (no waiting for completion)
            // - this callback destroys CommandBuffer object
            // - destroys staging buffer
            //
            // Till it's done, wait for completion:
            
            command->waitUntilCompleted();
            command = nullptr;
            staging = nullptr;

            offset += surfaceSize;
            }
         mipDepth = max(1U, mipDepth >> 1);
         }
      }	

   file = nullptr;
   return texture;
   }

   }
}
