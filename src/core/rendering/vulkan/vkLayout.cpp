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






//typedef enum D3D12_SHADER_VISIBILITY
//{
// D3D12_SHADER_VISIBILITY_ALL    = 0,
// D3D12_SHADER_VISIBILITY_VERTEX = 1,
// D3D12_SHADER_VISIBILITY_HULL   = 2,
// D3D12_SHADER_VISIBILITY_DOMAIN = 3,
// D3D12_SHADER_VISIBILITY_GEOMETRY   = 4,
// D3D12_SHADER_VISIBILITY_PIXEL  = 5
//} D3D12_SHADER_VISIBILITY;

// VkShaderStageFlags


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


   Ptr<SetLayout> VulkanDevice::createSetLayout(const uint32 count, const Resources* group)
   {
   Ptr<SetLayoutVK> result = nullptr;

   // Current assigned HLSL slot for each resource type
   uint32 types = underlyingType(ResourceType::Count);
   uint32* slot = new uint32[types];
   for(uint32 i=0; i<types; ++i)
      slot[i] = 0u;
      
   VkDescriptorSetLayoutBinding* rangeInfo = new VkDescriptorSetLayoutBinding[count];
   for(uint32 i=0; i<count; ++i)
      {
      // Single Descriptors range
      uint32 resourceType = underlyingType(group[i].type);
      rangeInfo[i].binding            = slot[resourceType];
      rangeInfo[i].descriptorType     = TranslateResourceType[resourceType];
      rangeInfo[i].descriptorCount    = group[i].count;
//VkShaderStageFlags stageFlags;
      rangeInfo[i].pImmutableSamplers = nullptr; // const VkSampler*

      // Increase index by amount of slots used
      slot[resourceType] += group[i].count;
      }

   // Descriptor Ranges Table
   VkDescriptorSetLayoutCreateInfo setInfo;
   setInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
   setInfo.pNext        = nullptr;
   setInfo.flags        = 0u;      // Reserved for future
   setInfo.bindingCount = count;
   setInfo.pBindings    = rangeInfo;
   
   VkDescriptorSetLayout setLayout;
      
   Profile( this, vkCreateDescriptorSetLayout(device, &setInfo, nullptr, &setLayout) )
   if (lastResult[Scheduler.core()] == VK_SUCCESS)
      {
      result = new SetLayoutVK(this, setLayout);
      }
    
   delete [] slot;
  
   return ptr_reinterpret_cast<SetLayout>(&result);
   }

   Ptr<PipelineLayout> VulkanDevice::createPipelineLayout(const uint32 sets,
                                                          const Ptr<SetLayout>* set,
                                                          const uint32 immutableSamplers,
                                                          const Ptr<Sampler>* sampler)
   {
   Ptr<PipelineLayoutVK> result = nullptr; 

   // We gather all Immutable Samplers into additional Descriptor Set to match Direct3D12 behavior.
   uint32 totalSets = sets;
   if (immutableSamplers > 0)
      totalSets++;
      
   // Gather descriptor set layouts
   VkDescriptorSetLayout* setsLayouts = new VkDescriptorSetLayout[totalSets];
   for(uint32 i=0; i<sets; ++i)
      {
      SetLayoutVK* ptr = raw_reinterpret_cast<SetLayoutVK>(&set[i]);
      setsLayouts[i] = ptr->handle;
      }
      
   // Create additional Descriptor Set for Immutable Samplers
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
      rangeInfo.binding            = ;
      rangeInfo.descriptorType     = VK_DESCRIPTOR_TYPE_SAMPLER;
      rangeInfo.descriptorCount    = immutableSamplers;
//VkShaderStageFlags stageFlags;  // TODO: What about ShaderStages for Immutable Samplers ?
      rangeInfo.pImmutableSamplers = immutable;

      // Descriptor Range Table
      VkDescriptorSetLayoutCreateInfo setInfo;
      setInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      setInfo.pNext        = nullptr;
      setInfo.flags        = 0u;      // Reserved for future
      setInfo.bindingCount = 1u;
      setInfo.pBindings    = &rangeInfo;
   
      // Create additional Set and store it immediately in the Sets array
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
   layoutInfo.setLayoutCount         = sets;
   layoutInfo.pSetLayouts            = setsLayouts;
   layoutInfo.pushConstantRangeCount = 0;       // uint32_t
   layoutInfo.pPushConstantRanges    = nullptr; // const VkPushConstantRange*

   VkPipelineLayout layout;
   Profile( this, vkCreatePipelineLayout(device, &layoutInfo, nullptr, &layout) )
   if (lastResult[Scheduler.core()] == VK_SUCCESS)
      {
      result = new PipelineLayoutVK(gpu, layout));
      }

   delete [] setsLayouts;

   return ptr_reinterpret_cast<PipelineLayout>(&result);
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
//   // VK_DESCRIPTOR_ TYPE_COMBINED_IMAGE_SAMPLER
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









   }
}
#endif
