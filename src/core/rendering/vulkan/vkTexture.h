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

#include "core/rendering/sampler.h"
#include "core/rendering/common/texture.h"
#include "core/rendering/vulkan/vkHeap.h"

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
    VkImage       handle;        // Vulkan Image ID
    VkMemoryRequirements memoryRequirements; // Memory requirements of this Texture
    HeapVK*       heap;          // Memory backing heap
    uint64        offset;        // Offset in the heap

    TextureVK(VulkanDevice* gpu, const TextureState& state);

    virtual Heap*        parent(void) const;
    virtual TextureView* view(void);
    virtual TextureView* view(const TextureType type,
                              const Format format,
                              const uint32v2 mipmaps,
                              const uint32v2 layers);


    virtual ~TextureVK();
};
      
class TextureViewVK : public CommonTextureView
{
    public:
    TextureVK&  texture; // Parent texture
    VkImageView handle;  // Vulkan Image View ID

    TextureViewVK(TextureVK&        parent,
                  const VkImageView view,
                  const TextureType type,
                  const Format      format,
                  const uint32v2    mipmaps,
                  const uint32v2    layers);

    Texture& parent(void) const;
 
    virtual ~TextureViewVK();
};

VkImageAspectFlags TranslateImageAspect(const Format format);
TextureVK* createTexture(VulkanDevice* gpu, const TextureState& state);

} // en::gpu
} // en

#endif
#endif
