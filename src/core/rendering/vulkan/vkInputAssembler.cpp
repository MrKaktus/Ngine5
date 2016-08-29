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

#include "core/rendering/vulkan/vkInputAssembler.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/vulkan/vkDevice.h"    // TODO: We only want Device class, not all subsystems

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
   static const VkFormat TranslateVertexFormat[underlyingType(Attribute::Count)] =
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


   //// Columns representing data in fixed point sheme 16.16 were introduced 
   //// in OpenGL ES for low end devices without HW acceleration. Currently
   //// almost all mobile devices has HW acceleration for OpenGL ES which means 
   //// that floating point values will be better choose in almost all cases. 
   //// Therefore fixed column formats are not supported by engine.

   //// Type of data in columns
   //enum Attribute
   //     {
   //     None                      = 0,   
   //     Float_8                      ,
   //     Float2_8                     ,
   //     Float3_8                     ,
   //     Float4_8                     ,
   //     Float_16                     ,
   //     Float2_16                    ,
   //     Float3_16                    ,
   //     Float4_16                    ,
   //     UFloat_8                     ,
   //     UFloat2_8                    ,
   //     UFloat3_8                    ,
   //     UFloat4_8                    ,
   //     UFloat_16                    ,
   //     UFloat2_16                   ,
   //     UFloat3_16                   ,
   //     UFloat4_16                   ,
   //     Half                         ,
   //     Half2                        ,
   //     Half3                        ,
   //     Half4                        ,
   //     Float                        ,
   //     Float2                       ,
   //     Float3                       ,
   //     Float4                       ,
   //     Double                       ,
   //     Double2                      ,
   //     Double3                      ,
   //     Double4                      ,
   //     Float_8_SNorm                ,
   //     Float2_8_SNorm               ,
   //     Float3_8_SNorm               ,
   //     Float4_8_SNorm               ,
   //     Half_SNorm                   ,
   //     Half2_SNorm                  ,
   //     Half3_SNorm                  ,
   //     Half4_SNorm                  ,
   //     Float_SNorm                  ,
   //     Float2_SNorm                 ,
   //     Float3_SNorm                 ,
   //     Float4_SNorm                 ,
   //     Float_8_Norm                 ,
   //     Float2_8_Norm                ,
   //     Float3_8_Norm                ,
   //     Float4_8_Norm                ,
   //     Half_Norm                    ,
   //     Half2_Norm                   ,
   //     Half3_Norm                   ,
   //     Half4_Norm                   ,
   //     Float_Norm                   ,
   //     Float2_Norm                  ,
   //     Float3_Norm                  ,
   //     Float4_Norm                  ,
   //     Byte                         ,
   //     Byte2                        ,
   //     Byte3                        ,
   //     Byte4                        ,
   //     Short                        ,
   //     Short2                       ,
   //     Short3                       ,
   //     Short4                       ,
   //     Int                          ,
   //     Int2                         ,
   //     Int3                         ,
   //     Int4                         ,
   //     UByte                        ,
   //     UByte2                       ,
   //     UByte3                       ,
   //     UByte4                       ,
   //     UShort                       ,
   //     UShort2                      ,
   //     UShort3                      ,
   //     UShort4                      ,
   //     UInt                         ,
   //     UInt2                        ,
   //     UInt3                        ,
   //     UInt4                        ,
   //     Float4_10_10_10_2_SNorm      ,
   //     Float4_10_10_10_2_Norm       ,
   //     ColumnTypesCount
   //     };


            
