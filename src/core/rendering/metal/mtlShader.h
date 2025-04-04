/*

 Ngine v5.0
 
 Module      : Metal Shader.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_SHADER
#define ENG_CORE_RENDERING_METAL_SHADER

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/shader.h"

namespace en
{
namespace gpu
{

class ShaderMTL : public Shader
{
    public:
    // TODO: In future separate library objects for reuse and batching of functions
    id <MTLLibrary> library;

    ShaderMTL(id <MTLLibrary> library);
    virtual ~ShaderMTL();
};

} // en::gpu
} // en

#endif
#endif
