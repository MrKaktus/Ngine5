/*

 Ngine v5.0
 
 Module      : OpenGL GPU Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_OPENGL_DEVICE
#define ENG_CORE_RENDERING_OPENGL_DEVICE

#include "core/defines.h"

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/opengl/opengl.h"
#include "core/rendering/common/device.h"

#include "core/rendering/inputAssembler.h"
#include "core/rendering/blend.h"
#include "core/rendering/raster.h"
#include "core/rendering/multisampling.h"
#include "core/rendering/viewport.h"
#include "core/rendering/depthStencil.h"
#include "core/rendering/pipeline.h"
#include "core/rendering/renderPass.h"
#include "core/rendering/texture.h"

// OpenGL calls can be performed only inside OpenGL Device class.
// "lastResult" is OpenGL Device variable.
#ifdef EN_DEBUG
namespace en
{
   namespace gpu
   {
   extern bool IsError(const char* line);
   }
}

   #ifdef EN_PROFILER_TRACE_GRAPHICS_API
   #define Profile( x )                        \
           {                                   \
           Log << "OpenGL: " << #x << endl;    \
           x;                                  \
           if (en::gpu::IsError( #x ))         \
              assert(0);                       \
           }
   #else 
   #define Profile( x )                        \
           {                                   \
           x;                                  \
           if (en::gpu::IsError( #x ))         \
              assert(0);                       \
           }
   #endif
#else
   #define Profile( x ) x; /* Nothing in Release */
#endif

namespace en
{
   namespace gpu
   {
   class OpenGLDevice : public CommonDevice
      {
      public:

      OpenGLDevice();
     ~OpenGLDevice();

      virtual void init(void);
      virtual Ptr<Buffer> create(const BufferType type, const uint32 size, const void* data = nullptr);
      };
   }
}
#endif

#endif

