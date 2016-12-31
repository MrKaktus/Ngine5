/*

 Ngine v5.0
 
 Module      : D3D12 Shader.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12Shader.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/state.h"
#include "core/rendering/d3d12/dx12Device.h"

namespace en
{
   namespace gpu
   {
   // See: https://msdn.microsoft.com/en-us/library/windows/desktop/bb509710(v=vs.85).aspx
   
   //ShaderD3D12::ShaderD3D12(VulkanDevice* gpu, const VkShaderModule handle, const ShaderStage stage)
   //{
   // TODO: Finish!
   //}
   
   ShaderD3D12::~ShaderD3D12()
   {
   // TODO: Finish!
   }
      
   Ptr<Shader> Direct3D12Device::createShader(const ShaderStage stage, const string& source)
   {
   // TODO: Finish!
   return Ptr<Shader>(nullptr);
   }

   }
}
#endif
