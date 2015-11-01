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

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/mtlInputAssembler.h"

namespace en
{
   namespace gpu
   {   
   // Metal will add double types in the future to support compute kernels
   // porting from OpenCL to Metal API. sint64 and uint64 can be added but
   // it is not verified yet. Other missing types could be added as well but
   // Epic and Crytek didn't requested them when API was designed so they
   // were ommited!

   // Type of data in attributes
   static const MTLVertexFormat TranslateAttributeFormat[AttributeFormatsCount] = 
      {
      MTLVertexFormatInvalid,              // None                      
      MTLVertexFormatInvalid,              // Half             (unsupported)               
      MTLVertexFormatHalf2,                // Half2                  
      MTLVertexFormatHalf3,                // Half3                  
      MTLVertexFormatHalf4,                // Half4                  
      MTLVertexFormatFloat,                // Float                  
      MTLVertexFormatFloat2,               // Float2                 
      MTLVertexFormatFloat3,               // Float3                 
      MTLVertexFormatFloat4,               // Float4                 
      MTLVertexFormatInvalid,              // Double           (unsupported)             
      MTLVertexFormatInvalid,              // Double2          (unsupported) 
      MTLVertexFormatInvalid,              // Double3          (unsupported) 
      MTLVertexFormatInvalid,              // Double4          (unsupported) 
      MTLVertexFormatInvalid,              // Int8             (unsupported) 
      MTLVertexFormatChar2,                // Int8v2                 
      MTLVertexFormatChar3,                // Int8v3                 
      MTLVertexFormatChar4,                // Int8v4                 
      MTLVertexFormatInvalid,              // Int16            (unsupported)            
      MTLVertexFormatShort2,               // Int16v2                
      MTLVertexFormatShort3,               // Int16v3                
      MTLVertexFormatShort4,               // Int16v4                
      MTLVertexFormatInt,                  // Int32                  
      MTLVertexFormatInt2,                 // Int32v2                
      MTLVertexFormatInt3,                 // Int32v3                
      MTLVertexFormatInt4,                 // Int32v4                
      MTLVertexFormatInvalid,              // Int64            (unsupported)                
      MTLVertexFormatInvalid,              // Int64v2          (unsupported)               
      MTLVertexFormatInvalid,              // Int64v3          (unsupported)              
      MTLVertexFormatInvalid,              // Int64v4          (unsupported)              
      MTLVertexFormatInvalid,              // UInt8            (unsupported)             
      MTLVertexFormatUChar2,               // UInt8v2                
      MTLVertexFormatUChar3,               // UInt8v3                
      MTLVertexFormatUChar4,               // UInt8v4                
      MTLVertexFormatInvalid,              // UInt16           (unsupported)          
      MTLVertexFormatUShort2,              // UInt16v2               
      MTLVertexFormatUShort3,              // UInt16v3               
      MTLVertexFormatUShort4,              // UInt16v4               
      MTLVertexFormatUInt,                 // UInt32                 
      MTLVertexFormatUInt2,                // UInt32v2               
      MTLVertexFormatUInt3,                // UInt32v3               
      MTLVertexFormatUInt4,                // UInt32v4               
      MTLVertexFormatInvalid,              // UInt64           (unsupported) 
      MTLVertexFormatInvalid,              // UInt64v2         (unsupported) 
      MTLVertexFormatInvalid,              // UInt64v3         (unsupported) 
      MTLVertexFormatInvalid,              // UInt64v4         (unsupported)           
      MTLVertexFormatInvalid,              // Float8_SNorm     (unsupported)          
      MTLVertexFormatChar2Normalized,      // Float8v2_SNorm         
      MTLVertexFormatChar3Normalized,      // Float8v3_SNorm         
      MTLVertexFormatChar4Normalized,      // Float8v4_SNorm         
      MTLVertexFormatInvalid,              // Float16_SNorm    (unsupported)         
      MTLVertexFormatShort2Normalized,     // Float16v2_SNorm        
      MTLVertexFormatShort3Normalized,     // Float16v3_SNorm        
      MTLVertexFormatShort4Normalized,     // Float16v4_SNorm        
      MTLVertexFormatInvalid,              // Float8_Norm      (unsupported)           
      MTLVertexFormatUChar2Normalized,     // Float8v2_Norm          
      MTLVertexFormatUChar3Normalized,     // Float8v3_Norm          
      MTLVertexFormatUChar4Normalized,     // Float8v4_Norm          
      MTLVertexFormatInvalid,              // Float16_Norm     (unsupported)       
      MTLVertexFormatUShort2Normalized,    // Float16v2_Norm         
      MTLVertexFormatUShort3Normalized,    // Float16v3_Norm         
      MTLVertexFormatUShort4Normalized,    // Float16v4_Norm         
      MTLVertexFormatInt1010102Normalized, // Float4_10_10_10_2_SNorm
      MTLVertexFormatUInt1010102Normalized // Float4_10_10_10_2_Norm 
      };

