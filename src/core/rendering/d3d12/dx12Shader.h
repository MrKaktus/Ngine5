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

#ifndef ENG_CORE_RENDERING_D3D12_SHADER
#define ENG_CORE_RENDERING_D3D12_SHADER

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/shader.h"

namespace en
{
   namespace gpu
   {
   class Direct3D12Device;

   class ShaderD3D12 : public Shader
      {
      public:
      Direct3D12Device*  gpu;
      D3D12_SHADER_BYTECODE state;
      ShaderStage    stage;

      //ShaderD3D12(VulkanDevice* gpu, const VkShaderModule handle, const ShaderStage stage);
      virtual ~ShaderD3D12();
      };

   }
}
#endif

#endif
