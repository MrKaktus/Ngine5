/*

 Ngine v5.0
 
 Module      : OpenGL Framebuffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_OPENGL_31_FRAMEBUFFER
#define ENG_CORE_RENDERING_OPENGL_31_FRAMEBUFFER

#include "core/rendering/opengl/gl30Framebuffer.h"

#if defined(EN_MODULE_RENDERER_OPENGL)

namespace en
{
   namespace gpu
   {
   class FramebufferGL31 : public FramebufferGL30
      {
      public:
      FramebufferGL31();
                                      // Use: Texture1D, Texture2D, Texture2DRectangle, Texture2DMultisample
      virtual void color(             //      Texture1DArray, Texture2DArray, Texture2DMultisampleArray, Texture3D, TextureCubeMap, TextureCubeMapArray (all layers)
         const DataAccess access,     // Type of action that can be performed on this texture 
         const uint8 index,           // Color attachment
         const shared_ptr<Texture> texture,  // Texture
         const uint8 mipmap = 0);     // Mipmap
      
                                      // Use: Texture1D, Texture2D, Texture2DRectangle, Texture2DMultisample
                                      //      Texture3D, TextureCubeMap (specific depth, face)   
      virtual void color(             //      Texture1DArray, Texture2DArray, Texture2DMultisampleArray, TextureCubeMapArray (all layers)
         const DataAccess access,     // Type of action that can be performed on this texture 
         const uint8 index,           // Color attachment
         const shared_ptr<Texture> texture,  // Texture
         const uint16 layer,          // Depth of 3D texture or face of CubeMap
         const uint8 mipmap = 0);     // Mipmap
         
      };
   }
}
#endif

#endif
