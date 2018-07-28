/*

 Ngine v5.0
 
 Module      : D3D12 Texture.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12Texture.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12Device.h"  // Profile

namespace en
{
   namespace gpu
   {
   const DXGI_FORMAT TranslateTextureFormat[underlyingType(Format::Count)] = 
      { // Sized Internal Format                     
      DXGI_FORMAT_UNKNOWN                 ,   // Format::Unsupported
      DXGI_FORMAT_R8_UNORM                ,   // Format::R_8                    Uncompressed formats:
      DXGI_FORMAT_UNKNOWN                 ,   // Format::R_8_sRGB               (unsupported)
      DXGI_FORMAT_R8_SNORM                ,   // Format::R_8_sn
      DXGI_FORMAT_R8_UINT                 ,   // Format::R_8_u
      DXGI_FORMAT_R8_SINT                 ,   // Format::R_8_s
      DXGI_FORMAT_R16_UNORM               ,   // Format::R_16
      DXGI_FORMAT_R16_SNORM               ,   // Format::R_16_sn
      DXGI_FORMAT_R16_UINT                ,   // Format::R_16_u
      DXGI_FORMAT_R16_SINT                ,   // Format::R_16_s
      DXGI_FORMAT_R16_FLOAT               ,   // Format::R_16_hf
      DXGI_FORMAT_R32_UINT                ,   // Format::R_32_u
      DXGI_FORMAT_R32_SINT                ,   // Format::R_32_s
      DXGI_FORMAT_R32_FLOAT               ,   // Format::R_32_f
      DXGI_FORMAT_R8G8_UNORM              ,   // Format::RG_8
      DXGI_FORMAT_UNKNOWN                 ,   // Format::RG_8_sRGB              (unsupported)
      DXGI_FORMAT_R8G8_SNORM              ,   // Format::RG_8_sn
      DXGI_FORMAT_R8G8_UINT               ,   // Format::RG_8_u
      DXGI_FORMAT_R8G8_SINT               ,   // Format::RG_8_s
      DXGI_FORMAT_R16G16_UNORM            ,   // Format::RG_16
      DXGI_FORMAT_R16G16_SNORM            ,   // Format::RG_16_sn
      DXGI_FORMAT_R16G16_UINT             ,   // Format::RG_16_u
      DXGI_FORMAT_R16G16_SINT             ,   // Format::RG_16_s
      DXGI_FORMAT_R16G16_FLOAT            ,   // Format::RG_16_hf
      DXGI_FORMAT_R32G32_UINT             ,   // Format::RG_32_u
      DXGI_FORMAT_R32G32_SINT             ,   // Format::RG_32_s
      DXGI_FORMAT_R32G32_FLOAT            ,   // Format::RG_32_f
      DXGI_FORMAT_UNKNOWN                 ,   // Format::RGB_8                  (unsupported) - Not reccomended due to lack of memory aligment
      DXGI_FORMAT_UNKNOWN                 ,   // Format::RGB_8_sRGB             (unsupported) - Not reccomended due to lack of memory aligment
      DXGI_FORMAT_UNKNOWN                 ,   // Format::RGB_8_sn               (unsupported) - Not reccomended due to lack of memory aligment
      DXGI_FORMAT_UNKNOWN                 ,   // Format::RGB_8_u                (unsupported) - Not reccomended due to lack of memory aligment
      DXGI_FORMAT_UNKNOWN                 ,   // Format::RGB_8_s                (unsupported) - Not reccomended due to lack of memory aligment
      DXGI_FORMAT_UNKNOWN                 ,   // Format::RGB_16                 (unsupported)
      DXGI_FORMAT_UNKNOWN                 ,   // Format::RGB_16_sn              (unsupported)
      DXGI_FORMAT_UNKNOWN                 ,   // Format::RGB_16_u               (unsupported)
      DXGI_FORMAT_UNKNOWN                 ,   // Format::RGB_16_s               (unsupported)
      DXGI_FORMAT_UNKNOWN                 ,   // Format::RGB_16_hf              (unsupported)
      DXGI_FORMAT_R32G32B32_UINT          ,   // Format::RGB_32_u
      DXGI_FORMAT_R32G32B32_SINT          ,   // Format::RGB_32_s
      DXGI_FORMAT_R32G32B32_FLOAT         ,   // Format::RGB_32_f
      DXGI_FORMAT_R8G8B8A8_UNORM          ,   // Format::RGBA_8
      DXGI_FORMAT_R8G8B8A8_UNORM_SRGB     ,   // Format::RGBA_8_sRGB
      DXGI_FORMAT_R8G8B8A8_SNORM          ,   // Format::RGBA_8_sn
      DXGI_FORMAT_R8G8B8A8_UINT           ,   // Format::RGBA_8_u
      DXGI_FORMAT_R8G8B8A8_SINT           ,   // Format::RGBA_8_s
      DXGI_FORMAT_R16G16B16A16_UNORM      ,   // Format::RGBA_16
      DXGI_FORMAT_R16G16B16A16_SNORM      ,   // Format::RGBA_16_sn
      DXGI_FORMAT_R16G16B16A16_UINT       ,   // Format::RGBA_16_u
      DXGI_FORMAT_R16G16B16A16_SINT       ,   // Format::RGBA_16_s
      DXGI_FORMAT_R16G16B16A16_FLOAT      ,   // Format::RGBA_16_hf
      DXGI_FORMAT_R32G32B32A32_UINT       ,   // Format::RGBA_32_u            
      DXGI_FORMAT_R32G32B32A32_SINT       ,   // Format::RGBA_32_s
      DXGI_FORMAT_R32G32B32A32_FLOAT      ,   // Format::RGBA_32_f
      DXGI_FORMAT_D16_UNORM               ,   // Format::D_16
      DXGI_FORMAT_UNKNOWN                 ,   // Format::D_24
      DXGI_FORMAT_UNKNOWN                 ,   // Format::D_24_8                 - 24bit depth with 8 bit padding
      DXGI_FORMAT_UNKNOWN                 ,   // Format::D_32
      DXGI_FORMAT_D32_FLOAT               ,   // Format::D_32_f
      DXGI_FORMAT_UNKNOWN                 ,   // Format::S_8                    TODO: ???? is it supported ???? or R8_UINT ???
      DXGI_FORMAT_UNKNOWN                 ,   // Format::DS_16_8                - Stored in separate D & S channel, order is implementation dependent doesn't matter as this is GPU only texture
      DXGI_FORMAT_D24_UNORM_S8_UINT       ,   // Format::DS_24_8                - Stored in separate D & S channel, order is implementation dependent doesn't matter as this is GPU only texture
      DXGI_FORMAT_D32_FLOAT_S8X24_UINT    ,   // Format::DS_32_f_8              - Stored in separate D & S channel, order is implementation dependent doesn't matter as this is GPU only texture
      DXGI_FORMAT_UNKNOWN                 ,   // Format::RGB_5_6_5              Packed/special formats:
      DXGI_FORMAT_B5G6R5_UNORM            ,   // Format::BGR_5_6_5              - IOS supported, OSX not (MTLPixelFormatB5G6R5Unorm, IOS only)
      DXGI_FORMAT_B8G8R8X8_UNORM          ,   // Format::BGR_8                  - Special swizzled format for PNG used on Windows-based systems: http://www.libpng.org/pub/png/book/chapter08.html section 8.5.6
      DXGI_FORMAT_B8G8R8X8_UNORM_SRGB     ,   // Format::BGR_8_sRGB
      DXGI_FORMAT_UNKNOWN                 ,   // Format::BGR_8_sn
      DXGI_FORMAT_UNKNOWN                 ,   // Format::BGR_8_u
      DXGI_FORMAT_UNKNOWN                 ,   // Format::BGR_8_s
      DXGI_FORMAT_R11G11B10_FLOAT         ,   // Format::RGB_11_11_10_uf        - Packed unsigned float for HDR Textures, UAV's and Render Targets
      DXGI_FORMAT_R9G9B9E5_SHAREDEXP      ,   // Format::RGBE_9_9_9_5_uf        - Packed unsigned float for HDR Textures only
      DXGI_FORMAT_B8G8R8A8_UNORM          ,   // Format::BGRA_8                 - (MTLPixelFormatBGRA8Unorm)
      DXGI_FORMAT_B8G8R8A8_UNORM_SRGB     ,   // Format::BGRA_8_sRGB            - (MTLPixelFormatBGRA8Unorm_sRGB)
      DXGI_FORMAT_R8G8B8A8_SNORM          ,   // Format::BGRA_8_sn              (emulated with swizzling)
      DXGI_FORMAT_R8G8B8A8_UINT           ,   // Format::BGRA_8_u               (emulated with swizzling)
      DXGI_FORMAT_R8G8B8A8_SINT           ,   // Format::BGRA_8_s               (emulated with swizzling)
      DXGI_FORMAT_B5G5R5A1_UNORM          ,   // Format::BGRA_5_5_5_1
      DXGI_FORMAT_B5G5R5A1_UNORM          ,   // Format::ARGB_1_5_5_5           (emulated with swizzling)
      DXGI_FORMAT_B5G5R5A1_UNORM          ,   // Format::ABGR_1_5_5_5           (emulated with swizzling) - IOS supported, OSX not (MTLPixelFormatA1BGR5Unorm, IOS only)
      DXGI_FORMAT_R10G10B10A2_UNORM       ,   // Format::RGBA_10_10_10_2        - *Less precise than 8bit sRGB compression in dark areas
      DXGI_FORMAT_R10G10B10A2_UINT        ,   // Format::RGBA_10_10_10_2_u
      DXGI_FORMAT_R10G10B10A2_UNORM       ,   // Format::BGRA_10_10_10_2        (emulated with swizzling) - Special swizzled format for 30bpp BMP used by Windows-based systems
      DXGI_FORMAT_UNKNOWN                 ,   // Format::BC1_RGB                Compressed formats:
      DXGI_FORMAT_UNKNOWN                 ,   // Format::BC1_RGB_sRGB           -S3TC DXT1
      DXGI_FORMAT_BC1_UNORM               ,   // Format::BC1_RGBA               -S3TC DXT1
      DXGI_FORMAT_BC1_UNORM_SRGB          ,   // Format::BC1_RGBA_sRGB          -S3TC DXT1
      DXGI_FORMAT_UNKNOWN                 ,   // Format::BC2_RGBA_pRGB          -S3TC DXT2
      DXGI_FORMAT_BC2_UNORM               ,   // Format::BC2_RGBA               -S3TC DXT3
      DXGI_FORMAT_BC2_UNORM_SRGB          ,   // Format::BC2_RGBA_sRGB          -S3TC DXT3
      DXGI_FORMAT_UNKNOWN                 ,   // Format::BC3_RGBA_pRGB          -S3TC DXT4
      DXGI_FORMAT_BC3_UNORM               ,   // Format::BC3_RGBA               -S3TC DXT5
      DXGI_FORMAT_BC3_UNORM_SRGB          ,   // Format::BC3_RGBA_sRGB          -S3TC DXT5
      DXGI_FORMAT_BC4_UNORM               ,   // Format::BC4_R                  -RGTC
      DXGI_FORMAT_BC4_SNORM               ,   // Format::BC4_R_sn               -RGTC
      DXGI_FORMAT_BC5_UNORM               ,   // Format::BC5_RG                 -RGTC
      DXGI_FORMAT_BC5_SNORM               ,   // Format::BC5_RG_sn              -RGTC
      DXGI_FORMAT_BC6H_SF16               ,   // Format::BC6H_RGB_f             -BPTC
      DXGI_FORMAT_BC6H_UF16               ,   // Format::BC6H_RGB_uf            -BPTC
      DXGI_FORMAT_BC7_UNORM               ,   // Format::BC7_RGBA               -BPTC
      DXGI_FORMAT_BC7_UNORM_SRGB          ,   // Format::BC7_RGBA_sRGB          -BPTC
      DXGI_FORMAT_UNKNOWN                 ,   // Format::ETC2_R_11              -EAC
      DXGI_FORMAT_UNKNOWN                 ,   // Format::ETC2_R_11_sn           -EAC
      DXGI_FORMAT_UNKNOWN                 ,   // Format::ETC2_RG_11             -EAC
      DXGI_FORMAT_UNKNOWN                 ,   // Format::ETC2_RG_11_sn          -EAC
      DXGI_FORMAT_UNKNOWN                 ,   // Format::ETC2_RGB_8             -ETC1
      DXGI_FORMAT_UNKNOWN                 ,   // Format::ETC2_RGB_8_sRGB
      DXGI_FORMAT_UNKNOWN                 ,   // Format::ETC2_RGBA_8            -EAC
      DXGI_FORMAT_UNKNOWN                 ,   // Format::ETC2_RGBA_8_sRGB       -EAC
      DXGI_FORMAT_UNKNOWN                 ,   // Format::ETC2_RGB8_A1
      DXGI_FORMAT_UNKNOWN                 ,   // Format::ETC2_RGB8_A1_sRGB
      DXGI_FORMAT_UNKNOWN                 ,   // Format::PVRTC_RGB_2
      DXGI_FORMAT_UNKNOWN                 ,   // Format::PVRTC_RGB_2_sRGB
      DXGI_FORMAT_UNKNOWN                 ,   // Format::PVRTC_RGB_4
      DXGI_FORMAT_UNKNOWN                 ,   // Format::PVRTC_RGB_4_sRGB
      DXGI_FORMAT_UNKNOWN                 ,   // Format::PVRTC_RGBA_2
      DXGI_FORMAT_UNKNOWN                 ,   // Format::PVRTC_RGBA_2_sRGB
      DXGI_FORMAT_UNKNOWN                 ,   // Format::PVRTC_RGBA_4
      DXGI_FORMAT_UNKNOWN                 ,   // Format::PVRTC_RGBA_4_sRGB            WA for missing DXGI enums:
      (DXGI_FORMAT)134                    ,   // Format::ASTC_4x4                     DXGI_FORMAT_ASTC_4X4_UNORM        
      (DXGI_FORMAT)138                    ,   // Format::ASTC_5x4                     DXGI_FORMAT_ASTC_5X4_UNORM        
      (DXGI_FORMAT)142                    ,   // Format::ASTC_5x5                     DXGI_FORMAT_ASTC_5X5_UNORM        
      (DXGI_FORMAT)146                    ,   // Format::ASTC_6x5                     DXGI_FORMAT_ASTC_6X5_UNORM        
      (DXGI_FORMAT)150                    ,   // Format::ASTC_6x6                     DXGI_FORMAT_ASTC_6X6_UNORM        
      (DXGI_FORMAT)154                    ,   // Format::ASTC_8x5                     DXGI_FORMAT_ASTC_8X5_UNORM        
      (DXGI_FORMAT)158                    ,   // Format::ASTC_8x6                     DXGI_FORMAT_ASTC_8X6_UNORM        
      (DXGI_FORMAT)162                    ,   // Format::ASTC_8x8                     DXGI_FORMAT_ASTC_8X8_UNORM        
      (DXGI_FORMAT)166                    ,   // Format::ASTC_10x5                    DXGI_FORMAT_ASTC_10X5_UNORM       
      (DXGI_FORMAT)170                    ,   // Format::ASTC_10x6                    DXGI_FORMAT_ASTC_10X6_UNORM       
      (DXGI_FORMAT)174                    ,   // Format::ASTC_10x8                    DXGI_FORMAT_ASTC_10X8_UNORM       
      (DXGI_FORMAT)178                    ,   // Format::ASTC_10x10                   DXGI_FORMAT_ASTC_10X10_UNORM      
      (DXGI_FORMAT)182                    ,   // Format::ASTC_12x10                   DXGI_FORMAT_ASTC_12X10_UNORM      
      (DXGI_FORMAT)186                    ,   // Format::ASTC_12x12                   DXGI_FORMAT_ASTC_12X12_UNORM      
      (DXGI_FORMAT)135                    ,   // Format::ASTC_4x4_sRGB                DXGI_FORMAT_ASTC_4X4_UNORM_SRGB   
      (DXGI_FORMAT)139                    ,   // Format::ASTC_5x4_sRGB                DXGI_FORMAT_ASTC_5X4_UNORM_SRGB   
      (DXGI_FORMAT)143                    ,   // Format::ASTC_5x5_sRGB                DXGI_FORMAT_ASTC_5X5_UNORM_SRGB   
      (DXGI_FORMAT)147                    ,   // Format::ASTC_6x5_sRGB                DXGI_FORMAT_ASTC_6X5_UNORM_SRGB   
      (DXGI_FORMAT)151                    ,   // Format::ASTC_6x6_sRGB                DXGI_FORMAT_ASTC_6X6_UNORM_SRGB   
      (DXGI_FORMAT)155                    ,   // Format::ASTC_8x5_sRGB                DXGI_FORMAT_ASTC_8X5_UNORM_SRGB   
      (DXGI_FORMAT)159                    ,   // Format::ASTC_8x6_sRGB                DXGI_FORMAT_ASTC_8X6_UNORM_SRGB   
      (DXGI_FORMAT)163                    ,   // Format::ASTC_8x8_sRGB                DXGI_FORMAT_ASTC_8X8_UNORM_SRGB   
      (DXGI_FORMAT)167                    ,   // Format::ASTC_10x5_sRGB               DXGI_FORMAT_ASTC_10X5_UNORM_SRGB  
      (DXGI_FORMAT)171                    ,   // Format::ASTC_10x6_sRGB               DXGI_FORMAT_ASTC_10X6_UNORM_SRGB  
      (DXGI_FORMAT)175                    ,   // Format::ASTC_10x8_sRGB               DXGI_FORMAT_ASTC_10X8_UNORM_SRGB  
      (DXGI_FORMAT)179                    ,   // Format::ASTC_10x10_sRGB              DXGI_FORMAT_ASTC_10X10_UNORM_SRGB 
      (DXGI_FORMAT)183                    ,   // Format::ASTC_12x10_sRGB              DXGI_FORMAT_ASTC_12X10_UNORM_SRGB 
      (DXGI_FORMAT)187                    ,   // Format::ASTC_12x12_sRGB              DXGI_FORMAT_ASTC_12X12_UNORM_SRGB 
      };                 

   // D3D ASTC Support - https://msdn.microsoft.com/en-us/library/windows/desktop/dn903790(v=vs.85).aspx
   
   // TODO: Is D3D12 supporting single Stencil 8 bit attachment format - Format::S_8 ?
   //       Can BGR_8_sRGB emulated with DXGI_FORMAT_B8G8R8X8_UNORM_SRGB ?
   //       Can BGR_8 emulated with DXGI_FORMAT_B8G8R8X8_UNORM ?
   //       Can DXGI_FORMAT_B8G8R8X8_TYPELESS be used to emulate Format::BGR_8_sn BGR_8_u BGR_8_s ?
   
   static const D3D12_RESOURCE_DIMENSION TranslateTextureDimension[underlyingType(TextureType::Count)] =
      {
      D3D12_RESOURCE_DIMENSION_TEXTURE1D,     // Texture1D
      D3D12_RESOURCE_DIMENSION_TEXTURE1D,     // Texture1DArray
      D3D12_RESOURCE_DIMENSION_TEXTURE2D,     // Texture2D
      D3D12_RESOURCE_DIMENSION_TEXTURE2D,     // Texture2DArray
      D3D12_RESOURCE_DIMENSION_TEXTURE2D,     // Texture2DMultisample
      D3D12_RESOURCE_DIMENSION_TEXTURE2D,     // Texture2DMultisampleArray
      D3D12_RESOURCE_DIMENSION_TEXTURE3D,     // Texture3D
      D3D12_RESOURCE_DIMENSION_TEXTURE2D,     // TextureCubeMap
      D3D12_RESOURCE_DIMENSION_TEXTURE2D      // TextureCubeMapArray
      };
      
   // Optimisation: This table is not needed. Backend type can be directly cast to D3D12 type by adding 2.
   static const D3D12_SRV_DIMENSION TranslateTextureType[underlyingType(TextureType::Count)] =
      {
      D3D12_SRV_DIMENSION_TEXTURE1D,             // Texture1D
      D3D12_SRV_DIMENSION_TEXTURE1DARRAY,        // Texture1DArray
      D3D12_SRV_DIMENSION_TEXTURE2D,             // Texture2D
      D3D12_SRV_DIMENSION_TEXTURE2DARRAY,        // Texture2DArray
      D3D12_SRV_DIMENSION_TEXTURE2DMS,           // Texture2DMultisample
      D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY,      // Texture2DMultisampleArray
      D3D12_SRV_DIMENSION_TEXTURE3D,             // Texture3D
      D3D12_SRV_DIMENSION_TEXTURECUBE,           // TextureCubeMap
      D3D12_SRV_DIMENSION_TEXTURECUBEARRAY       // TextureCubeMapArray
      };

   // WA:
   // According to https://msdn.microsoft.com/en-us/library/windows/desktop/mt186591(v=vs.85).aspx
   // this helper function should be located in D3dx12.h, but during linking it is not beeing found,
   // so as WA, it is explicitly implemented here.
   // D3D stores surfaces of all mip levels together, repeated array size times.
   // Then everything is repeated again for secondary plane.
   UINT D3D12CalcSubresource(UINT MipSlice, UINT ArraySlice, UINT PlaneSlice, UINT MipLevels, UINT ArraySize)
   {
   return MipSlice + ArraySlice * MipLevels + PlaneSlice * MipLevels * ArraySize;
   }

   TextureD3D12::TextureD3D12(shared_ptr<HeapD3D12> _heap,
                              ID3D12Resource* _handle,
                              uint64 _offset,
                              uint64 _size,
                              const TextureState& _state) :
      heap(_heap),
      handle(_handle),
      offset(_offset),
      textureSize(_size),
      CommonTexture(_state)
   {
   }

   TextureD3D12::TextureD3D12(Direct3D12Device* gpu,
                   const TextureState& _state) :
      heap(nullptr),
      handle(nullptr),
      offset(0),
      textureSize(0),
      CommonTexture(_state)
   {
   // App needs to set handle after Texture object creation.
   }
   
   TextureD3D12::~TextureD3D12()
   {
   assert( handle );
   ValidateCom( handle->Release() )
   handle = nullptr;
   
   // Textures backed with Swap-Chain surfaces have no backing heap.
   if (heap)
      {
      // Deallocate from the Heap (let Heap allocator know that memory region is available again)
      heap->allocator->deallocate(offset, textureSize);
      heap = nullptr;
      }
   }
   
   shared_ptr<Heap> TextureD3D12::parent(void) const
   {
   return heap;
   }

   shared_ptr<TextureView> TextureD3D12::view(void)
   {
   return view(state.type, state.format, uint32v2(0, state.mipmaps), uint32v2(0, state.layers));
   }

   shared_ptr<TextureView> TextureD3D12::view(const TextureType _type,
      const Format _format,
      const uint32v2 _mipmaps,
      const uint32v2 _layers)
   {
   shared_ptr<TextureViewD3D12> view = make_shared<TextureViewD3D12>(dynamic_pointer_cast<TextureD3D12>(shared_from_this()),
                                                                     _type,
                                                                     _format,
                                                                     _mipmaps,
                                                                     _layers);
   
   //// Cache mipmaps and layers range, if View
   //// of other underlying type will be needed.
   //view->viewType = _type;
   //view->mipmaps  = _mipmaps;
   //view->layers   = _layers;
   
   // View descriptor
   view->desc.Format                  = TranslateTextureFormat[underlyingType(state.format)];
   view->desc.ViewDimension           = TranslateTextureType[underlyingType(_type)];
   view->desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

   // Emulate special Formats by using component swizzling
   // (destination formats are already substituted in the array)
   // TODO: This won't work for Render Targets !!!!
   if ( (state.format == Format::BGRA_8_sn) ||
        (state.format == Format::BGRA_8_u)  ||
        (state.format == Format::BGRA_8_s)  ||
        (state.format == Format::BGRA_10_10_10_2) )
      {
      view->desc.Shader4ComponentMapping = D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2       |
                                          (D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1 << 3) |
                                          (D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0 << 6) |
                                          (D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3 << 9);
      }
   else
   if (state.format == Format::ARGB_1_5_5_5)
      {
      view->desc.Shader4ComponentMapping = D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3       |
                                          (D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2 << 3) |
                                          (D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1 << 6) |
                                          (D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0 << 9);
      }
   else
   if (state.format == Format::ABGR_1_5_5_5)
      {
      view->desc.Shader4ComponentMapping = D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3       |
                                          (D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0 << 3) |
                                          (D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1 << 6) |
                                          (D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2 << 9);
      }

   // Fill out the descriptor depending on type of the view that is created
   if (_type == TextureType::Texture1D)
      {
      view->desc.Texture1D.MostDetailedMip     = (UINT)_mipmaps.base;
      view->desc.Texture1D.MipLevels           = (UINT)_mipmaps.count;
      view->desc.Texture1D.ResourceMinLODClamp = (FLOAT)0.0f;
      }
   else
   if (_type == TextureType::Texture1DArray)
      {
      view->desc.Texture1DArray.MostDetailedMip     = (UINT)_mipmaps.base;
      view->desc.Texture1DArray.MipLevels           = (UINT)_mipmaps.count;
      view->desc.Texture1DArray.FirstArraySlice     = (UINT)_layers.base;
      view->desc.Texture1DArray.ArraySize           = (UINT)_layers.count;
      view->desc.Texture1DArray.ResourceMinLODClamp = (FLOAT)0.0f;
      }
   else
   if (_type == TextureType::Texture2D)
      {
      view->desc.Texture2D.MostDetailedMip     = (UINT)_mipmaps.base;
      view->desc.Texture2D.MipLevels           = (UINT)_mipmaps.count;
      view->desc.Texture2D.PlaneSlice          = (UINT)_layers.base;   // TODO: What is this ????
      view->desc.Texture2D.ResourceMinLODClamp = (FLOAT)0.0f;
      }
   else
   if (_type == TextureType::Texture2DArray)
      {
      view->desc.Texture2DArray.MostDetailedMip     = (UINT)_mipmaps.base;
      view->desc.Texture2DArray.MipLevels           = (UINT)_mipmaps.count;
      view->desc.Texture2DArray.FirstArraySlice     = (UINT)_layers.base;
      view->desc.Texture2DArray.ArraySize           = (UINT)_layers.count;
      view->desc.Texture2DArray.PlaneSlice          = (UINT)0u;        // TODO: What is this ????
      view->desc.Texture2DArray.ResourceMinLODClamp = (FLOAT)0.0f;
      }
   // _type == TextureType::Texture2DMultisample - Nothing else need to be specified?
   else
   if (_type == TextureType::Texture2DMultisampleArray)
      {
      view->desc.Texture2DMSArray.FirstArraySlice = (UINT)_layers.base;
      view->desc.Texture2DMSArray.ArraySize       = (UINT)_layers.count;
      }
   else
   if (_type == TextureType::Texture3D)
      {
      view->desc.Texture3D.MostDetailedMip        = (UINT)_mipmaps.base;
      view->desc.Texture3D.MipLevels              = (UINT)_mipmaps.count;
      view->desc.Texture3D.ResourceMinLODClamp    = (FLOAT)0.0f;
      }
   else
   if (_type == TextureType::TextureCubeMap)
      {
      view->desc.TextureCube.MostDetailedMip      = (UINT)_mipmaps.base;
      view->desc.TextureCube.MipLevels            = (UINT)_mipmaps.count;
      view->desc.TextureCube.ResourceMinLODClamp  = (FLOAT)0.0f;
      }
   else
   if (_type == TextureType::TextureCubeMapArray)
      {
      view->desc.TextureCubeArray.MostDetailedMip     = (UINT)_mipmaps.base;
      view->desc.TextureCubeArray.MipLevels           = (UINT)_mipmaps.count;
      view->desc.TextureCubeArray.First2DArrayFace    = (UINT)_layers.base;
      view->desc.TextureCubeArray.NumCubes            = (UINT)_layers.count / 6u;
      view->desc.TextureCubeArray.ResourceMinLODClamp = (FLOAT)0.0f;
      }

   // TODO: ResourceMinLODClamp is currently not supported by the engine.
   //       It's useful when streaming in resources, so we can start from smallest mip-map,
   //       and recreate view with bumping up this value every mip upload!
   //       Hmmm, but then, why not just use MostDetailedMip for that ???
   
   return view;
   }
   
   D3D12_RESOURCE_DESC createTextureDescriptor(const TextureState& state)
   {
   // Texture descriptor
   D3D12_RESOURCE_DESC desc;
   desc.Dimension          = TranslateTextureDimension[underlyingType(state.type)];
   desc.Alignment          = 0u; // We will know alignment after querying it
   desc.Width              = static_cast<UINT64>(state.width);
   desc.Height             = static_cast<UINT>(state.height);
   desc.DepthOrArraySize   = static_cast<UINT16>(state.layers);
   desc.MipLevels          = static_cast<UINT16>(state.mipmaps);
   desc.Format             = TranslateTextureFormat[underlyingType(state.format)];
   desc.SampleDesc.Count   = static_cast<UINT>(state.samples);
   desc.SampleDesc.Quality = state.samples > 1u ? D3D12_STANDARD_MULTISAMPLE_PATTERN : (UINT)0u;
   desc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
   desc.Flags              = D3D12_RESOURCE_FLAG_NONE;

   // Used for shared resources, as linear storage:
   // - D3D12_TEXTURE_LAYOUT_ROW_MAJOR
   //
   // Used for Sparse Textures:
   // - D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE
   // - D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE

   if (state.usage == TextureUsage::RenderTargetRead ||
       state.usage == TextureUsage::RenderTargetWrite)
      {
      desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
      }

   if (isDepthStencil(state.format) ||
       isDepth(state.format)        ||
       isStencil(state.format))
      {
      desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
      }

   // TODO: GPU possible internal optimizations:
   //
   // Use below when creating "Images"
   // - D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
   //
   // Use to allow additional compression of read only DS resources
   // - D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE
   //
   // Support for sharing resource between several GPU's, processes and queues.
   // - D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER
   // - D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS
   
   // Acquire resource alignment and size
   D3D12_RESOURCE_ALLOCATION_INFO allocInfo;
   allocInfo = gpu->device->GetResourceAllocationInfo(0u /* currently no multi-GPU support */, 1u, &desc);

   // Patch descriptor with missing alignment
   desc.Alignment = allocInfo.Alignment;
   
   return desc;
   }
   
   shared_ptr<Texture> HeapD3D12::createTexture(const TextureState state)
   {
   shared_ptr<TextureD3D12> result = nullptr;
   
   // Do not create textures on Heaps designated for Streaming.
   // (Engine currently is not supporting Linear Textures).
   assert( _usage == MemoryUsage::Tiled ||
           _usage == MemoryUsage::Renderable );

   D3D12_RESOURCE_DESC desc = createTextureDescriptor(state);

   bool enableOptimizedClear = false;
   if (state.usage == TextureUsage::RenderTargetRead ||
       state.usage == TextureUsage::RenderTargetWrite)
      {
      enableOptimizedClear = true;
      }

   if (isDepthStencil(state.format) ||
       isDepth(state.format)        ||
       isStencil(state.format))
      {
      enableOptimizedClear = true;
      }
  
   // Acquire resource alignment and size
   D3D12_RESOURCE_ALLOCATION_INFO allocInfo;
   allocInfo = gpu->device->GetResourceAllocationInfo(0u /* currently no multi-GPU support */, 1u, &desc);

   // Find memory region in the Heap where this texture can be placed.
   // If allocation succeeded, texture is mapped to given offset.
   uint64 offset = 0;
   if (!allocator->allocate(static_cast<uint64>(allocInfo.SizeInBytes),
                            static_cast<uint64>(allocInfo.Alignment),
                            offset))
      {
      return result;
      }

   // Optimized clear value (completly not matching other abstractions)
   D3D12_CLEAR_VALUE clearValue;
   if (isDepthStencil(state.format) ||
       isDepth(state.format)        ||
       isStencil(state.format))
      {
      clearValue.Format   = desc.Format;
      clearValue.DepthStencil.Depth   = 1.0f;
      clearValue.DepthStencil.Stencil = 0;
      }
   else
      {
      clearValue.Format   = desc.Format;
      clearValue.Color[0] = 0.0f;
      clearValue.Color[1] = 0.0f;
      clearValue.Color[2] = 0.0f;
      clearValue.Color[3] = 1.0f;
      }

   // Resource is in generic state at creation time
   // (it needs to be explicitly transferred with initial barrier).
   D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON;
   
   // TODO: When creating Images
   // - D3D12_RESOURCE_STATE_UNORDERED_ACCESS
   
	ID3D12Resource* textureHandle = nullptr;
	
	Validate( gpu, CreatePlacedResource(handle,
                                      static_cast<UINT64>(offset),
                                      &desc,
                                      initState,
                                      enableOptimizedClear ? &clearValue : nullptr, // Clear value - currently not supported
                                      IID_PPV_ARGS(&textureHandle)) ) // __uuidof(ID3D12Resource), reinterpret_cast<void**>(&textureHandle)
      
   if ( SUCCEEDED(gpu->lastResult[Scheduler.core()]) )
      result = make_shared<TextureD3D12>(dynamic_pointer_cast<HeapD3D12>(shared_from_this()),
                                         textureHandle,
                                         offset,
                                         static_cast<uint64>(allocInfo.SizeInBytes),
                                         state);

   return result;
   }

   TextureViewD3D12::TextureViewD3D12(shared_ptr<TextureD3D12> parent,
         const TextureType _type,
         const Format _format,
         const uint32v2 _mipmaps,
         const uint32v2 _layers) :
      texture(parent),
      CommonTextureView(_type, _format, _mipmaps, _layers)
   {
   // desc needs to be set after construction
   }

   TextureViewD3D12::~TextureViewD3D12()
   {
   texture = nullptr;
   }

   shared_ptr<Texture> TextureViewD3D12::parent(void) const
   {
   return texture;
   }
   
 
   // DEVICE
   //////////////////////////////////////////////////////////////////////////


   ImageMemoryAlignment Direct3D12Device::textureMemoryAlignment(const TextureState& state,
                                                                 const uint32 mipmap,
                                                                 const uint32 layer) const
   {
   assert( state.mipmaps > mipmap );
   assert( state.layers > layer );
   assert( powerOfTwo(state.samples) );
   
   D3D12_RESOURCE_DESC desc = createTextureDescriptor(state);

   // Based on amount of mip-maps and layers, calculates
   // index of subresource to modify.
   UINT subresource = D3D12CalcSubresource(mipmap,
                                           layer,
                                           0u,
                                           state.mipmaps,
                                           state.layers);
   
   D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
   UINT                               linesCount = 0;
   UINT64                             lineSize = 0;
   UINT64                             requiredUploadBufferSize = 0;

   ValidateNoRet( this, GetCopyableFootprints(&desc,
                                              subresource, // First subresource to modify
                                              1,           // Subresources count
                                              0,
                                              &layout,
                                              &linesCount, // Count of lines in given subresource
                                              &lineSize,   // Size of line in bytes
                                              &requiredUploadBufferSize) )
 
   // Address alignment is always power of two.
   // Thus only that power can be stored to save memory space.
   uint32 power = 0;
      
   ImageMemoryAlignment result;
   result.sampleSize            = texelBlockSize
   
   whichPowerOfTwo(state.samples, power);
   result.samplesCountPower     = power;
   result.sampleAlignmentPower  = 0; // Tightly packed sample after sample
   result.texelAlignmentPower   = 0; // Tightly packed texel after texel (block after block)
    
   // layout.Footprint.RowPitch is multiple of D3D12_TEXTURE_DATA_PITCH_ALIGNMENT (256)
   whichPowerOfTwo(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT, power);
   result.rowAlignmentPower     = power;
   result.surfaceAlignmentPower = power;

   return result;
   }

   }
}
#endif
