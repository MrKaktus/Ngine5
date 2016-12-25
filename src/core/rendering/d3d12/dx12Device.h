/*

 Ngine v5.0
 
 Module      : Direct3D 12 GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_DEVICE
#define ENG_CORE_RENDERING_D3D12_DEVICE

#include "core/rendering/d3d12/dx12.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/common/device.h"

#include "core/rendering/d3d12/dx12InputLayout.h"
#include "core/rendering/d3d12/dx12Blend.h"
#include "core/rendering/d3d12/dx12Raster.h"
#include "core/rendering/d3d12/dx12Viewport.h"
#include "core/rendering/d3d12/dx12DepthStencil.h"

using namespace std;

namespace en
{
   namespace gpu
   {
   class Direct3D12Device : public CommonDevice
      {
      public:

      Direct3D12Device();
     ~Direct3D12Device();

      virtual Ptr<InputLayout> createInputLayout(const DrawableType primitiveType,
                                                    const uint32 controlPoints,
                                                    const uint32 usedAttributes,
                                                    const uint32 usedBuffers,
                                                    const AttributeDesc* attributes,
                                                    const BufferDesc* buffers);

      virtual void init(void);

      virtual Ptr<SetLayout> createSetLayout(const uint32 count, 
                                             const ResourceGroup* group,
                                             const ShaderStage stageMask);

      virtual Ptr<PipelineLayout> createPipelineLayout(const uint32 sets,
                                                       const Ptr<SetLayout>* set,
                                                       const uint32 immutableSamplers,
                                                       const Ptr<Sampler>* sampler);
      };
   }
}
#endif

#endif
