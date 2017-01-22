/*

 Ngine v5.0
 
 Module      : Metal Input Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlInputLayout.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/mtlDevice.h"

namespace en
{
   namespace gpu
   {   
   // Metal will add double types in the future to support compute kernels
   // porting from OpenCL to Metal API. sint64 and uint64 can be added but
   // it is not verified yet. Other missing types could be added as well but
   // Epic and Crytek didn't requested them when API was designed so they
   // were ommited!


   // Metal OSX Vertex Attribute Formats:
   // https://developer.apple.com/library/mac/documentation/Metal/Reference/MTLVertexAttributeDescriptor_Ref/#//apple_ref/c/tdef/MTLVertexFormat
   // (last verified for Metal on OSX 10.11)
   //
   // Metal IOS Vertex Attribute Formats:
   // https://developer.apple.com/library/ios/documentation/Metal/Reference/MTLVertexAttributeDescriptor_Ref/#//apple_ref/c/tdef/MTLVertexFormat
   // (last verified for Metal on IOS 9.0)
   //
   // All available formats are available since IOS 8.0 or OSX 10.11 unless later version of introduction is explicitly stated.
   //
   // Type of data in attributes
   static const MTLVertexFormat TranslateAttributeFormat[underlyingType(Attribute::Count)] =
      {
      MTLVertexFormatInvalid               ,  // None                 
      MTLVertexFormatInvalid               ,  // u8_norm                (unsupported)
      MTLVertexFormatInvalid               ,  // s8_norm                (unsupported)
      MTLVertexFormatInvalid               ,  // u8                     (unsupported)
      MTLVertexFormatInvalid               ,  // s8                     (unsupported)
      MTLVertexFormatInvalid               ,  // u16_norm               (unsupported)
      MTLVertexFormatInvalid               ,  // s16_norm               (unsupported)
      MTLVertexFormatInvalid               ,  // u16                    (unsupported)
      MTLVertexFormatInvalid               ,  // s16                    (unsupported)
      MTLVertexFormatInvalid               ,  // f16                    (unsupported)
      MTLVertexFormatUInt                  ,  // u32                    
      MTLVertexFormatInt                   ,  // s32                    
      MTLVertexFormatFloat                 ,  // f32                    
      MTLVertexFormatInvalid               ,  // u64                    (unsupported)
      MTLVertexFormatInvalid               ,  // s64                    (unsupported)
      MTLVertexFormatInvalid               ,  // f64                    (unsupported)
      MTLVertexFormatUChar2Normalized      ,  // v2u8_norm              
      MTLVertexFormatChar2Normalized       ,  // v2s8_norm              
      MTLVertexFormatUChar2                ,  // v2u8                   
      MTLVertexFormatChar2                 ,  // v2s8                   
      MTLVertexFormatUShort2Normalized     ,  // v2u16_norm             
      MTLVertexFormatShort2Normalized      ,  // v2s16_norm             
      MTLVertexFormatUShort2               ,  // v2u16                  
      MTLVertexFormatShort2                ,  // v2s16                  
      MTLVertexFormatHalf2                 ,  // v2f16                  
      MTLVertexFormatUInt2                 ,  // v2u32                  
      MTLVertexFormatInt2                  ,  // v2s32                  
      MTLVertexFormatFloat2                ,  // v2f32                  
      MTLVertexFormatInvalid               ,  // v2u64                  (unsupported)
      MTLVertexFormatInvalid               ,  // v2s64                  (unsupported)
      MTLVertexFormatInvalid               ,  // v2f64                  (unsupported)
      MTLVertexFormatUChar3Normalized      ,  // v3u8_norm              
      MTLVertexFormatInvalid               ,  // v3u8_srgb              (unsupported)
      MTLVertexFormatChar3Normalized       ,  // v3s8_norm              
      MTLVertexFormatUChar3                ,  // v3u8                   
      MTLVertexFormatChar3                 ,  // v3s8                   
      MTLVertexFormatUShort3Normalized     ,  // v3u16_norm             
      MTLVertexFormatShort3Normalized      ,  // v3s16_norm             
      MTLVertexFormatUShort3               ,  // v3u16                  
      MTLVertexFormatShort3                ,  // v3s16                  
      MTLVertexFormatHalf3                 ,  // v3f16                  
      MTLVertexFormatUInt3                 ,  // v3u32                  
      MTLVertexFormatInt3                  ,  // v3s32                  
      MTLVertexFormatFloat3                ,  // v3f32                  
      MTLVertexFormatInvalid               ,  // v3u64                  (unsupported)
      MTLVertexFormatInvalid               ,  // v3s64                  (unsupported)
      MTLVertexFormatInvalid               ,  // v3f64                  (unsupported)
      MTLVertexFormatUChar4Normalized      ,  // v4u8_norm              
      MTLVertexFormatChar4Normalized       ,  // v4s8_norm              
      MTLVertexFormatUChar4                ,  // v4u8                   
      MTLVertexFormatChar4                 ,  // v4s8                   
      MTLVertexFormatUShort4Normalized     ,  // v4u16_norm             
      MTLVertexFormatShort4Normalized      ,  // v4s16_norm             
      MTLVertexFormatUShort4               ,  // v4u16                  
      MTLVertexFormatShort4                ,  // v4s16                  
      MTLVertexFormatHalf4                 ,  // v4f16                  
      MTLVertexFormatUInt4                 ,  // v4u32                  
      MTLVertexFormatInt4                  ,  // v4s32                  
      MTLVertexFormatFloat4                ,  // v4f32                  
      MTLVertexFormatInvalid               ,  // v4u64                  (unsupported)
      MTLVertexFormatInvalid               ,  // v4s64                  (unsupported)
      MTLVertexFormatInvalid               ,  // v4f64                  (unsupported)
      MTLVertexFormatInvalid               ,  // v3f11_11_10            (unsupported)
      MTLVertexFormatUInt1010102Normalized ,  // v4u10_10_10_2_norm     
      MTLVertexFormatInt1010102Normalized  ,  // v4s10_10_10_2_norm     
      MTLVertexFormatInvalid               ,  // v4u10_10_10_2          (unsupported)
      MTLVertexFormatInvalid               ,  // v4s10_10_10_2          (unsupported)
      MTLVertexFormatInvalid               ,  // v4u10_10_10_2_norm_rev (unsupported)
      MTLVertexFormatInvalid               ,  // v4s10_10_10_2_norm_rev (unsupported)
      MTLVertexFormatInvalid               ,  // v4u10_10_10_2_rev      (unsupported)
      MTLVertexFormatInvalid               ,  // v4s10_10_10_2_rev      (unsupported)
      };

   // Size of each attribute in memory taking into notice required padding (4bytes on Metal)
   const uint8 AttributeSize[underlyingType(Attribute::Count)] =
      {
      0,    // None           
      0,    // u8_norm                (unsupported)
      0,    // s8_norm                (unsupported)
      0,    // u8                     (unsupported)
      0,    // s8                     (unsupported)
      0,    // u16_norm               (unsupported)
      0,    // s16_norm               (unsupported)
      0,    // u16                    (unsupported)
      0,    // s16                    (unsupported)
      0,    // f16                    (unsupported)
      4,    // u32                   
      4,    // s32                   
      4,    // f32                   
      0,    // u64                    (unsupported)
      0,    // s64                    (unsupported)
      0,    // f64                    (unsupported)
      4,    // v2u8_norm              (2 bytes, 2 bytes padding)
      4,    // v2s8_norm              (2 bytes, 2 bytes padding)
      4,    // v2u8                   (2 bytes, 2 bytes padding)
      4,    // v2s8                   (2 bytes, 2 bytes padding)
      4,    // v2u16_norm            
      4,    // v2s16_norm            
      4,    // v2u16                 
      4,    // v2s16                 
      4,    // v2f16                 
      8,    // v2u32                 
      8,    // v2s32                 
      8,    // v2f32                 
      0,    // v2u64                  (unsupported)
      0,    // v2s64                  (unsupported)
      0,    // v2f64                  (unsupported)
      4,    // v3u8_norm              (3 bytes, 1 byte padding)
      0,    // v3u8_srgb              (unsupported)
      4,    // v3s8_norm              (3 bytes, 1 byte padding)
      4,    // v3u8                   (3 bytes, 1 byte padding)
      4,    // v3s8                   (3 bytes, 1 byte padding)
      8,    // v3u16_norm             (6 bytes, 2 bytes padding)
      8,    // v3s16_norm             (6 bytes, 2 bytes padding)
      8,    // v3u16                  (6 bytes, 2 bytes padding)
      8,    // v3s16                  (6 bytes, 2 bytes padding)
      8,    // v3f16                  (6 bytes, 2 bytes padding)
      12,   // v3u32
      12,   // v3s32
      12,   // v3f32                  
      0,    // v3u64                  (unsupported)
      0,    // v3s64                  (unsupported)
      0,    // v3f64                  (unsupported)
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
      0,    // v4u64                  (unsupported)
      0,    // v4s64                  (unsupported)
      0,    // v4f64                  (unsupported)
      0,    // v3f11_11_10            (unsupported)
      4,    // v4u10_10_10_2_norm    
      4,    // v4s10_10_10_2_norm    
      0,    // v4u10_10_10_2          (unsupported)
      0,    // v4s10_10_10_2          (unsupported)
      0,    // v4u10_10_10_2_norm_rev (unsupported)
      0,    // v4s10_10_10_2_norm_rev (unsupported)
      0,    // v4u10_10_10_2_rev      (unsupported)
      0,    // v4s10_10_10_2_rev      (unsupported)
      };
      
      // OLD:
      //{
      //0,    // None                      
      //0,    // Half                   
      //4,    // Half2                  
      //8,    // Half3               (6 bytes + 2 bytes of padding)
      //8,    // Half4                  
      //4,    // Float                  
      //8,    // Float2                 
      //12,   // Float3                 
      //16,   // Float4                 
      //0,    // Double                 
      //0,    // Double2                
      //0,    // Double3                
      //0,    // Double4                
      //0,    // Int8                   
      //4,    // Int8v2              (2 bytes + 2 bytes of padding)
      //4,    // Int8v3              (3 bytes + 1 byte  of padding)
      //4,    // Int8v4                 
      //0,    // Int16                  
      //4,    // Int16v2                
      //8,    // Int16v3             (6 bytes + 2 bytes of padding)
      //8,    // Int16v4                
      //4,    // Int32                  
      //8,    // Int32v2                
      //12,   // Int32v3                
      //16,   // Int32v4                
      //0,    // Int64                  
      //0,    // Int64v2                
      //0,    // Int64v3                
      //0,    // Int64v4                
      //0,    // UInt8                  
      //4,    // UInt8v2             (2 bytes + 2 bytes of padding)        
      //4,    // UInt8v3             (3 bytes + 1 byte  of padding)
      //4,    // UInt8v4                
      //0,    // UInt16                 
      //4,    // UInt16v2               
      //8,    // UInt16v3            (6 bytes + 2 bytes of padding)
      //8,    // UInt16v4               
      //4,    // UInt32                 
      //8,    // UInt32v2               
      //12,   // UInt32v3               
      //16,   // UInt32v4               
      //0,    // UInt64                 
      //0,    // UInt64v2               
      //0,    // UInt64v3               
      //0,    // UInt64v4               
      //0,    // Float8_SNorm           
      //4,    // Float8v2_SNorm      (2 bytes + 2 bytes of padding)
      //4,    // Float8v3_SNorm      (3 bytes + 1 byte  of padding)
      //4,    // Float8v4_SNorm         
      //0,    // Float16_SNorm          
      //4,    // Float16v2_SNorm        
      //8,    // Float16v3_SNorm     (6 bytes + 2 bytes of padding)
      //8,    // Float16v4_SNorm        
      //0,    // Float8_Norm            
      //4,    // Float8v2_Norm       (2 bytes + 2 bytes of padding)
      //4,    // Float8v3_Norm       (3 bytes + 1 byte  of padding)
      //4,    // Float8v4_Norm          
      //0,    // Float16_Norm           
      //4,    // Float16v2_Norm         
      //8,    // Float16v3_Norm      (6 bytes + 2 bytes of padding)
      //8,    // Float16v4_Norm         
      //4,    // Float4_10_10_10_2_SNorm
      //4     // Float4_10_10_10_2_Norm 
      //};

   uint32 stepRate;  // Rate at which consecutive elements are sourced
   uint32 elementSize; // Size of single element (row size)
   

   InputLayoutMTL::InputLayoutMTL(const DrawableType primitiveType,
                                        const uint32 controlPoints, 
                                        const uint32 usedAttributes, 
                                        const uint32 usedBuffers, 
                                        const AttributeDesc* attributes,  
                                        const BufferDesc* buffers) :
      desc([[MTLVertexDescriptor alloc] init]),
      primitive(primitiveType)
   {
   for(uint32 i=0; i<usedBuffers; ++i)
      {
      // There is no point in using MTLVertexStepFunctionConstant
      // as we can pass the same data using regular buffer and sample
      // it from any shader. There is also no real life case scenario
      // to set "default" const values for missing buffers.

      // MTLVertexBufferLayoutDescriptor
      desc.layouts[i].stepFunction = buffers[i].stepRate == 0U ? MTLVertexStepFunctionPerVertex : MTLVertexStepFunctionPerInstance;
      desc.layouts[i].stepRate     = max(1U, buffers[i].stepRate);
      desc.layouts[i].stride       = buffers[i].elementSize;
      }

   for(uint32 i=0; i<usedAttributes; ++i)
      {
      // MTLVertexAttributeDescriptor
      desc.attributes[i].format      = TranslateAttributeFormat[underlyingType(attributes[i].format)];
      desc.attributes[i].offset      = attributes[i].offset;
      desc.attributes[i].bufferIndex = attributes[i].buffer;
      }
   }
   
   InputLayoutMTL::~InputLayoutMTL()
   {
   // Auto-release pool to ensure that Metal ARC will flush garbage collector
   @autoreleasepool
      {
      [desc release];
      }
   }

   Ptr<InputLayout> MetalDevice::createInputLayout(const DrawableType primitiveType,
                                                   const uint32 controlPoints,
                                                   const uint32 usedAttributes,
                                                   const uint32 usedBuffers,
                                                   const AttributeDesc* attributes,
                                                   const BufferDesc* buffers)
   {
   Ptr<InputLayoutMTL> input = Ptr<InputLayoutMTL>(new InputLayoutMTL(primitiveType, controlPoints, usedAttributes, usedBuffers, attributes, buffers));

   return ptr_dynamic_cast<InputLayout, InputLayoutMTL>(input);
   }


//   Ptr<InputLayout> MetalDevice::create(Ptr<BufferView> buffer)
//   {
//   AttributeDesc* attributes = new AttributeDesc[buffer->attributes];
//   
//   BufferDesc buffers;
//   buffers.elementSize = buffer->elementSize;
//   buffers.stepRate = 0;
//
//   Ptr<InputLayoutMTL> input = nullptr;
//   
//   input = new InputLayoutMTL(buffer->primitiveType, buffer->controlPoints, buffer->attributes, 1, &attributes, &buffers);
//   
//   delete [] attributes;
//   
//   return ptr_dynamic_cast<InputAssember, InputLayoutMTL>(input);
//   }
//
//   Ptr<InputLayout> MetalDevice::create(InputLayoutSettings& attributes)
//   {
//   Ptr<InputLayoutMTL> input = nullptr;
//   
//   input = new InputLayoutMTL(primitiveType, controlPoints,
// 
//                                        const uint32 usedAttributes, 
//                                        const uint32 usedBuffers, 
//                                        const AttributeDesc* attributes,  
//                                        const BufferDesc* buffers
//   
//   return ptr_dynamic_cast<InputAssember, InputLayoutMTL>(input);
//   }






   //  Ptr<InputLayout> Create(AttributeInfo& attribute[MaxInputLayoutAttributesCount],   // Reference to array specifying each vertex attribute, and it's source buffer
   //                             Ptr<Buffer>    buffer[MaxInputLayoutAttributesCount])      // Array of buffer handles that will be used
   //  {
   //  Ptr<InputLayoutMTL> state = new InputLayoutMTL();
   //  
   //  // Create array describing buffers used by Vertex Fetch.
   //  uint32 buffers = 0;
   //  for(; buffers<MaxInputLayoutAttributesCount; ++buffers)
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
   //  uint32 offset[MaxInputLayoutAttributesCount];
   //  memset(&offset, 0, sizeof(uint32) * MaxInputLayoutAttributesCount);
   //  
   //  for(uint32 i=0; i<MaxInputLayoutAttributesCount; ++i)
   //     {
   //     assert( attribute[i].buffer < MaxInputLayoutAttributesCount );
   //  
   //     // Calculate new offset in currently used buffer
   //     uint32 attributeSize = AttributeSize[ attribute[i].type ];
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
   //  return ptr_dynamic_cast<InputLayout, InputLayoutMTL>(state);
   //  }

   }
}

#endif
