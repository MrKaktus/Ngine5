/*

 Ngine v5.0
 
 Module      : Vulkan Texture.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_TEXTURE
#define ENG_CORE_RENDERING_VULKAN_TEXTURE

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/common/texture.h"
#include "core/rendering/sampler.h"

namespace en
{
   namespace gpu
   {
   //extern static const VkImageType TranslateType[];

   extern const VkFormat TranslateTextureFormat[underlyingType(Format::Count)];

   class VulkanDevice;
   class Heap;

   class TextureVK : public CommonTexture
      {
      public:
      VulkanDevice* gpu;
      VkImage       handle;     // Vulkan Image ID
      VkImageView   viewHandle; // Vulkan default Image View ID
      VkMemoryRequirements memoryRequirements; // Memory requirements of this Texture
      Ptr<Heap>     heap;       // Memory backing heap
      uint64        offset;     // Offset in the heap

      TextureVK(VulkanDevice* gpu, const TextureState& state);
      TextureVK(VulkanDevice* gpu, const TextureState& state, const uint32 id);    // Create texture interface for texture that already exists

      Ptr<TextureView> view(const TextureType type,
                            const Format format,
                            const uint32v2 mipmaps,         
                            const uint32v2 layers) const;
         
      virtual void*    map(const uint8 mipmap = 0, const uint16 surface = 0);  // Surface is: CubeMap face, 3D depth slice, Array layer or CubeMapArray face-layer
      virtual bool     unmap(void);
      virtual bool     read(uint8* buffer, const uint8 mipmap = 0, const uint16 surface = 0) const; // Reads texture mipmap to given buffer (app needs to allocate it)

      virtual ~TextureVK();
      };
      
   class TextureViewVK : public CommonTextureView
      {
      public:
      Ptr<TextureVK> texture; // Parent texture
      VkImageView    handle;  // Vulkan Image View ID

      TextureViewVK(Ptr<TextureVK>    parent,
                    const VkImageView view,
                    const TextureType type,
                    const Format      format,
                    const uint32v2    mipmaps,
                    const uint32v2    layers);

      Ptr<Texture> parent(void) const;
   
      virtual ~TextureViewVK();
      };
      
   class SamplerVK : public Sampler
      {
      public:
      VulkanDevice* gpu;
      VkSampler     handle;  // Vulkan Sampler ID
   
      SamplerVK(VulkanDevice* gpu, VkSampler handle);
     ~SamplerVK();
      };


   class VulkanDevice;

   VkImageAspectFlags TranslateImageAspect(const Format format);
   Ptr<TextureVK> createTexture(VulkanDevice* gpu, const TextureState& state);
   }
}

#endif

#endif
