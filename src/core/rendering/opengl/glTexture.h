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

#ifndef ENG_CORE_RENDERING_OPENGL_TEXTURE
#define ENG_CORE_RENDERING_OPENGL_TEXTURE

#include "threading/mutex.h"
#include "core/rendering/common/texture.h"

namespace en
{
   namespace gpu
   {
   struct SurfaceDescriptorGL
      {
      Nmutex lock;    // Locks this texture instance, to prevent it from beeing modified by other thread while it is mapped  
      uint8  mipmap;  // MipMap level
      uint16 layer;   // MipMap depth slice / CubeMap face / Array layer / CubeMapArray face-layer
      void*  ptr;     // Pointer to local memory (if PBO's are not supported)
      uint32 pbo;     // OpenGL PBO handle (OpenGL 2.1+)
      
      SurfaceDescriptorGL();
      };

   class TextureGL : public CommonTexture
      {
      public:
      SurfaceDescriptorGL desc;  // Mapped surface description
      uint32 id;               // OpenGL handle

      TextureGL();
      TextureGL(const TextureState& state);
      TextureGL(const TextureState& state, const uint32 id);    // Create OpenGL texture interface for texture that already exists

      virtual void*    map(const uint8 mipmap = 0, const uint16 surface = 0);  // Surface is: CubeMap face, 3D depth slice, Array layer or CubeMapArray face-layer
      virtual bool     unmap(void);
      virtual bool     read(uint8* buffer, const uint8 mipmap = 0, const uint16 surface = 0) const; // Reads texture mipmap to given buffer (app needs to allocate it)

      virtual ~TextureGL();
      };

   struct cachealign TextureFormatTranslation
      {
      uint16   dstFormat;             // OpenGL sized internal format
      uint16   srcFormat;             // OpenGL input format
      uint16   srcType;               // OpenGL input type
      };

   extern const uint16 TranslateTextureFace[6];
   extern const uint16 TranslateTextureType[underlyingType(TextureType::Count)]; 
   extern const TextureFormatTranslation TranslateTextureFormat[underlyingType(Format::Count)];

#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   void InitTextureSupport(void);
   void ClearTextureSupport(void);
#endif
   }
}

#endif
