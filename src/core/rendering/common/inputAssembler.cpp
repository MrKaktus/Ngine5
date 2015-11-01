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

#include "core/rendering/inputAssembler.h"

namespace en
{
   namespace gpu
   {   


// Attribute::Attribute() :
//    type(None),
//    buffer(0)
// {
// }
// 
// Attribute::Attribute(const AttributeType type) :
//    type(type),
//    buffer(0)
// {
// }
// 
// Attribute::Attribute(const AttributeType type, const uint32 buffer) :
//    type(type),
//    buffer(buffer)
// {
// assert( buffer < MaxInputAssemblerAttributesCount );
// }

//   Ptr<InputAssembler> Create(Attribute* attribute[MaxInputAssemblerAttributesCount],   // Reference to array specifying each vertex attribute, and it's source buffer
//                              Ptr<Buffer> buffer[MaxInputAssemblerAttributesCount])     // Array of buffer handles that will be used
//   {
//   // Compute amount of used attributes
//   uint32 usedAttributes = 0;
//   for(usedAttributes<MaxInputAssemblerAttributesCount; ++usedAttributes)
//      if (attribute[usedAttributes].format == None)
//         break;
//
//   // Compute amount of used buffers
//   uint32 usedBuffers = 0;
//   for(usedBuffers<MaxInputAssemblerAttributesCount; ++usedBuffers)
//      if (!buffer[usedBuffers])
//         break;
//
//   // Create temporary buffers
//   AttributeDesc* attributes = new AttributeDesc[usedAttributes];
//   for(uint32 i=0; i<usedAttributes; ++i)
//      {
//      attributes[i].format = attribute[i].format;
//      attributes[i].buffer = 
//      attributes[i].offset = 
//      }
//
//   BufferDesc* buffers = new BufferDesc[usedBuffers];
//   for(uint32 i=0; i<usedBuffers; ++i)
//      {
//      //Ptr<BufferVK> buffer = ptr_dynamic_cast<BufferVK, Buffer>(buffers[i]);
//      buffers[i].elementSize = 
//      buffers[i].stepRate    = 
//      }
//
//   Ptr<InputAssembler> inputAssembler = nullptr;
//  
//#if defined(EN_PLATFORM_WINDOWS) 
//   inputAssembler = new InputAssemblerD3D12(usedAttributes, attributes, usedBuffers, buffers);
//
//   inputAssembler = new InputAssemblerVK(usedAttributes, attributes, usedBuffers, buffers);
//
//#elif defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
//   inputAssembler = new InputAssemblerMTL(usedAttributes, attributes, usedBuffers, buffers);
//#endif
//
//   // Free temporary buffers
//   delete [] attributes;
//   delete [] buffers;
//
//   return inputAssembler;
//   }
   }
}