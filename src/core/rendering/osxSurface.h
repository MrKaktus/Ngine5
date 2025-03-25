/*

 Ngine v5.0
 
 Module      : OSX Shared Surface.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_OSX_SHARED_SURFACE
#define ENG_CORE_RENDERING_OSX_SHARED_SURFACE

#include "core/defines.h"
#include "core/rendering/surface.h"

#if defined(EN_PLATFORM_OSX)

#pragma push_macro("aligned")
#undef aligned
#include "IOSurface/IOSurfaceAPI.h"

// For osxOpenGLCompositor only
#include <OpenGL/gl.h>
#include <AppKit/AppKit.h>
#pragma pop_macro("aligned")

namespace en
{
namespace gpu
{

class SharedSurfaceOSX : public SharedSurface
{
    public:
    IOSurfaceRef surface;
    uint32v2 resolution;
   
    SharedSurfaceOSX(const IOSurfaceRef surface,
                     const uint32v2 resolution);
    virtual ~SharedSurfaceOSX();
};

/*
class osxOpenGLContext : OpenGLContext
{
    public:
    NSOpenGLPixelFormat* pixelFormat;
    NSOpenGLContext*     context;

    osxOpenGLContext();
    virtual ~osxOpenGLContext();

    virtual uint32 createSharedTexture(std::shared_ptr<SharedSurface> backingSurface);
    virtual void destroySharedTexture(const uint32 handle);
};
//*/

} // en::gpu
} // en
#endif

#endif
