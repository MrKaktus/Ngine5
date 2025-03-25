/*

 Ngine v5.0
 
 Module      : Vulkan Depth-Stencil State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_DEPTH_STENCIL_STATE
#define ENG_CORE_RENDERING_VULKAN_DEPTH_STENCIL_STATE

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/depthStencil.h"

namespace en
{
namespace gpu
{

class DepthStencilStateVK : public DepthStencilState
{
    public:
    VkPipelineDepthStencilStateCreateInfo state;

    DepthStencilStateVK(const DepthStencilStateInfo& desc);
};

} // en::gpu
} // en
#endif

#endif
