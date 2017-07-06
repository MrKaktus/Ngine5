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

#include "core/rendering/vulkan/vkDevice.h"
#include "core/rendering/vulkan/vkCommandBuffer.h"
#include "core/rendering/vulkan/vkBuffer.h"
#include "core/rendering/vulkan/vkSampler.h"
#include "core/rendering/vulkan/vkTexture.h"

namespace en
{
   namespace gpu
   {
   static const VkDescriptorType TranslateResourceType[underlyingType(ResourceType::Count)] =
      {
      VK_DESCRIPTOR_TYPE_SAMPLER,          // Sampler
      VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,    // Texture
      VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,    // Image
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,   // Uniform
      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,   // Storage
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

   SetLayoutVK::SetLayoutVK(VulkanDevice* _gpu, VkDescriptorSetLayout _handle) :
      gpu(_gpu),
      handle(_handle)
   {
   }

   SetLayoutVK::~SetLayoutVK()
   {
   ProfileNoRet( gpu, vkDestroyDescriptorSetLayout(gpu->device, handle, nullptr) )
   }

   PipelineLayoutVK::PipelineLayoutVK(VulkanDevice* _gpu, VkPipelineLayout _handle) :
      gpu(_gpu),
      handle(_handle)
   {
   }

   PipelineLayoutVK::~PipelineLayoutVK()
   {
   ProfileNoRet( gpu, vkDestroyPipelineLayout(gpu->device, handle, nullptr) )
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
//   Ptr<Layout> result = nullptr;
//   
//   uint32 descriptorSets = 0; // <= VkPhysicalDeviceLimits::maxPerStageDescriptorSamplers
//   
//   
//	typedef struct {
//	    VkShaderStageFlags                          stageFlags;
//	    uint32_t                                    offset;
//	    uint32_t                                    size; // <= VkPhysicalDeviceLimits::maxPushConstantsSize - offset
//	} VkPushConstantRange;
//   
//   // total accessible to any given shader stage across all elements of pSetLayouts:
//   
//   // VK_DESCRIPTOR_TYPE_SAMPLER
//   // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
//   // <= VkPhysicalDeviceLimits::maxPerStageDescriptorSamplers
//   
//   // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
//   // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
//   // <= VkPhysicalDeviceLimits::maxPerStageDescriptorUniformBuffers
//   
//   // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
//   // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
//   // <= VkPhysicalDeviceLimits::maxPerStageDescriptorStorageBuffers
//   
//   // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
//   // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
//   // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
//   // <= VkPhysicalDeviceLimits::maxPerStageDescriptorSampledImages
//   
//   // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
//   // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
//   // <= VkPhysicalDeviceLimits::maxPerStageDescriptorStorageImages
//  

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


   DescriptorSetVK::DescriptorSetVK(Ptr<DescriptorsVK> _parent, VkDescriptorSet _handle) :
      parent(_parent),
      handle(_handle)
   {
   }

   DescriptorSetVK::~DescriptorSetVK()
   {
   Profile( parent->gpu, vkFreeDescriptorSets(parent->gpu->device, parent->handle, 1u, &handle) )
   parent = nullptr;
   }
   
   // TODO: Update DescriptorSets with batches of resources (or cache updates)
 
   //VkWriteDescriptorSet writeDesc;
   //writeDesc.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   //writeDesc.pNext           = nullptr;
   //writeDesc.dstSet          = handle;
   //writeDesc.dstBinding      = slot;
   //uint32_t                         dstArrayElement;
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



   void DescriptorSetVK::setBuffer(const uint32 slot, const Ptr<Buffer> _buffer)
   {
   assert( _buffer );

   VulkanDevice* gpu = parent->gpu;

   BufferVK* src = raw_reinterpret_cast<BufferVK>(&_buffer);

   assert( src->apiType == BufferType::Uniform ||
           src->apiType == BufferType::Storage );

   VkDescriptorBufferInfo bufferInfo;
   bufferInfo.buffer = src->handle;
   bufferInfo.offset = 0U;
   bufferInfo.range  = src->size;

   VkWriteDescriptorSet writeDesc;
   writeDesc.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   writeDesc.pNext            = nullptr;
   writeDesc.dstSet           = handle;
   writeDesc.dstBinding       = slot;
   writeDesc.dstArrayElement  = 0U; // Starting element in Array, looks like arrays have separate indexing from global namespace?
   writeDesc.descriptorCount  = 1U;
   writeDesc.descriptorType   = src->apiType == BufferType::Uniform ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
   writeDesc.pImageInfo       = nullptr;
   writeDesc.pBufferInfo      = &bufferInfo; // Array of buffer descriptors
   writeDesc.pTexelBufferView = nullptr;     // Texel Buffers are not supported

   ProfileNoRet( gpu, vkUpdateDescriptorSets(gpu->device, 1, &writeDesc, 0, nullptr) )
   }

   void DescriptorSetVK::setSampler(const uint32 slot, const Ptr<Sampler> _sampler)
   {
   assert( _sampler );

   VulkanDevice* gpu = parent->gpu;

   SamplerVK* src = raw_reinterpret_cast<SamplerVK>(&_sampler);

   VkDescriptorImageInfo imageInfo;
   imageInfo.sampler     = src->handle;
   imageInfo.imageView   = VK_NULL_HANDLE;
   imageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Ommited for Samplers

   VkWriteDescriptorSet writeDesc;
   writeDesc.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   writeDesc.pNext            = nullptr;
   writeDesc.dstSet           = handle;
   writeDesc.dstBinding       = slot;
   writeDesc.dstArrayElement  = 0U; // Starting element in Array, looks like arrays have separate indexing from global namespace?
   writeDesc.descriptorCount  = 1U;
   writeDesc.descriptorType   = VK_DESCRIPTOR_TYPE_SAMPLER; // Sampler
   writeDesc.pImageInfo       = &imageInfo;  // Array of image descriptors
   writeDesc.pBufferInfo      = nullptr; 
   writeDesc.pTexelBufferView = nullptr;     // Texel Buffers are not supported

   ProfileNoRet( gpu, vkUpdateDescriptorSets(gpu->device, 1, &writeDesc, 0, nullptr) )
   }

   void DescriptorSetVK::setTextureView(const uint32 slot, const Ptr<TextureView> _view)
   {
   assert( _view );

   VulkanDevice* gpu = parent->gpu;

   TextureViewVK* src = raw_reinterpret_cast<TextureViewVK>(&_view);

   VkDescriptorImageInfo imageInfo;
   imageInfo.sampler     = VK_NULL_HANDLE;
   imageInfo.imageView   = src->handle;
   imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

   // TODO: This could be precomputed and stored in TextureView as a bool
   if ( TextureFormatIsDepth(src->viewFormat) ||
        TextureFormatIsStencil(src->viewFormat) ||
        TextureFormatIsDepthStencil(src->viewFormat) )
       imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

   VkWriteDescriptorSet writeDesc;
   writeDesc.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
   writeDesc.pNext            = nullptr;
   writeDesc.dstSet           = handle;
   writeDesc.dstBinding       = slot;
   writeDesc.dstArrayElement  = 0U; // Starting element in Array, looks like arrays have separate indexing from global namespace?
   writeDesc.descriptorCount  = 1U;
   writeDesc.descriptorType   = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE; // Texture
   writeDesc.pImageInfo       = &imageInfo;  // Array of image descriptors
   writeDesc.pBufferInfo      = nullptr; 
   writeDesc.pTexelBufferView = nullptr;     // Texel Buffers are not supported

   ProfileNoRet( gpu, vkUpdateDescriptorSets(gpu->device, 1, &writeDesc, 0, nullptr) )
   }

   
   // DESCRIPTOR POOL
   //////////////////////////////////////////////////////////////////////////


   DescriptorsVK::DescriptorsVK(VulkanDevice* _gpu, VkDescriptorPool _handle) :
      gpu(_gpu),
      handle(_handle)
   {
   }

   DescriptorsVK::~DescriptorsVK()
   {
   ProfileNoRet( gpu, vkDestroyDescriptorPool(gpu->device, handle, nullptr) )
   }

   Ptr<DescriptorSet> DescriptorsVK::allocate(const Ptr<SetLayout> layout)
   {
   Ptr<DescriptorSetVK> result = nullptr;

   VkDescriptorSetAllocateInfo allocInfo;
   allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
   allocInfo.pNext              = nullptr;
   allocInfo.descriptorPool     = handle;
   allocInfo.descriptorSetCount = 1u;
   allocInfo.pSetLayouts        = &raw_reinterpret_cast<SetLayoutVK>(&layout)->handle;

   VkDescriptorSet set = VK_NULL_HANDLE;

   Profile( gpu, vkAllocateDescriptorSets(gpu->device, &allocInfo, &set) )
   if (gpu->lastResult[Scheduler.core()] == VK_SUCCESS)
      result = new DescriptorSetVK(this, set);

   return ptr_reinterpret_cast<DescriptorSet>(&result);
   }
   
   bool DescriptorsVK::allocate(const uint32 count, const Ptr<SetLayout>* layouts, Ptr<DescriptorSet>** sets)
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
      SetLayoutVK* ptr = raw_reinterpret_cast<SetLayoutVK>(&layouts[i]);
      layoutHandle[i] = ptr->handle;
      setHandle[i]    = VK_NULL_HANDLE;
      }
   
   VkDescriptorSetAllocateInfo allocInfo;
   allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
   allocInfo.pNext              = nullptr;
   allocInfo.descriptorPool     = handle;
   allocInfo.descriptorSetCount = count;
   allocInfo.pSetLayouts        = layoutHandle;

   Profile( gpu, vkAllocateDescriptorSets(gpu->device, &allocInfo, setHandle) )
   if (gpu->lastResult[Scheduler.core()] == VK_SUCCESS)
      {
      // Unpack results
      *sets = new Ptr<DescriptorSet>[count];
      for(uint32 i=0; i<count; ++i)
         {
         Ptr<DescriptorSetVK> ptr = new DescriptorSetVK(this, setHandle[i]);
         (*sets)[i] = raw_reinterpret_cast<DescriptorSet>(&ptr);
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


   void CommandBufferVK::setDescriptors(const Ptr<PipelineLayout> _layout, 
                                        const Ptr<DescriptorSet> _set,
                                        const uint32 index)
   {
   assert( started );
   assert( _layout );
   assert( _set );

   PipelineLayoutVK* layout = raw_reinterpret_cast<PipelineLayoutVK>(&_layout);
   DescriptorSetVK*  set    = raw_reinterpret_cast<DescriptorSetVK>(&_set);

   VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

   // TODO: Support Compute!!!
   //if (!ptr->graphics)
   //   bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

   // TODO: Support for Dynamic Offsets - offsets in Dynamic Uniform/Storage buffers
   ProfileNoRet( gpu, vkCmdBindDescriptorSets(handle, bindPoint, layout->handle, index, 1, &set->handle, 0, nullptr) )
   }

   void CommandBufferVK::setDescriptors(const Ptr<PipelineLayout> _layout, 
                                        const uint32 count,
                                        const Ptr<DescriptorSet>* sets,
                                        const uint32 firstIndex)
   {
   assert( started );
   assert( _layout );
   assert( count );
   assert( sets );

   PipelineLayoutVK* layout = raw_reinterpret_cast<PipelineLayoutVK>(&_layout);

   // Pack sets handles
   VkDescriptorSet* setHandles = new VkDescriptorSet[count];
   assert( setHandles );
   for(uint32 i=0; i<count; ++i)
      {
      DescriptorSetVK* ptr = raw_reinterpret_cast<DescriptorSetVK>(&sets[i]);
      setHandles[i] = ptr->handle;
      }

   VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

   // TODO: Support Compute!!!
   //if (!ptr->graphics)
   //   bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

   // TODO: Support for Dynamic Offsets - offsets in Dynamic Uniform/Storage buffers
   ProfileNoRet( gpu, vkCmdBindDescriptorSets(handle, bindPoint, layout->handle, firstIndex, count, setHandles, 0, nullptr) )

   delete [] setHandles;
   }


   // DEVICE
   //////////////////////////////////////////////////////////////////////////

      
   Ptr<SetLayout> VulkanDevice::createSetLayout(const uint32 count, 
                                                const ResourceGroup* group,
                                                const ShaderStages stageMask)
   {
   assert( count );
   assert( group );
   
   Ptr<SetLayoutVK> result = nullptr;

   // TODO: Those slots are numerated separately per each resource type in D3D12, but have shared binding pool in Vulkan.
   //       Verify if D3D12 implementation is consistent with that.
   //       See this: https://msdn.microsoft.com/en-us/library/windows/desktop/dn899207(v=vs.85).aspx
   uint32 slot = 0;

   VkDescriptorSetLayoutBinding* rangeInfo = new VkDescriptorSetLayoutBinding[count];
   for(uint32 i=0; i<count; ++i)
      {
      // Single Descriptors range
      uint32 resourceType = underlyingType(group[i].type);
      rangeInfo[i].binding            = slot;
      rangeInfo[i].descriptorType     = TranslateResourceType[resourceType];
      rangeInfo[i].descriptorCount    = group[i].count;
      rangeInfo[i].stageFlags         = static_cast<VkShaderStageFlagBits>(underlyingType(stageMask));
      rangeInfo[i].pImmutableSamplers = nullptr; 

      // Increase index by amount of slots used
      slot += group[i].count;
      }

   // Descriptor Ranges Table
   VkDescriptorSetLayoutCreateInfo setInfo;
   setInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
   setInfo.pNext        = nullptr;
   setInfo.flags        = 0u;      // VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR to push Descriptors update using vkCmdPushDescriptorSetKHR
   setInfo.bindingCount = count;
   setInfo.pBindings    = rangeInfo;
   
   VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;
      
   Profile( this, vkCreateDescriptorSetLayout(device, &setInfo, nullptr, &setLayout) )
   if (lastResult[Scheduler.core()] == VK_SUCCESS)
      {
      result = new SetLayoutVK(this, setLayout);
      }
    
   return ptr_reinterpret_cast<SetLayout>(&result);
   }

   Ptr<PipelineLayout> VulkanDevice::createPipelineLayout(const uint32 sets,
                                                          const Ptr<SetLayout>* set,
                                                          const uint32 immutableSamplers,
                                                          const Ptr<Sampler>* sampler,
                                                          const ShaderStages stageMask)
   {
   Ptr<PipelineLayoutVK> result = nullptr; 

   // We gather all Immutable Samplers into additional Descriptor Set to match Direct3D12 behavior.
   uint32 totalSets = sets;
   if (immutableSamplers > 0)
      totalSets++;
      
   // Gather descriptor set layouts
   VkDescriptorSetLayout* setsLayouts = nullptr;
   if (totalSets)
      setsLayouts = new VkDescriptorSetLayout[totalSets];
   for(uint32 i=0; i<sets; ++i)
      {
      SetLayoutVK* ptr = raw_reinterpret_cast<SetLayoutVK>(&set[i]);
      setsLayouts[i] = ptr->handle;
      }
      
   // Create additional Descriptor Set for Immutable Samplers
   if (immutableSamplers)
      {
      // Vulkan requires already created samplers to be passed as immutable.
      // D3D12 requires sampler states. We could emulate that by storing in
      // each sampler object in D3D12 backend it's original SamplerState.
      
      // Gather Sampler handles
      VkSampler* immutable = new VkSampler[immutableSamplers];
      for(uint32 i=0; i<sets; ++i)
         {
         SamplerVK* ptr = raw_reinterpret_cast<SamplerVK>(&sampler[i]);
         immutable[i] = ptr->handle;
         }
      
      // Single Descriptors range
      VkDescriptorSetLayoutBinding rangeInfo;
      rangeInfo.binding            = 0u; // TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      rangeInfo.descriptorType     = VK_DESCRIPTOR_TYPE_SAMPLER;
      rangeInfo.descriptorCount    = immutableSamplers;
      rangeInfo.stageFlags         = static_cast<VkShaderStageFlagBits>(underlyingType(stageMask));
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
      Profile( this, vkCreateDescriptorSetLayout(device, &setInfo, nullptr, &setsLayouts[sets]) )
      assert( lastResult[Scheduler.core()] == VK_SUCCESS );
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
   Profile( this, vkCreatePipelineLayout(device, &layoutInfo, nullptr, &layout) )
   if (lastResult[Scheduler.core()] == VK_SUCCESS)
      {
      result = new PipelineLayoutVK(this, layout);
      }

   delete [] setsLayouts;

   return ptr_reinterpret_cast<PipelineLayout>(&result);
   }

   Ptr<Descriptors> VulkanDevice::createDescriptorsPool(const uint32 maxSets, const uint32 (&count)[underlyingType(ResourceType::Count)])
   {
   Ptr<DescriptorsVK> result = nullptr;
   
   uint32 resourceTypesCount = underlyingType(ResourceType::Count);
   
   VkDescriptorPoolSize ranges[underlyingType(ResourceType::Count)];
   for(uint32 i=0; i<resourceTypesCount; ++i)
      {
      ranges[i].type            = TranslateResourceType[i];
      ranges[i].descriptorCount = count[i];
      }
      
   VkDescriptorPoolCreateInfo poolInfo;
   poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
   poolInfo.pNext         = nullptr;
   poolInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // Allow freeing and re-allocation of descriptor sets. May lead to Pool fragmentation.
   poolInfo.maxSets       = maxSets;
   poolInfo.poolSizeCount = resourceTypesCount;
   poolInfo.pPoolSizes    = &ranges[0];

   VkDescriptorPool handle = VK_NULL_HANDLE;
   Profile( this, vkCreateDescriptorPool(device, &poolInfo, nullptr, &handle) )
   if (lastResult[Scheduler.core()] == VK_SUCCESS)
      {
      result = new DescriptorsVK(this, handle);
      }

   return ptr_reinterpret_cast<Descriptors>(&result);
   }

   }
}
#endif
