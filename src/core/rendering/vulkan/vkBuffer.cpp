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

namespace en
{
   namespace gpu
   {
   BufferVK::BufferVK(VkDevice _device, VkBuffer _handle, const BufferType type, const uint32 size) :
      device(_device),
      handle(_handle),
      BufferCommon(type, size)
   {
   }
   
   BufferVK::~BufferVK()
   {
   vkDestroyBuffer(device, handle, nullptr);
   }

   BufferViewVK::BufferViewVK(const VkDevice _device) :
      device(_device),
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
   vkDestroyBufferView(device, handle, nullptr);
   }








   
   

   Ptr<Buffer> VulkanDevice::create(const uint32 elements, const Formatting& formatting, const uint32 step)
   {
   assert( elements );
   assert( formatting.column[0] != Attribute::None );
   
   uint32 rowSize = formatting.rowSize();
   uint32 size    = elements * rowSize;
   Ptr<Buffer> buffer = create(BufferType::Vertex, size);
   if (buffer)
      {
      Ptr<BufferVK> ptr = ptr_dynamic_cast<BufferVK, Buffer>(buffer);
      
      // TODO: Which of those are later needed ?
      //ptr->size = size;
      //ptr->rowSize = rowSize;
      //ptr->elements = elements;
      //ptr->formatting = formatting;
      }

   return buffer;
   }
   
   Ptr<Buffer> VulkanDevice::create(const uint32 elements, const Attribute format)
   {
   assert( elements );
   assert( format == Attribute::R_8_u  ||
           format == Attribute::R_16_u ||
           format == Attribute::R_32_u );
      
   uint32 rowSize = TranslateAttributeSize[underlyingType(format)];
   uint32 size    = elements * rowSize;
   return create(BufferType::Index, size);
   }
   
   
   
   
   
   
   

   

   
   Ptr<Buffer> VulkanDevice::create(const BufferType type, const uint32 size)
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
         assert(0);
         break;
      };

// Need to be set to enable buffer views:
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
//    bufferInfo.sharingMode           = VK_SHARING_MODE_CONCURRENT;
//    bufferInfo.queueFamilyIndexCount = queueFamiliesCount;  // Count of Queue Families (for eg.g GPU has 40 Rendering and 10 Compute Queues grouped into 2 Families)
//    bufferInfo.pQueueFamilyIndices   = ;                    // pQueueFamilyIndices is a list of queue families that will access this buffer
//    }
// else
      {
      bufferInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE; // Sharing method when accessed by multiple Queue Families (alternative: VK_SHARING_MODE_CONCURRENT)
      bufferInfo.queueFamilyIndexCount = 0;
      bufferInfo.pQueueFamilyIndices   = nullptr;
      }
      
   VkBuffer handle;
   VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &handle);
   if (result >= 0)
      {
      buffer = new BufferVK(device, handle, type, size);
      } 
   
   // TODO: Assign memory to Buffer object !
   
   return ptr_dynamic_cast<Buffer, BufferVK>(buffer); 
   }
   


   // TODO: ifdef Mobile / or Transient
   Ptr<Buffer> VulkanDevice::create(const BufferType type, const uint32 size, const void* data)
   {
   assert( size );
   

   }

   }
}
#endif
