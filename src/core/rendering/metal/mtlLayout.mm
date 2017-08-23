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
#include "core/rendering/metal/mtlBuffer.h"
#include "core/rendering/metal/mtlSampler.h"
#include "core/rendering/metal/mtlTexture.h"
#include "core/rendering/metal/mtlCommandBuffer.h"

#include <algorithm>

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
      
      
   // SET LAYOUT
   //////////////////////////////////////////////////////////////////////////
   
   
   SetLayoutMTL::SetLayoutMTL(id<MTLArgumentEncoder> encoder, const ShaderStages _stageMask, const uint32 _descriptors) :
      handle(encoder),
      stageMask(_stageMask),
      descriptors(_descriptors)
   {
   }
   
   SetLayoutMTL::~SetLayoutMTL()
   {
   // Auto-release pool to ensure that Metal ARC will flush garbage collector
   @autoreleasepool
      {
      [handle release];
      }
   }
   
      
   // PIPELINE LAYOUT
   //////////////////////////////////////////////////////////////////////////
   
   
   PipelineLayoutMTL::PipelineLayoutMTL(MetalDevice* _gpu) :
      gpu(_gpu)
   {
   }
   
   PipelineLayoutMTL::~PipelineLayoutMTL()
   {
   }
   
   
   // DESCRIPTOR SET
   //////////////////////////////////////////////////////////////////////////
   
   
   DescriptorSetMTL::DescriptorSetMTL(MetalDevice* _gpu, Ptr<SetLayoutMTL> _layout) :
      gpu(_gpu),
      layout(_layout),
      handle(nullptr),
      heapId(nullptr),
      heapsUsed(nullptr),
      heapsRefs(nullptr),
      heapsCount(0)
   {
   MTLResourceOptions options = (MTLCPUCacheModeDefaultCache << MTLResourceCPUCacheModeShift); // MTLCPUCacheModeWriteCombined
   
   // ArgumentBuffer need to be CPU accessible all the time,
   // and it's VRAM copy is explicitly managed by the Driver,
   // thus storage mode needs to be Shared or Managed.
   // Because of the above, it is suballocated directly from
   // Device memory as engine is not supporting Managed Heaps.
#if defined(EN_PLATFORM_IOS)
   options |= (MTLStorageModeShared << MTLResourceStorageModeShift);
#else
   // TODO: This requires manual syncs! (on set descriptors)
   options |= (MTLStorageModeManaged << MTLResourceStorageModeShift);
#endif

   handle = [gpu->device newBufferWithLength:(NSUInteger)[layout->handle encodedLength]
                                     options:options];
                                     
   // Per Descriptor array of Id's referencing slots in array of required Heaps
   heapId = new uint8[layout->descriptors];
   memset(heapId, 0, layout->descriptors);
   
   // Array of id<MTLHeap> pointers, representing backing heaps for bound resources
   // As Id's are of type uint8, each DescriptorSet can use max of 256 backing Heaps.
   heapsUsed = new id<MTLHeap>[256];
   
   heapsRefs = new uint32[256];
   memset(heapsRefs, 0, sizeof(uint32) * layout->descriptors);
   }
   
   DescriptorSetMTL::~DescriptorSetMTL()
   {
   delete [] heapId;
   delete [] heapsUsed;
   delete [] heapsRefs;
      
   // Auto-release pool to ensure that Metal ARC will flush garbage collector
   @autoreleasepool
      {
      [handle release];
      }
      
   layout = nullptr;
   }
   
   // To track residency, of resources currently bound, to this DescriptorSet,
   // we can bookkeep list of their backing Heaps, and then just make those
   // Heaps resident when DescriptorSet is bound to be used.
   void DescriptorSetMTL::updateResidencyTracking(const uint32 slot, const id<MTLHeap> heap)
   {
   // If currently bound resource backing Heap is the same
   // Heap that was backing resource previously bound to
   // this descriptor, then no action is needed.
   if (heapsUsed[heapId[slot]] == heap)
      return;
      
   // This Descriptor is no longer referencing that Heap.
   // If Heap backing previously bound resource to this
   // DescriptorSlot, is no longer used by any other bound
   // resource, it can be removed from the list.
   if (heapsRefs[heapId[slot]] > 0)
      {
      heapsRefs[heapId[slot]]--;
      if (heapsRefs[heapId[slot]] == 0)
         {
         heapsUsed[heapId[slot]] = nullptr;
         if (heapId[slot] == (heapsCount-1))
            heapsCount--;
         }
      }
      
   // If new resource backing Heap, is already backing some
   // other resources bound in this DescriptorSet, Heap index
   // of this Descriptor is fixed to that Heap slot and Heap
   // references counter is increased.
   for(uint32 i=0; i<heapsCount; ++i)
      if (heapsUsed[i] == heap)
         {
         heapId[slot] = i;
         heapsRefs[i]++;
         return;
         }
         
   // Max 256 different Heaps can back all bound resources.
   assert( heapsCount < 256 );
   
   // Newly bounds resource backing Heap is not cached yet.
   for(uint32 i=0; i<heapsCount; ++i)
      if (heapsUsed[i] == nullptr)
         {
         heapId[slot] = i;
         heapsRefs[i] = 1;
         heapsUsed[i] = heap;
         return;
         }
         
   // All slots are tightly packed, so use one at the end.
   heapId[slot] = heapsCount;
   heapsRefs[heapsCount] = 1;
   heapsUsed[heapsCount] = heap;
   heapsCount++;
   }

   void DescriptorSetMTL::setBuffer(const uint32 slot, const Ptr<Buffer> buffer)
   {
   // TODO: Shouldn't above global slot be recomputed to local Buffer slot?
   //       Is this "bind point index" global or local?
   
   // TODO: Layout mutex lock, to prevent of other thread corrupting currently set buffer for encoding
   [layout->handle setArgumentBuffer:handle offset:0];
   
   // TODO: Buffer offsets are not supported yet. Maybe use BufferView for that?
   BufferMTL* ptr = raw_reinterpret_cast<BufferMTL>(&buffer);
   [layout->handle setBuffer:ptr->handle offset:0 atIndex:slot];
   
   // TODO: Unlock layout mutex
   
   updateResidencyTracking(slot, ptr->heap->handle);
   }
   
   void DescriptorSetMTL::setSampler(const uint32 slot, const Ptr<Sampler> sampler)
   {
   // TODO: Shouldn't above global slot be recomputed to local Sampler slot?
   //       Is this "bind point index" global or local?
   
   // TODO: Layout mutex lock, to prevent of other thread corrupting currently set buffer for encoding
   [layout->handle setArgumentBuffer:handle offset:0];
   
   SamplerMTL* ptr = raw_reinterpret_cast<SamplerMTL>(&sampler);
   [layout->handle setSamplerState:ptr->handle atIndex:slot];

   // TODO: Unlock layout mutex
   }
   
   void DescriptorSetMTL::setTextureView(const uint32 slot, const Ptr<TextureView> view)
   {
   // TODO: Shouldn't above global slot be recomputed to local TextureView slot?
   //       Is this "bind point index" global or local?
   
   // TODO: Layout mutex lock, to prevent of other thread corrupting currently set buffer for encoding
   [layout->handle setArgumentBuffer:handle offset:0];
   
   TextureViewMTL* ptr = raw_reinterpret_cast<TextureViewMTL>(&view);
   [layout->handle setTexture:ptr->handle atIndex:slot];
   
   // TODO: Unlock layout mutex
   
   updateResidencyTracking(slot, ptr->texture->heap->handle);
   }
   
   // TODO: Consider in future exposing API that allows binding N resources at the same time
   //void DescriptorSetMTL::setTextureViews(const uint32 slot, const uint32 count, const Ptr<TextureView>* views)
   //{
   //assert( views );
   //[layout->handle setBuffers:(const id <MTLBuffer> __nullable [__nonnull])buffers offsets:(const NSUInteger [__nonnull])offsets withRange:NSMakeRange(slot, count)];
   //[layout->handle setTextures:(const id <MTLTexture> __nullable [__nonnull])textures withRange:NSMakeRange(slot, count)];
   //[layout->handle setSamplerStates:(const id <MTLSamplerState> __nullable [__nonnull])samplers withRange:NSMakeRange(slot, count)];
   //}
   
   
   // DESCRIPTOR POOL
   //////////////////////////////////////////////////////////////////////////
   
   
   DescriptorsMTL::DescriptorsMTL(MetalDevice* _gpu) :
      gpu(_gpu)
   {
   }
   
   DescriptorsMTL::~DescriptorsMTL()
   {
   }
   
   Ptr<DescriptorSet> DescriptorsMTL::allocate(const Ptr<SetLayout> layout)
   {
   Ptr<DescriptorSetMTL> result = nullptr;
   
   // TODO: In Metal, we allocate DescriptorSets directly from Device memory,
   //       thus Descriptors container is not needed. Consider hiding this
   //       abstraction from API, the same way like CommandAllocators.
   result = new DescriptorSetMTL(gpu, ptr_reinterpret_cast<SetLayoutMTL>(&layout));
   
   return ptr_reinterpret_cast<DescriptorSet>(&result);
   }
   
   bool DescriptorsMTL::allocate(const uint32 count,
                                 const Ptr<SetLayout>* layouts,
                                 Ptr<DescriptorSet>** sets)
   {
   // Allocate group of Descriptor Sets from Desriptor Pool
   *sets = new Ptr<DescriptorSet>[count];
   for(uint32 i=0; i<count; ++i)
      {
      (*sets)[i] = allocate(layouts[i]);
      if ((*sets)[i] == nullptr)
         {
         // Release already allocated DescriptorSets
         for(uint32 j=0; j<i; ++j)
            (*sets)[i] = nullptr;
            
         // Delete array of pointers and reset return pointer to null
         delete [] *sets;
         *sets = nullptr;
         return false;
         }
      }
      
   return true;
   }
   
      
   // COMMAND BUFFER
   //////////////////////////////////////////////////////////////////////////
   
   
   void CommandBufferMTL::setDescriptors(const Ptr<PipelineLayout> _layout,
                                         const Ptr<DescriptorSet> _set,
                                         const uint32 index)
   {
   assert( renderEncoder );
   assert( _layout );
   assert( _set );
   
   PipelineLayoutMTL* layout = raw_reinterpret_cast<PipelineLayoutMTL>(&_layout);
   DescriptorSetMTL*  set    = raw_reinterpret_cast<DescriptorSetMTL>(&_set);
   
   // TODO: Verify that index is in range of the layout
   // assert( layout->setsCount > index );
   
   // TODO: Verify that set is based on setLayout that matches N'th setLayout from PipelineLayout
   
   // Bind descriptor set only to pipeline stage it is visible in
   uint32 mask = underlyingType(set->layout->stageMask);
   if (checkBitmask(mask, underlyingType(ShaderStages::Vertex)))
      {
      [renderEncoder setVertexBuffer:set->handle
                              offset:0
                             atIndex:index];
      }
   if (checkBitmask(mask, underlyingType(ShaderStages::Fragment)))
      {
      [renderEncoder setFragmentBuffer:set->handle
                                offset:0
                               atIndex:index];
      }
      
   // TODO: Binding for Compute Shader
   // setBuffer:offset:atIndex:
                          
   // Ensure that Heaps that are backing all bound resources are resident in GPU VRAM
   for(uint32 i=0; i<set->heapsCount; ++i)
      if (set->heapsUsed[i] != nullptr)
         [renderEncoder useHeap:set->heapsUsed[i]];
   }
                                  
   void CommandBufferMTL::setDescriptors(const Ptr<PipelineLayout> layout,
                                         const uint32 count,
                                         const Ptr<DescriptorSet>* sets,
                                         const uint32 firstIndex)
   {
   // TODO: Finish!
   }
   
   

                                  
   // DEVICE
   //////////////////////////////////////////////////////////////////////////
   
   
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
      MTLArgumentDescriptor* desc = [[MTLArgumentDescriptor alloc] init];
      
      // Single Descriptors range
      uint32 resourceType = underlyingType(group[i].type);
      
      desc.dataType               = TranslateDataType[resourceType];
      desc.index                  = slot[resourceType];
      desc.arrayLength            = group[i].count;
      desc.access                 = MTLArgumentAccessReadOnly; // RO indicates Texture, Buffer
                                 // MTLArgumentAccessReadWrite // RW indicates Image/UAV
                                 // MTLArgumentAccessWriteOnly // WO indicates Image/UAV
      desc.textureType            = TranslateTextureType[underlyingType(group[i].textureType)];
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
      uint32 descriptors = 0;
      for(uint32 i=0; i<types; ++i)
         descriptors += slot[i];
         
      result = new SetLayoutMTL(encoder, stageMask, descriptors);
      }
      
   return ptr_reinterpret_cast<SetLayout>(&result);
   }
   
   Ptr<PipelineLayout> MetalDevice::createPipelineLayout(const uint32 sets,
                                                         const Ptr<SetLayout>* set,
                                                         const uint32 immutableSamplers,
                                                         const Ptr<Sampler>* sampler,
                                                         const ShaderStages stageMask)
   {
   Ptr<PipelineLayoutMTL> result = nullptr;
   
   MTLMutability mutability = MTLMutabilityImmutable;

   result = new PipelineLayoutMTL(this);
   result->setsCount = sets;
   
   
   // TODO: Finish / Emulate !
   
   return ptr_reinterpret_cast<PipelineLayout>(&result);
   }
   
   Ptr<Descriptors> MetalDevice::createDescriptorsPool(const uint32 maxSets,
                                                       const uint32 (&count)[underlyingType(ResourceType::Count)])
   {
   Ptr<DescriptorsMTL> result = nullptr;
   
   // TODO: In Metal, we allocate DescriptorSets directly from Device memory,
   //       thus Descriptors container is not needed. Consider hiding this
   //       abstraction from API, the same way like CommandAllocators.
   result = new DescriptorsMTL(this);
   
   return ptr_reinterpret_cast<Descriptors>(&result);
   }
   
   }
}
#endif
