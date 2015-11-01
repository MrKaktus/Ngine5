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

#include "core/rendering/opengl/opengl.h"
#include "core/rendering/opengl/glTexture.h"
#include "core/rendering/opengl/gl31Framebuffer.h"

#include "core/rendering/context.h"

namespace en
{
   namespace gpu
   {
   FramebufferGL31::FramebufferGL31() :
      FramebufferGL30()
   {
   }

   // Use: Texture1D, Texture2D, Texture2DRectangle, Texture2DMultisample (single layer)
   //      Texture1DArray, Texture2DArray, Texture2DMultisampleArray, Texture3D, TextureCubeMap, TextureCubeMapArray (all layers)
   void FramebufferGL31::color(const DataAccess access, const uint8 index, const Ptr<Texture> texture, const uint8 mipmap)
   {
   // API independent debug validation layer
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   assert( texture );
   assert( index < GpuContext.support.maxFramebufferColorAttachments );
   assert( mipmap < texture->mipmaps() );
   assert( TextureCapabilities[texture->format()].rendertarget );
#endif

#ifdef EN_DEBUG
   TextureType type = texture->type();
   assert( type != TextureBuffer );

   // Layered texture binding is supported since OpenGL 3.2
   assert( type != Texture1DArray );
   assert( type != Texture2DArray );
   assert( type != Texture3D      );
   assert( type != TextureCubeMap );
#endif

   // Select framebuffer access type
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   if (access == Read)
      accessType = GL_READ_FRAMEBUFFER;

   Profile( glBindFramebuffer(accessType, id) )
   uint16 glType = TranslateTextureType[texture->type()];
   Ptr<TextureGL> tex = ptr_dynamic_cast<TextureGL, Texture>(texture);
   switch(texture->type())
      {
      case Texture1D:
         Profile( glFramebufferTexture1D(accessType, (GL_COLOR_ATTACHMENT0 + index), glType, tex->id, mipmap) )
         break;

      case Texture2D:
      case Texture2DRectangle:
         Profile( glFramebufferTexture2D(accessType, (GL_COLOR_ATTACHMENT0 + index), glType, tex->id, mipmap) )
         break;

      default:
         // How we get here?
         assert( 0 );
         break;
      }

#ifdef EN_DEBUG
   GLenum result;
   Profile( result = glCheckFramebufferStatus(accessType) )
   assert( result == GL_FRAMEBUFFER_COMPLETE );
#endif
   }

   // Use: Texture1D, Texture2D, Texture2DRectangle, Texture2DMultisample
   //      Texture3D, TextureCubeMap (specific depth, face)   
   //      Texture1DArray, Texture2DArray, Texture2DMultisampleArray, TextureCubeMapArray (all layers)
   void FramebufferGL31::color(const DataAccess access, const uint8 index, const Ptr<Texture> texture, const uint16 layer, const uint8 mipmap)
   {
   // API independent debug validation layer
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   assert( texture );
   assert( index < GpuContext.support.maxFramebufferColorAttachments );
   assert( mipmap < texture->mipmaps() );
   assert( TextureCapabilities[texture->format()].rendertarget );
#endif

#ifdef EN_DEBUG
   TextureType type = texture->type();
   assert( type != TextureBuffer );

   // Layered texture binding is supported since OpenGL 3.2
   assert( type != Texture1DArray );
   assert( type != Texture2DArray );
#endif

   // Select framebuffer access type
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   if (access == Read)
      accessType = GL_READ_FRAMEBUFFER;

   Profile( glBindFramebuffer(accessType, id) )
   uint16 glType = TranslateTextureType[texture->type()];
   Ptr<TextureGL> tex = ptr_dynamic_cast<TextureGL, Texture>(texture);
   switch(texture->type())
      {
      case Texture1D:
         Profile( glFramebufferTexture1D(accessType, (GL_COLOR_ATTACHMENT0 + index), glType, tex->id, mipmap) )
         break;

      case Texture2D:
      case Texture2DRectangle:
         Profile( glFramebufferTexture2D(accessType, (GL_COLOR_ATTACHMENT0 + index), glType, tex->id, mipmap) )
         break;

      case Texture3D:
         assert( layer < texture->depth() );
         Profile( glFramebufferTexture3D(accessType, (GL_COLOR_ATTACHMENT0 + index), glType, tex->id, mipmap, layer) )
         break;

      case TextureCubeMap:
         assert( layer < 6 );
         Profile( glFramebufferTexture2D(accessType, (GL_COLOR_ATTACHMENT0 + index), TranslateTextureFace[layer], tex->id, mipmap) )
         break;

      default:
         // How we get here?
         assert( 0 );
         break;
      }

#ifdef EN_DEBUG
   GLenum result;
   Profile( result = glCheckFramebufferStatus(accessType) )
   assert( result == GL_FRAMEBUFFER_COMPLETE );
#endif
   }

   }
}