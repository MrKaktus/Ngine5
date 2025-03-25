/*

 Ngine v5.0
 
 Module      : Vulkan Input Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_INPUT_LAYOUT
#define ENG_CORE_RENDERING_VULKAN_INPUT_LAYOUT

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/common/inputLayout.h"
#include "core/rendering/state.h"

namespace en
{
namespace gpu
{

class InputLayoutVK : public InputLayout
{
    public:
    VkPipelineVertexInputStateCreateInfo   state;
    VkPipelineInputAssemblyStateCreateInfo statePrimitive;
    VkPipelineTessellationStateCreateInfo  stateTessellator;

    InputLayoutVK(const DrawableType primitiveType,
                  const bool primitiveRestart,
                  const uint32 controlPoints, 
                  const uint32 usedAttributes, 
                  const uint32 usedBuffers, 
                  const AttributeDesc* attributes,  
                  const BufferDesc* buffers);

    virtual ~InputLayoutVK() {};
};

} // en::gpu
} // en
#endif

#endif
