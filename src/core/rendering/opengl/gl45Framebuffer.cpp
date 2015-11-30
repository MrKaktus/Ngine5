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
#include "core/rendering/opengl/gl45Framebuffer.h"

#include "core/rendering/context.h"

namespace en
{
   namespace gpu
   {
#ifndef EN_PLATFORM_OSX

   FramebufferGL45::FramebufferGL45() :
      FramebufferGL40()
   {
   }

   // Use: Texture1D, Texture2D, Texture2DRectangle, Texture2DMultisample (single layer)
   //      Texture1DArray, Texture2DArray, Texture2DMultisampleArray, Texture3D, TextureCubeMap, TextureCubeMapArray (all layers)
   void FramebufferGL45::color(const DataAccess access, const uint8 index, const Ptr<Texture> texture, const uint8 mipmap)
   {
   // API independent debug validation layer
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   assert( texture );
   assert( index < GpuContext.support.maxFramebufferColorAttachments );
   assert( mipmap < texture->mipmaps() );
   assert( TextureCapabilities[texture->format()].rendertarget );
#endif

#ifdef EN_DEBUG
   assert( texture->type() != TextureBuffer );
#endif

   uint16 glType = TranslateTextureType[texture->type()];
   Ptr<TextureGL> tex = ptr_dynamic_cast<TextureGL, Texture>(texture);
   switch(texture->type())
      {
      case Texture1D:
      case Texture1DArray:
      case Texture2D:
      case Texture2DArray:
      case Texture2DRectangle:
      case Texture2DMultisample:
      case Texture2DMultisampleArray:
      case Texture3D:
      case TextureCubeMap:
      case TextureCubeMapArray:
         Profile( glNamedFramebufferTexture(id, (GL_COLOR_ATTACHMENT0 + index), tex->id, mipmap) )
         break;

      default:
         // How we get here?
         assert( 0 );
         break;
      }
   }

   // Use: Texture1D, Texture2D, Texture2DRectangle, Texture2DMultisample
   //      Texture3D, TextureCubeMap (specific depth, face)   
   //      Texture1DArray, Texture2DArray, Texture2DMultisampleArray, TextureCubeMapArray (all layers)
   void FramebufferGL45::color(const DataAccess access, const uint8 index, const Ptr<Texture> texture, const uint16 layer, const uint8 mipmap)
   {
   // API independent debug validation layer
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   assert( texture );
   assert( index < GpuContext.support.maxFramebufferColorAttachments );
   assert( mipmap < texture->mipmaps() );
   assert( TextureCapabilities[texture->format()].rendertarget );
#endif

#ifdef EN_DEBUG
   assert( texture->type() != TextureBuffer );
#endif

   // Select framebuffer access type
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   if (access == Read)
      accessType = GL_READ_FRAMEBUFFER;

   uint16 glType = TranslateTextureType[texture->type()];
   Ptr<TextureGL> tex = ptr_dynamic_cast<TextureGL, Texture>(texture);
   switch(texture->type())
      {
      case Texture1D:
      case Texture1DArray:
      case Texture2D:
      case Texture2DArray:
      case Texture2DRectangle:
      case Texture2DMultisample:
      case Texture2DMultisampleArray:
      case TextureCubeMapArray:
         Profile( glNamedFramebufferTexture(id, (GL_COLOR_ATTACHMENT0 + index), tex->id, mipmap) )
         break;

      // Single layer binding
      case Texture3D:
         assert( layer < texture->depth() );
         Profile( glBindFramebuffer(accessType, id) )
         Profile( glFramebufferTexture3D(accessType, (GL_COLOR_ATTACHMENT0 + index), glType, tex->id, mipmap, layer) )
         break;

      case TextureCubeMap:
         assert( layer < 6 );
         Profile( glBindFramebuffer(accessType, id) )
         Profile( glFramebufferTexture2D(accessType, (GL_COLOR_ATTACHMENT0 + index), TranslateTextureFace[layer], tex->id, mipmap) )
         break;

      default:
         // How we get here?
         assert( 0 );
         break;
      }
   }
   
#endif
   }
}