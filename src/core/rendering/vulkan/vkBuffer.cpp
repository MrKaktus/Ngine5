/*

 Ngine v5.0
 
 Module      : Blend State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkBuffer.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/vulkan/vkHeap.h"

namespace en
{
   namespace gpu
   {
   BufferVK::BufferVK(VulkanDevice* _gpu, const VkBuffer _handle, const BufferType type, const uint32 size) :
      gpu(_gpu),
      handle(_handle),
      heap(nullptr),
      offset(0u),
      CommonBuffer(type, size)
   {
   }
   
   BufferVK::~BufferVK()
   {
   ProfileNoRet( gpu, vkDestroyBuffer(gpu->device, handle, nullptr) )
   
   // Deallocate from the Heap (let Heap allocator know that memory region is available again)
   heap->allocator->deallocate(offset, memoryRequirements.size);
   heap = nullptr;
   }

   BufferViewVK::BufferViewVK(VulkanDevice* _gpu) :
      gpu(_gpu),
      parent(nullptr),
      handle(nullptr),
      BufferView()
   {
   VkBufferViewCreateInfo viewInfo = {};
   viewInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
   viewInfo.pNext = nullptr;
   viewInfo.flags = 0; // Reserved
   viewInfo. VkBuffer                   buffer;
   viewInfo. VkFormat                   format;
   viewInfo. VkDeviceSize               offset; // multiple of VkPhysicalDeviceLimits::minTexelBufferOffsetAlignment
   viewInfo. VkDeviceSize               range;

   // TODO!
   }
   
   BufferViewVK::~BufferViewVK()
   {
   ProfileNoRet( gpu, vkDestroyBufferView(gpu->device, handle, nullptr) )
   }

   Ptr<Buffer> createBuffer(VulkanDevice* gpu, const BufferType type, const uint32 size)
   {
   Ptr<BufferVK> buffer = nullptr;
   
   assert( size );
   
   VkBufferUsageFlags bufferUsage = 0;
   switch(type)
      {
      case BufferType::Vertex:
         bufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
         break;

      case BufferType::Index:
         bufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
         break;
         
      case BufferType::Uniform:
         bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
         break;

      case BufferType::Storage:
         bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
         break;
         
      case BufferType::Indirect:
         bufferUsage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
         break;

      case BufferType::Transfer:
         // For now universal transfer buffer. In future could specialise to Source and Destination buffers.
         bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
         break;
            
      default:
         assert( 0 );
         break;
      };

   // Need to be set to enable buffer views (linear textures?):
   //
   // VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT
   // VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT
   //
   // Sparse Buffers are not supported yet:
   //
   // VK_BUFFER_CREATE_SPARSE_BINDING_BIT                                         - backed using sparse binding.
   // VK_BUFFER_CREATE_SPARSE_BINDING_BIT | VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT - can be partially backed using sparse binding.
   // VK_BUFFER_CREATE_SPARSE_BINDING_BIT | VK_BUFFER_CREATE_SPARSE_ALIASED_BIT   - backed using sparse binding. may alias with others.
  
   VkBufferCreateInfo bufferInfo = {};
   bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
   bufferInfo.pNext = nullptr;
   bufferInfo.flags = 0;
   bufferInfo.size  = size;
   bufferInfo.usage = bufferUsage;
   
// It's possible that current GPU driver will distinguish different Queue Families
// for different use. For example it can have 10 Rendering Queues in one Family and
// only 1 Transfer Queue in other family. In such case, depending on resource
// usage, it may be required to share it between Families.
// if (queueFamiliesCount > 1)
//    {
//    bufferInfo.sharingMode           = VK_SHARING_MODE_CONCURRENT; // Sharing between multiple Queue Families
//    bufferInfo.queueFamilyIndexCount = queueFamiliesCount;         // Count of Queue Families (for eg.g GPU has 40 Rendering and 10 Compute Queues grouped into 2 Families)
//    bufferInfo.pQueueFamilyIndices   = queueFamilyIndices;         // pQueueFamilyIndices is a list of queue families that will access this buffer
//    }
// else
      {
      bufferInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;  // No sharing between multiple Queue Families
      bufferInfo.queueFamilyIndexCount = 0;
      bufferInfo.pQueueFamilyIndices   = nullptr;
      }
      
   VkBuffer handle;
   Profile( gpu, vkCreateBuffer(gpu->device, &bufferInfo, nullptr, &handle) )
   if (gpu->lastResult[Scheduler.core()] >= 0)
      {
      buffer = new BufferVK(gpu, handle, type, size);
      
      // Query buffer requirements
      ProfileNoRet( gpu, vkGetBufferMemoryRequirements(gpu->device, buffer->handle, &buffer->memoryRequirements) )

      // TODO: Create default Buffer View !
      } 

   return ptr_dynamic_cast<Buffer, BufferVK>(buffer); 
   }
   
   // TODO: ifdef Mobile / or Transient
   // TODO: Is there a way to support it internally by some WA? On Vulkan probably not :/.
   Ptr<Buffer> createBufferAndPopulate(VulkanDevice* gpu, const BufferType type, const uint32 size, const void* data)
   {
   assert( size );
   assert( data );

   }

   }
}
#endif
