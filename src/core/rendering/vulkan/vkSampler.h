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

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/sampler.h"

namespace en
{
namespace gpu
{

class VulkanDevice;

class SamplerVK : public Sampler
{
    public:
    VulkanDevice* gpu;
    VkSampler     handle;  // Vulkan Sampler ID

    SamplerVK(VulkanDevice* gpu, VkSampler handle);
   ~SamplerVK();
};

} // en::gpu
} // en
#endif
