/*

 Ngine v5.0
 
 Module      : Metal Sampler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlSampler.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/types.h"
#include "core/rendering/metal/mtlDevice.h"
#include "utilities/Nversion.h"

namespace en
{
   namespace gpu
   {
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME

#if defined(EN_PLATFORM_OSX)
   // Last verified for Metal 2.0 and OSX 10.11
   static const Nversion TextureWrapingSupportedMTL[underlyingType(SamplerAdressing::Count)] =
      {
      Metal_OSX_1_0           ,   // Repeat
      Metal_OSX_1_0           ,   // RepeatMirrored
      Metal_OSX_1_0           ,   // ClampToEdge
      Metal_OSX_1_0           ,   // ClampToBorder
      Metal_OSX_Unsupported       // MirrorClampToEdge
      };
#endif

#if defined(EN_PLATFORM_IOS)
   // Last verified for Metal 2.0 and iOS 9.0
   static const NVersion TextureWrapingSupportedMTL[underlyingType(SamplerAdressing::Count)] = 
      {
      Metal_OSX_1_0           ,   // Repeat
      Metal_OSX_1_0           ,   // RepeatMirrored
      Metal_OSX_1_0           ,   // ClampToEdge
      Metal_OSX_1_0           ,   // ClampToBorder
      Metal_OSX_Unsupported       // MirrorClampToEdge
      };
#endif

#endif

   // SAMPLER


   // Optimisation: This table is not needed. Backend type can be directly cast to Metal type.
   static const MTLSamplerMinMagFilter TranslateSamplerFilter[underlyingType(SamplerFilter::Count)] =
      {
      MTLSamplerMinMagFilterNearest,   // Nearest
      MTLSamplerMinMagFilterLinear     // Linear
      };

   // Optimisation: This table is not needed. Backend type can be directly cast to Metal type by adding 1.
   static const MTLSamplerMipFilter TranslateSamplerMipMapMode[underlyingType(SamplerMipMapMode::Count)] =
      {
    //MTLSamplerMipFilterNotMipmapped
      MTLSamplerMipFilterNearest,      // Nearest
      MTLSamplerMipFilterLinear        // Linear
      };
    
   static const MTLSamplerAddressMode TranslateSamplerAdressing[underlyingType(SamplerAdressing::Count)] =
      {
      MTLSamplerAddressModeRepeat      ,  // Repeat
      MTLSamplerAddressModeMirrorRepeat,  // RepeatMirrored
      MTLSamplerAddressModeClampToEdge ,  // ClampToEdge
      MTLSamplerAddressModeClampToZero ,  // ClampToBorder     (for Metal border is always zero - "OpaqueBlack")
      (MTLSamplerAddressMode)0            // MirrorClampToEdge (unsupported)
      };

   // Optimisation: This table is not needed. Backend type can be directly cast to Metal type.
   static const MTLCompareFunction TranslateCompareOperation[underlyingType(CompareOperation::Count)] =
      {
      MTLCompareFunctionNever        ,   // Never
      MTLCompareFunctionLess         ,   // Less
      MTLCompareFunctionEqual        ,   // Equal
      MTLCompareFunctionLessEqual    ,   // LessOrEqual
      MTLCompareFunctionGreater      ,   // Greater
      MTLCompareFunctionNotEqual     ,   // NotEqual
      MTLCompareFunctionGreaterEqual ,   // GreaterOrEqual
      MTLCompareFunctionAlways       ,   // Always
      };

// TODO: Metal has no border color selection, it's always OpaqueBlack
//
//   static const BorderColor TranslateSamplerBorder[underlyingType(SamplerBorder::Count)] =
//      {
//      VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, // TransparentBlack
//      VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,      // OpaqueBlack
//      VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,      // OpaqueWhite
//      };
//
//   // VK_BORDER_COLOR_INT_TRANSPARENT_BLACK
//   // VK_BORDER_COLOR_INT_OPAQUE_BLACK
//   // VK_BORDER_COLOR_INT_OPAQUE_WHITE

   SamplerMTL::SamplerMTL(const MetalDevice* gpu, const SamplerState& state)
   {
   MTLSamplerDescriptor* samplerInfo = [[MTLSamplerDescriptor alloc] init];
   samplerInfo.magFilter             = static_cast<MTLSamplerMinMagFilter>(underlyingType(state.magnification)); // Optimisation: TranslateSamplerFilter[underlyingType(state.magnification)];
   samplerInfo.minFilter             = static_cast<MTLSamplerMinMagFilter>(underlyingType(state.minification));  // Optimisation: TranslateSamplerFilter[underlyingType(state.minification)];
   samplerInfo.mipFilter             = static_cast<MTLSamplerMipFilter>(underlyingType(state.mipmap) + 1);       // Optimisation: TranslateSamplerMipMapMode[underlyingType(state.mipmap)];
   samplerInfo.rAddressMode          = TranslateSamplerAdressing[underlyingType(state.coordU)];
   samplerInfo.sAddressMode          = TranslateSamplerAdressing[underlyingType(state.coordV)];
   samplerInfo.tAddressMode          = TranslateSamplerAdressing[underlyingType(state.coordW)];
   samplerInfo.maxAnisotropy         = min(state.anisotropy, gpu->support.maxAnisotropy);                    // [1.0f - ??]
   samplerInfo.lodMinClamp           = state.minLod;
   samplerInfo.lodMaxClamp           = state.maxLod;
   samplerInfo.compareFunction       = static_cast<MTLCompareFunction>(underlyingType(state.compare));           // Optimisation: TranslateCompareOperation[underlyingType(state.compare)];  IOS 9.0+ !
   samplerInfo.normalizedCoordinates = TRUE;     // TODO: Unnormalized coordinates are not supported for now (both supported by Vulkan & Metal)

   handle = [gpu->device newSamplerStateWithDescriptor:samplerInfo];       // or getDevice()
   [samplerInfo release];

   // Metal vs OpenGL/Vulkan diff:
   //
   // - mipLodBias
   // - borderColor / Type
   //
   // Metal & Vulkan:
   // + normalizedCoordinates, effectively always TRUE

   assert( handle != nil );
   }

   SamplerMTL::~SamplerMTL()
   {
   [handle release];
   }

   Ptr<Sampler> MetalDevice::createSampler(const SamplerState& state)
   {
   Ptr<SamplerMTL> sampler = new SamplerMTL(this, state);
   return ptr_reinterpret_cast<Sampler>(&sampler);
   };

#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   void InitSamplers(const CommonDevice* gpu)
   {
//   // Init array of currently supported wrapping types
//   for(uint16 i=0; i<underlyingType(SamplerAdressing::Count); ++i)
//      {
//      if (gpu->api.release >= TextureWrapingSupportedMTL[i].release)
//         TextureWrapingSupported[i] = true;
//      }
   }
#endif

   }
}

#endif
