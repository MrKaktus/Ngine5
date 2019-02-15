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
//
// Metal OSX Vertex Attribute Formats:
// https://developer.apple.com/library/mac/documentation/Metal/Reference/MTLVertexAttributeDescriptor_Ref/#//apple_ref/c/tdef/MTLVertexFormat
// (last verified for Metal on OSX 10.11)
//
// Metal IOS Vertex Attribute Formats:
// https://developer.apple.com/library/ios/documentation/Metal/Reference/MTLVertexAttributeDescriptor_Ref/#//apple_ref/c/tdef/MTLVertexFormat
// (last verified for Metal on IOS 9.0)
//
// All available formats are available since IOS 8.0 or OSX 10.11 unless later version of introduction is explicitly stated.
// From IOS 11.0 and OSX 10.13 those two are unified (there is no distinction between IOS and OSX types).
//
// Type of data in attributes
static const MTLVertexFormat TranslateAttributeFormat[underlyingType(Attribute::Count)] =
{
    MTLVertexFormatInvalid               ,  // None                 
    MTLVertexFormatUCharNormalized       ,  // u8_norm                (10.13+)
    MTLVertexFormatCharNormalized        ,  // s8_norm                (10.13+)
    MTLVertexFormatUChar                 ,  // u8                     (10.13+)
    MTLVertexFormatChar                  ,  // s8                     (10.13+)
    MTLVertexFormatUShortNormalized      ,  // u16_norm               (10.13+)
    MTLVertexFormatShortNormalized       ,  // s16_norm               (10.13+)
    MTLVertexFormatUShort                ,  // u16                    (10.13+)
    MTLVertexFormatShort                 ,  // s16                    (10.13+)
    MTLVertexFormatHalf                  ,  // f16                    (10.13+)
    MTLVertexFormatUInt                  ,  // u32                    
    MTLVertexFormatInt                   ,  // s32                    
    MTLVertexFormatFloat                 ,  // f32                    
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
    MTLVertexFormatUInt3                 ,  // v3u32                  
    MTLVertexFormatInt3                  ,  // v3s32                  
    MTLVertexFormatFloat3                ,  // v3f32                  
  //MTLVertexFormatUChar4Normalized_BGRA ,  // v4u8_norm_rev          (Metal specific type, not exposed, why it is needed?)
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
    MTLVertexFormatUInt1010102Normalized    // v4u10_10_10_2_norm     
};

// Size of each attribute in memory taking into notice required padding (4bytes on Metal)
const uint8 AttributeSize[underlyingType(Attribute::Count)] =
{
    0,    // None           
    4,    // u8_norm                (1 byte, 3 bytes padding - 10.13+)
    4,    // s8_norm                (1 byte, 3 bytes padding - 10.13+)
    4,    // u8                     (1 byte, 3 bytes padding - 10.13+)
    4,    // s8                     (1 byte, 3 bytes padding - 10.13+)
    4,    // u16_norm               (2 bytes, 2 bytes padding - 10.13+)
    4,    // s16_norm               (2 bytes, 2 bytes padding - 10.13+)
    4,    // u16                    (2 bytes, 2 bytes padding - 10.13+)
    4,    // s16                    (2 bytes, 2 bytes padding - 10.13+)
    4,    // f16                    (2 bytes, 2 bytes padding - 10.13+)
    4,    // u32                   
    4,    // s32                   
    4,    // f32                   
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
    4     // v4u10_10_10_2_norm    
};
      
InputLayoutMTL::InputLayoutMTL(
        const DrawableType primitiveType,
        const bool primitiveRestart,
        const uint32 controlPoints,
        const uint32 usedAttributes,
        const uint32 usedBuffers,
        const AttributeDesc* attributes,
        const BufferDesc* buffers) :
    desc(allocateObjectiveC(MTLVertexDescriptor)),
    primitive(primitiveType)
{
    // In Metal API primitive restart is always enabled, and there is
    // no way of disabling it (except of not using max index itself).
   
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
    deallocateObjectiveC(desc);
}

InputLayout* MetalDevice::createInputLayout(
    const DrawableType primitiveType,
    const bool primitiveRestart,
    const uint32 controlPoints,
    const uint32 usedAttributes,
    const uint32 usedBuffers,
    const AttributeDesc* attributes,
    const BufferDesc* buffers)
{
    return new InputLayoutMTL(primitiveType,
                              primitiveRestart,
                              controlPoints,
                              usedAttributes,
                              usedBuffers,
                              attributes,
                              buffers);
}


//   std::shared_ptr<InputLayout> MetalDevice::create(std::shared_ptr<BufferView> buffer)
//   {
//   AttributeDesc* attributes = new AttributeDesc[buffer->attributes];
//   
//   BufferDesc buffers;
//   buffers.elementSize = buffer->elementSize;
//   buffers.stepRate = 0;
//
//   std::shared_ptr<InputLayoutMTL> input = nullptr;
//   
//   input = std::make_shared<InputLayoutMTL>(buffer->primitiveType, buffer->controlPoints, buffer->attributes, 1, &attributes, &buffers);
//   
//   delete [] attributes;
//   
//   return input;
//   }
//
//   std::shared_ptr<InputLayout> MetalDevice::create(InputLayoutSettings& attributes)
//   {
//   std::shared_ptr<InputLayoutMTL> input = nullptr;
//   
//   input = std::make_shared<InputLayoutMTL>(primitiveType, controlPoints,
// 
//                                        const uint32 usedAttributes, 
//                                        const uint32 usedBuffers, 
//                                        const AttributeDesc* attributes,  
//                                        const BufferDesc* buffers
//   
//   return input;
//   }






   //  std::shared_ptr<InputLayout> Create(AttributeInfo& attribute[MaxInputLayoutAttributesCount],   // Reference to array specifying each vertex attribute, and it's source buffer
   //                             std::shared_ptr<Buffer>    buffer[MaxInputLayoutAttributesCount])      // Array of buffer handles that will be used
   //  {
   //  std::shared_ptr<InputLayoutMTL> state = std::make_shared<InputLayoutMTL>();
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
   //     BufferMTL* src = reinterpret_cast<BufferMTL*>(buffer[buffers].get());
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
   //  return state;
   //  }

} // en::gpu
} // en

#endif
