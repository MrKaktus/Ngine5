/*

 Ngine v5.0
 
 Module      : Vulkan Input Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkInputLayout.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/vulkan/vkDevice.h"    // TODO: We only want Device class, not all subsystems
#include "core/rendering/vulkan/vkBuffer.h" 

namespace en
{
   namespace gpu
   {
   // Vulkan Formats HW support:
   //
   // Feature Survey/Proposal  (Bug 14574)
   // https://docs.google.com/spreadsheets/d/1rva7gNP7Vk_vTTqSKOcJibDJfhiBcyxxJQOzgYlcJ84/edit#gid=1703792510
   //
   // Limit Survey/Proposal (Bug 14780)
   // https://docs.google.com/spreadsheets/d/1FPpAsErMMVRBZQ24Y8c6Q6dF3L6lEkuxkDMSqpCHzb4/edit#gid=1870836081
   //
   // Format Survey (Bug 12998)
   // https://docs.google.com/spreadsheets/d/1VuAYTNhkfJ1UiL-ITU-1Wk64m5B6iqPjOy4hUeHvEbU/edit#gid=2114123332
   //
   // Last Verified during Vulkan 1.0 Release
   //
   static const VkFormat TranslateAttributeFormat[underlyingType(Attribute::Count)] =
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
      VK_FORMAT_R32G32B32_UINT             ,   // VertexFormat::RGB_32_u
      VK_FORMAT_R32G32B32_SINT             ,   // VertexFormat::RGB_32_s
      VK_FORMAT_R32G32B32_SFLOAT           ,   // VertexFormat::RGB_32_f
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
      VK_FORMAT_A2B10G10R10_UNORM_PACK32       // VertexFormat::RGBA_10_10_10_2
      };
 
   // Vulkan is not supporting 3 component formats that's size is not
   // multiple of four bytes (they are not reccomended anyway due to 
   // mentioned lack of memory aligment).
   //
   // VK_FORMAT_R8G8B8_UNORM               ,   // VertexFormat::RGB_8                  
   // VK_FORMAT_R8G8B8_SRGB                ,   // VertexFormat::RGB_8_sRGB             
   // VK_FORMAT_R8G8B8_SNORM               ,   // VertexFormat::RGB_8_sn               
   // VK_FORMAT_R8G8B8_UINT                ,   // VertexFormat::RGB_8_u                
   // VK_FORMAT_R8G8B8_SINT                ,   // VertexFormat::RGB_8_s                
   // VK_FORMAT_R16G16B16_UNORM            ,   // VertexFormat::RGB_16                
   // VK_FORMAT_R16G16B16_SNORM            ,   // VertexFormat::RGB_16_sn             
   // VK_FORMAT_R16G16B16_UINT             ,   // VertexFormat::RGB_16_u              
   // VK_FORMAT_R16G16B16_SINT             ,   // VertexFormat::RGB_16_s              
   // VK_FORMAT_R16G16B16_SFLOAT           ,   // VertexFormat::RGB_16_hf             
   //
   // Even though Vulkan specification describes 64bit formats layout
   // for Input Assembler, it's not listing those formats as supported:
   //

   // Size of each attribute in memory taking into notice required padding
   const uint8 AttributeSize[underlyingType(Attribute::Count)] =
      {
      0,    // None           
      1,    // u8_norm                
      1,    // s8_norm                
      1,    // u8                     
      1,    // s8                     
      2,    // u16_norm               
      2,    // s16_norm               
      2,    // u16                    
      2,    // s16                    
      2,    // f16                    
      4,    // u32                   
      4,    // s32                   
      4,    // f32                                     
      2,    // v2u8_norm              
      2,    // v2s8_norm              
      2,    // v2u8                   
      2,    // v2s8                   
      4,    // v2u16_norm            
      4,    // v2s16_norm            
      4,    // v2u16                 
      4,    // v2s16                 
      4,    // v2f16                 
      8,    // v2u32                 
      8,    // v2s32                 
      8,    // v2f32                                  
      12,   // v3u32
      12,   // v3s32
      12,   // v3f32                                  
      4,    // v4u8_norm             
      4,    // v4s8_norm             
      4,    // v4u8                  
      4,    // v4s8                  
      8,    // v4u16_norm            
      8,    // v4s16_norm            
      8,    // v4u16                 
      8,    // v4s16                 
      8,    // v4f16                 
      16,   // v4u32                 
      16,   // v4s32                 
      16,   // v4f32                                           
      4,    // v4u10_10_10_2_norm           
      };

 

 //  // Vulkan formats represent byte order in memory (RGB -> R byte 0, B byte 2)


   //// Columns representing data in fixed point sheme 16.16 were introduced 
   //// in OpenGL ES for low end devices without HW acceleration. Currently
   //// almost all mobile devices has HW acceleration for OpenGL ES which means 
   //// that floating point values will be better choose in almost all cases. 
   //// Therefore fixed column formats are not supported by engine.


    
  
   // Vulkan is not supporting Line Loops !
   static const VkPrimitiveTopology TranslateDrawableType[DrawableTypesCount] = 
      {
      VK_PRIMITIVE_TOPOLOGY_POINT_LIST,         // Points
      VK_PRIMITIVE_TOPOLOGY_LINE_LIST,          // Lines
      VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,         // LineStripes
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,      // Triangles
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,     // TriangleStripes
      VK_PRIMITIVE_TOPOLOGY_PATCH_LIST          // Patches
      };

   // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,       // TriangleFans - Supported only by Vulkan.
   // VK_PRIMITIVE_TOPOLOGY_LINE_LIST_ADJ
   // VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_ADJ
   // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_ADJ
   // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_ADJ







   //- Interface holds local copy of all used attribute names
   //- Each Buffer holds pointer/handle to name in that collection


   //(Float3, "inPosition")


   //class CommonBuffer : public Buffer
   //   {
   //   public:
   //   // Buffer general
   //   BufferType type;     // Buffer type
   //   uint32     size;     // Total size

   //   // Input buffer specific
   //   uint32 attributes  : 8;  // Attributes count
   //   uint32 elementSize : 9;  // Size of one element (all attributes combined, with paddings)
   //   uint32             : 15;
   //   union 
   //      {                     // Vertex buffer can store geometry vertices
   //      uint32 vertices;      // or like Index buffer "elements" of some 
   //      uint32 elements;      // other type.
   //      };
   //   uint16 offset[16];       // Offset of each attribute in element (taking into notice data padding)
   //   AttributeFormat format[16];   // Format of each attribute
   //   };

   //BufferSettings
   //

   //      BufferSettings(const BufferType type  = VertexBuffer, // Default Constructor   
   //                     const uint32 vertices  = 0,  
   //                     const AttributeFormat attributes[16]
   //                     );

   //Ptr<Buffer> Interface::IBuffer::Create()
   //{

   //}


   // Buffer:
   // - one or more attributes (tightly packed if not taking into accound data alignment) creating one or more elements
   // - all attributes are updated with the same step/update rate



   // Metal / Vulkan

   // per buffer:
   // - element size
   // - step rate
   //
   // per attribute:
   // - format
   // - offset in element -> this can be queried/calculated if known exact field in buffer
   // - buffer index ->

   

   // Input:
   // - fixed array of input attribute descriptors
   // - array of buffer pointers (setup used to deduce connections)
   //   OR
   // - array of buffer settings descriptions




 //  stepFunction - update type (per Vertex, per few Instances)
 //  stepRate - how ofter buffer will be updated
 //  stride - stride (width of the buffer that will be used)
//   offset - in buffer that will be used

//uint32 usedAttributes;
//uint32 usedBuffers;
//Attribute*   attributes,   // Pointer to array specifying each vertex attribute, and it's source buffer
//Ptr<Buffer>* buffers)      // Array of buffer handles that will be used


   // Inits Input Assembler with default set of buffers to use.
   // Offsets and strides will be calculated based on these buffers.





   InputLayoutVK::InputLayoutVK(const DrawableType primitiveType,
                                const bool primitiveRestart,
                                const uint32 controlPoints,
                                const uint32 usedAttributes,
                                const uint32 usedBuffers,
                                const AttributeDesc* attributes,
                                const BufferDesc* buffers)
   {
   // Describe Primitive Type stored in incoming buffers
   statePrimitive.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
   statePrimitive.pNext                  = nullptr;
   statePrimitive.flags                  = 0u;
   statePrimitive.topology               = TranslateDrawableType[primitiveType];
   statePrimitive.primitiveRestartEnable = VK_FALSE;
   //
   // Primitive restart is always: 
   // off - for: Points, Lines, Triangles, Patches
   // on  - for: LineStripes, TriangleStripes (LineLoops, TriangleFans)
   //
   if (primitiveRestart &&
       (primitiveType == LineStripes ||
        primitiveType == TriangleStripes))
      statePrimitive.primitiveRestartEnable = VK_TRUE;

   // Optional: Describe Tessellation Incoming Patch size
   stateTessellator.sType                = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
   stateTessellator.pNext                = nullptr;
   stateTessellator.flags                = 0u;
   stateTessellator.patchControlPoints   = (primitiveType == Patches) ? controlPoints : 0u;

   // Optional: Describe Attributes mappings
   state.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
   state.pNext                           = nullptr;
   state.flags                           = 0u;
   state.vertexBindingDescriptionCount   = usedBuffers;
   state.pVertexBindingDescriptions      = usedBuffers ? new VkVertexInputBindingDescription[usedBuffers] : nullptr;
   state.vertexAttributeDescriptionCount = usedAttributes;
   state.pVertexAttributeDescriptions    = usedAttributes ? new VkVertexInputAttributeDescription[usedAttributes] : nullptr;

   for(uint32 i=0; i<usedBuffers; ++i)
      {
      VkVertexInputBindingDescription* desc = (VkVertexInputBindingDescription*)(&state.pVertexBindingDescriptions[i]);

      //Ptr<BufferVK> buffer = ptr_dynamic_cast<BufferVK, Buffer>(buffers[i]);
      //buffer->id;    // THis should be buffer binding slot to decouple buffers!
      
      desc->binding   = i;                      // Described binding slot.  [0 .. VkPhysicalDeviceLimits::maxVertexInputBindings)
      desc->stride    = buffers[i].elementSize; // Stride between elements. [0 .. VkPhysicalDeviceLimits::maxVertexInputBindingStride)
      desc->inputRate = buffers[i].stepRate == 0 ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;
      // TODO: Vulkan - where is final step rate set ?
      }

   for(uint32 i=0; i<usedAttributes; ++i)
      {
      VkVertexInputAttributeDescription* desc = (VkVertexInputAttributeDescription*)(&state.pVertexAttributeDescriptions[i]);

      desc->location = i;                    // Location in attributes array.
      desc->binding  = attributes[i].buffer; // Index of binding slot of input buffers to use (indirection decoupling buffers).
      desc->format   = TranslateAttributeFormat[underlyingType(attributes[i].format)];
      desc->offset   = attributes[i].offset;
      }
   }
   
   // Implemented by CommonDevice
   // Ptr<InputLayout> VulkanDevice::createInputLayout(const DrawableType primitiveType,
   //                                                  const bool primitiveRestart,
   //                                                  const uint32 controlPoints,
   //                                                  const Ptr<Buffer> buffer)
      
   Ptr<InputLayout> VulkanDevice::createInputLayout(const DrawableType primitiveType,
                                                    const bool primitiveRestart,
                                                    const uint32 controlPoints,
                                                    const uint32 usedAttributes,
                                                    const uint32 usedBuffers,
                                                    const AttributeDesc* attributes,
                                                    const BufferDesc* buffers)
   {
   Ptr<InputLayoutVK> input = Ptr<InputLayoutVK>(new InputLayoutVK(primitiveType, primitiveRestart, controlPoints, usedAttributes, usedBuffers, attributes, buffers));

   return ptr_reinterpret_cast<InputLayout>(&input);
   }

   }
}
#endif
