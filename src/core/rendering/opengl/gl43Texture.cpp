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

#include "core/rendering/opengl/opengl.h"
#include "core/rendering/opengl/gl43Texture.h"

#include "core/rendering/context.h"

namespace en
{
   namespace gpu
   {
   TextureGL43::TextureGL43(const TextureState& _state) :
      TextureGL()
   {
   state = _state;

   // Calculate amount of mipmaps texture will have
   state.mipmaps = TextureMipMapCount(state);

   // Calculate amount of layers texture will have
   if ( state.layers == 1 &&
        ( state.type == TextureCubeMap ||
          state.type == TextureCubeMapArray ) )
      state.layers = 6;

   // Gather API specific texture state 
   uint16 glType           = TranslateTextureType[state.type];
   uint16 glInternalFormat = TranslateTextureFormat[state.format].dstFormat;
   uint32 mipmaps          = this->mipmaps();
   uint16 width            = this->width();
   uint16 height           = this->height();
   uint16 depth            = state.layers;      // 3D depth
   uint32 layers           = state.layers;      // Array layers, CubeMap faces, CubeMapArray face-layers
#ifndef EN_PLATFORM_OSX
   uint32 samples          = state.samples;
#endif

   // Generate texture descriptor with default state
   Profile( glGenTextures(1, (GLuint*)&id) );
   Profile( glBindTexture(glType, id) );
   if ( (state.type != Texture2DMultisample) &&
        (state.type != Texture2DMultisampleArray) )
      {
      if (state.type != Texture2DRectangle)
         {
         Profile( glTexParameteri(glType, GL_TEXTURE_WRAP_S, GL_REPEAT) );
         Profile( glTexParameteri(glType, GL_TEXTURE_WRAP_T, GL_REPEAT) );
         Profile( glTexParameteri(glType, GL_TEXTURE_WRAP_R, GL_REPEAT) );
         }
      Profile( glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
      Profile( glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
      }

   // Generate PBO
   Profile( glGenBuffers(1, &desc.pbo) );

   // Reserve memory for texture completness (OpenGL 4.2 or ARB_texture_storage) 
   switch(state.type)
      {
      case Texture1D:
         Profile( glTexStorage1D(glType, mipmaps, glInternalFormat, width) );
         break;
   
      case Texture1DArray:
         Profile( glTexStorage2D(glType, mipmaps, glInternalFormat, width, layers) );
         break;
   
      case Texture2D:
         Profile( glTexStorage2D(glType, mipmaps, glInternalFormat, width, height) );
         break;
   
      case Texture2DArray:
         Profile( glTexStorage3D(glType, mipmaps, glInternalFormat, width, height, layers) );
         break;
   
      case Texture2DRectangle:
         Profile( glTexStorage2D(glType, 1, glInternalFormat, width, height) );
         break;

#ifndef EN_PLATFORM_OSX
      // OpenGL 4.3  or  ARB_texture_storage_multisample
      case Texture2DMultisample:
         // TODO: Check samples count (depth/color/integer)???
         Profile( glTexStorage2DMultisample(glType, samples, glInternalFormat, width, height, GL_TRUE) );
         break;
   
      // OpenGL 4.3  or  ARB_texture_storage_multisample
      case Texture2DMultisampleArray:
         // TODO: Check samples count (depth/color/integer)???
         Profile( glTexStorage3DMultisample(glType, samples, glInternalFormat, width, height, layers, GL_TRUE) );
         break;
#endif

      case TextureBuffer:
         // Buffer textures don't have their own storage
         break;
   
      case Texture3D:
         Profile( glTexStorage3D(glType, mipmaps, glInternalFormat, width, height, depth) );
         break;
   
      case TextureCubeMap:
         Profile( glTexStorage2D(glType, mipmaps, glInternalFormat, width, width) );
         break;
   
      case TextureCubeMapArray:
         Profile( glTexStorage3D(glType, mipmaps, glInternalFormat, width, width, layers) );
         break;
   
      default:
         // How we get here?
         assert( 0 );
         break;
      };
   }

   TextureGL43::~TextureGL43()
   {
   // TextureGL destructor will handle objects 
   }

   }
}