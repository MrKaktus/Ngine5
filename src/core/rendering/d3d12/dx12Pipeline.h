/*

 Ngine v5.0
 
 Module      : D3D12 Pipeline.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_PIPELINE
#define ENG_CORE_RENDERING_D3D12_PIPELINE

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/pipeline.h"

namespace en
{
   namespace gpu
   {
   class Direct3D12Device;

   class PipelineD3D12 : public Pipeline
      {
      public:
      Direct3D12Device* gpu;
      VkPipeline    handle;

      PipelineD3D12(VulkanDevice* gpu, VkPipeline handle);
      virtual ~PipelineD3D12();
      };

   }
}
#endif

#endif
