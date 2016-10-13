/*

 Ngine v5.0
 
 Module      : OpenGL Texture.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_OPENGL_43_TEXTURE
#define ENG_CORE_RENDERING_OPENGL_43_TEXTURE

#include "core/rendering/opengl/glTexture.h"

#if defined(EN_MODULE_RENDERER_OPENGL)

namespace en
{
   namespace gpu
   {
   class TextureGL43 : public TextureGL
      {
      public:
      TextureGL43(const TextureState& state);
      virtual ~TextureGL43();
      };
   }
}
#endif

#endif
