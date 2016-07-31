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
#include "core/rendering/opengl/gl30Framebuffer.h"

#include "core/rendering/context.h"

#if !defined(EN_PLATFORM_OSX)

namespace en
{
   namespace gpu
   {

   FramebufferGL30::FramebufferGL30()
   {
   assert( GpuContext.screen.created );
   Profile( glGenFramebuffers(1, &id) )
   }

   FramebufferGL30::~FramebufferGL30()
   {
   assert( GpuContext.screen.created );
   Profile( glDeleteFramebuffers(1, &id) )
   }

   // Use: Texture1D, Texture2D, Texture2DRectangle, Texture2DMultisample (single layer)
   //      Texture1DArray, Texture2DArray, Texture2DMultisampleArray, Texture3D, TextureCubeMap, TextureCubeMapArray (all layers)
   void FramebufferGL30::color(const DataAccess access, const uint8 index, const Ptr<Texture> texture, const uint8 mipmap)
   {
   // API independent debug validation layer
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   assert( texture );
   assert( index < GpuContext.support.maxFramebufferColorAttachments );
   assert( mipmap < texture->mipmaps() );
   assert( TextureCapabilities[underlyingType(texture->format())].rendertarget );
#endif

#ifdef EN_DEBUG
   // Layered texture binding is supported since OpenGL 3.2
   TextureType type = texture->type();
   assert( type != TextureType::Texture1DArray );
   assert( type != TextureType::Texture2DArray );
   assert( type != TextureType::Texture3D      );
   assert( type != TextureType::TextureCubeMap );
#endif

   // Select framebuffer access type
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   if (access == Read)
      accessType = GL_READ_FRAMEBUFFER;

   Profile( glBindFramebuffer(accessType, id) )
   uint16 glType = TranslateTextureType[underlyingType(texture->type())];
   Ptr<TextureGL> tex = ptr_dynamic_cast<TextureGL, Texture>(texture);
   switch(texture->type())
      {
      case TextureType::Texture1D:
         Profile( glFramebufferTexture1D(accessType, (GL_COLOR_ATTACHMENT0 + index), glType, tex->id, mipmap) )
         break;

      case TextureType::Texture2D:
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
   void FramebufferGL30::color(const DataAccess access, const uint8 index, const Ptr<Texture> texture, const uint16 layer, const uint8 mipmap)
   {
   // API independent debug validation layer
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   assert( texture );
   assert( index < GpuContext.support.maxFramebufferColorAttachments );
   assert( mipmap < texture->mipmaps() );
   assert( TextureCapabilities[underlyingType(texture->format())].rendertarget );
#endif

#ifdef EN_DEBUG
   // Layered texture binding is supported since OpenGL 3.2
   TextureType type = texture->type();
   assert( type != TextureType::Texture1DArray );
   assert( type != TextureType::Texture2DArray );
#endif

   // Select framebuffer access type
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   if (access == Read)
      accessType = GL_READ_FRAMEBUFFER;

   Profile( glBindFramebuffer(accessType, id) )
   uint16 glType = TranslateTextureType[underlyingType(texture->type())];
   Ptr<TextureGL> tex = ptr_dynamic_cast<TextureGL, Texture>(texture);
   switch(texture->type())
      {
      case TextureType::Texture1D:
         Profile( glFramebufferTexture1D(accessType, (GL_COLOR_ATTACHMENT0 + index), glType, tex->id, mipmap) )
         break;

      case TextureType::Texture2D:
         Profile( glFramebufferTexture2D(accessType, (GL_COLOR_ATTACHMENT0 + index), glType, tex->id, mipmap) )
         break;

      case TextureType::Texture3D:
         assert( layer < texture->depth() );
         Profile( glFramebufferTexture3D(accessType, (GL_COLOR_ATTACHMENT0 + index), glType, tex->id, mipmap, layer) )
         break;

      case TextureType::TextureCubeMap:
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

   void FramebufferGL30::depth(const DataAccess access, const Ptr<Texture> texture)
   {
   // API independent debug validation layer
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   assert( texture );
   TextureType type = texture->type();
   assert( type == TextureType::Texture2D ); // or 2DRectangle
   Format format = texture->format();
   assert( format == Format::D_16   ||
           format == Format::D_24   ||
           format == Format::D_32   ||
           format == Format::D_32_f );
#endif

   // Select framebuffer access type
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   if (access == Read)
      accessType = GL_READ_FRAMEBUFFER;

   uint16 glType = TranslateTextureType[underlyingType(texture->type())];
   Ptr<TextureGL> tex = ptr_dynamic_cast<TextureGL, Texture>(texture);
   Profile( glBindFramebuffer(accessType, id) )
   Profile( glFramebufferTexture2D(accessType, GL_DEPTH_ATTACHMENT, glType, tex->id, 0) )

#ifdef EN_DEBUG
   GLenum result;
   Profile( result = glCheckFramebufferStatus(accessType) )
   assert( result == GL_FRAMEBUFFER_COMPLETE );
#endif
   } 

   void FramebufferGL30::stencil(const DataAccess access, const Ptr<Texture> texture)
   {
   // API independent debug validation layer
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   assert( texture );
   TextureType type = texture->type();
   assert( type == TextureType::Texture2D ); // or 2DRectangle
   Format format = texture->format();
   assert( format == Format::S_8 );
#endif

   // Select framebuffer access type
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   if (access == Read)
      accessType = GL_READ_FRAMEBUFFER;

   uint16 glType = TranslateTextureType[underlyingType(texture->type())];
   Ptr<TextureGL> tex = ptr_dynamic_cast<TextureGL, Texture>(texture);
   Profile( glBindFramebuffer(accessType, id) )
   Profile( glFramebufferTexture2D(accessType, GL_STENCIL_ATTACHMENT, glType, tex->id, 0) )

#ifdef EN_DEBUG
   GLenum result;
   Profile( result = glCheckFramebufferStatus(accessType) )
   assert( result == GL_FRAMEBUFFER_COMPLETE );
#endif
   } 

   void FramebufferGL30::depthStencil(const DataAccess access, const Ptr<Texture> texture)
   {
   // API independent debug validation layer
#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   assert( texture );
   TextureType type = texture->type();
   assert( type == TextureType::Texture2D ); // or 2DRectangle
   Format format = texture->format();
   assert( format == Format::DS_24_8   ||
           format == Format::DS_32_f_8 );
#endif

   // Select framebuffer access type
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   if (access == Read)
      accessType = GL_READ_FRAMEBUFFER;

   uint16 glType = TranslateTextureType[underlyingType(texture->type())];
   Ptr<TextureGL> tex = ptr_dynamic_cast<TextureGL, Texture>(texture);
   Profile( glBindFramebuffer(accessType, id) )
   Profile( glFramebufferTexture2D(accessType, GL_DEPTH_STENCIL_ATTACHMENT, glType, tex->id, 0) )

#ifdef EN_DEBUG
   GLenum result;
   Profile( result = glCheckFramebufferStatus(accessType) )
   assert( result == GL_FRAMEBUFFER_COMPLETE );
#endif
   } 

   void FramebufferGL30::defaultColor(const uint8 index)
   {
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   Profile( glBindFramebuffer(accessType, id) )
   Profile( glFramebufferTexture2D(accessType, (GL_COLOR_ATTACHMENT0 + index), GL_TEXTURE_2D, 0, 0) )
   }

   void FramebufferGL30::defaultDepth(void)
   {
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   Profile( glBindFramebuffer(accessType, id) )
   Profile( glFramebufferTexture2D(accessType, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0) )
   }

   void FramebufferGL30::defaultStencil(void)
   {
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   Profile( glBindFramebuffer(accessType, id) )
   Profile( glFramebufferTexture2D(accessType, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0) )
   }

   void FramebufferGL30::defaultDepthStencil(void)
   {
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   Profile( glBindFramebuffer(accessType, id) )
   Profile( glFramebufferTexture2D(accessType, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0) )
   }



   void FramebufferGL30::defaultColor(const DataAccess access, const uint8 index)
   {
   assert( index < GpuContext.support.maxFramebufferColorAttachments );

   // Select framebuffer access type
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   if (access == Read)
      accessType = GL_READ_FRAMEBUFFER;

   Profile( glBindFramebuffer(accessType, id) )
   Profile( glFramebufferTexture2D(accessType, (GL_COLOR_ATTACHMENT0 + index), GL_TEXTURE_2D, 0, 0) )
   }

   void FramebufferGL30::defaultDepth(const DataAccess access)
   {
   // Select framebuffer access type
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   if (access == Read)
      accessType = GL_READ_FRAMEBUFFER;

   Profile( glBindFramebuffer(accessType, id) )
   Profile( glFramebufferTexture2D(accessType, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0) )
   }

   void FramebufferGL30::defaultStencil(const DataAccess access)
   {
   // Select framebuffer access type
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   if (access == Read)
      accessType = GL_READ_FRAMEBUFFER;

   Profile( glBindFramebuffer(accessType, id) )
   Profile( glFramebufferTexture2D(accessType, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0) )
   }

   void FramebufferGL30::defaultDepthStencil(const DataAccess access)
   {
   // Select framebuffer access type
   uint16 accessType = GL_DRAW_FRAMEBUFFER;
   if (access == Read)
      accessType = GL_READ_FRAMEBUFFER;

   Profile( glBindFramebuffer(accessType, id) )
   Profile( glFramebufferTexture2D(accessType, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0) )
   }

   }
}

#endif
