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

#include "core/rendering/vulkan/vkDevice.h"
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




   // Only one continous memory range can be mapped for each heap at the same time.
   // Thus to update multiple objects, create separate heaps with staging buffers
   // for each of them.

   // There may be only one heap resource mapped at a time. To update
   // several resources at the same time (for e.g. simultaneous streaming
   // by several threads), create separate heap's for each such update.

   void* BufferVK::map(void)
   {
   return map(0u, size);
   }
   
   void* BufferVK::map(const uint64 _offset, const uint64 _size)
   {
   assert( _offset + _size <= size );
   
   // Buffers can be mapped only on Streamed and Immediate Heaps.
   assert( heap->_usage == MemoryType::Streamed ||
           heap->_usage == MemoryType::Immediate );
      
   // If heap is already locked by other thread mapping resource, fail.
   if (!heap->mapped.tryLock())
      return nullptr;
     
   void* mappedPtr = nullptr;
   
   mappedOffset = _offset;
   mappedSize   = _size;
   
   Profile( heap->gpu, vkMapMemory(heap->gpu->device, heap->handle, mappedOffset, mappedSize, 0u, &mappedPtr) )

   return mappedPtr;
   }

   void BuferVK::unmap(void)
   {
   assert( heap->mapped.isLocked() );

   // Unmaps memory object
   ProfileNoRet( heap->gpu, vkUnmapMemory(heap->gpu->device, heap->handle) )

   mapped.unlock();
   }


   // Vulkan Buffer View is created only for Linear Textures backed by Buffers - not supported currently.
   //
#if 0
   Ptr<BufferView> BufferVK::view(const Format format, const uint64 offset, const uint64 length)
   {
   Ptr<BufferViewVK> result = nullptr;

   uint64 finalOffset = roundUp( offset, gpu->properties.limits.minTexelBufferOffsetAlignment );
   uint64 finalLength = roundUp( length, gpu->properties.limits.minTexelBufferOffsetAlignment );
   if (finalOffset + finalLength > size)
      finalLength = VK_WHOLE_SIZE;  // TODO: Round up to Texel Format size, then to limits, then calculate adjusted length

   VkBufferViewCreateInfo viewInfo = {};
   viewInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
   viewInfo.pNext  = nullptr;
   viewInfo.flags  = 0; // Reserved
   viewInfo.buffer = handle;
   viewInfo.format = TranslateTextureFormat[underlyingType(format)];
   viewInfo.offset = finalOffset;
   viewInfo.range  = finalLength;
   
   VkBufferView viewHandle = VK_NULL_HANDLE;

   Profile( gpu, vkCreateBufferView(gpu->device, &viewInfo, nullptr, &viewHandle) )
   if (gpu->lastResult[Scheduler.core()] == VK_SUCCESS)
      result = new BufferViewVK(Ptr<BufferVK>(this), viewHandle, format, finalOffset, finalLength);

   return ptr_dynamic_cast<BufferView, BufferViewVK>(result);
   }

   BufferViewVK::BufferViewVK(Ptr<BufferVK>      parent,
                              const VkBufferView view,
                              const Format       format,
                              const uint32       offset,
                              const uint32       length) :
      buffer(parent),
      handle(view),
      CommonBufferView(format, offset, length)
   {
   }
   
   BufferViewVK::~BufferViewVK()
   {
   ProfileNoRet( buffer->gpu, vkDestroyBufferView(buffer->gpu->device, handle, nullptr) )
   buffer = nullptr;
   }

   Ptr<Buffer> BufferViewVK::parent(void) const
   {
   return ptr_dynamic_cast<Buffer, BufferVK>(buffer);
   }
#endif

   Ptr<Buffer> createBuffer(const HeapVK* heap, const BufferType type, const uint32 size)
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
            
      default:
         assert( 0 );
         break;
      };
      
   // If buffer is created on Heaps designated to data transfer, mark that
   if (heap->_usage == MemoryUsage::Streamed ||
       heap->_usage == MemoryUsage::Immediate)
      {
      // TODO: Optimize it so that transfer direction is specified
      bufferUsage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      bufferUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
      }

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
   VulkanDevice* gpu = heap->gpu;
   Profile( gpu, vkCreateBuffer(gpu->device, &bufferInfo, nullptr, &handle) )
   if (gpu->lastResult[Scheduler.core()] >= 0)
      {
      buffer = new BufferVK(gpu, handle, type, size);
      
      // Query buffer requirements
      ProfileNoRet( gpu, vkGetBufferMemoryRequirements(gpu->device, buffer->handle, &buffer->memoryRequirements) )

      // TODO: Create default Buffer View !
      } 

   return ptr_reinterpret_cast<Buffer>(&buffer);
   }
   
   }
}
#endif
