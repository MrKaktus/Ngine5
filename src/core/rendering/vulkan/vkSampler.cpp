/*

 Ngine v5.0
 
 Module      : Vulkan Sampler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkSampler.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/vulkan/vkValidate.h"
#include "core/rendering/vulkan/vkDevice.h"

namespace en
{
   namespace gpu
   {
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

   SamplerVK::SamplerVK(VulkanDevice* _gpu, VkSampler _handle) :
      gpu(_gpu),
      handle(_handle)
   {
   }
  
   SamplerVK::~SamplerVK()
   {
   ValidateNoRet( gpu, vkDestroySampler(gpu->device, handle, nullptr) )
   }

   std::shared_ptr<Sampler> VulkanDevice::createSampler(const SamplerState& state)
   {
   std::shared_ptr<SamplerVK> sampler = nullptr;
   
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
   samplerInfo.anisotropyEnable        = (state.anisotropy <= 1.0f) ? VK_FALSE : VK_TRUE;
   samplerInfo.maxAnisotropy           = min(state.anisotropy, support.maxAnisotropy);                    // [1.0f - VkPhysicalDeviceLimits::maxSamplerAnisotropy]
   samplerInfo.compareEnable           = state.compare == CompareOperation::Always ? VK_FALSE :  VK_TRUE;
   samplerInfo.compareOp               = static_cast<VkCompareOp>(underlyingType(state.compare));         // Optimisation: TranslateCompareOperation[underlyingType(state.compare)];
   samplerInfo.minLod                  = state.minLod;
   samplerInfo.maxLod                  = state.maxLod;
   samplerInfo.borderColor             = TranslateSamplerBorder[underlyingType(state.borderColor)];
   samplerInfo.unnormalizedCoordinates = VK_FALSE;  // TODO: Unnormalized coordinates are not supported for now. (both supported by Vulkan & Metal)

   VkSampler handle = VK_NULL_HANDLE;
   Validate( this, vkCreateSampler(device, &samplerInfo, nullptr, &handle) )
   if (lastResult[currentThreadId()] == VK_SUCCESS)
      sampler = std::make_shared<SamplerVK>(this, handle);

   return sampler;
   };

   }
}
#endif
