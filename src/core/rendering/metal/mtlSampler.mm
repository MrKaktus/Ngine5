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

// Optimisation: This table is not needed. Frontend type can be directly cast to Metal type.
static const MTLSamplerMinMagFilter TranslateSamplerFilter[underlyingType(SamplerFilter::Count)] =
{
    MTLSamplerMinMagFilterNearest,   // Nearest
    MTLSamplerMinMagFilterLinear     // Linear
};

// Optimisation: This table is not needed. Frontend type can be directly cast to Metal type by adding 1.
static const MTLSamplerMipFilter TranslateSamplerMipMapMode[underlyingType(SamplerMipMapMode::Count)] =
{
  //MTLSamplerMipFilterNotMipmapped
    MTLSamplerMipFilterNearest,      // Nearest
    MTLSamplerMipFilterLinear        // Linear
};
    
static const MTLSamplerAddressMode TranslateSamplerAdressing[underlyingType(SamplerAdressing::Count)] =
{
    MTLSamplerAddressModeRepeat             ,  // Repeat
    MTLSamplerAddressModeMirrorRepeat       ,  // RepeatMirrored
    MTLSamplerAddressModeClampToEdge        ,  // ClampToEdge
  //MTLSamplerAddressModeClampToZero        ,  // ClampToBorder     (with alpha - "TransparentBlack", without - "OpaqueBlack")
    MTLSamplerAddressModeClampToBorderColor ,  // ClampToBorder     (10.12+)
    MTLSamplerAddressModeMirrorClampToEdge  ,  // MirrorClampToEdge (10.11+)
};

// Optimisation: This table is not needed. Frontend type can be directly cast to Metal type.
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

// Optimisation: This table is not needed. Frontend type can be directly cast to Metal type.
static const MTLSamplerBorderColor TranslateSamplerBorder[underlyingType(SamplerBorder::Count)] =
{
    MTLSamplerBorderColorTransparentBlack ,   // TransparentBlack
    MTLSamplerBorderColorOpaqueBlack      ,   // OpaqueBlack
    MTLSamplerBorderColorOpaqueWhite      ,   // OpaqueWhite
};

SamplerMTL::SamplerMTL(const MetalDevice* gpu, const SamplerState& state)
{
    MTLSamplerDescriptor* samplerInfo  = allocateObjectiveC(MTLSamplerDescriptor);
    samplerInfo.magFilter              = static_cast<MTLSamplerMinMagFilter>(underlyingType(state.magnification)); // Optimisation: TranslateSamplerFilter[underlyingType(state.magnification)];
    samplerInfo.minFilter              = static_cast<MTLSamplerMinMagFilter>(underlyingType(state.minification));  // Optimisation: TranslateSamplerFilter[underlyingType(state.minification)];
    samplerInfo.mipFilter              = static_cast<MTLSamplerMipFilter>(underlyingType(state.mipmap) + 1);       // Optimisation: TranslateSamplerMipMapMode[underlyingType(state.mipmap)];
    samplerInfo.sAddressMode           = TranslateSamplerAdressing[underlyingType(state.coordU)];
    samplerInfo.tAddressMode           = TranslateSamplerAdressing[underlyingType(state.coordV)];
    samplerInfo.rAddressMode           = TranslateSamplerAdressing[underlyingType(state.coordW)];
    samplerInfo.borderColor            = static_cast<MTLSamplerBorderColor>(underlyingType(state.borderColor));    // Optimisation: TranslateSamplerBorder[underlyingType(state.borderColor)]; macOS 10.12+ !
    samplerInfo.maxAnisotropy          = min(state.anisotropy, gpu->support.maxAnisotropy);                    // [1.0f - ??]
    samplerInfo.lodMinClamp            = state.minLod;
    samplerInfo.lodMaxClamp            = state.maxLod;
    samplerInfo.compareFunction        = static_cast<MTLCompareFunction>(underlyingType(state.compare));           // Optimisation: TranslateCompareOperation[underlyingType(state.compare)];  iOS 9.0+ !
    samplerInfo.normalizedCoordinates  = TRUE;     // TODO: Unnormalized coordinates are not supported for now (both supported by Vulkan & Metal)
    samplerInfo.supportArgumentBuffers = TRUE; // Required for Descriptor Tables - macOS 10.13+
   
    handle = [gpu->device newSamplerStateWithDescriptor:samplerInfo];       // or getDevice()
   
    deallocateObjectiveC(samplerInfo);

    // Metal vs OpenGL/Vulkan diff:
    //
    // - mipLodBias   <------ It's not a bug but a feature. Like in ES2, its programmable set on sample call in shader
    //                        Section 5.10.3 in MetalSL document.
    //
    // Metal & Vulkan:
    // + normalizedCoordinates, effectively always TRUE

    assert( handle != nil );
}

SamplerMTL::~SamplerMTL()
{
    deallocateObjectiveC(handle);
}

Sampler* MetalDevice::createSampler(const SamplerState& state)
{
    return new SamplerMTL(this, state);
};

} // en::gpu
} // en

#endif
