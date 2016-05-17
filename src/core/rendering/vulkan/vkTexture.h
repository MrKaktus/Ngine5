

#ifndef ENG_CORE_RENDERING_VULKAN_TEXTURE
#define ENG_CORE_RENDERING_VULKAN_TEXTURE

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/common/texture.h"
#include "core/rendering/sampler.h"

namespace en
{
   namespace gpu
   {
   //extern static const VkImageType TranslateType[];

   extern const VkFormat TranslateTextureFormat[underlyingType(Format::Count)];

   class TextureVK : public TextureCommon
      {
      public:
      VkImage     handle;    // Vulkan Image ID
      VkImageView view;      // Vulkan default Image View ID
      VkMemoryRequirements memoryRequirements; // Memory requirements of this Texture
                     // Vulkan memory pool ID

      TextureVK();
      TextureVK(const TextureState& state);
      TextureVK(const TextureState& state, const uint32 id);    // Create texture interface for texture that already exists

      virtual void*    map(const uint8 mipmap = 0, const uint16 surface = 0);  // Surface is: CubeMap face, 3D depth slice, Array layer or CubeMapArray face-layer
      virtual bool     unmap(void);
      virtual bool     read(uint8* buffer, const uint8 mipmap = 0, const uint16 surface = 0) const; // Reads texture mipmap to given buffer (app needs to allocate it)

      virtual ~TextureVK();
      };
      
   class TextureViewVK : public TextureView
      {
      public:
      VkImageView view;      // Vulkan Image View ID
      
      TextureViewVK();
      virtual ~TextureViewVK();
      };
   
   class SamplerVK : public Sampler
      {
      public:
      VkSampler handle;  // Vulkan Sampler ID
   
      SamplerVK();
     ~SamplerVK();
      };
   }
}

#endif

#endif
