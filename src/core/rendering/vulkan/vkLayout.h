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

      SetLayoutVK(VulkanDevice* gpu, VkDescriptorSetLayout layout);
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

   class DescriptorsVK : public Descriptors
      {
      public:
      VulkanDevice*    gpu;
      VkDescriptorPool handle;
      
      virtual Ptr<DescriptorSet> allocate(const Ptr<SetLayout> layout);
      virtual bool allocate(const uint32 count,
                            const Ptr<SetLayout>* layouts,
                            Ptr<DescriptorSet>* sets);
         
      DescriptorsVK(VulkanDevice* gpu, VkDescriptorPool handle);
      virtual ~DescriptorsVK();
      };

   class DescriptorSetVK : public DescriptorSet
      {
      public:
      Ptr<DescriptorsVK> parent; // Reference to Descriptors Pool
      VkDescriptorSet    handle;
      
      DescriptorSetVK(Ptr<DescriptorsVK> parent, VkDescriptorSet handle);
      virtual ~DescriptorSetVK();
      };
   }
}
#endif

#endif
