/*

 Ngine v5.0
 
 Module      : Surface.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.
               
               Cross-process / cross-API surface.

*/

#ifndef ENG_CORE_RENDERING_SURFACE
#define ENG_CORE_RENDERING_SURFACE

#include <memory>

#include "core/types.h"

namespace en
{
namespace gpu
{
/// Surface that can be shared between processes and API's
/// (by using it as backing surface for API specific Textures).
class SharedSurface
{
    public:
    virtual ~SharedSurface() {};
};

/// Deprecated:
/// Dummy OpenGL Context. Should be used only to create OpenGL handle,
/// of SharedSurface, for passing to process that accepts only OpenGL
/// handles. (for e.g. Valve VRCompositor on macOS).
class OpenGLContext
{
    public:
    virtual uint32 createSharedTexture(std::shared_ptr<SharedSurface> backingSurface) = 0;
    virtual void destroySharedTexture(const uint32 handle) = 0;
    
    virtual ~OpenGLContext() {};
};
      
OpenGLContext* createDummyOpenGLContext(void);

} // en::gpu
} // en

#endif
