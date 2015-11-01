/*

 Ngine v5.0
 
 Module      : Input Assembler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vulkan.h"

#include "core/rendering/vulkan/vkInputAssembler.h"

namespace en
{
   namespace gpu
   {   
   static const VkFormat TranslateAttributeFormat[AttributeFormatsCount]
      {
      VK_FORMAT_UNDEFINED           ,  // None                      
      VK_FORMAT_R16_SFLOAT          ,  // Half                   
      VK_FORMAT_R16G16_SFLOAT       ,  // Half2                  
      VK_FORMAT_R16G16B16_SFLOAT    ,  // Half3                  
      VK_FORMAT_R16G16B16A16_SFLOAT ,  // Half4                  
      VK_FORMAT_R32_SFLOAT          ,  // Float                  
      VK_FORMAT_R32G32_SFLOAT       ,  // Float2                 
      VK_FORMAT_R32G32B32_SFLOAT    ,  // Float3                 
      VK_FORMAT_R32G32B32A32_SFLOAT ,  // Float4                 
      VK_FORMAT_R64_SFLOAT          ,  // Double                 
      VK_FORMAT_R64G64_SFLOAT       ,  // Double2                
      VK_FORMAT_R64G64B64_SFLOAT    ,  // Double3                
      VK_FORMAT_R64G64B64A64_SFLOAT ,  // Double4                
      VK_FORMAT_R8_SINT             ,  // Int8                   
      VK_FORMAT_R8G8_SINT           ,  // Int8v2                 
      VK_FORMAT_R8G8B8_SINT         ,  // Int8v3                 
      VK_FORMAT_R8G8B8A8_SINT       ,  // Int8v4                 
      VK_FORMAT_R16_SINT            ,  // Int16                  
      VK_FORMAT_R16G16_SINT         ,  // Int16v2                
      VK_FORMAT_R16G16B16_SINT      ,  // Int16v3                
      VK_FORMAT_R16G16B16A16_SINT   ,  // Int16v4                
      VK_FORMAT_R32_SINT            ,  // Int32                  
      VK_FORMAT_R32G32_SINT         ,  // Int32v2                
      VK_FORMAT_R32G32B32_SINT      ,  // Int32v3                
      VK_FORMAT_R32G32B32A32_SINT   ,  // Int32v4                
      VK_FORMAT_UNDEFINED           ,  // Int64          (unsupported)            
      VK_FORMAT_UNDEFINED           ,  // Int64v2        (unsupported) 
      VK_FORMAT_UNDEFINED           ,  // Int64v3        (unsupported) 
      VK_FORMAT_UNDEFINED           ,  // Int64v4        (unsupported) 
      VK_FORMAT_R8_UINT             ,  // UInt8                  
      VK_FORMAT_R8G8_UINT           ,  // UInt8v2                
      VK_FORMAT_R8G8B8_UINT         ,  // UInt8v3                
      VK_FORMAT_R8G8B8A8_UINT       ,  // UInt8v4                
      VK_FORMAT_R16_UINT            ,  // UInt16                 
      VK_FORMAT_R16G16_UINT         ,  // UInt16v2               
      VK_FORMAT_R16G16B16_UINT      ,  // UInt16v3               
      VK_FORMAT_R16G16B16A16_UINT   ,  // UInt16v4               
      VK_FORMAT_R32_UINT            ,  // UInt32                 
      VK_FORMAT_R32G32_UINT         ,  // UInt32v2               
      VK_FORMAT_R32G32B32_UINT      ,  // UInt32v3               
      VK_FORMAT_R32G32B32A32_UINT   ,  // UInt32v4               
      VK_FORMAT_UNDEFINED           ,  // UInt64         (unsupported) 
      VK_FORMAT_UNDEFINED           ,  // UInt64v2       (unsupported) 
      VK_FORMAT_UNDEFINED           ,  // UInt64v3       (unsupported) 
      VK_FORMAT_UNDEFINED           ,  // UInt64v4       (unsupported)            
      VK_FORMAT_R8_SNORM            ,  // Float8_SNorm           
      VK_FORMAT_R8G8_SNORM          ,  // Float8v2_SNorm         
      VK_FORMAT_R8G8B8_SNORM        ,  // Float8v3_SNorm         
      VK_FORMAT_R8G8B8A8_SNORM      ,  // Float8v4_SNorm         
      VK_FORMAT_R16_SNORM           ,  // Float16_SNorm          
      VK_FORMAT_R16G16_SNORM        ,  // Float16v2_SNorm        
      VK_FORMAT_R16G16B16_SNORM     ,  // Float16v3_SNorm        
      VK_FORMAT_R16G16B16A16_SNORM  ,  // Float16v4_SNorm        
      VK_FORMAT_R8_UNORM            ,  // Float8_Norm            
      VK_FORMAT_R8G8_UNORM          ,  // Float8v2_Norm          
      VK_FORMAT_R8G8B8_UNORM        ,  // Float8v3_Norm          
      VK_FORMAT_R8G8B8A8_UNORM      ,  // Float8v4_Norm          
      VK_FORMAT_R16_UNORM           ,  // Float16_Norm           
      VK_FORMAT_R16G16_UNORM        ,  // Float16v2_Norm         
      VK_FORMAT_R16G16B16_UNORM     ,  // Float16v3_Norm         
      VK_FORMAT_R16G16B16A16_UNORM  ,  // Float16v4_Norm         
      VK_FORMAT_B10G10R10A2_SNORM   ,  // Float4_10_10_10_2_SNorm
      VK_FORMAT_B10G10R10A2_UNORM      // Float4_10_10_10_2_Norm 
      };

   // Vulkan is not supporting Line Loops !
   static const VkPrimitiveTopology TranslateDrawableType[DrawableTypesCount] = 
      {
      VK_PRIMITIVE_TOPOLOGY_POINT_LIST,         // Points
      VK_PRIMITIVE_TOPOLOGY_LINE_LIST,          // Lines
      VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,         // LineLoops          (Unsupported, WA with Line Strips even though one line will be missing)
      VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,         // LineStripes
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,      // Triangles
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,       // TriangleFans
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,     // TriangleStripes
      VK_PRIMITIVE_TOPOLOGY_PATCH_LIST          // Patches
      };

   // VK_PRIMITIVE_TOPOLOGY_LINE_LIST_ADJ
   // VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_ADJ
   // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_ADJ
   // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_ADJ







   //- Interface holds local copy of all used attribute names
   //- Each Buffer holds pointer/handle to name in that collection


   //(Float3, "inPosition")


   //class BufferCommon : public Buffer
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


// GpuContext.support.maxInputAssemblerAttributesCount;







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


   //Ptr<InputAssembler> Interface::IInputAssembler::Create(Attribute&  attribute[MaxInputAssemblerAttributesCount],   // Reference to array specifying each vertex attribute, and it's source buffer
   //                                                       Ptr<Buffer> buffer[MaxInputAssemblerAttributesCount])      // Array of buffer handles that will be used
   //{
   //}


   //struct Attribute
   //   {
   //   AttributeFormat format; // Data format
   //   };


   InputAssemblerVK::InputAssemblerVK(const DrawableType primitiveType,
                                      const uint32 controlPoints, 
                                      const uint32 usedAttributes, 
                                      const uint32 usedBuffers, 
                                      const AttributeDesc* attributes,  
                                      const BufferDesc* buffers)
   {
   // Describe Primitive Type stored in incoming buffers
   statePrimitive.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
   statePrimitive.pNext                  = nullptr;
   statePrimitive.topology               = TranslateDrawableType[primitiveType];
   statePrimitive.primitiveRestartEnable = VK_FALSE;
   //
   // Primitive restart is always: 
   // off - for: Points, Lines, Triangles, Patches
   // on  - for: LineLoops, LineStripes, TriangleFans, TriangleStripes
   //
   if ( primitiveType == LineLoops       ||
        primitiveType == LineStripes     ||
        primitiveType == TriangleFans    ||
        primitiveType == TriangleStripes )
      statePrimitive.primitiveRestartEnable = VK_TRUE;

   // Optional: Describe Tessellation Incoming Patch size
   stateTessellator.sType                = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
   stateTessellator.pNext                = nullptr;
   stateTessellator.patchControlPoints   = (primitiveType == Patches) ? controlPoints : 0;

   // Optional: Describe Attributes mappings
   state.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
   state.pNext                           = nullptr;
   state.vertexBindingDescriptionCount   = usedBuffers;
   state.pVertexBindingDescriptions      = new VkVertexInputBindingDescription[usedBuffers];
   state.vertexAttributeDescriptionCount = usedAttributes;
   state.pVertexAttributeDescriptions    = new VkVertexInputAttributeDescription[usedAttributes];
   
   for(uint32 i=0; i<usedBuffers; ++i)
      {
      VkVertexInputBindingDescription* desc = (VkVertexInputBindingDescription*)(&state.pVertexBindingDescriptions[i]);

      //Ptr<BufferVK> buffer = ptr_dynamic_cast<BufferVK, Buffer>(buffers[i]);
      //desc->binding       = /* buffer Handle or buffer slot ??? */ buffer->id;    // THis should be buffer binding slot to decouple buffers!
      desc->stride    = buffers[i].elementSize;
      desc->inputRate = buffers[i].stepRate == 0 ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;
      // TODO: Vulkan - where if final step rate set ?
      }

   for(uint32 i=0; i<usedAttributes; ++i)
      {
      VkVertexInputAttributeDescription* desc = (VkVertexInputAttributeDescription*)(&state.pVertexAttributeDescriptions[i]);

      desc->location = i; // Location is adequate to attribute's location in array.
      desc->binding  = attributes[i].buffer;
      desc->format   = TranslateAttributeFormat[attributes[i].format];
      desc->offset   = attributes[i].offset;
      }
   }

   }
}
