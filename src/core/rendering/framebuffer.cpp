/*

 Ngine v5.0
 
 Module      : Framebuffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/log/log.h"
#include "core/rendering/context.h"
#include "core/rendering/opengl/gl43Texture.h"

#if 0 //!defined(EN_PLATFORM_OSX)

namespace en
{
   namespace gpu
   {
   Framebuffer::~Framebuffer()
   {
   };


      namespace gl30
      {
      #ifdef EN_OPENGL_DESKTOP


//    // Use: Texture1D, Texture2D, Texture2DRectangle, Texture2DMultisample
//    //      Texture3D, TextureCubeMap (specific depth, face)   
//    //      Texture1DArray, Texture2DArray, Texture2DMultisampleArray, TextureCubeMapArray (all layers)
//    bool Framebuffer::color(const DataAccess access, const uint8 index, const Ptr<Texture> texture, const uint16 layer, const uint8 mipmap)
//    {
//    // Default layer binding
//    Profile( glBindFramebuffer(accessType, id) )
//    if (type == Texture1D)
//       Profile( glFramebufferTexture1D(accessType, (GL_COLOR_ATTACHMENT0 + index), GL_TEXTURE_1D, tex->id, mipmap) )
//    if (type == Texture2D)
//       Profile( glFramebufferTexture2D(accessType, (GL_COLOR_ATTACHMENT0 + index), GL_TEXTURE_2D, tex->id, mipmap) )
//    // OpenGL 3.1
//    if (type == Texture2DRectangle)
//       Profile( glFramebufferTexture2D(accessType, (GL_COLOR_ATTACHMENT0 + index), GL_TEXTURE_RECTANGLE, tex->id, 0) )
//    // OpenGL 3.2
//    if (type == Texture2DMultisample)
//       Profile( glFramebufferTexture2D(accessType, (GL_COLOR_ATTACHMENT0 + index), GL_TEXTURE_2D_MULTISAMPLE, tex->id, 0) )
//
//    // Single layer binding
//    if (type == Texture3D)
//       {
//       assert( layer < texture->depth() );
//       Profile( glFramebufferTexture3D(accessType, (GL_COLOR_ATTACHMENT0 + index), GL_TEXTURE_3D, tex->id, mipmap, layer) )
//       }
//    if (type == TextureCubeMap)
//       {
//       assert( layer < 6 );
//       Profile( glFramebufferTexture2D(accessType, (GL_COLOR_ATTACHMENT0 + index), TranslateTextureFace[layer], tex->id, mipmap) )
//       }
//
//    // OpenGL 3.2 (layered attachments)
//    if (type == Texture1DArray            ||
//        type == Texture2DArray            ||
//        type == Texture2DMultisampleArray || // mipmap = 0
//        type == Texture3D                 ||
//        type == TextureCubeMap      )
//       Profile( glFramebufferTexture(accessType, (GL_COLOR_ATTACHMENT0 + index), tex->id, mipmap) )
//
//    // OpenGL 4.0 (layered attachments)
//    if (type == TextureCubeMapArray )
//       Profile( glFramebufferTexture(accessType, (GL_COLOR_ATTACHMENT0 + index), tex->id, mipmap) )
//
//    // OpenGL 4.5 (layered attachments)
//    if (type == Texture1DArray            ||
//        type == Texture2DArray            ||
//        type == Texture2DMultisampleArray || // mipmap = 0
//        type == Texture3D                 ||
//        type == TextureCubeMap            ||
//        type == TextureCubeMapArray )
//       Profile( glNamedFramebufferTexture(id, (GL_COLOR_ATTACHMENT0 + index), tex->id, mipmap) )
//    }

      #endif // EN_OPENGL_DESKTOP
      }
   }
}

#endif
