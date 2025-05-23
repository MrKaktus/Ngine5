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

#if defined(EN_MODULE_RENDERER_OPENGL)

#include "core/rendering/opengl/opengl.h"
#include "core/rendering/common/device.h"

#include "core/rendering/inputLayout.h"
#include "core/rendering/blend.h"
#include "core/rendering/raster.h"
#include "core/rendering/multisampling.h"
#include "core/rendering/viewport.h"
#include "core/rendering/depthStencil.h"
#include "core/rendering/pipeline.h"
#include "core/rendering/renderPass.h"
#include "core/rendering/texture.h"

#include "core/rendering/opengl/glCache.h"

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
   #define Validate( x )                       \
           {                                   \
           Log << "OpenGL: " << #x << endl;    \
           x;                                  \
           if (en::gpu::IsError( #x ))         \
              assert(0);                       \
           }
   #else 
   #define Validate( x )                       \
           {                                   \
           x;                                  \
           if (en::gpu::IsError( #x ))         \
              assert(0);                       \
           }
   #endif
#else
   #define Validate( x ) x; /* Nothing in Release */
#endif

namespace en
{
   namespace gpu
   {
   class OpenGLDevice : public CommonDevice
      {
      public:
      uint32 lastResult;
      DeviceVector cache;   // Cached OpenGL State Machine

      OpenGLDevice();
     ~OpenGLDevice();

      virtual void init(void);
      bool IsError(const char* function);
      
      virtual shared_ptr<Buffer> create(const BufferType type, const uint32 size, const void* data = nullptr);
      };
   }
}
#endif

#endif

