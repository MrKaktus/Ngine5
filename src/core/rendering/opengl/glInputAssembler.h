/*

 Ngine v5.0
 
 Module      : OpenGL Input Assembler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_OPENGL_INPUT_ASSEMBLER
#define ENG_CORE_RENDERING_OPENGL_INPUT_ASSEMBLER

#include "core/defines.h"

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/opengl/opengl.h"
#include "core/rendering/common/inputAssembler.h"

#include "utilities/Nversion.h"

namespace en
{
   namespace gpu
   {
   extern const Nversion AttributeVersion[underlyingType(Attribute::Count)];
   
   class InputAssemblerGL : public InputAssembler
      {
      public:
      uint16 vao;
      DrawableType primitive;

      InputAssemblerGL(const DrawableType primitiveType,
                       const uint32 controlPoints,
                       const uint32 usedAttributes,
                       const uint32 usedBuffers,
                       const AttributeDesc* attributes,
                       const BufferDesc* buffers);

      virtual ~InputAssemblerGL();
      };
   }
}
#endif

#endif