   // Size of each attribute in memory taking into notice required padding
   static const uint8 TranslateAttributeSize[AttributeTypesCount] = 
      {
      0,    // None                      
      0,    // Half                   
      4,    // Half2                  
      8,    // Half3               (6 bytes + 2 bytes of padding)
      8,    // Half4                  
      4,    // Float                  
      8,    // Float2                 
      12,   // Float3                 
      16,   // Float4                 
      0,    // Double                 
      0,    // Double2                
      0,    // Double3                
      0,    // Double4                
      0,    // Int8                   
      4,    // Int8v2              (2 bytes + 2 bytes of padding)
      4,    // Int8v3              (3 bytes + 1 byte  of padding)
      4,    // Int8v4                 
      0,    // Int16                  
      4,    // Int16v2                
      8,    // Int16v3             (6 bytes + 2 bytes of padding)
      8,    // Int16v4                
      4,    // Int32                  
      8,    // Int32v2                
      12,   // Int32v3                
      16,   // Int32v4                
      0,    // Int64                  
      0,    // Int64v2                
      0,    // Int64v3                
      0,    // Int64v4                
      0,    // UInt8                  
      4,    // UInt8v2             (2 bytes + 2 bytes of padding)        
      4,    // UInt8v3             (3 bytes + 1 byte  of padding)
      4,    // UInt8v4                
      0,    // UInt16                 
      4,    // UInt16v2               
      8,    // UInt16v3            (6 bytes + 2 bytes of padding)
      8,    // UInt16v4               
      4,    // UInt32                 
      8,    // UInt32v2               
      12,   // UInt32v3               
      16,   // UInt32v4               
      0,    // UInt64                 
      0,    // UInt64v2               
      0,    // UInt64v3               
      0,    // UInt64v4               
      0,    // Float8_SNorm           
      4,    // Float8v2_SNorm      (2 bytes + 2 bytes of padding)
      4,    // Float8v3_SNorm      (3 bytes + 1 byte  of padding)
      4,    // Float8v4_SNorm         
      0,    // Float16_SNorm          
      4,    // Float16v2_SNorm        
      8,    // Float16v3_SNorm     (6 bytes + 2 bytes of padding)
      8,    // Float16v4_SNorm        
      0,    // Float8_Norm            
      4,    // Float8v2_Norm       (2 bytes + 2 bytes of padding)
      4,    // Float8v3_Norm       (3 bytes + 1 byte  of padding)
      4,    // Float8v4_Norm          
      0,    // Float16_Norm           
      4,    // Float16v2_Norm         
      8,    // Float16v3_Norm      (6 bytes + 2 bytes of padding)
      8,    // Float16v4_Norm         
      4,    // Float4_10_10_10_2_SNorm
      4     // Float4_10_10_10_2_Norm 
      };

