/*

 Ngine v5.0
 
 Module      : Metal Internal functions.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_INTERNAL
#define ENG_CORE_RENDERING_METAL_INTERNAL

#include "core/defines.h"

// Due to way Objective-C interacts with C/C++ code,
// all code that in any way touches Objective-C needs
// to be encapsulated into separately compiled mm files.
// This means that any internal functionality cannot
// be simply shared with other modules by including
// iOS/OSX internal headers, but special "sanitized"
// headers need to be used to keep C/C++ clean from
// Objective-C abstraction.
#if defined(EN_MODULE_RENDERER_METAL)

#include "core/types.h"
#include "core/rendering/surface.h"

namespace en
{
namespace gpu
{

extern std::shared_ptr<SharedSurface> createSharedSurface(const uint32v2 resolution);

} // en::gpu
} // en
#endif

#endif
