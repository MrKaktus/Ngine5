/*

 Ngine v5.0
 
 Module      : Shader.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkShader.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/defines.h"
#include "core/utilities/TintrusivePointer.h"
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
   ProfileNoRet( gpu, vkDestroyShaderModule(gpu->device, handle, nullptr) )
   }

   Ptr<Shader> VulkanDevice::createShader(const ShaderStage stage, const string& source)
   {
   // VK_NV_glsl_shader - allows passing in GLSL instead of SPIR-V

   Ptr<ShaderVK> shader = nullptr;

   VkShaderModuleCreateInfo createInfo;
   createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
   createInfo.pNext    = nullptr;
   createInfo.flags    = 0u; // Reserved for future use
   createInfo.codeSize = source.size();
   createInfo.pCode    = (const uint32_t*)source.c_str();

   VkShaderModule handle = VK_NULL_HANDLE;
   Profile( this, vkCreateShaderModule(device, &createInfo, nullptr, &handle) )
   if (lastResult[Scheduler.core()] == VK_SUCCESS)
      shader = new ShaderVK(this, handle, stage);


   // TODO: Verify initial compilation with VK_EXT_debug_report

   return ptr_reinterpret_cast<Shader>(&shader);
   }

   }
}
#endif