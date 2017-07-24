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
   
   // TODO: Verify design decision about slot space per resource type vs shared slot space
   uint32 types = underlyingType(ResourceType::Count);
   uint32* slot = new uint32[types];
   for(uint32 i=0; i<types; ++i)
      slot[i] = 0u;
      
   NSMutableArray* rangeInfo = [NSMutableArray arrayWithCapacity:count];
   
   //MTLArgumentDescriptor* rangeInfo = new MTLArgumentDescriptor[count];
   for(uint32 i=0; i<count; ++i)
      {
      MTLArgumentDescriptor* desc = [MTLArgumentDescriptor alloc];
      
      // Single Descriptors range
      uint32 resourceType = underlyingType(group[i].type);
      
      desc.dataType               = TranslateDataType[resourceType];
      desc.index                  = slot[resourceType];
      desc.arrayLength            = group[i].count;
      desc.access                 = MTLArgumentAccessReadWrite; // Missing information, could optimize it.
                                 // MTLArgumentAccessReadOnly
                                 // MTLArgumentAccessWriteOnly
      desc.textureType            = MTLTextureType2D; // TODO: FIXME: Missing information needed by Metal !
      desc.constantBlockAlignment = 0; // Unsupported by Engine
      
      rangeInfo[i] = desc;
      
      // Increase index by amount of slots used
      slot[resourceType] += group[i].count;
      }
      
   // Convert temporary mutable array, to fixed non-mutable one
   NSArray<MTLArgumentDescriptor *>* arguments = rangeInfo;
   
   // Create arguments encoder (Descriptors Layout)
   id<MTLArgumentEncoder> encoder = [device newArgumentEncoderWithArguments:arguments];
   if (encoder)
      {
      result = new SetLayoutMTL(this, encoder);
      }
      
   return ptr_reinterpret_cast<SetLayout>(&result);
   }
   
   Ptr<Descriptors> MetalDevice::createDescriptorsPool(const uint32 maxSets,
                                                       const uint32 (&count)[underlyingType(ResourceType::Count)])
   {
   // TODO: Finish / Emulate !
   return Ptr<Descriptors>(nullptr);
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