   InputAssemblerMTL::InputAssemblerMTL(const DrawableType primitiveType,
                                        const uint32 controlPoints, 
                                        const uint32 usedAttributes, 
                                        const uint32 usedBuffers, 
                                        const AttributeDesc* attributes,  
                                        const BufferDesc* buffers) :
      desc([[MTLVertexDescriptor alloc] autorelease]),
      primitive(primitiveType)
   {
   for(uint32 i=0; i<usedBuffers; ++i)
      {
      // There is no point in using MTLVertexStepFunctionConstant
      // as we can pass the same data using regular buffer and sample
      // it from any shader. There is also no real life case scenario
      // to set "default" const values for missing buffers.

      // MTLVertexBufferLayoutDescriptor
      desc.layouts[buffers].stepFunction = buffers[i].stepRate == 0 ? MTLVertexStepFunctionPerVertex : MTLVertexStepFunctionPerInstance;
      desc.layouts[buffers].stepRate     = max(1, buffers[i].stepRate);
      desc.layouts[buffers].stride       = buffers[i].elementSize; 
      }

   for(uint32 i=0; i<usedAttributes; ++i)
      {
      // MTLVertexAttributeDescriptor
      desc.attributes[i].format      = TranslateAttributeFormat[attributes[i].format];
      desc.attributes[i].offset      = attributes[i].offset;
      desc.attributes[i].bufferIndex = attributes[i].buffer;
      }
   }
   
   InputAssemblerMTL::~InputAssemblerMTL()
   {
   [desc release];
   }

   //  Ptr<InputAssembler> Create(AttributeInfo& attribute[MaxInputAssemblerAttributesCount],   // Reference to array specifying each vertex attribute, and it's source buffer
   //                             Ptr<Buffer>    buffer[MaxInputAssemblerAttributesCount])      // Array of buffer handles that will be used
   //  {
   //  Ptr<InputAssemblerMTL> state = new InputAssemblerMTL();
   //  
   //  // Create array describing buffers used by Vertex Fetch.
   //  uint32 buffers = 0;
   //  for(; buffers<MaxInputAssemblerAttributesCount; ++buffers)
   //     {
   //     // Passed array of buffers need to be tightly packed
   //     if (!buffer[buffers])
   //        break;
   //  
   //     // There is no point in using MTLVertexStepFunctionConstant
   //     // as we can pass the same data using regular buffer and sample
   //     // it from any shader. There is also no real life case scenario
   //     // to set "default" const values for missing buffers.
   //     Ptr<BufferMTL> src = ptr_dynamic_cast<BufferMTL, Buffer>(buffer[buffers]);
   //     if (src->step == 0)
   //        {
   //        state->desc.layouts[buffers].stepFunction = MTLVertexStepFunctionPerVertex;
   //        state->desc.layouts[buffers].stepRate     = 1;
   //        }
   //     else
   //        {
   //        state->desc.layouts[buffers].stepFunction = MTLVertexStepFunctionPerInstance;
   //        state->desc.layouts[buffers].stepRate     = src->step;
   //        }
   //  
   //     // distance between data of consecutive vertices in bound buffer, ( multiple of 4 bytes )
   //     state->desc.layouts[buffers].stride = src->rowSize; 
   //     }
   //  
   //  // Describe attributes and their connection with input buffers
   //  uint32 offset[MaxInputAssemblerAttributesCount];
   //  memset(&offset, 0, sizeof(uint32) * MaxInputAssemblerAttributesCount);
   //  
   //  for(uint32 i=0; i<MaxInputAssemblerAttributesCount; ++i)
   //     {
   //     assert( attribute[i].buffer < MaxInputAssemblerAttributesCount );
   //  
   //     // Calculate new offset in currently used buffer
   //     uint32 attributeSize = TranslateAttributeSize[ attribute[i].type ];
   //     assert( attributeSize != 0 );
   //  
   //     state->desc.attributes[i].format      = TranslateAttributeType[ attribute[i].type ];
   //     state->desc.attributes[i].bufferIndex = attribute[i].buffer;
   //  
   //     // It's assumed that if buffer is used, all it's attributes are used
   //     // in the order in which they appear in it. So offsets start from 0.
   //     state->desc.attributes[i].offset      = offset[ attribute[i].buffer ];
   //  
   //     // We assume that all attributes sharing the same buffer are 
   //     // tightly packed one after another from it's beginning.
   //     offset[ attribute[i].buffer ] += attributeSize;
   //     }
   //  
   //  return ptr_dynamic_cast<InputAssembler, InputAssemblerMTL>(state);
   //  }

   }
}

#endif