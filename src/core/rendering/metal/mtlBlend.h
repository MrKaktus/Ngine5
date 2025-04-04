/*

 Ngine v5.0
 
 Module      : Metal Blend State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_BLEND_STATE
#define ENG_CORE_RENDERING_METAL_BLEND_STATE

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/blend.h"

namespace en
{
namespace gpu
{

class BlendStateMTL : public BlendState
{
    public:
    float4 blendColor;
    uint32 attachments;
    MTLRenderPipelineColorAttachmentDescriptorArray* blendInfo;

    BlendStateMTL(const BlendStateInfo& state,
                  const uint32 attachments, 
                  const BlendAttachmentInfo* color);
    virtual ~BlendStateMTL();
};

} // en::gpu
} // en

#endif
#endif
