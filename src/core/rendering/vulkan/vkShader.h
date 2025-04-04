/*

 Ngine v5.0
 
 Module      : Vulkan Shader.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_SHADER
#define ENG_CORE_RENDERING_VULKAN_SHADER

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/shader.h"

namespace en
{
namespace gpu
{

class VulkanDevice;

class ShaderVK : public Shader
{
    public:
    VulkanDevice*  gpu;
    VkShaderModule handle;
    ShaderStage    stage;

    ShaderVK(VulkanDevice* gpu, const VkShaderModule handle, const ShaderStage stage);
    virtual ~ShaderVK();
};

} // en::gpu
} // en

#endif
#endif
