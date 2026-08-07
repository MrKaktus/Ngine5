/*

 Ngine v5.0
 
 Module      : Vulkan Resource Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_RESOURCE_LAYOUT
#define ENG_CORE_RENDERING_VULKAN_RESOURCE_LAYOUT

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/layout.h"

namespace en
{
namespace gpu
{

class VulkanDevice;

class SetLayoutVK : public SetLayout
{
    public:
    VulkanDevice* gpu;
    VkDescriptorSetLayout handle;

    // Translates each resource group type into backing it first binding index.
    uint32 bindingOfResourceType[underlyingType(ResourceType::Count)];

    // If true, resource group of given type, is represented by unbound array
    // of descriptors in the shader (array size is known on CPU side). If false
    // that resource group is represented by array of bindings (for e.g. 5 
    // bindings for 5 typed storage buffers).
    bool resourceGroupIsDescriptorsArray[underlyingType(ResourceType::Count)];

    SetLayoutVK(VulkanDevice* gpu, 
                VkDescriptorSetLayout layout, 
                const uint32 (&bindingOfResourceType)[underlyingType(ResourceType::Count)],
                const bool   (&resourceGroupIsDescriptorsArray)[underlyingType(ResourceType::Count)]);

    virtual ~SetLayoutVK();
};

class PipelineLayoutVK : public PipelineLayout
{
    public:
    VulkanDevice* gpu;
    VkPipelineLayout handle;

    PipelineLayoutVK(VulkanDevice* gpu, VkPipelineLayout handle);
    virtual ~PipelineLayoutVK();
};

class DescriptorsVK : public DescriptorsPool
{
    public:
    VulkanDevice*    gpu;
    VkDescriptorPool handle;

    virtual DescriptorSet* allocate(const SetLayout& layout);
    virtual bool allocate(const uint32 count,
                          const SetLayout*(&layouts)[],
                          DescriptorSet**& sets);
       
    DescriptorsVK(VulkanDevice* gpu, VkDescriptorPool handle);
    virtual ~DescriptorsVK();
};

// AMD hints on Descriptors:
// http://32ipi028l5q82yhj72224m8j.wpengine.netdna-cdn.com/wp-content/uploads/2016/03/VulkanFastPaths.pdf
class DescriptorSetVK : public DescriptorSet
{
    public:
    DescriptorsVK*  parent; // Reference to Descriptors Pool
    VkDescriptorSet handle;

    // Translates each resource group type into backing it first binding index.
    uint32 bindingOfResourceType[underlyingType(ResourceType::Count)];

    // If true, resource group of given type, is represented by unbound array
    // of descriptors in the shader (array size is known on CPU side). If false
    // that resource group is represented by array of bindings (for e.g. 5 
    // bindings for 5 typed storage buffers).
    bool resourceGroupIsDescriptorsArray[underlyingType(ResourceType::Count)];

    DescriptorSetVK(DescriptorsVK* parent, 
                    VkDescriptorSet handle,
                    const uint32(&bindingOfResourceType)[underlyingType(ResourceType::Count)],
                    const bool(&resourceGroupIsDescriptorsArray)[underlyingType(ResourceType::Count)]);

    virtual ~DescriptorSetVK();

    virtual void setBuffer(const uint32 slot, const Buffer& buffer);
    virtual void setSampler(const uint32 slot, const Sampler& sampler);
    virtual void setTextureView(const uint32 slot, const TextureView& view);
};

} // en::gpu
} // en

#endif
#endif
