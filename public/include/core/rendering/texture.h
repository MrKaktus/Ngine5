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
//#include "core/utilities/Tproxy.h"
//#include "core/rendering/state.h"

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
   // Most formats have byte-aligned components, and the components are in 
   // C-array order (the least address comes first). For those formats that 
   // don't have power-of-2-aligned components, the first named component is
   // in the least-significant bits.
   // This corresponds to DirectX DXGI_FORMAT, Metal MTLPixelFormat and 
   // Vulkan VK_FORMAT, and is reversed to old DirectX D3DFMT format.
   //
   // Direct3D DXGI_FORMAT:    https://msdn.microsoft.com/en-us/library/windows/desktop/bb173059(v=vs.85).aspx
   // Metal IOS Pixel Formats: https://developer.apple.com/library/prerelease/ios/documentation/Metal/Reference/MetalConstants_Ref/index.html#//apple_ref/c/tdef/MTLPixelFormat
   // Metal OSX Pixel Formats: https://developer.apple.com/library/mac/documentation/Metal/Reference/MetalConstants_Ref/#//apple_ref/c/tdef/MTLPixelFormat
   // 
   // Direct3D D3DFMT:         https://msdn.microsoft.com/en-us/library/windows/desktop/bb172558(v=vs.85).aspx
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



   // Texture format
   //
   // Texture format describes how data of each texel is located in memory
   // during texture creation, as well as how it would be ordered, if read
   // to CPU register of little endian machine (similar to DXGI_FORMAT, and
   // opposed to LO-HI bits order of D3DFMT and VULKAN).
   // 
   // Channel names order reflects their order from HI to LO bits, and is
   // followed by number of bits each channel, or each channel in channel 
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
   // Examples:
   //
   // FormatRGB_3_3_2 :
   // B - byte 0 - 2 bits [1:0]
   // G - byte 0 - 3 bits [4:2]
   // R - byte 0 - 3 bits [7:5]
   //
   // FormatRGBA_10_10_10_2:
   // A - byte 0 - 2 bits [1:0]
   // B - byte 0 - 6 bits [7:2], byte 1 - 4 bits [3:0]
   // G - byte 1 - 4 bits [7:4], byte 2 - 6 bits [5:0]
   // R - byte 2 - 2 bits [7:6], byte 3 - 8 bits [7:0]
   //
   // It is important to note that texture data conversion types are not supported
   // (types that assume different upload format and different storage format) as
   // well as channels swizzled formats are not exposed here (there are only few
   // exceptions for loading specific file formats). If channels swizzling is 
   // possible, it is set up separately.
   //
   //enum TextureFormat
   //   {
   //   FormatUnsupported         = 0,
   //   FormatR_8                    , // Uncompressed formats
   //   FormatR_8_sn                 ,
   //   FormatR_8_u                  ,
   //   FormatR_8_s                  ,
   //   FormatR_16                   ,
   //   FormatR_16_sn                ,
   //   FormatR_16_u                 ,
   //   FormatR_16_s                 ,
   //   FormatR_16_hf                ,
   //   FormatR_32_u                 ,
   //   FormatR_32_s                 ,
   //   FormatR_32_f                 ,
   //// FormatR_64_u                 ,
   //   FormatRG_8                   , // RG are in fact stored reversed R is in byte 0, G is in byte 1
   //   FormatRG_8_sn                ,
   //   FormatRG_8_u                 ,
   //   FormatRG_8_s                 ,
   //   FormatRG_16                  ,
   //   FormatRG_16_sn               ,
   //   FormatRG_16_u                ,
   //   FormatRG_16_s                ,
   //   FormatRG_16_hf               ,
   //   FormatRG_32_u                ,
   //   FormatRG_32_s                ,
   //   FormatRG_32_f                ,
   //// FormatRG_64_u                ,
   //   FormatRGB_8                  ,
   //   FormatRGB_8_sRGB             ,
   //   FormatRGB_8_sn               ,
   //   FormatRGB_8_u                ,
   //   FormatRGB_8_s                ,
   //   FormatRGB_16                 ,
   //   FormatRGB_16_sn              ,
   //   FormatRGB_16_u               ,
   //   FormatRGB_16_s               ,
   //   FormatRGB_16_hf              ,
   //   FormatRGB_32_u               ,
   //   FormatRGB_32_s               ,
   //   FormatRGB_32_f               ,
   //// FormatRGB_64_u               ,
   //   FormatRGBA_8                 ,
   //   FormatRGBA_8_sRGB            ,
   //   FormatRGBA_8_sn              ,
   //   FormatRGBA_8_u               ,
   //   FormatRGBA_8_s               ,
   //   FormatRGBA_16                ,
   //   FormatRGBA_16_sn             ,
   //   FormatRGBA_16_u              ,
   //   FormatRGBA_16_s              ,
   //   FormatRGBA_16_hf             ,
   //   FormatRGBA_32_u              ,
   //   FormatRGBA_32_s              ,
   //   FormatRGBA_32_f              ,
   //// FormatRGBA_64_u              ,
   //   FormatD_16                   ,
   //   FormatD_24                   ,
   //   FormatD_32                   ,
   //   FormatD_32_f                 ,
   //   FormatS_8                    ,
   //   FormatSD_8_24                ,
   //   FormatSD_8_32_f              ,
   //   FormatRGB_5_6_5              ,  // Packed/special formats
   //   FormatBGR_5_6_5              , 
   //   FormatBGR_8                  ,  // - Special swizzled format for PNG (some Windows software use RGB_8)
   //   FormatBGR_10_11_11_f         ,
   //   FormatEBGR_5_9_9_9_f         ,
   //   FormatBGR_16                 ,  // - Special swizzled format for PNG (some Windows software use RGB_16)
   //   FormatRGBA_5_5_5_1           ,  
   //   FormatBGRA_5_5_5_1           ,  
   //   FormatARGB_1_5_5_5           ,  
   //   FormatABGR_1_5_5_5           ,  
   //   FormatABGR_8                 ,  // - Special swizzled format for: PNG
   //   FormatARGB_8                 ,  // - Special swizzled format for: BMP, TGA
   //   FormatBGRA_8                 ,
   //   FormatRGBA_10_10_10_2        ,
   //   FormatRGBA_10_10_10_2_u      ,
   //   FormatBC1_RGB                ,  // Compressed formats
   //   FormatBC1_RGB_sRGB           ,  // S3TC DXT1
   //   FormatBC1_RGBA               ,  // S3TC DXT1
   //   FormatBC1_RGBA_sRGB          ,  // S3TC DXT1
   //   FormatBC2_RGBA_pRGB          ,  // S3TC DXT2
   //   FormatBC2_RGBA               ,  // S3TC DXT3
   //   FormatBC2_RGBA_sRGB          ,  // S3TC DXT3
   //   FormatBC3_RGBA_pRGB          ,  // S3TC DXT4
   //   FormatBC3_RGBA               ,  // S3TC DXT5
   //   FormatBC3_RGBA_sRGB          ,  // S3TC DXT5
   //   FormatBC4_R                  ,  // RGTC
   //   FormatBC4_R_sn               ,  // RGTC
   //   FormatBC5_RG                 ,  // RGTC
   //   FormatBC5_RG_sn              ,  // RGTC
   //   FormatBC6H_RGB_f             ,  // BPTC
   //   FormatBC6H_RGB_uf            ,  // BPTC
   //   FormatBC7_RGBA               ,  // BPTC
   //   FormatBC7_RGBA_sRGB          ,  // BPTC
   //   FormatETC2_R_11              ,  // EAC
   //   FormatETC2_R_11_sn           ,  // EAC
   //   FormatETC2_RG_11             ,  // EAC
   //   FormatETC2_RG_11_sn          ,  // EAC
   //   FormatETC2_RGB_8             ,  // ETC1
   //   FormatETC2_RGB_8_sRGB        ,  
   //   FormatETC2_RGBA_8            ,  // EAC
   //   FormatETC2_RGBA_8_sRGB       ,  // EAC
   //   FormatETC2_RGB8_A1           ,  
   //   FormatETC2_RGB8_A1_sRGB      ,  
   //   FormatPVRTC_RGB_2            ,
   //   FormatPVRTC_RGB_2_sRGB       ,
   //   FormatPVRTC_RGB_4            ,
   //   FormatPVRTC_RGB_4_sRGB       ,
   //   FormatPVRTC_RGBA_2           ,
   //   FormatPVRTC_RGBA_2_sRGB      ,
   //   FormatPVRTC_RGBA_4           ,
   //   FormatPVRTC_RGBA_4_sRGB      ,
   //   FormatASTC_4x4               ,
   //   FormatASTC_5x4               ,
   //   FormatASTC_5x5               ,
   //   FormatASTC_6x5               ,
   //   FormatASTC_6x6               ,
   //   FormatASTC_8x5               ,
   //   FormatASTC_8x6               ,
   //   FormatASTC_8x8               ,
   //   FormatASTC_10x5              ,
   //   FormatASTC_10x6              ,
   //   FormatASTC_10x8              ,
   //   FormatASTC_10x10             ,
   //   FormatASTC_12x10             ,
   //   FormatASTC_12x12             ,
   //   FormatASTC_4x4_sRGB          ,
   //   FormatASTC_5x4_sRGB          ,
   //   FormatASTC_5x5_sRGB          ,
   //   FormatASTC_6x5_sRGB          ,
   //   FormatASTC_6x6_sRGB          ,
   //   FormatASTC_8x5_sRGB          ,
   //   FormatASTC_8x6_sRGB          ,
   //   FormatASTC_8x8_sRGB          ,
   //   FormatASTC_10x5_sRGB         ,
   //   FormatASTC_10x6_sRGB         ,
   //   FormatASTC_10x8_sRGB         ,
   //   FormatASTC_10x10_sRGB        ,
   //   FormatASTC_12x10_sRGB        ,
   //   FormatASTC_12x12_sRGB        ,
   //   TextureFormatsCount
   //   };

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

   class TextureView;
   
   class Texture : public SafeObject<Texture>
      {
      public:
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
      
      // TODO: Delete: Texture transfer is done through usage of "StagingBuffer" and "CommandBuffer" transfer calls.
      //               Are we sure? Depending on type of backing memory, there may be a performance gain in
      //               "mapping" resources:
      //               - iOS, tvOS - those are UMA devices.
      //               - Vulkan specific Heaps backing memory types
      //virtual void*    map(const uint8 mipmap = 0, const uint16 surface = 0) = 0;  // Surface is: CubeMap face, 3D depth, Array layer or CubeMapArray face-layer
      //virtual bool     unmap(void) = 0;
      
      
      virtual bool     read(uint8* buffer, const uint8 mipmap = 0, const uint16 surface = 0) const = 0; // Reads texture mipmap to given buffer (app needs to allocate it)
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
      































   //// Texture format
   ////
   //// Texture format describes amount, size and order of texture components.
   //// Order of the components correspond to their order in memory, if treating
   //// it as linear space, with bytes ordered from left to right (like D3DFMT). 
   //// For e.g:
   //// 
   //// FormatBGR_2_3_3 :
   //// B - byte 0 - bits [1:0]
   //// G - byte 0 - bits [4:2]
   //// R - byte 0 - bits [7:5]
   ////
   //// FormatARGB_2_10_10_10:
   //// A - byte 0 - 2 bits [1:0]
   //// R - byte 0 - 6 bits [7:2], byte 1 - 4 bits [3:0]
   //// G - byte 1 - 4 bits [7:4], byte 2 - 6 bits [5:0]
   //// B - byte 2 - 2 bits [7:6], byte 3 - 8 bits [7:0]
   ////
   //// Note that this type of notation is independent from machine word size
   //// and endianness, as it describes data layout in memory and not in registers.
   //// Commonly used notations describes components order from highest to lowest 
   //// bits in given data type, which means they are reversed notations to one
   //// used below.
   ////
   //// Data formats stored:
   //// unsigned normalized - (default, no postfix)
   //// signed normalized   - sn
   //// unsigned integral   - u
   //// signed integral     - s
   //// floating point      - f
   ////
   //enum TextureFormat
   //     {
   //     Unassigned                = 0,
   //     FormatR_8                    ,
   //     FormatR_8_sn                 ,
   //     FormatR_8_u                  ,
   //     FormatR_8_s                  ,
   //     FormatR_16                   ,
   //     FormatR_16_sn                ,
   //     FormatR_16_u                 ,
   //     FormatR_16_s                 ,
   //     FormatR_16_hf                ,
   //     FormatR_16_f                 ,
   //     FormatR_32_u                 ,
   //     FormatR_32_s                 ,
   //     FormatR_32_f                 ,
   //     FormatGR_8                   ,
   //     FormatGR_8_sn                ,
   //     FormatGR_8_u                 ,
   //     FormatGR_8_s                 ,
   //     FormatGR_16                  ,
   //     FormatGR_16_sn               ,
   //     FormatGR_16_u                ,
   //     FormatGR_16_s                ,
   //     FormatGR_16_hf               ,
   //     FormatGR_16_f                ,
   //     FormatGR_32_u                ,
   //     FormatGR_32_s                ,
   //     FormatGR_32_f                ,
   //     FormatRGB_3_3_2              ,
   //     FormatBGR_2_3_3              ,
   //     FormatBGR_4                  ,
   //     FormatBGR_5                  ,
   //     FormatRGB_5_6_5              ,
   //     FormatBGR_5_6_5              ,
   //     FormatRGB_8                  ,
   //     FormatBGR_8                  ,
   //     FormatBGR_8_sn               ,
   //     FormatBGR_8_u                ,
   //     FormatBGR_8_s                ,
   //     Format_sBGR_8                ,
   //     FormatBGR_10                 ,
   //     FormatRGB_11_11_10_f         ,
   //     FormatRGBe_9_9_9_5_f         ,
   //     FormatBGR_12                 ,
   //     FormatRGB_16                 ,
   //     FormatBGR_16                 ,
   //     FormatBGR_16_sn              ,
   //     FormatBGR_16_u               ,
   //     FormatBGR_16_s               ,
   //     FormatBGR_16_hf              ,
   //     FormatBGR_16_f               ,
   //     FormatBGR_32_u               ,
   //     FormatBGR_32_s               ,
   //     FormatBGR_32_f               ,
   //     FormatABGR_2                 ,
   //     FormatRGBA_4                 ,
   //     FormatBGRA_4                 ,
   //     FormatARGB_4                 ,  
   //     FormatABGR_4                 ,
   //     FormatRGBA_5_5_5_1           ,
   //     FormatBGRA_5_5_5_1           ,
   //     FormatARGB_1_5_5_5           ,
   //     FormatABGR_1_5_5_5           ,
   //     FormatRGBA_8                 ,
   //     FormatBGRA_8                 ,
   //     FormatARGB_8                 ,
   //     FormatABGR_8                 ,
   //     Format_sABGR_8               ,
   //     FormatABGR_8_sn              ,
   //     FormatABGR_8_u               ,
   //     FormatABGR_8_s               ,
   //     FormatRGBA_10_10_10_2        ,
   //     FormatBGRA_10_10_10_2        ,
   //     FormatARGB_2_10_10_10        ,
   //     FormatABGR_2_10_10_10        ,
   //     FormatRGBA_10_10_10_2_u      ,
   //     FormatBGRA_10_10_10_2_u      ,
   //     FormatARGB_2_10_10_10_u      ,
   //     FormatABGR_2_10_10_10_u      ,
   //     FormatABGR_12                ,
   //     FormatARGB_16                ,
   //     FormatABGR_16                ,
   //     FormatABGR_16_sn             ,
   //     FormatABGR_16_u              ,
   //     FormatABGR_16_s              ,
   //     FormatABGR_16_hf             ,
   //     FormatABGR_16_f              ,
   //     FormatABGR_32_u              ,
   //     FormatABGR_32_s              ,
   //     FormatABGR_32_f              ,             
   //     FormatD_16                   ,
   //     FormatD_24                   ,
   //     FormatD_32                   ,
   //     FormatD_32_f                 ,
   //     FormatDS_24_8                ,
   //     FormatDS_32f_8               ,
   //     FormatBC1_RGB                ,  // S3TC DXT1
   //     FormatBC1_RGB_sRGB           ,  // S3TC DXT1
   //     FormatBC1_RGBA               ,  // S3TC DXT1
   //     FormatBC1_RGBA_sRGB          ,  // S3TC DXT1
   //     FormatBC2_RGBA               ,  // S3TC DXT3
   //     FormatBC2_RGBA_sRGB          ,  // S3TC DXT3
   //     FormatBC3_RGBA               ,  // S3TC DXT5
   //     FormatBC3_RGBA_sRGB          ,  // S3TC DXT5
   //     FormatBC4_R                  ,  // RGTC
   //     FormatBC4_R_sn               ,  // RGTC
   //     FormatBC5_RG                 ,  // RGTC
   //     FormatBC5_RG_sn              ,  // RGTC
   //     FormatBC6H_RGB_f             ,  // BPTC
   //     FormatBC6H_RGB_uf            ,  // BPTC
   //     FormatBC7_RGBA               ,  // BPTC
   //     FormatBC7_RGBA_sRGB          ,  // BPTC
   //     FormatETC2_R_11              ,  // EAC
   //     FormatETC2_R_11_sn           ,  // EAC
   //     FormatETC2_RG_11             ,  // EAC
   //     FormatETC2_RG_11_sn          ,  // EAC
   //     FormatETC2_RGB_8             ,  // ETC1
   //     FormatETC2_RGB_8_sRGB        ,  
   //     FormatETC2_RGBA_8            ,  // EAC
   //     FormatETC2_RGBA_8_sRGB       ,  // EAC
   //     FormatETC2_RGB8_A1           ,  
   //     FormatETC2_RGB8_A1_sRGB      ,  
   //     FormatPVRTC_RGB_2            ,
   //     FormatPVRTC_RGB_2_sRGB       ,
   //     FormatPVRTC_RGB_4            ,
   //     FormatPVRTC_RGB_4_sRGB       ,
   //     FormatPVRTC_RGBA_2           ,
   //     FormatPVRTC_RGBA_2_sRGB      ,
   //     FormatPVRTC_RGBA_4           ,
   //     FormatPVRTC_RGBA_4_sRGB      ,
   //     FormatASTC_4x4               ,
   //     FormatASTC_5x4               ,
   //     FormatASTC_5x5               ,
   //     FormatASTC_6x5               ,
   //     FormatASTC_6x6               ,
   //     FormatASTC_8x5               ,
   //     FormatASTC_8x6               ,
   //     FormatASTC_8x8               ,
   //     FormatASTC_10x5              ,
   //     FormatASTC_10x6              ,
   //     FormatASTC_10x8              ,
   //     FormatASTC_10x10             ,
   //     FormatASTC_12x10             ,
   //     FormatASTC_12x12             ,
   //     FormatASTC_4x4_sRGB          ,
   //     FormatASTC_5x4_sRGB          ,
   //     FormatASTC_5x5_sRGB          ,
   //     FormatASTC_6x5_sRGB          ,
   //     FormatASTC_6x6_sRGB          ,
   //     FormatASTC_8x5_sRGB          ,
   //     FormatASTC_8x6_sRGB          ,
   //     FormatASTC_8x8_sRGB          ,
   //     FormatASTC_10x5_sRGB         ,
   //     FormatASTC_10x6_sRGB         ,
   //     FormatASTC_10x8_sRGB         ,
   //     FormatASTC_10x10_sRGB        ,
   //     FormatASTC_12x10_sRGB        ,
   //     FormatASTC_12x12_sRGB        ,
   //     TextureFormatsCount
   //     };

   //// Texture Filtering
   //enum TextureFiltering
   //     {
   //     Nearest                   = 0,
   //     NearestMipmaped              ,
   //     NearestMipmapedSmooth        ,
   //     Linear                       ,
   //     Bilinear                     ,
   //     Trilinear                    ,
   //     Anisotropic2x                ,
   //     Anisotropic4x                ,
   //     Anisotropic8x                ,
   //     Anisotropic16x               ,
   //     TextureFilteringMethodsCount
   //     };
 
   //// Texture border mode
   //enum TextureWraping
   //     {
   //     Clamp                     = 0,
   //     Repeat                       ,
   //     RepeatMirrored               ,
   //     ClampMirrored                ,
   //     ClampToBorder                ,    // Not exposed in OpenGL backend currently !!
   //     TextureWrapingMethodsCount
   //     };
  
   //// Texture border color
   //enum TextureBorder
   //   {
   //   OpaqueBlack                 = 0,
   //   OpaqueWhite                    ,
   //   TransparentBlack               ,
   //   TextureBorderColorsCount
   //   };



   //struct SamplerState
   //   {
   //   TextureFiltering magFilter;
   //   TextureFiltering minFilter;
   //   TextureWraping   coordU;
   //   TextureWraping   coordV;
   //   TextureWraping   coordW;
   //   TextureBorder    borderColor;
   //   CompareMethod    depthCompare;     // For Shadow maps sampling 
   //   float            LODbias;          // 0.0f;
   //   float            minLOD;           // 0.0f;        // Can force sampling of less detailed LOD levels (less mip levels used saves GPU memory)
   //   float            maxLOD;           // mipmaps.0f;  // Can force sampling of more detailed LOD levels

   //   SamplerState();
   //   };

   ////class Sampler
   ////   {
   ////   private:
   ////   SamplerState state;

   ////   public:
   ////   };

   ////Ptr<Sampler> Create(const SamplerState& state);



   //// Texture
   //class Texture : public ProxyInterface<class TextureDescriptor>
   //      {
   //      public:
   //      Texture();                                            // Constructor
   //      Texture(const Texture& src);                          // Copy constructor
   //      Texture(TextureDescriptor* src);                      // Copy constructor

   //      void*         map(const uint8 mipmap = 0, 
   //                        const uint16 layer = 0);            // Maps texture layer mipmap level to client memory
   //      void*         map(const en::gpu::TextureFace face, 
   //                        const uint8 mipmap = 0, 
   //                        const uint16 layer = 0);            // Maps cubemap texture layer mipmap to client memory
   //      bool          unmap(void);                            // Unmaps texture from client memory
   //      uint16        mipmaps(void) const;                    // Mipmaps count
   //      TextureFormat format(void) const;                     // Texture format
   //      TextureType   type(void) const;                       // Texture type
   //      uint16        layers(void) const;                     // Layers count
   //      uint32        width(const uint8 mipmap = 0) const;    // Mipmap width
   //      uint32        height(const uint8 mipmap = 0) const;   // Mipmap height
   //      uint32        size(const uint8 mipmap = 0) const;     // Mipmap size in bytes
   //      void          read(uint8* buffer, 
   //                         const uint8 mipmap = 0) const;     // Reads texture mipmap to given buffer (app needs to allocate it)
   //      };

   //// Texture sampler
   //class Sampler : public ProxyInterface<class SamplerDescriptor>
   //      {
   //      public:
   //      Sampler(class SamplerDescriptor* src);

   //      const char* name(void);
   //      bool set(const Texture& texture);       
   //      bool magnification(const TextureFiltering filtering);
   //      bool minification(const TextureFiltering filtering);
   //      bool wraping(const TextureWraping coordU, const TextureWraping coordV, const TextureWraping coordW = Repeat);
   //      };
   }
}

#endif
