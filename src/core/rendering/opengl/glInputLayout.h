/*

 Ngine v5.0
 
 Module      : OpenGL Input Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_OPENGL_INPUT_LAYOUT
#define ENG_CORE_RENDERING_OPENGL_INPUT_LAYOUT

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_OPENGL)

#include "core/rendering/opengl/opengl.h"
#include "core/rendering/common/inputLayout.h"

#include "utilities/Nversion.h"

namespace en
{
   namespace gpu
   {
   // Buffer Column
   struct cachealign AttributeTranslation
      {
      uint16   size;       // Attribute size in bytes
      uint16   channels;   // Channels count
      bool     normalized; // Should data be normalized
      bool     integer;    // Should data be kept in integer format (no conversion to float)
      bool     qword;      // Is data 64bit per channel
      uint32   type;       // OpenGL input type
      };

   extern const AttributeTranslation TranslateAttribute[underlyingType(Attribute::Count)];
   extern const Nversion AttributeVersion[underlyingType(Attribute::Count)];
   
   class InputLayoutGL : public InputLayout
      {
      public:
      uint16 vao;
      DrawableType primitive;

      InputLayoutGL(const DrawableType primitiveType,
                       const uint32 controlPoints,
                       const uint32 usedAttributes,
                       const uint32 usedBuffers,
                       const AttributeDesc* attributes,
                       const BufferDesc* buffers);

      virtual ~InputLayoutGL();
      };
   }
}
#endif

#endif
