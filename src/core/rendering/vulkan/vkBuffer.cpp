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
   // Vulkan Formats HW support:
   // https://docs.google.com/spreadsheets/d/1VuAYTNhkfJ1UiL-ITU-1Wk64m5B6iqPjOy4hUeHvEbU/edit#gid=2114123332


// Feature Survey/Proposal  (Bug 14574)
// https://docs.google.com/spreadsheets/d/1rva7gNP7Vk_vTTqSKOcJibDJfhiBcyxxJQOzgYlcJ84/edit#gid=1703792510
 
// Limit Survey/Proposal (Bug 14780)
// https://docs.google.com/spreadsheets/d/1FPpAsErMMVRBZQ24Y8c6Q6dF3L6lEkuxkDMSqpCHzb4/edit#gid=1870836081
 
// Format Survey (Bug 12998)
// https://docs.google.com/spreadsheets/d/1VuAYTNhkfJ1UiL-ITU-1Wk64m5B6iqPjOy4hUeHvEbU/edit#gid=2114123332


   
   // Last Verified during Vulkan 1.0 Release
   //
   static const VkFormat TranslateVertexFormat[underlyingType(VertexFormat::Count)] =
      { // Sized Internal Format
      VK_FORMAT_UNDEFINED                  ,   // VertexFormat::None
      VK_FORMAT_R8_UNORM                   ,   // VertexFormat::R_8                    Uncompressed formats:
      VK_FORMAT_R8_SNORM                   ,   // VertexFormat::R_8_sn
      VK_FORMAT_R8_UINT                    ,   // VertexFormat::R_8_u
      VK_FORMAT_R8_SINT                    ,   // VertexFormat::R_8_s
      VK_FORMAT_R16_UNORM                  ,   // VertexFormat::R_16
      VK_FORMAT_R16_SNORM                  ,   // VertexFormat::R_16_sn
      VK_FORMAT_R16_UINT                   ,   // VertexFormat::R_16_u
      VK_FORMAT_R16_SINT                   ,   // VertexFormat::R_16_s
      VK_FORMAT_R16_SFLOAT                 ,   // VertexFormat::R_16_hf
      VK_FORMAT_R32_UINT                   ,   // VertexFormat::R_32_u
      VK_FORMAT_R32_SINT                   ,   // VertexFormat::R_32_s
      VK_FORMAT_R32_SFLOAT                 ,   // VertexFormat::R_32_f
      VK_FORMAT_R64_UINT                   ,   // VertexFormat::R_64_u
      VK_FORMAT_R64_SINT                   ,   // VertexFormat::R_64_s
      VK_FORMAT_R64_SFLOAT                 ,   // VertexFormat::R_64_f
      VK_FORMAT_R8G8_UNORM                 ,   // VertexFormat::RG_8
      VK_FORMAT_R8G8_SNORM                 ,   // VertexFormat::RG_8_sn
      VK_FORMAT_R8G8_UINT                  ,   // VertexFormat::RG_8_u
      VK_FORMAT_R8G8_SINT                  ,   // VertexFormat::RG_8_s
      VK_FORMAT_R16G16_UNORM               ,   // VertexFormat::RG_16
      VK_FORMAT_R16G16_SNORM               ,   // VertexFormat::RG_16_sn
      VK_FORMAT_R16G16_UINT                ,   // VertexFormat::RG_16_u
      VK_FORMAT_R16G16_SINT                ,   // VertexFormat::RG_16_s
      VK_FORMAT_R16G16_SFLOAT              ,   // VertexFormat::RG_16_hf
      VK_FORMAT_R32G32_UINT                ,   // VertexFormat::RG_32_u
      VK_FORMAT_R32G32_SINT                ,   // VertexFormat::RG_32_s
      VK_FORMAT_R32G32_SFLOAT              ,   // VertexFormat::RG_32_f
      VK_FORMAT_R64G64_UINT                ,   // VertexFormat::RG_64_u
      VK_FORMAT_R64G64_SINT                ,   // VertexFormat::RG_64_s
      VK_FORMAT_R64G64_SFLOAT              ,   // VertexFormat::RG_64_f
      VK_FORMAT_R8G8B8_UNORM               ,   // VertexFormat::RGB_8                  - Not reccomended due to lack of memory aligment
      VK_FORMAT_R8G8B8_SRGB                ,   // VertexFormat::RGB_8_sRGB             - Not reccomended due to lack of memory aligment
      VK_FORMAT_R8G8B8_SNORM               ,   // VertexFormat::RGB_8_sn               - Not reccomended due to lack of memory aligment
      VK_FORMAT_R8G8B8_UINT                ,   // VertexFormat::RGB_8_u                - Not reccomended due to lack of memory aligment
      VK_FORMAT_R8G8B8_SINT                ,   // VertexFormat::RGB_8_s                - Not reccomended due to lack of memory aligment
      VK_FORMAT_R16G16B16_UNORM            ,   // VertexFormat::RGB_16
      VK_FORMAT_R16G16B16_SNORM            ,   // VertexFormat::RGB_16_sn
      VK_FORMAT_R16G16B16_UINT             ,   // VertexFormat::RGB_16_u
      VK_FORMAT_R16G16B16_SINT             ,   // VertexFormat::RGB_16_s
      VK_FORMAT_R16G16B16_SFLOAT           ,   // VertexFormat::RGB_16_hf
      VK_FORMAT_R32G32B32_UINT             ,   // VertexFormat::RGB_32_u
      VK_FORMAT_R32G32B32_SINT             ,   // VertexFormat::RGB_32_s
      VK_FORMAT_R32G32B32_SFLOAT           ,   // VertexFormat::RGB_32_f
      VK_FORMAT_R64G64B64_UINT             ,   // VertexFormat::RGB_64_u
      VK_FORMAT_R64G64B64_SINT             ,   // VertexFormat::RGB_64_s
      VK_FORMAT_R64G64B64_SFLOAT           ,   // VertexFormat::RGB_64_f
      VK_FORMAT_R8G8B8A8_UNORM             ,   // VertexFormat::RGBA_8
      VK_FORMAT_R8G8B8A8_SNORM             ,   // VertexFormat::RGBA_8_sn
      VK_FORMAT_R8G8B8A8_UINT              ,   // VertexFormat::RGBA_8_u
      VK_FORMAT_R8G8B8A8_SINT              ,   // VertexFormat::RGBA_8_s
      VK_FORMAT_R16G16B16A16_UNORM         ,   // VertexFormat::RGBA_16
      VK_FORMAT_R16G16B16A16_SNORM         ,   // VertexFormat::RGBA_16_sn
      VK_FORMAT_R16G16B16A16_UINT          ,   // VertexFormat::RGBA_16_u
      VK_FORMAT_R16G16B16A16_SINT          ,   // VertexFormat::RGBA_16_s
      VK_FORMAT_R16G16B16A16_SFLOAT        ,   // VertexFormat::RGBA_16_hf
      VK_FORMAT_R32G32B32A32_UINT          ,   // VertexFormat::RGBA_32_u
      VK_FORMAT_R32G32B32A32_SINT          ,   // VertexFormat::RGBA_32_s
      VK_FORMAT_R32G32B32A32_SFLOAT        ,   // VertexFormat::RGBA_32_f
      VK_FORMAT_R64G64B64A64_UINT          ,   // VertexFormat::RGBA_64_u
      VK_FORMAT_R64G64B64A64_SINT          ,   // VertexFormat::RGBA_64_s
      VK_FORMAT_R64G64B64A64_SFLOAT        ,   // VertexFormat::RGBA_64_f   Compressed formats:
      VK_FORMAT_B10G11R11_UFLOAT_PACK32    ,   // VertexFormat::RGB_11_11_10_uf     - Packed unsigned float for HDR Textures, UAV's and Render Targets
      VK_FORMAT_A2B10G10R10_UNORM_PACK32   ,   // VertexFormat::RGBA_10_10_10_2
      VK_FORMAT_A2B10G10R10_SNORM_PACK32   ,   // VertexFormat::RGBA_10_10_10_2_sn
      VK_FORMAT_A2B10G10R10_UINT_PACK32    ,   // VertexFormat::RGBA_10_10_10_2_u
      VK_FORMAT_A2B10G10R10_SINT_PACK32    ,   // VertexFormat::RGBA_10_10_10_2_s
      VK_FORMAT_A2R10G10B10_UNORM_PACK32   ,   // VertexFormat::BGRA_10_10_10_2
      VK_FORMAT_A2R10G10B10_SNORM_PACK32   ,   // VertexFormat::BGRA_10_10_10_2_sn
      VK_FORMAT_A2R10G10B10_UINT_PACK32    ,   // VertexFormat::BGRA_10_10_10_2_u
      VK_FORMAT_A2R10G10B10_SINT_PACK32    ,   // VertexFormat::BGRA_10_10_10_2_s
      };
 
     // Vertex Fetch fixed point formats deliberately not supported:
     //  
     // ucf - unsigned integer cast to float (also known as USCALED)
     // scf - signed integer cast to float (also known as SSCALED)
     //
     // It's better to use unsignd/signed integer formats and cast to float manually if needed.
     //          
     // VK_FORMAT_R8_USCALED                 ,   // VertexFormat::R_8_ucf
     // VK_FORMAT_R8_SSCALED                 ,   // VertexFormat::R_8_scf
     // VK_FORMAT_R16_USCALED                ,   // VertexFormat::R_16_ucf
     // VK_FORMAT_R16_SSCALED                ,   // VertexFormat::R_16_scf
     // VK_FORMAT_R8G8_USCALED               ,   // VertexFormat::RG_8_ucf
     // VK_FORMAT_R8G8_SSCALED               ,   // VertexFormat::RG_8_scf
     // VK_FORMAT_R16G16_USCALED             ,   // VertexFormat::RG_16_ucf
     // VK_FORMAT_R16G16_SSCALED             ,   // VertexFormat::RG_16_scf
     // VK_FORMAT_R8G8B8_USCALED             ,   // VertexFormat::RGB_8_ucf
     // VK_FORMAT_R8G8B8_SSCALED             ,   // VertexFormat::RGB_8_scf
     // VK_FORMAT_R16G16B16_USCALED          ,   // VertexFormat::RGB_16_ucf
     // VK_FORMAT_R16G16B16_SSCALED          ,   // VertexFormat::RGB_16_scf
     // VK_FORMAT_R8G8B8A8_USCALED           ,   // VertexFormat::RGBA_8_ucf    - Endiannes Independent
     // VK_FORMAT_R8G8B8A8_SSCALED           ,   // VertexFormat::RGBA_8_scf
     // VK_FORMAT_A8B8G8R8_USCALED_PACK32    ,   // VertexFormat::RGBA_8_ucf    - Endiannes Dependent
     // VK_FORMAT_A8B8G8R8_SSCALED_PACK32    ,   // VertexFormat::RGBA_8_scf
     // VK_FORMAT_R16G16B16A16_USCALED       ,   // VertexFormat::RGBA_16_ucf
     // VK_FORMAT_R16G16B16A16_SSCALED       ,   // VertexFormat::RGBA_16_scf
     // VK_FORMAT_B8G8R8_USCALED             ,   // VertexFormat::BGR_8_ucf
     // VK_FORMAT_B8G8R8_SSCALED             ,   // VertexFormat::BGR_8_scf
     // VK_FORMAT_B8G8R8A8_USCALED           ,   // VertexFormat::BGRA_8_ucf
     // VK_FORMAT_B8G8R8A8_SSCALED           ,   // VertexFormat::BGRA_8_scf
     // VK_FORMAT_A2B10G10R10_USCALED_PACK32 ,   // VertexFormat::RGBA_10_10_10_2_ucf
     // VK_FORMAT_A2B10G10R10_SSCALED_PACK32 ,   // VertexFormat::RGBA_10_10_10_2_scf
     // VK_FORMAT_A2R10G10B10_USCALED_PACK32 ,   // VertexFormat::BGRA_10_10_10_2_ucf
     // VK_FORMAT_A2R10G10B10_SSCALED_PACK32 ,   // VertexFormat::BGRA_10_10_10_2_scf




 //
 //  // Vulkan formats represent byte order in memory (RGB -> R byte 0, B byte 2)


   // Columns representing data in fixed point sheme 16.16 were introduced 
   // in OpenGL ES for low end devices without HW acceleration. Currently
   // almost all mobile devices has HW acceleration for OpenGL ES which means 
   // that floating point values will be better choose in almost all cases. 
   // Therefore fixed column formats are not supported by engine.

   // Type of data in columns
   enum Attribute
        {
        None                      = 0,   
        Float_8                      ,
        Float2_8                     ,
        Float3_8                     ,
        Float4_8                     ,
        Float_16                     ,
        Float2_16                    ,
        Float3_16                    ,
        Float4_16                    ,
        UFloat_8                     ,
        UFloat2_8                    ,
        UFloat3_8                    ,
        UFloat4_8                    ,
        UFloat_16                    ,
        UFloat2_16                   ,
        UFloat3_16                   ,
        UFloat4_16                   ,
        Half                         ,
        Half2                        ,
        Half3                        ,
        Half4                        ,
        Float                        ,
        Float2                       ,
        Float3                       ,
        Float4                       ,
        Double                       ,
        Double2                      ,
        Double3                      ,
        Double4                      ,
        Float_8_SNorm                ,
        Float2_8_SNorm               ,
        Float3_8_SNorm               ,
        Float4_8_SNorm               ,
        Half_SNorm                   ,
        Half2_SNorm                  ,
        Half3_SNorm                  ,
        Half4_SNorm                  ,
        Float_SNorm                  ,
        Float2_SNorm                 ,
        Float3_SNorm                 ,
        Float4_SNorm                 ,
        Float_8_Norm                 ,
        Float2_8_Norm                ,
        Float3_8_Norm                ,
        Float4_8_Norm                ,
        Half_Norm                    ,
        Half2_Norm                   ,
        Half3_Norm                   ,
        Half4_Norm                   ,
        Float_Norm                   ,
        Float2_Norm                  ,
        Float3_Norm                  ,
        Float4_Norm                  ,
        Byte                         ,
        Byte2                        ,
        Byte3                        ,
        Byte4                        ,
        Short                        ,
        Short2                       ,
        Short3                       ,
        Short4                       ,
        Int                          ,
        Int2                         ,
        Int3                         ,
        Int4                         ,
        UByte                        ,
        UByte2                       ,
        UByte3                       ,
        UByte4                       ,
        UShort                       ,
        UShort2                      ,
        UShort3                      ,
        UShort4                      ,
        UInt                         ,
        UInt2                        ,
        UInt3                        ,
        UInt4                        ,
        Float4_10_10_10_2_SNorm      ,
        Float4_10_10_10_2_Norm       ,
        ColumnTypesCount
        };


            
//   // Buffer type
//   enum BufferType                      // OpenGL:
//        {
//        FeedbackBuffer               ,  // Transform Feedback Buffer Object
//        DrawBuffer                   ,  // Draw Indirect Buffer Object
//        CounterBuffer                ,  // Atomic Counters Buffer Object
//        DispatchBuffer               ,  // Dispatch Indirect Buffer Object
//      QueryBuffer                  ,  // Query Buffer Object
//        };
      

     

      
   
      
   BufferVK::BufferVK(VkDevice _device, VkBuffer _handle) :
      device(_device),
      handle(_handle)
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
         
      default:
         assert(0);
         break;
      };

// Currently unsupported types:
//
// VK_BUFFER_USAGE_TRANSFER_SRC_BIT
// VK_BUFFER_USAGE_TRANSFER_DST_BIT
//
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
   bufferInfo.size  = state.size;
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
      buffer = new BufferVK(device, handle);
      } 
   
   return ptr_dynamic_cast<Buffer, BufferVK>(buffer); 
   }
   
   

   }
}
#endif
