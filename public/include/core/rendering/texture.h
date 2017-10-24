/*

 Ngine v5.0
 
 Module      : Texture.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_TEXTURE
#define ENG_CORE_RENDERING_TEXTURE

#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"

namespace en
{
   namespace gpu
   {
   // Type of texture
   enum class TextureType : uint32
      {
      Texture1D                 = 0,
      Texture1DArray               ,
      Texture2D                    ,
      Texture2DArray               ,
      Texture2DMultisample         ,
      Texture2DMultisampleArray    ,
      Texture3D                    ,
      TextureCubeMap               ,
      TextureCubeMapArray          ,
      Count
      };

   // Texture format
   //
   // Texture format describes how data of each texel is located in memory
   // during texture creation. This means that the first named component is
   // in the least-significant bits (LO to HI bits order), which corresponds 
   // to DirectX DXGI_FORMAT, Metal MTLPixelFormat and Vulkan VK_FORMAT, and 
   // is reversed to old DirectX D3DFMT format.
   // 
   // Channel names order reflects their order from LO to HI bits, and is
   // followed by number of bits in each channel, or each channel in channel 
   // group occupies. This way, texel description is independent from 
   // computing unit endiannes and machine word size. There are also 
   // possible additional postfixes describing data quantization and
   // used color space. 
   //
   // Possible quantization types:
   //
   // unsigned normalized     - (default, no postfix)
   // signed normalized       - sn
   // unsigned integral       - u
   // signed integral         - s
   // floating point          - f
   // unsigned floating point - uf
   //
   // Possible special postfixes:
   //
   // sRGB - RGB channels are in sRGB color space (default is linear color space), 
   //        Alpha channel if present is in linear color space.
   //        See more at: https://gamedevdaily.io/the-srgb-learning-curve-773b7f68cf7a#.j3b7zxdqn
   // 
   // pRGB - RGB channels are premultiplied by Alpha, and stored in linear color space
   //
   // Packed formats are described in the same order (LO to HI bits). Example:
   //
   // RGB_11_11_10_uf:
   // R - 11 bits [0:10]
   // G - 11 bits [11:21]
   // B - 10 bits [22:31]
   //
   // It is important to note that texture data conversion types are not supported
   // (types that assume different upload format and different storage format) as
   // well as channels swizzled formats are not exposed here (there are only few
   // exceptions for loading specific file formats). If channels swizzling is 
   // possible, it is set up separately.
   //
   enum class Format : uint16
      {
      Unsupported         = 0,  //
      R_8                    ,  // Uncompressed formats:
      R_8_sRGB               ,  //
      R_8_sn                 ,  //
      R_8_u                  ,  //
      R_8_s                  ,  //
      R_16                   ,  //
      R_16_sn                ,  //
      R_16_u                 ,  //
      R_16_s                 ,  //
      R_16_hf                ,  //
      R_32_u                 ,  //
      R_32_s                 ,  //
      R_32_f                 ,  //
      RG_8                   ,  //
      RG_8_sRGB              ,  //
      RG_8_sn                ,  //
      RG_8_u                 ,  //
      RG_8_s                 ,  //
      RG_16                  ,  //
      RG_16_sn               ,  //
      RG_16_u                ,  //
      RG_16_s                ,  //
      RG_16_hf               ,  //
      RG_32_u                ,  //
      RG_32_s                ,  //
      RG_32_f                ,  //
      RGB_8                  ,  // - Not reccomended due to lack of memory aligment
      RGB_8_sRGB             ,  // - Not reccomended due to lack of memory aligment
      RGB_8_sn               ,  // - Not reccomended due to lack of memory aligment
      RGB_8_u                ,  // - Not reccomended due to lack of memory aligment
      RGB_8_s                ,  // - Not reccomended due to lack of memory aligment
      RGB_16                 ,  //
      RGB_16_sn              ,  //
      RGB_16_u               ,  //
      RGB_16_s               ,  //
      RGB_16_hf              ,  //
      RGB_32_u               ,  //
      RGB_32_s               ,  //
      RGB_32_f               ,  //
      RGBA_8                 ,  //
      RGBA_8_sRGB            ,  //
      RGBA_8_sn              ,  //
      RGBA_8_u               ,  //
      RGBA_8_s               ,  //
      RGBA_16                ,  //
      RGBA_16_sn             ,  //
      RGBA_16_u              ,  //
      RGBA_16_s              ,  //
      RGBA_16_hf             ,  //
      RGBA_32_u              ,  //
      RGBA_32_s              ,  //
      RGBA_32_f              ,  //
      D_16                   ,  //
      D_24                   ,  //
      D_24_8                 ,  // - 24bit depth with 8 bit padding
      D_32                   ,  // 
      D_32_f                 ,  // 
      S_8                    ,  // 
      DS_16_8                ,  // - Stored in separate D & S channel, order is implementation dependent doesn't matter as this is GPU only texture
      DS_24_8                ,  // - Stored in separate D & S channel, order is implementation dependent doesn't matter as this is GPU only texture
      DS_32_f_8              ,  // - Stored in separate D & S channel, order is implementation dependent doesn't matter as this is GPU only texture
      RGB_5_6_5              ,  // Packed/special formats:
      BGR_5_6_5              ,  // - IOS supported, OSX not (MTLPixelFormatB5G6R5Unorm, IOS only)
      BGR_8                  ,  // - Special swizzled format for PNG used on Windows-based systems: http://www.libpng.org/pub/png/book/chapter08.html section 8.5.6
      BGR_8_sRGB             ,  // - Special swizzled SRGB format for PNG used on Windows-based systems: http://www.libpng.org/pub/png/book/chapter08.html section 8.5.6
      BGR_8_sn               ,  // 
      BGR_8_u                ,  // 
      BGR_8_s                ,  // 
      RGB_11_11_10_uf        ,  // - Packed unsigned float for HDR Textures, UAV's and Render Targets
      RGBE_9_9_9_5_uf        ,  // - Packed unsigned float for HDR Textures only
      BGRA_8                 ,  // - Special swizzled format for PNG, BMP, TGA used by Windows-based systems (MTLPixelFormatBGRA8Unorm)
      BGRA_8_sRGB            ,  // - (MTLPixelFormatBGRA8Unorm_sRGB)
      BGRA_8_sn              ,  //
      BGRA_8_u               ,  //
      BGRA_8_s               ,  //
      BGRA_5_5_5_1           ,  //
      ARGB_1_5_5_5           ,  //
      ABGR_1_5_5_5           ,  // - IOS supported, OSX not (MTLPixelFormatA1BGR5Unorm, IOS only)
      RGBA_10_10_10_2        ,  // - *Less precise than 8bit sRGB compression in dark areas
      RGBA_10_10_10_2_u      ,  //
      BGRA_10_10_10_2        ,  // - Special swizzled format for 30bpp BMP used by Windows-based systems
      BC1_RGB                ,  // Compressed formats:
      BC1_RGB_sRGB           ,  // -S3TC DXT1
      BC1_RGBA               ,  // -S3TC DXT1
      BC1_RGBA_sRGB          ,  // -S3TC DXT1
      BC2_RGBA_pRGB          ,  // -S3TC DXT2
      BC2_RGBA               ,  // -S3TC DXT3
      BC2_RGBA_sRGB          ,  // -S3TC DXT3
      BC3_RGBA_pRGB          ,  // -S3TC DXT4
      BC3_RGBA               ,  // -S3TC DXT5
      BC3_RGBA_sRGB          ,  // -S3TC DXT5
      BC4_R                  ,  // -RGTC
      BC4_R_sn               ,  // -RGTC
      BC5_RG                 ,  // -RGTC
      BC5_RG_sn              ,  // -RGTC
      BC6H_RGB_f             ,  // -BPTC
      BC6H_RGB_uf            ,  // -BPTC
      BC7_RGBA               ,  // -BPTC
      BC7_RGBA_sRGB          ,  // -BPTC
      ETC2_R_11              ,  // -EAC
      ETC2_R_11_sn           ,  // -EAC
      ETC2_RG_11             ,  // -EAC
      ETC2_RG_11_sn          ,  // -EAC
      ETC2_RGB_8             ,  // -ETC1
      ETC2_RGB_8_sRGB        ,  // 
      ETC2_RGBA_8            ,  // -EAC
      ETC2_RGBA_8_sRGB       ,  // -EAC
      ETC2_RGB8_A1           ,  //
      ETC2_RGB8_A1_sRGB      ,  //
      PVRTC_RGB_2            ,  //
      PVRTC_RGB_2_sRGB       ,  //
      PVRTC_RGB_4            ,  //
      PVRTC_RGB_4_sRGB       ,  //
      PVRTC_RGBA_2           ,  //
      PVRTC_RGBA_2_sRGB      ,  //
      PVRTC_RGBA_4           ,  //
      PVRTC_RGBA_4_sRGB      ,  //
      ASTC_4x4               ,  //
      ASTC_5x4               ,  //
      ASTC_5x5               ,  //
      ASTC_6x5               ,  //
      ASTC_6x6               ,  //
      ASTC_8x5               ,  //
      ASTC_8x6               ,  //
      ASTC_8x8               ,  //
      ASTC_10x5              ,  //
      ASTC_10x6              ,  //
      ASTC_10x8              ,  //
      ASTC_10x10             ,  //
      ASTC_12x10             ,  //
      ASTC_12x12             ,  //
      ASTC_4x4_sRGB          ,  //
      ASTC_5x4_sRGB          ,  //
      ASTC_5x5_sRGB          ,  //
      ASTC_6x5_sRGB          ,  //
      ASTC_6x6_sRGB          ,  //
      ASTC_8x5_sRGB          ,  //
      ASTC_8x6_sRGB          ,  //
      ASTC_8x8_sRGB          ,  //
      ASTC_10x5_sRGB         ,  //
      ASTC_10x6_sRGB         ,  //
      ASTC_10x8_sRGB         ,  //
      ASTC_10x10_sRGB        ,  //
      ASTC_12x10_sRGB        ,  //
      ASTC_12x12_sRGB        ,  //
      Count
      };

   // Examples:
   // - Static        - TextureUsage::Read
   // - ShadowMap     - TextureUsage::RenderTargetWrite | TextureUsage::Read
   // - DepthBuffer   - TextureUsage::RenderTargetWrite
   // - GBuffer Depth - TextureUsage::RenderTargetWrite | TextureUsage::Read
   //
   enum class TextureUsage : uint32
      {
      Read                = 0x0001,  // Allows Reading and Sampling Textures by Shaders
      Write               = 0x0002,  // Allows Writing to Textures (for eg. by Compute Shaders)
      Atomic              = 0x0004,  // Allows Atomic operations on Texture
      RenderTargetRead    = 0x0008,  // Texture can be one of Color, Depth, Stencil read sources during rendering operations
      RenderTargetWrite   = 0x0010,  // Texture can be one of Color, Depth, Stencil destination for Rendering operations
      MultipleViews       = 0x0020,  // Allows creation of multiple Texture Views from one Texture
      Streamed            = 0x0040,  // Optimal for fast streaming of data between CPU and GPU
      Sparse              = 0x0100,  // Texture is partially backed with memory
      };

   inline TextureUsage operator| (TextureUsage a, TextureUsage b)
   {
   return static_cast<TextureUsage>(underlyingType(a) | underlyingType(b));
   }

   // Texture face
   enum TextureFace
      {
      Right                     = 0,
      Left                         ,
      Top                          ,
      Bottom                       ,
      Front                        ,
      Back                         ,
      TextureFacesCount
      };

   // Structure defining texture data layout in linear memory,
   // for copying from staging buffer to destination texture.
   struct LinearAlignment
      {
      uint64 size;      // Total size of linear block of memory
      uint32 alignment; // Required alignment for data adress
      uint32 rowSize;   // Size of single row in bytes, with padding alignment. If it matches size of data in row, whole texture can be read as single linear block.
      uint32 rowsCount; // Count of separate rows (rows of texels or blocks of texels for compressed resources).
      };

   // Texture settings structure
   struct TextureState
      {
      TextureType   type;
      Format        format;   
      TextureUsage  usage;
      uint16        width;
      uint16        height;
      uint16        depth;
      uint16        layers;   // Layers of Array texture, or Layers * Faces for CubeMap Array texture
      uint16        samples;
      uint16        mipmaps;  // Ignored, will be calculated automatically during texture creation TODO: Shouldn't be ignored we may want to create partial mipmap stack for streaming
      TextureState();
      TextureState(const TextureType type, 
         const Format format,
         const TextureUsage usage,
         const uint16 width,
         const uint16 height = 1,
         const uint16 depth = 1,
         const uint16 layers = 1,
         const uint16 samples = 1,
         const uint16 mipmaps = 1);
      
      bool operator !=(const TextureState& b);
      };

   class Heap;
   class TextureView;
   
   class Texture : public SafeObject<Texture>
      {
      public:
      virtual Ptr<Heap> parent(void) const = 0;
      virtual TextureType type(void) const = 0;
      virtual Format   format(void) const = 0;
      virtual uint32   mipmaps(void) const = 0;
      virtual uint32   size(const uint8 mipmap = 0) const = 0;
      virtual uint32   width(const uint8 mipmap = 0) const = 0;
      virtual uint32   height(const uint8 mipmap = 0) const = 0;
      virtual uint32   depth(const uint8 mipmap = 0) const = 0;
      virtual uint32v2 resolution(const uint8 mipmap = 0) const = 0;
      virtual uint32v3 volume(const uint8 mipmap = 0) const = 0;
      virtual uint16   layers(void) const = 0;
      virtual uint16   samples(void) const = 0;

      virtual Ptr<TextureView> view(void) const = 0;                  // Default view representing this texture
      
      // "layer" parameter can pass specific information, for specific texture types:
      // - for 3D it represents "depth" slice
      // - for CubeMap it represents "face" surface
      // - for CubeMapArray it represents "layer-face" surface
      virtual Ptr<TextureView> view(const TextureType type,           // Creates new texture view with given
                                    const Format format,              // type, format, base mipmap and mipmaps count
                                    const uint32v2 mipmaps,           // as well as base layer and layers count
                                    const uint32v2 layers) const = 0;
      
      virtual ~Texture() {};                           // Polymorphic deletes require a virtual base destructor
      };

   class TextureView : public SafeObject<TextureView>
      {
      public:
      virtual Ptr<Texture> parent(void) const = 0;
      virtual TextureType  type(void) const = 0;
      virtual Format       format(void) const = 0;
      virtual uint32v2 parentMipmaps(void) const = 0;    // Sub-set of parent texture mipmaps, creating this view
      virtual uint32v2 parentLayers(void) const = 0;     // Sub-set of parent texture layers, creating this view
      
      virtual ~TextureView() {};                       // Polymorphic deletes require a virtual base destructor
      };
      
   // Device independent, texel size in bytes, based on the given format. 
   // It is not taking into account texel padding or any other device
   // specific optimizations. For compressed formats, it's texel block size.
   extern uint32 genericTexelSize(const Format format);
   }
}

#endif