//   // Buffer type
//   enum BufferType                      // OpenGL:
//        {
//        FeedbackBuffer               ,  // Transform Feedback Buffer Object
//        DrawBuffer                   ,  // Draw Indirect Buffer Object
//        CounterBuffer                ,  // Atomic Counters Buffer Object
//        DispatchBuffer               ,  // Dispatch Indirect Buffer Object
//      QueryBuffer                  ,  // Query Buffer Object
//        };
      

     
      
   static const VkFormat TranslateAttributeFormat[underlyingType(Attribute::Count)] = 
      {
      VK_FORMAT_UNDEFINED                ,  // None                      
      VK_FORMAT_R16_SFLOAT               ,  // Half                   
      VK_FORMAT_R16G16_SFLOAT            ,  // Half2                  
      VK_FORMAT_R16G16B16_SFLOAT         ,  // Half3                  
      VK_FORMAT_R16G16B16A16_SFLOAT      ,  // Half4                  
      VK_FORMAT_R32_SFLOAT               ,  // Float                  
      VK_FORMAT_R32G32_SFLOAT            ,  // Float2                 
      VK_FORMAT_R32G32B32_SFLOAT         ,  // Float3                 
      VK_FORMAT_R32G32B32A32_SFLOAT      ,  // Float4                 
      VK_FORMAT_R64_SFLOAT               ,  // Double                 
      VK_FORMAT_R64G64_SFLOAT            ,  // Double2                
      VK_FORMAT_R64G64B64_SFLOAT         ,  // Double3                
      VK_FORMAT_R64G64B64A64_SFLOAT      ,  // Double4                
      VK_FORMAT_R8_SINT                  ,  // Int8                   
      VK_FORMAT_R8G8_SINT                ,  // Int8v2                 
      VK_FORMAT_R8G8B8_SINT              ,  // Int8v3                 
      VK_FORMAT_R8G8B8A8_SINT            ,  // Int8v4                 
      VK_FORMAT_R16_SINT                 ,  // Int16                  
      VK_FORMAT_R16G16_SINT              ,  // Int16v2                
      VK_FORMAT_R16G16B16_SINT           ,  // Int16v3                
      VK_FORMAT_R16G16B16A16_SINT        ,  // Int16v4                
      VK_FORMAT_R32_SINT                 ,  // Int32                  
      VK_FORMAT_R32G32_SINT              ,  // Int32v2                
      VK_FORMAT_R32G32B32_SINT           ,  // Int32v3                
      VK_FORMAT_R32G32B32A32_SINT        ,  // Int32v4                
      VK_FORMAT_R64_SINT                 ,  // Int64              
      VK_FORMAT_R64G64_SINT              ,  // Int64v2 
      VK_FORMAT_R64G64B64_SINT           ,  // Int64v3 
      VK_FORMAT_R64G64B64A64_SINT        ,  // Int64v4 
      VK_FORMAT_R8_UINT                  ,  // UInt8                  
      VK_FORMAT_R8G8_UINT                ,  // UInt8v2                
      VK_FORMAT_R8G8B8_UINT              ,  // UInt8v3                
      VK_FORMAT_R8G8B8A8_UINT            ,  // UInt8v4                
      VK_FORMAT_R16_UINT                 ,  // UInt16                 
      VK_FORMAT_R16G16_UINT              ,  // UInt16v2               
      VK_FORMAT_R16G16B16_UINT           ,  // UInt16v3               
      VK_FORMAT_R16G16B16A16_UINT        ,  // UInt16v4               
      VK_FORMAT_R32_UINT                 ,  // UInt32                 
      VK_FORMAT_R32G32_UINT              ,  // UInt32v2               
      VK_FORMAT_R32G32B32_UINT           ,  // UInt32v3               
      VK_FORMAT_R32G32B32A32_UINT        ,  // UInt32v4               
      VK_FORMAT_R64_UINT                 ,  // UInt64          
      VK_FORMAT_R64G64_UINT              ,  // UInt64v2        
      VK_FORMAT_R64G64B64_UINT           ,  // UInt64v3        
      VK_FORMAT_R64G64B64A64_UINT        ,  // UInt64v4                   
      VK_FORMAT_R8_SNORM                 ,  // Float8_SNorm           
      VK_FORMAT_R8G8_SNORM               ,  // Float8v2_SNorm         
      VK_FORMAT_R8G8B8_SNORM             ,  // Float8v3_SNorm         
      VK_FORMAT_R8G8B8A8_SNORM           ,  // Float8v4_SNorm         
      VK_FORMAT_R16_SNORM                ,  // Float16_SNorm          
      VK_FORMAT_R16G16_SNORM             ,  // Float16v2_SNorm        
      VK_FORMAT_R16G16B16_SNORM          ,  // Float16v3_SNorm        
      VK_FORMAT_R16G16B16A16_SNORM       ,  // Float16v4_SNorm        
      VK_FORMAT_R8_UNORM                 ,  // Float8_Norm            
      VK_FORMAT_R8G8_UNORM               ,  // Float8v2_Norm          
      VK_FORMAT_R8G8B8_UNORM             ,  // Float8v3_Norm          
      VK_FORMAT_R8G8B8A8_UNORM           ,  // Float8v4_Norm          
      VK_FORMAT_R16_UNORM                ,  // Float16_Norm           
      VK_FORMAT_R16G16_UNORM             ,  // Float16v2_Norm         
      VK_FORMAT_R16G16B16_UNORM          ,  // Float16v3_Norm         
      VK_FORMAT_R16G16B16A16_UNORM       ,  // Float16v4_Norm         
      VK_FORMAT_A2R10G10B10_SNORM_PACK32 ,  // Float4_10_10_10_2_SNorm
      VK_FORMAT_A2R10G10B10_UNORM_PACK32 ,  // Float4_10_10_10_2_Norm   
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
   
   Ptr<InputAssembler> VulkanDevice::create(const DrawableType primitiveType,
                                            const uint32 controlPoints,
                                            const uint32 usedAttributes,
                                            const uint32 usedBuffers,
                                            const AttributeDesc* attributes,
                                            const BufferDesc* buffers)
   {
   Ptr<InputAssemblerVK> input = Ptr<InputAssemblerVK>(new InputAssemblerVK(primitiveType, controlPoints, usedAttributes, usedBuffers, attributes, buffers));

   return ptr_dynamic_cast<InputAssembler, InputAssemblerVK>(input);
   }

   }
}
#endif
