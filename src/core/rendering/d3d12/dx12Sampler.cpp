/*

 Ngine v5.0
 
 Module      : D3D12 Sampler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12Sampler.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12Device.h"

namespace en
{
   namespace gpu
   {
   // D3D12_FILTER_MIN_MAG_MIP_POINT                           = 0,
   // D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR                    = 0x1,
   // D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT              = 0x4,
   // D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR                    = 0x5,
   // D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT                    = 0x10,
   // D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR             = 0x11,
   // D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT                    = 0x14,
   // D3D12_FILTER_MIN_MAG_MIP_LINEAR                          = 0x15,
    
   // D3D12_FILTER_ANISOTROPIC                                 = 0x55,
    
   // D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT                = 0x80,
   // D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR         = 0x81,
   // D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT   = 0x84,
   // D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR         = 0x85,
   // D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT         = 0x90,
   // D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR  = 0x91,
   // D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT         = 0x94,
   // D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR               = 0x95,
   // D3D12_FILTER_COMPARISON_ANISOTROPIC                      = 0xd5,
    
   // Features that need to be queried :
    
   // Returns minimum from the fetched texels:
   //
   // D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT                   = 0x100,
   // D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR            = 0x101,
   // D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT      = 0x104,
   // D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR            = 0x105,
   // D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT            = 0x110,
   // D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR     = 0x111,
   // D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT            = 0x114,
   // D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR                  = 0x115,
   // D3D12_FILTER_MINIMUM_ANISOTROPIC                         = 0x155,
   //
   // Returns maximum from the fetched texels:
   //
   // D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT                   = 0x180,
   // D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR            = 0x181,
   // D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT      = 0x184,
   // D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR            = 0x185,
   // D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT            = 0x190,
   // D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR     = 0x191,
   // D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT            = 0x194,
   // D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR                  = 0x195,
   // D3D12_FILTER_MAXIMUM_ANISOTROPIC                         = 0x1d5

   // Optimisation: This table is not needed. Backend type can be directly cast to D3D12 type by adding 1.
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770441(v=vs.85).aspx#D3D12_TEXTURE_ADDRESS_MODE_BORDER
   static const D3D12_TEXTURE_ADDRESS_MODE TranslateSamplerAdressing[underlyingType(SamplerAdressing::Count)] =
      {
      D3D12_TEXTURE_ADDRESS_MODE_WRAP,        // Repeat
      D3D12_TEXTURE_ADDRESS_MODE_MIRROR,      // RepeatMirrored
      D3D12_TEXTURE_ADDRESS_MODE_CLAMP,       // ClampToEdge
      D3D12_TEXTURE_ADDRESS_MODE_BORDER,      // ClampToBorder
      D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE  // MirrorClampToEdge
      };

   // Optimisation: This table is not needed. Backend type can be directly cast to D3D12 type by adding  1.
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770349(v=vs.85).aspx
   static const D3D12_COMPARISON_FUNC TranslateCompareOperation[underlyingType(CompareOperation::Count)] =
      {
      D3D12_COMPARISON_FUNC_NEVER,              // Never
      D3D12_COMPARISON_FUNC_LESS,               // Less
      D3D12_COMPARISON_FUNC_EQUAL,              // Equal
      D3D12_COMPARISON_FUNC_LESS_EQUAL,         // LessOrEqual
      D3D12_COMPARISON_FUNC_GREATER,            // Greater
      D3D12_COMPARISON_FUNC_NOT_EQUAL,          // NotEqual
      D3D12_COMPARISON_FUNC_GREATER_EQUAL,      // GreaterOrEqual
      D3D12_COMPARISON_FUNC_ALWAYS              // Always
      };

   static const D3D12_STATIC_BORDER_COLOR TranslateStaticSamplerBorder[underlyingType(SamplerBorder::Count)] =
      {
      D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK, // TransparentBlack
      D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,      // OpaqueBlack
      D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE       // OpaqueWhite
      };
  
   static const float4 TranslateSamplerBorder[underlyingType(SamplerBorder::Count)] =
      {
      float4(0.0f, 0.0f, 0.0f, 0.0f), // TransparentBlack
      float4(0.0f, 0.0f, 0.0f, 1.0f), // OpaqueBlack
      float4(1.0f, 1.0f, 1.0f, 1.0f)  // OpaqueWhite
      };

   SamplerD3D12::SamplerD3D12(D3D12_SAMPLER_DESC _state, D3D12_STATIC_BORDER_COLOR _border) :
      state(_state),
      border(_border)
   {
   }
  
   SamplerD3D12::~SamplerD3D12()
   {
   }

   std::shared_ptr<Sampler> Direct3D12Device::createSampler(const SamplerState& state)
   {
   // Sampler Filtering modes:
   //
   // bits 1-0 - MipMap          Value: 0-Neares   1-Linear
   // bits 3-2 - Magnification   Value: 0-Neares   1-Linear
   // bits 5-4 - Minification    Value: 0-Neares   1-Linear
   // bit    6 - Anisotropic     Value: 0-Disabled 1-Enabled
   // bit    7 - Comparison      Value: 00-Disabled      01-Comparison Enabled
   // bit    8 - Minimum Value          10-Minimum Value 11-Maximum Value
   //
   uint8 filterMask = underlyingType(state.mipmap) |
                      (underlyingType(state.magnification) << 2) |
                      (underlyingType(state.minification)  << 4);
      
   if (state.compare != CompareOperation::Always)
      filterMask |= 0x40;
      
   // TODO: Minimum and Maximum Values are currently not supported.
   //       Unnormalized coordinates are not supported.
   
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dn770403(v=vs.85).aspx
   //
   D3D12_SAMPLER_DESC samplerInfo;
   samplerInfo.Filter         = static_cast<D3D12_FILTER>(filterMask);
   samplerInfo.AddressU       = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(underlyingType(state.coordU) + 1);  // Optimisation: TranslateSamplerAdressing[underlyingType(state.coordU)];
   samplerInfo.AddressV       = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(underlyingType(state.coordV) + 1);  // Optimisation: TranslateSamplerAdressing[underlyingType(state.coordV)];
   samplerInfo.AddressW       = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(underlyingType(state.coordW) + 1);  // Optimisation: TranslateSamplerAdressing[underlyingType(state.coordW)];
   samplerInfo.MipLODBias     = state.LodBias;
   samplerInfo.MaxAnisotropy  = static_cast<UINT>(max(1.0f, min(state.anisotropy, support.maxAnisotropy))); // 1 .. 16
   samplerInfo.ComparisonFunc = static_cast<D3D12_COMPARISON_FUNC>(underlyingType(state.compare) + 1);      // Optimisation: TranslateCompareOperation[underlyingType(state.compare)];
   samplerInfo.BorderColor[0] = TranslateSamplerBorder[underlyingType(state.borderColor)].r;
   samplerInfo.BorderColor[1] = TranslateSamplerBorder[underlyingType(state.borderColor)].g;
   samplerInfo.BorderColor[2] = TranslateSamplerBorder[underlyingType(state.borderColor)].b;
   samplerInfo.BorderColor[3] = TranslateSamplerBorder[underlyingType(state.borderColor)].a;
   samplerInfo.MinLOD         = state.minLod;
   samplerInfo.MaxLOD         = state.maxLod;

   // In Direct3D12 Samplers are created directly on Descriptor Heaps.
   // Therefore sampler descriptor is cached, until it will be bound
   // to Descriptor through Sampler creation call.
   return std::make_shared<SamplerD3D12>(samplerInfo, TranslateStaticSamplerBorder[underlyingType(state.borderColor)]);
   };

//   D3D12_CPU_DESCRIPTOR_HANDLE handle = m_samplerHeap->GetCPUDescriptorHandleForHeapStart();
//   Validate( gpu, CreateSampler(&samplerInfo, handle) )
//   
//   if (lastResult[currentThreadId()] == VK_SUCCESS)
//      sampler = new SamplerVK(this, handle);

   }
}
#endif
