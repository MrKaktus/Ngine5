/*

 Ngine v5.0
 
 Module      : Metal Input Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_INPUT_LAYOUT
#define ENG_CORE_RENDERING_METAL_INPUT_LAYOUT

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/state.h"
#include "core/rendering/common/inputLayout.h"
#include "core/rendering/metal/metal.h"

namespace en
{
namespace gpu
{

class InputLayoutMTL : public InputLayout
{
    public:
    MTLVertexDescriptor* desc;
    DrawableType primitive;

    InputLayoutMTL(const DrawableType primitiveType,
                   const bool primitiveRestart,
                   const uint32 controlPoints,
                   const uint32 usedAttributes,
                   const uint32 usedBuffers,
                   const AttributeDesc* attributes,
                   const BufferDesc* buffers);

    virtual ~InputLayoutMTL();
};

} // en::gpu
} // en
#endif

#endif
