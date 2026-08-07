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

#include "core/rendering/vulkan/vkLayout.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/log/log.h"
#include "core/rendering/vulkan/vkValidate.h"
#include "core/rendering/vulkan/vkDevice.h"
#include "core/rendering/vulkan/vkCommandBuffer.h"
#include "core/rendering/vulkan/vkBuffer.h"
#include "core/rendering/vulkan/vkSampler.h"
#include "core/rendering/vulkan/vkTexture.h"

namespace en
{
namespace gpu
{

// If not for Vulkan SPIR-V / GLSL limitation, this translation wouldn't be needed at all.
static ResourceType TranslateTextureType[underlyingType(TextureType::Count)] = 
{
    ResourceType::Texture1D                 , // TextureType::Texture1D
    ResourceType::Texture1DArray            , // TextureType::Texture1DArray
    ResourceType::Texture2D                 , // TextureType::Texture2D
    ResourceType::Texture2DArray            , // TextureType::Texture2DArray
    ResourceType::Texture2DMultisample      , // TextureType::Texture2DMultisample
    ResourceType::Texture2DMultisampleArray , // TextureType::Texture2DMultisampleArray
    ResourceType::Texture3D                 , // TextureType::Texture3D
    ResourceType::TextureCubeMap            , // TextureType::TextureCubeMap
    ResourceType::TextureCubeMapArray       , // TextureType::TextureCubeMapArray
};

static ResourceType TranslateBufferType[underlyingType(BufferType::Count)] =
{
    ResourceType::Invalid       , // BufferType::Vertex
    ResourceType::Invalid       , // BufferType::Index
    ResourceType::UniformBuffer , // BufferType::Uniform
    ResourceType::StorageBuffer , // BufferType::Storage
    ResourceType::Invalid       , // BufferType::Indirect
    ResourceType::Invalid       , // BufferType::Transfer
};

static const VkDescriptorType TranslateResourceType[underlyingType(ResourceType::Count)] =
{
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,  // Texture1D
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,  // Texture1DArray
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,  // Texture2D
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,  // Texture2DArray
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,  // Texture2DMultisample
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,  // Texture2DMultisampleArray
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,  // Texture3D
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,  // TextureCubeMap
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,  // TextureCubeMapArray
    VK_DESCRIPTOR_TYPE_SAMPLER,        // Sampler
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // UniformBuffer
    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, // StorageBuffer
};

// Is using VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER introducing any performance regression on AMD or Intel?
// Or is it safe to use it instead of VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, as NVidia HW benefits from this?
      
// VK_DESCRIPTOR_TYPE_SAMPLER = 0,                 //   Sampler
// VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,  // ? Texture + Sampler (NVidia optimal)
// VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,           //   Texture
// VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,           //   Image   (write, atomics, no filtering)
// VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,    // x Buffer Texture backed by UBO memory - buffer texture is simply a way for the shader to directly access a large array of data, generally larger than uniform buffer objects allow.
// VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,    // x Buffer Texture backed by Storage memory (write, atomics)
// VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,          //   Uniform
// VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,          //   Storage
// VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,  // * Uniform (dynamic array size, specifcied on runtime)
// VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,  //   Storage (dynamic array size, specifcied on runtime)
// VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,       // Render Pass Input Attachment for multi-sub-pass graph (Fragment Shader read of RT)

SetLayoutVK::SetLayoutVK(
        VulkanDevice* _gpu, 
        VkDescriptorSetLayout _handle,
        const uint32(&_bindingOfResourceType)[underlyingType(ResourceType::Count)],
        const bool(&_resourceGroupIsDescriptorsArray)[underlyingType(ResourceType::Count)]) :
    gpu(_gpu),
    handle(_handle)
{
    // Layout needs to retain information about which resource type is expected per each binding group.
    memcpy(&bindingOfResourceType, &_bindingOfResourceType, sizeof(uint32) * underlyingType(ResourceType::Count));
    memcpy(&resourceGroupIsDescriptorsArray, &_resourceGroupIsDescriptorsArray, underlyingType(ResourceType::Count));
}

SetLayoutVK::~SetLayoutVK()
{
    ValidateNoRet( gpu, vkDestroyDescriptorSetLayout(gpu->device, handle, nullptr) )
}

PipelineLayoutVK::PipelineLayoutVK(
        VulkanDevice* _gpu, 
        VkPipelineLayout _handle) :
    gpu(_gpu),
    handle(_handle)
{
}

PipelineLayoutVK::~PipelineLayoutVK()
{
    ValidateNoRet( gpu, vkDestroyPipelineLayout(gpu->device, handle, nullptr) )
}

// PipelineLayout:
// - constants
// - Descriptors Set/Table Layout
//   - ranges
//   - Static Samplers 

// Root Signature:
// - 32bit constants
// - Descriptors Set/Table Layout
//   - ranges of different sets    <- Vulkan: Stage per group/range
// - CBV (Uniform, Storage)
// - SRV (Texture)
// - UAV (Image)
//
// - Static Samplers (separate)




//   ResourcesSet*
//   
//   // SetLayout:
//   // N - Samplers           - stages: V, F
//   // K - Immutable Samplers - stages: V
//   // W - Textures           - stages: F
//   // H - Images             - stages: V, F
//   
//
//   
//   {
//   std::shared_ptr<Layout> result = nullptr; 
//   
//	typedef struct {
//	    VkShaderStageFlags                          stageFlags;
//	    uint32_t                                    offset;
//	    uint32_t                                    size; // <= VkPhysicalDeviceLimits::maxPushConstantsSize - offset
//	} VkPushConstantRange;
//      
//   return result;
//   }





// TODO: Creation and binding of Descriptors move to vkDescriptor.cpp
//
// VkDescriptorPool
// VkDescriptorSet   -> matches VkDescriptorSetLayout
//





// DESCRIPTOR SET
//////////////////////////////////////////////////////////////////////////


DescriptorSetVK::DescriptorSetVK(
        DescriptorsVK* _parent, 
        VkDescriptorSet _handle,
        const uint32(&_bindingOfResourceType)[underlyingType(ResourceType::Count)],
        const bool(&_resourceGroupIsDescriptorsArray)[underlyingType(ResourceType::Count)]) :
    parent(_parent),
    handle(_handle)
{
    // Descriptor set needs to retain information about which resource type is expected per each binding group.
    memcpy(&bindingOfResourceType, &_bindingOfResourceType, sizeof(uint32) * underlyingType(ResourceType::Count));
    memcpy(&resourceGroupIsDescriptorsArray, &_resourceGroupIsDescriptorsArray, underlyingType(ResourceType::Count));
}

DescriptorSetVK::~DescriptorSetVK()
{
    Validate( parent->gpu, vkFreeDescriptorSets(parent->gpu->device, parent->handle, 1u, &handle) )
}
   
// TODO: Update DescriptorSets with batches of resources (or cache updates)

//VkWriteDescriptorSet writeDesc;
//writeDesc.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//writeDesc.pNext           = nullptr;
//writeDesc.dstSet          = handle;
//writeDesc.dstBinding      = 0; // TODO: Binding index is per group of descriptors! This means we need to keep layout to determine it! FIX IT!
//writeDesc.dstArrayElement = slot;
//writeDesc.descriptorCount = 1U;
//writeDesc.descriptorType  = TranslateResourceType[underlyingType(ResourceType::Count)];
//const VkDescriptorImageInfo*     pImageInfo;
//const VkDescriptorBufferInfo*    pBufferInfo;
//const VkBufferView*              pTexelBufferView;

//vkUpdateDescriptorSets(gpu->device
//uint32_t                                    descriptorWriteCount,
//const VkWriteDescriptorSet*                 pDescriptorWrites,
//uint32_t                                    descriptorCopyCount,
//const VkCopyDescriptorSet*                  pDescriptorCopies);


void DescriptorSetVK::setBuffer(const uint32 _slot, const Buffer& _buffer)
{
    VulkanDevice* gpu = parent->gpu;

    const BufferVK& src = reinterpret_cast<const BufferVK&>(_buffer);

    uint32 bufferTypeIndex = underlyingType(src.type());

    assert(bufferTypeIndex < underlyingType(BufferType::Count));
    ResourceType resourceType = TranslateBufferType[bufferTypeIndex];
    if (resourceType == ResourceType::Invalid) // unlikely
    {
        logError("Buffers of type %u are not supported for binding to descriptor set!\n", bufferTypeIndex);
        return;
    }
    uint32 resourceTypeIndex = underlyingType(resourceType);

    assert(resourceTypeIndex < underlyingType(ResourceType::Count));
    uint32 binding = bindingOfResourceType[resourceTypeIndex];
    if (binding == 0xFFFFFFFF) // unlikely
    {
        logError("Buffers of type %u are not part of this descriptor set!\n", bufferTypeIndex);
        return;
    }

    uint32 slot = _slot;

    if (!resourceGroupIsDescriptorsArray[resourceTypeIndex])
    {
        binding += slot;
        slot = 0;
    }

    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = src.handle;
    bufferInfo.offset = 0U;
    bufferInfo.range  = src.size;

    VkWriteDescriptorSet writeDesc;
    writeDesc.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDesc.pNext            = nullptr;
    writeDesc.dstSet           = handle;
    writeDesc.dstBinding       = binding;
    writeDesc.dstArrayElement  = slot;
    writeDesc.descriptorCount  = 1U;
    writeDesc.descriptorType   = src.apiType == BufferType::Uniform ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeDesc.pImageInfo       = nullptr;
    writeDesc.pBufferInfo      = &bufferInfo; // Array of buffer descriptors
    writeDesc.pTexelBufferView = nullptr;     // Texel Buffers are not supported

    ValidateNoRet( gpu, vkUpdateDescriptorSets(gpu->device, 1, &writeDesc, 0, nullptr) )
}

void DescriptorSetVK::setSampler(const uint32 slot, const Sampler& _sampler)
{
    VulkanDevice* gpu = parent->gpu;

    const SamplerVK& src = reinterpret_cast<const SamplerVK&>(_sampler);

    uint32 binding = bindingOfResourceType[underlyingType(ResourceType::Sampler)];
    if (binding == 0xFFFFFFFF) // unlikely
    {
        logError("Samplers are not part of this descriptor set!\n");
        return;
    }

    VkDescriptorImageInfo imageInfo;
    imageInfo.sampler     = src.handle;
    imageInfo.imageView   = VK_NULL_HANDLE;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Ommited for Samplers

    VkWriteDescriptorSet writeDesc;
    writeDesc.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDesc.pNext            = nullptr;
    writeDesc.dstSet           = handle;
    writeDesc.dstBinding       = binding;
    writeDesc.dstArrayElement  = slot;
    writeDesc.descriptorCount  = 1U;
    writeDesc.descriptorType   = VK_DESCRIPTOR_TYPE_SAMPLER; // Sampler
    writeDesc.pImageInfo       = &imageInfo;  // Array of image descriptors
    writeDesc.pBufferInfo      = nullptr; 
    writeDesc.pTexelBufferView = nullptr;     // Texel Buffers are not supported

    ValidateNoRet( gpu, vkUpdateDescriptorSets(gpu->device, 1, &writeDesc, 0, nullptr) )
}

void DescriptorSetVK::setTextureView(const uint32 _slot, const TextureView& _view)
{
    VulkanDevice* gpu = parent->gpu;

    const TextureViewVK& src = reinterpret_cast<const TextureViewVK&>(_view);

    uint32 textureTypeIndex = underlyingType(src.type());

    assert(textureTypeIndex < underlyingType(TextureType::Count) );
    ResourceType resourceType = TranslateTextureType[textureTypeIndex];
    uint32 resourceTypeIndex = underlyingType(resourceType);

    assert( resourceTypeIndex < underlyingType(ResourceType::Count) );
    uint32 binding = bindingOfResourceType[resourceTypeIndex];
    if (binding == 0xFFFFFFFF) // unlikely
    {
        logError("Textures of type %u are not part of this descriptor set!\n", textureTypeIndex);
        return;
    }

    uint32 slot = _slot;

    if (!resourceGroupIsDescriptorsArray[resourceTypeIndex])
    {
        binding += slot;
        slot = 0;
    }

    VkDescriptorImageInfo imageInfo;
    imageInfo.sampler     = VK_NULL_HANDLE;
    imageInfo.imageView   = src.handle;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // TODO: This could be precomputed and stored in TextureView as a bool
    if ( isDepth(src.viewFormat) ||
         isStencil(src.viewFormat) ||
         isDepthStencil(src.viewFormat) )
    {
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    }

    VkWriteDescriptorSet writeDesc;
    writeDesc.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDesc.pNext            = nullptr;
    writeDesc.dstSet           = handle;
    writeDesc.dstBinding       = binding;
    writeDesc.dstArrayElement  = slot;
    writeDesc.descriptorCount  = 1U;
    writeDesc.descriptorType   = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE; // Texture
    writeDesc.pImageInfo       = &imageInfo;  // Array of image descriptors
    writeDesc.pBufferInfo      = nullptr; 
    writeDesc.pTexelBufferView = nullptr;     // Texel Buffers are not supported

    ValidateNoRet( gpu, vkUpdateDescriptorSets(gpu->device, 1, &writeDesc, 0, nullptr) )
}

   
// DESCRIPTOR POOL
//////////////////////////////////////////////////////////////////////////


DescriptorsVK::DescriptorsVK(
        VulkanDevice* _gpu, 
        VkDescriptorPool _handle) :
    gpu(_gpu),
    handle(_handle)
{
}

DescriptorsVK::~DescriptorsVK()
{
    ValidateNoRet( gpu, vkDestroyDescriptorPool(gpu->device, handle, nullptr) )
}

DescriptorSet* DescriptorsVK::allocate(const SetLayout& layout)
{
    DescriptorSetVK* result = nullptr;

    const SetLayoutVK& srcLayout = reinterpret_cast<const SetLayoutVK&>(layout);

    VkDescriptorSetAllocateInfo allocInfo;
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext              = nullptr;
    allocInfo.descriptorPool     = handle;
    allocInfo.descriptorSetCount = 1u;
    allocInfo.pSetLayouts        = &srcLayout.handle;

    VkDescriptorSet set = VK_NULL_HANDLE;

    Validate( gpu, vkAllocateDescriptorSets(gpu->device, &allocInfo, &set) )
    if (gpu->lastResult[currentThreadId()] == VK_SUCCESS)
    {
        result = new DescriptorSetVK(this, set, srcLayout.bindingOfResourceType, srcLayout.resourceGroupIsDescriptorsArray);
    }

    return result;
}
   
bool DescriptorsVK::allocate(
    const uint32 count,
    const SetLayout*(&layouts)[],
    DescriptorSet**& sets)
{
    bool result = false;
   
    // This is the place where OOD of Rendering Abstraction is unwanted, and DOD would match better.

    // Pack layouts
    VkDescriptorSet*       setHandle    = new VkDescriptorSet[count];
    VkDescriptorSetLayout* layoutHandle = new VkDescriptorSetLayout[count];
    assert( setHandle );
    assert( layoutHandle );
    for(uint32 i=0; i<count; ++i)
    {
        const SetLayoutVK* layout = reinterpret_cast<const SetLayoutVK*>(layouts[i]);
        assert( layout );

        layoutHandle[i] = layout->handle;
        setHandle[i]    = VK_NULL_HANDLE;
    }
   
    VkDescriptorSetAllocateInfo allocInfo;
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext              = nullptr;
    allocInfo.descriptorPool     = handle;
    allocInfo.descriptorSetCount = count;
    allocInfo.pSetLayouts        = layoutHandle;

    Validate( gpu, vkAllocateDescriptorSets(gpu->device, &allocInfo, setHandle) )
    if (gpu->lastResult[currentThreadId()] == VK_SUCCESS)
    {
        // Unpack results
        sets = new DescriptorSet*[count];
        for(uint32 i=0; i<count; ++i)
        {
            const SetLayoutVK* layout = reinterpret_cast<const SetLayoutVK*>(layouts[i]);
            assert( layout );

            sets[i] = new DescriptorSetVK(this, setHandle[i], layout->bindingOfResourceType, layout->resourceGroupIsDescriptorsArray);
        }

        result = true;
    }

    delete [] setHandle;
    delete [] layoutHandle;
   
    return result;
}

//void DescriptorsVK::free(void)
//{
// This would break dependency with child DescriptorSets
// vkResetDescriptorPool
//}


// COMMAND BUFFER
//////////////////////////////////////////////////////////////////////////


void CommandBufferVK::setDescriptors(
    const PipelineLayout& _layout,
    const DescriptorSet& _set,
    const uint32 index)
{
    assert( started );

    const PipelineLayoutVK& layout = reinterpret_cast<const PipelineLayoutVK&>(_layout);
    const DescriptorSetVK&  set    = reinterpret_cast<const DescriptorSetVK&>(_set);

    VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // TODO: Support Compute!!!
    //if (!ptr->graphics)
    //   bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

    // TODO: Support for Dynamic Offsets - offsets in Dynamic Uniform/Storage buffers
    ValidateNoRet( gpu, vkCmdBindDescriptorSets(handle, bindPoint, layout.handle, index, 1, &set.handle, 0, nullptr) )
}

void CommandBufferVK::setDescriptors(
    const PipelineLayout& _layout,
    const uint32 count,
    const DescriptorSet*(&sets)[],
    const uint32 firstIndex)
{
    assert( started );
    assert( count );

    const PipelineLayoutVK& layout = reinterpret_cast<const PipelineLayoutVK&>(_layout);

    // Pack sets handles
    VkDescriptorSet* setHandles = new VkDescriptorSet[count];
    assert( setHandles );
    for(uint32 i=0; i<count; ++i)
    {
        const DescriptorSetVK* set = reinterpret_cast<const DescriptorSetVK*>(sets[i]);
        assert( set );
        setHandles[i] = set->handle;
    }

    VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // TODO: Support Compute!!!
    //if (!ptr->graphics)
    //   bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

    // TODO: Support for Dynamic Offsets - offsets in Dynamic Uniform/Storage buffers
    ValidateNoRet( gpu, vkCmdBindDescriptorSets(handle, bindPoint, layout.handle, firstIndex, count, setHandles, 0, nullptr) )

    delete [] setHandles;
}


// DEVICE
//////////////////////////////////////////////////////////////////////////

bool validateLayout(const uint32 count, const ResourceGroup* group)
{
    if (count == 0)
    {
        logError("Invalid parameter passed in: count!\n");
        assert(count);

        return false;
    }

    if (!group)
    {
        logError("Invalid parameter passed in: group!\n");
        assert(group);

        return false;
    }

}

bool VulkanDevice::validateLayoutIsSupported(const uint32 count, const ResourceGroup* group)
{
    uint32 descriptorsCount = 0;
    for (uint32 i = 0; i < count; ++i)
    {
        descriptorsCount += group[i].count;
    }

    if (descriptorsCount > properties.limits.maxPerStageResources)
    {
        logError("Requested layout with too many descriptors: %u total per-stage supported: %u!\n", descriptorsCount, properties.limits.maxPerStageResources);
        return false;
    }

    // Multiple resource types can be backed by the same descriptor type
    uint32 textureDescriptorsCount = 0;
    for (uint32 i = 0; i < count; ++i)
    {
        if (TranslateResourceType[underlyingType(group[i].type)] == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
        {
            textureDescriptorsCount += group[i].count;
        }
    }

    if (textureDescriptorsCount > properties.limits.maxDescriptorSetSampledImages)
    {
        logError("Requested layout with too many texture descriptors: %u supported: %u!\n", textureDescriptorsCount, properties.limits.maxDescriptorSetSampledImages);
        return false;
    }

    // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
    // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
    // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
    if (textureDescriptorsCount > properties.limits.maxPerStageDescriptorSampledImages)
    {
        logError("Requested layout with too many texture descriptors: %u per-stage supported: %u!\n", textureDescriptorsCount, properties.limits.maxPerStageDescriptorSampledImages);
        return false;
    }

    for (uint32 i = 0; i < count; ++i)
    {
        // Check if requested descriptors count is supported by HW
        if (group[i].type == ResourceType::Sampler &&
            group[i].count > properties.limits.maxDescriptorSetSamplers)
        {
            logError("Requested layout with too many sampler descriptors: %u supported: %u!\n", group[i].count, properties.limits.maxDescriptorSetSamplers);
            return false;
        }

        if (group[i].type == ResourceType::UniformBuffer &&
            group[i].count > properties.limits.maxDescriptorSetUniformBuffers)
        {
            logError("Requested layout with too many uniform descriptors: %u supported: %u!", group[i].count, properties.limits.maxDescriptorSetUniformBuffers);
            return false;
        }
/*
        if (group[i].type == ResourceType::Image &&
            group[i].count > properties.limits.maxDescriptorSetStorageImages)
        {
            logError("Requested layout with too many image descriptors: %u supported: %u!", group[i].count, properties.limits.maxDescriptorSetStorageImages);
            return false;
        }
//*/
        if (group[i].type == ResourceType::StorageBuffer &&
            group[i].count > properties.limits.maxDescriptorSetStorageBuffers)
        {
            logError("Requested layout with too many storage descriptors: %u supported: %u!", group[i].count, properties.limits.maxDescriptorSetStorageBuffers);
            return false;
        }

        // Check if requested descriptors count is supported by pipeline stages (if only this set is bound)
        // VK_DESCRIPTOR_TYPE_SAMPLER
        // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
        if (group[i].type == ResourceType::Sampler &&
            group[i].count > properties.limits.maxPerStageDescriptorSamplers)
        {
            logError("Requested layout with too many sampler descriptors: %u per-stage supported: %u!\n", group[i].count, properties.limits.maxPerStageDescriptorSamplers);
            return false;
        }

        // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
        // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
        if (group[i].type == ResourceType::UniformBuffer &&
            group[i].count > properties.limits.maxPerStageDescriptorUniformBuffers)
        {
            logError("Requested layout with too many uniform descriptors: %u per-stage supported: %u!\n", group[i].count, properties.limits.maxPerStageDescriptorUniformBuffers);
            return false;
        }
/*
        // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
        // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
        if (group[i].type == ResourceType::Image &&
            group[i].count > properties.limits.maxPerStageDescriptorStorageImages)
        {
            logError("Requested layout with too many image descriptors: %u per-stage supported: %u!\n", group[i].count, properties.limits.maxPerStageDescriptorStorageImages);
            return false;
        }
//*/
        // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
        // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
        if (group[i].type == ResourceType::StorageBuffer &&
            group[i].count > properties.limits.maxPerStageDescriptorStorageBuffers)
        {
            logError("Requested layout with too many storage descriptors: %u per-stage supported: %u!\n", group[i].count, properties.limits.maxPerStageDescriptorStorageBuffers);
            return false;
        }
    }

    return true;
}

SetLayout* VulkanDevice::createSetLayout(
    const uint32 count, 
    const ResourceGroup* group,
    const ShaderStages stageMask)
{
    // Check if requested descriptors count is supported by HW
    if (!validateLayout(count, group) ||
        !validateLayoutIsSupported(count, group))
    {
        return nullptr;
    }

    // TODO: Those slots are numerated separately per each resource type in D3D12, but have shared binding pool in Vulkan.
    //       Verify if D3D12 implementation is consistent with that.
    //       See this: https://msdn.microsoft.com/en-us/library/windows/desktop/dn899207(v=vs.85).aspx

    // Builds array mapping resource type to binding index. 
    // Compacts multiple groups of the same resource type 
    // (and size of 1) into group of bindings.
    const uint32 resourceTypesCount = underlyingType(ResourceType::Count);
    uint32 bindingOfResourceType[resourceTypesCount];
    bool   resourceGroupIsDescriptorsArray[resourceTypesCount];
    for(uint32 i=0; i<resourceTypesCount; ++i)
    {
        bindingOfResourceType[i]           = 0xFFFFFFFF;
        resourceGroupIsDescriptorsArray[i] = false;
    }

    VkDescriptorSetLayoutBinding* rangeInfo = new VkDescriptorSetLayoutBinding[count];
    for(uint32 i=0; i<count; ++i)
    {
        uint32 resourceTypeIndex = underlyingType(group[i].type);
        if (bindingOfResourceType[resourceTypeIndex] == 0xFFFFFFFF)
        {
            // This resource type is encountered first time. It's binding is cached
            // together with information if its array of descriptors, or array of bindings.
            bindingOfResourceType[resourceTypeIndex] = i;
            if (group[i].count > 1)
            {
                resourceGroupIsDescriptorsArray[i] = true;
            }
        }
        else 
        {
            // This resource type was already encountered in resource groups array.
            // This is only allowed, if its consecutive group of the same type, of
            // size of one (so its group of singular bindings).
            assert( i > 0 );
            if (group[i].type != group[i-1].type)
            {
                logError("Descriptors set layout with multiple unrelated groups of the same type is not supported!\n Group %u redefines type %u!\n", i, underlyingType(group[i].type));
                return nullptr;
            }
            if (group[i].count > 1)
            {
                logError("Descriptors set layout with multiple groups of the same type but different sizes is not supported!\n Group %u redefines type %u with size %u but previous group of the same type was of size 1!\n", i, underlyingType(group[i].type), group[i].count);
                return nullptr;
            }
        }
 
        // Single Descriptors range
        rangeInfo[i].binding            = i;
        rangeInfo[i].descriptorType     = TranslateResourceType[resourceTypeIndex];
        rangeInfo[i].descriptorCount    = group[i].count;
        rangeInfo[i].stageFlags         = static_cast<VkShaderStageFlagBits>(underlyingType(stageMask));
        rangeInfo[i].pImmutableSamplers = nullptr;
    }

    // Descriptor Ranges Table
    VkDescriptorSetLayoutCreateInfo setInfo;
    setInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    setInfo.pNext        = nullptr;
    setInfo.flags        = 0u;      // VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR to push Descriptors update using vkCmdPushDescriptorSetKHR
    setInfo.bindingCount = count;
    setInfo.pBindings    = rangeInfo;

    SetLayoutVK* result = nullptr;
    VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;

    Validate( this, vkCreateDescriptorSetLayout(device, &setInfo, nullptr, &setLayout) )
    if (lastResult[currentThreadId()] == VK_SUCCESS)
    {
        result = new SetLayoutVK(this, setLayout, bindingOfResourceType, resourceGroupIsDescriptorsArray);
    }

    delete rangeInfo;

    return result;
}

PipelineLayout* VulkanDevice::createPipelineLayout(
    const uint32      setsCount,
    const SetLayout** sets,
    const uint32      immutableSamplersCount,
    const Sampler**   immutableSamplers,
    const ShaderStages stagesMask)
{
    PipelineLayoutVK* result = nullptr; 

    // We gather all Immutable Samplers into additional Descriptor Set to match Direct3D12 behavior.
    uint32 totalSets = setsCount;
    if (immutableSamplersCount > 0)
    {
        totalSets++;
    }
      
    // Gather descriptor set layouts
    VkDescriptorSetLayout* setsLayouts = nullptr;
    if (totalSets)
    {
        setsLayouts = new VkDescriptorSetLayout[totalSets];
    }

    for(uint32 i=0; i<setsCount; ++i)
    {
        const SetLayoutVK* set = reinterpret_cast<const SetLayoutVK*>(sets[i]);
        assert( set );
        setsLayouts[i] = set->handle;
    }
      
    // Create additional Descriptor Set for Immutable Samplers
    if (immutableSamplersCount > 0)
    {
        // Vulkan requires already created samplers to be passed as immutable.
        // D3D12 requires sampler states. We could emulate that by storing in
        // each sampler object in D3D12 backend it's original SamplerState.
      
        // Gather Sampler handles
        VkSampler* immutable = new VkSampler[immutableSamplersCount];
        for(uint32 i=0; i< setsCount; ++i)
        {
            const SamplerVK* sampler = reinterpret_cast<const SamplerVK*>(immutableSamplers[i]);
            assert( sampler );
            immutable[i] = sampler->handle;
        }
      
        // Single Descriptors range
        VkDescriptorSetLayoutBinding rangeInfo;
        rangeInfo.binding            = 0u; // TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        rangeInfo.descriptorType     = VK_DESCRIPTOR_TYPE_SAMPLER;
        rangeInfo.descriptorCount    = immutableSamplersCount;
        rangeInfo.stageFlags         = static_cast<VkShaderStageFlagBits>(underlyingType(stagesMask));
        rangeInfo.pImmutableSamplers = immutable;

        // Descriptor Range Table
        VkDescriptorSetLayoutCreateInfo setInfo;
        setInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setInfo.pNext        = nullptr;
        setInfo.flags        = 0u;      // Reserved for future
        setInfo.bindingCount = 1u;
        setInfo.pBindings    = &rangeInfo;
   
        // Create additional Set and store it immediately in the Sets array
        // TODO: Keep pointer of this set in the layout as well! Otherwise we will leak it!!!!!!!!!!!!!!!!!!!!!!!!
        Validate( this, vkCreateDescriptorSetLayout(device, &setInfo, nullptr, &setsLayouts[setsCount]) )
        assert( lastResult[currentThreadId()] == VK_SUCCESS );
    }

    // TODO: Push Constants

    //typedef struct VkPushConstantRange {
    //VkShaderStageFlags stageFlags;
    //uint32_t offset;
    //uint32_t size;
    //} VkPushConstantRange;

    VkPipelineLayoutCreateInfo layoutInfo;
    layoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.pNext                  = nullptr;
    layoutInfo.flags                  = 0u;      // Reserved for future
    layoutInfo.setLayoutCount         = totalSets;
    layoutInfo.pSetLayouts            = setsLayouts;
    layoutInfo.pushConstantRangeCount = 0;       // uint32_t
    layoutInfo.pPushConstantRanges    = nullptr; // const VkPushConstantRange*

    VkPipelineLayout layout = VK_NULL_HANDLE;
    Validate( this, vkCreatePipelineLayout(device, &layoutInfo, nullptr, &layout) )
    if (lastResult[currentThreadId()] == VK_SUCCESS)
    {
        result = new PipelineLayoutVK(this, layout);
    }

    delete [] setsLayouts;

    return result;
}

DescriptorsPool* VulkanDevice::createDescriptorsPool(
    const uint32 count,
    const ResourceGroup* group)
{
    // Calculates amount of descriptors needed per each distinct 
    // Vulkan backing descriptor type (as different resource types
    // may end up being backed by the same descriptor types).
    uint32 descriptorsCountPerType[VK_DESCRIPTOR_TYPE_RANGE_SIZE] = {};
    for(uint32 i=0; i<count; ++i)
    {
        VkDescriptorType type = TranslateResourceType[underlyingType(group[i].type)];
        descriptorsCountPerType[type] += group[i].count;
    }

    uint32 descriptorGroupsCount = 0;
    for (uint32 i=0; i<VK_DESCRIPTOR_TYPE_RANGE_SIZE; ++i)
    {
        if (descriptorsCountPerType[i])
        {
            descriptorGroupsCount++;
        }
    }

    if (descriptorGroupsCount == 0)
    {
        logError("No descriptor groups requested to be allocated!\n");
        return nullptr;
    }

    VkDescriptorPoolSize* ranges = new VkDescriptorPoolSize[descriptorGroupsCount];
    uint32 rangeIndex = 0;
    for(uint32 i=0; i< VK_DESCRIPTOR_TYPE_RANGE_SIZE; ++i)
    {
        if (descriptorsCountPerType[i])
        {
            ranges[rangeIndex].type            = (VkDescriptorType)i;
            ranges[rangeIndex].descriptorCount = descriptorsCountPerType[i];

            rangeIndex++;
        }
    }

    // It is assumed application will never create more than 1024 descriptor sets at the same time.
    // If needed, this limit can be changed, or made controllable by boot-time config variable.
    uint32 maxConcurrentDescriptorSetsCount = 1024;

    VkDescriptorPoolCreateInfo poolInfo;
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pNext         = nullptr;
    poolInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // Allow freeing and re-allocation of descriptor sets. May lead to Pool fragmentation.
    poolInfo.maxSets       = maxConcurrentDescriptorSetsCount;
    poolInfo.poolSizeCount = descriptorGroupsCount;
    poolInfo.pPoolSizes    = ranges;

    DescriptorsVK* result = nullptr;
    VkDescriptorPool handle = VK_NULL_HANDLE;
    Validate( this, vkCreateDescriptorPool(device, &poolInfo, nullptr, &handle) )
    if (lastResult[currentThreadId()] == VK_SUCCESS)
    {
        result = new DescriptorsVK(this, handle);
    }

    delete ranges;

    return result;
}

} // en::gpu
} // en

#endif
