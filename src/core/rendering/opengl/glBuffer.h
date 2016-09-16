/*

 Ngine v5.0
 
 Module      : OpenGL Buffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_OPENGL_BUFFER
#define ENG_CORE_RENDERING_OPENGL_BUFFER

#include "core/defines.h"

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/opengl/opengl.h"
#include "core/rendering/common/buffer.h"

namespace en
{
   namespace gpu
   {
   class BufferGL : public CommonBuffer
      {
      public:
      uint32 handle;  // OpenGL VBO handle
      uint32 vao;     // OpenGL VAO handle
      uint16 glType;
      
      BufferGL(const BufferType type, const uint32 size, const void* data);
      virtual void* map(const DataAccess access = ReadWrite);
      virtual bool unmap(void);
      virtual ~BufferGL();
      };
   }
}
#endif

#endif
