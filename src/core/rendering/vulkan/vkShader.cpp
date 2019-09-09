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

#include "core/rendering/vulkan/vkShader.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/vulkan/vkValidate.h"
#include "core/rendering/vulkan/vkDevice.h"
#include "utilities/utilities.h" // For underlyingType()

namespace en
{
   namespace gpu
   {
   ShaderVK::ShaderVK(VulkanDevice* _gpu, const VkShaderModule _handle, const ShaderStage _stage) :
      gpu(_gpu),
      handle(_handle),
      stage(_stage)
   {
   }

   ShaderVK::~ShaderVK()
   {
   ValidateNoRet( gpu, vkDestroyShaderModule(gpu->device, handle, nullptr) )
   }

   std::shared_ptr<Shader> VulkanDevice::createShader(const ShaderStage stage, const std::string& source)
   {
   // VK_NV_glsl_shader - allows passing in GLSL instead of SPIR-V
   //                     (we can compile GLSL to SPIRV offline)

   return createShader(stage,(const uint8*)source.c_str(), source.size());
   }

   std::shared_ptr<Shader> VulkanDevice::createShader(const ShaderStage stage, const uint8* data, const uint64 size)
   {
   std::shared_ptr<ShaderVK> shader = nullptr;

   VkShaderModuleCreateInfo createInfo;
   createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
   createInfo.pNext    = nullptr;
   createInfo.flags    = 0u; // Reserved for future use
   createInfo.codeSize = size;
   createInfo.pCode    = (const uint32_t*)data;

   VkShaderModule handle = VK_NULL_HANDLE;
   Validate( this, vkCreateShaderModule(device, &createInfo, nullptr, &handle) )
   if (lastResult[currentThreadId()] == VK_SUCCESS)
      shader = std::make_shared<ShaderVK>(this, handle, stage);

   // In Debug mode, VK_EXT_debug_report will handle logging of initial compilation errors.

   return shader;
   }

   }
}
#endif
