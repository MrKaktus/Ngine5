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

#ifndef ENG_CORE_RENDERING_COMMON_TEXTURE
#define ENG_CORE_RENDERING_COMMON_TEXTURE

//#include "core/defines.h"
#include "core/configuration.h"
#include "core/rendering/texture.h"

namespace en
{
   namespace gpu
   {
   class CommonTextureView;
   
   class CommonTexture : public Texture
      {
      public:
      TextureState      state;

      CommonTexture();
      CommonTexture(const TextureState& state);

      virtual TextureType type(void) const;
      virtual Format   format(void) const;
      virtual uint32   mipmaps(void) const;
      virtual uint32   size(const uint8 mipmap = 0) const;
      virtual uint32   width(const uint8 mipmap = 0) const;
      virtual uint32   height(const uint8 mipmap = 0) const;
      virtual uint32   depth(const uint8 mipmap = 0) const;
      virtual uint32v2 resolution(const uint8 mipmap = 0) const;
      virtual uint32v3 volume(const uint8 mipmap = 0) const;
      virtual uint16   layers(void) const;
      virtual uint16   samples(void) const;

      virtual bool     read(uint8* buffer, const uint8 mipmap = 0, const uint16 surface = 0) const = 0; // Reads texture mipmap to given buffer (app needs to allocate it)

      virtual Ptr<TextureView> view(void) const;
      
      virtual ~CommonTexture() {};                         // Polymorphic deletes require a virtual base destructor
      };

   class CommonTextureView : public TextureView
      {
      public:
      TextureType  viewType;
      Format       viewFormat;
      uint32v2     mipmaps;    // Base mipmap in parent texture and mipmaps count
      uint32v2     layers;     // Base layer in parent texture and layers count

      CommonTextureView(const TextureType type,
                        const Format   format,
                        const uint32v2 mipmaps,
                        const uint32v2 layers);
         
      virtual TextureType type(void) const;
      virtual Format   format(void) const;
      virtual uint32v2 parentMipmaps(void) const;
      virtual uint32v2 parentLayers(void) const;
      
      virtual ~CommonTextureView() {};               // Polymorphic deletes require a virtual base destructor
      };

   struct TextureCompressedBlockInfo
      {
      uint32 width      : 8;   // Block width
      uint32 height     : 8;   // Block height
      uint32 size       : 8;   // Block size or texel size in bytes
      uint32 compressed : 1;   // Validation flag
      uint32            : 7;   // Padding
      };

   extern const TextureCompressedBlockInfo TextureCompressionInfo[underlyingType(Format::Count)];

#ifdef EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
   struct TextureInfo
      {
      uint32 supported    : 1;  // Supported
      uint32 rendertarget : 1;  // Can be used as rendertarget
      uint32              : 30; // Reserved
      };

   extern bool TextureTypeSupported[underlyingType(TextureType::Count)];
   extern TextureInfo TextureCapabilities[underlyingType(Format::Count)];
#endif

   bool   TextureFormatIsDepth(const Format format);
   bool   TextureFormatIsStencil(const Format format);
   bool   TextureFormatIsDepthStencil(const Format format);
   uint32 TextureMipMapCount(const TextureState& state);
   }
}

#endif
