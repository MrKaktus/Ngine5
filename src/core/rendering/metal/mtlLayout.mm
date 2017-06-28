/*

 Ngine v5.0
 
 Module      : Metal Resource Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlLayout.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/mtlDevice.h"
#include "core/rendering/metal/mtlSampler.h"

namespace en
{
   namespace gpu
   {
   static const MTLDataType TranslateDataType[underlyingType(ResourceType::Count)] =
      {
      MTLDataTypeSampler,   // Sampler
      MTLDataTypeTexture,   // Texture
      MTLDataTypeTexture,   // Image
      MTLDataTypePointer,   // Uniform
      MTLDataTypePointer,   // Storage
      };
   
   static const MTLArgumentType TranslateArgumentType[underlyingType(ResourceType::Count)] =
      {
      MTLArgumentTypeSampler,   // Sampler
      MTLArgumentTypeTexture,   // Texture
      MTLArgumentTypeTexture,   // Image
      MTLArgumentTypeBuffer ,   // Uniform
      MTLArgumentTypeBuffer ,   // Storage
      };

    
      
   // stageMask is ignored in Metal
   Ptr<SetLayout> MetalDevice::createSetLayout(const uint32 count,
                                               const ResourceGroup* group,
                                               const ShaderStages stageMask)
   {
   assert( count );
   assert( group );
   
   Ptr<SetLayoutMTL> result = nullptr;

   uint32 types = underlyingType(ResourceType::Count);
   uint32* slot = new uint32[types];
   for(uint32 i=0; i<types; ++i)
      slot[i] = 0u;
      
   NSMutableArray* rangeInfo = [NSMutableArray arrayWithCapacity:count];
   
   //MTLIndirectArgumentDescriptor* rangeInfo = new MTLIndirectArgumentDescriptor[count];
   for(uint32 i=0; i<count; ++i)
      {
      rangeInfo[i] = [MTLIndirectArgumentDescriptor alloc];
      
      // Single Descriptors range
      uint32 resourceType = underlyingType(group[i].type);
     
      rangeInfo[i].argumentType = TranslateArgumentType[resourceType];
    MTLDataType       dataType;
      rangeInfo[i].index       = slot[resourceType];
      rangeInfo[i].arrayLength = group[i].count;
      rangeInfo[i].access      = MTLArgumentAccessReadWrite;
      rangeInfo[i].textureType =
      
      rangeInfo[i].index       = slot[resourceType];
      rangeInfo[i].dataType    = TranslateResourceType[resourceType];
      rangeInfo[i].arrayLength = group[i].count;
      rangeInfo[i].access      = MTLArgumentAccessReadWrite; // Resource access type (read/write/readWrite)
                              // MTLArgumentAccessReadOnly
                              // MTLArgumentAccessReadWrite
                              // MTLArgumentAccessWriteOnly
      rangeInfo[i].textureType =

      // Increase index by amount of slots used
      slot[resourceType] += group[i].count;
      }


   NSArray<MTLIndirectArgumentDescriptor *>* rangeInfo =
   
   //NSArray<MTLIndirectArgumentDescriptor *> *)arguments

   MTLIndirectArgumentDescriptor* descriptor = [MTLIndirectArgumentDescriptor indirectArgumentDescriptor];
   

   
   // TODO: Finish / Emulate !
   return Ptr<SetLayout>(nullptr);
   }

   Ptr<PipelineLayout> MetalDevice::createPipelineLayout(const uint32 sets,
                                                         const Ptr<SetLayout>* set,
                                                         const uint32 immutableSamplers,
                                                         const Ptr<Sampler>* sampler,
                                                         const ShaderStages stageMask)
   {

   id<MTLIndirectArgumentEncoder> layout = nil;

   // - (id<MTLIndirectArgumentEncoder>)newIndirectArgumentEncoderWithArguments:(NSArray<MTLIndirectArgumentDescriptor *> *)arguments;
   
   // TODO: Finish / Emulate !
   return Ptr<PipelineLayout>(nullptr);
   }
   
   }
}
#endif
