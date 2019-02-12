/*

 Ngine v5.0
 
 Module      : D3D12 Resource Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12Layout.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/memory/alignedAllocator.h"
#include "core/rendering/d3d12/dx12Device.h"
#include "core/rendering/d3d12/dx12CommandBuffer.h"
#include "core/rendering/d3d12/dx12Buffer.h"
#include "core/rendering/d3d12/dx12Sampler.h"
#include "core/rendering/d3d12/dx12Texture.h"
#include "core/utilities/basicAllocator.h"

namespace en
{
   namespace gpu
   { 
   static const D3D12_DESCRIPTOR_RANGE_TYPE TranslateResourceType[underlyingType(ResourceType::Count)] =
      {
      D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, // Sampler
      D3D12_DESCRIPTOR_RANGE_TYPE_SRV,     // Texture
      D3D12_DESCRIPTOR_RANGE_TYPE_UAV,     // Image
      D3D12_DESCRIPTOR_RANGE_TYPE_CBV,     // Uniform
      D3D12_DESCRIPTOR_RANGE_TYPE_UAV,     // Storage
      };




   // SET LAYOUT
   //////////////////////////////////////////////////////////////////////////


   SetLayoutD3D12::SetLayoutD3D12() :
      type(nullptr),
      tablesCount(0)
   {
   }

   SetLayoutD3D12::~SetLayoutD3D12()
   {
   delete [] type;
   }


   // PIPELINE LAYOUT
   //////////////////////////////////////////////////////////////////////////


   PipelineLayoutD3D12::PipelineLayoutD3D12(ID3D12RootSignature* _handle, const uint32 _sets, uint32* _setBindingIndex) :
      handle(_handle),
      sets(_sets),
      setBindingIndex(_setBindingIndex)
   {
   }

   PipelineLayoutD3D12::~PipelineLayoutD3D12()
   {
   assert( handle );
   ValidateCom( handle->Release() )
   handle = nullptr;

   if (setBindingIndex)
      delete [] setBindingIndex;
   }


   // DESCRIPTOR SET
   //////////////////////////////////////////////////////////////////////////

   DescriptorSetD3D12::DescriptorSetD3D12(Direct3D12Device* _gpu, std::shared_ptr<DescriptorsD3D12> _parent, const uint64* _offsets, const uint32* _slots, RangeMapping* const _mappings, uint32 _mappingsCount) :
      gpu(_gpu),
      parent(_parent),
      mappings(_mappings),
      mappingsCount(_mappingsCount)
   {
   for(uint32 i=0; i<2; ++i)
      {
      offset[i] = _offsets[i];
      slots[i]  = _slots[i];
      }
   }

   DescriptorSetD3D12::~DescriptorSetD3D12()
   {
   // Deallocate sub-allocated ranges from parent DescriptorHeap
   for(uint32 i=0; i<2; ++i)
      if (slots[i])
         parent->allocator[i]->deallocate(offset[i], slots[i]);

   // Free mappings array
   deallocate<RangeMapping>(mappings);

   parent = nullptr;
   gpu    = nullptr;
   }

   bool DescriptorSetD3D12::translateSlot(const uint32 slot, uint32& heapSlot)
   {
   bool found = false;
   for(uint32 i=0; i<mappingsCount; ++i)
      if (mappings[i].layoutOffset <= slot &&
          (mappings[i].layoutOffset + mappings[i].elements) > slot)
         {
         uint32 offset = slot - mappings[i].layoutOffset;
         heapSlot = mappings[i].heapOffset + offset;
         found = true;
         break;
         }

   return found;
   }

   void DescriptorSetD3D12::setBuffer(const uint32 slot, const Buffer& _buffer)
   {
   // Convert slot in DescriptorSet to slot in Heap
   uint32 heapSlot = 0;
   if (!translateSlot(slot, heapSlot))
      return;

   const BufferD3D12& buffer = reinterpret_cast<const BufferD3D12&>(_buffer);

   // TODO: In future expose true BufferViews, allowing mapping ranges into descriptor
   D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
   desc.BufferLocation = buffer.handle->GetGPUVirtualAddress();
   desc.SizeInBytes    = static_cast<UINT>(buffer.size);

   ValidateNoRet( gpu, CreateConstantBufferView(&desc, parent->pointerToDescriptorOnCPU(heapSlot)) )
   }

   void DescriptorSetD3D12::setSampler(const uint32 slot, const Sampler& _sampler)
   {
   // Convert slot in DescriptorSet to slot in Heap
   uint32 heapSlot = 0;
   if (!translateSlot(slot, heapSlot))
      return;

   // Sampler objects are created in D3D12 by encoding their state directly 
   // into Descriptor in DescriptorSet's backing DescriptorPool (Heap)
   const SamplerD3D12& sampler = reinterpret_cast<const SamplerD3D12&>(_sampler);
   ValidateNoRet( gpu, CreateSampler(&sampler.state, parent->pointerToSamplerDescriptorOnCPU(heapSlot)) )
   }

   void DescriptorSetD3D12::setTextureView(const uint32 slot, const TextureView& _view)
   {
   // Convert slot in DescriptorSet to slot in Heap
   uint32 heapSlot = 0;
   if (!translateSlot(slot, heapSlot))
      return;

   // Views are created in D3D12 by encoding them directly into 
   // Descriptor in DescriptorSet's backing DescriptorPool (Heap)
   const TextureViewD3D12& view = reinterpret_cast<const TextureViewD3D12&>(_view);
   ValidateNoRet( gpu, CreateShaderResourceView(view.texture.handle, &view.desc, parent->pointerToDescriptorOnCPU(heapSlot)) )
   }

   // DESCRIPTOR POOL
   //////////////////////////////////////////////////////////////////////////


   DescriptorsD3D12::DescriptorsD3D12(Direct3D12Device* _gpu) :
      gpu(_gpu)
   {
   assert( gpu );
   descriptorSize        = gpu->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
   samplerDescriptorSize = gpu->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
   }
   
   DescriptorsD3D12::~DescriptorsD3D12()
   {
   assert( handle[0] || handle[1] );

   for(uint32 i=0; i<2; ++i)
      {
      if (handle[i])
         {
         ValidateCom( handle[i]->Release() )
         handle[i] = nullptr;
         }

      if (allocator[i])
         delete allocator[i];
      }

   gpu = nullptr;
   }

   // D3D12_CPU_DESCRIPTOR_HANDLE is 64bit pointer on CPU memory, where descriptor is located in the CPU copy of the Descriptors Heap
   D3D12_CPU_DESCRIPTOR_HANDLE DescriptorsD3D12::pointerToDescriptorOnCPU(uint32 index)
   {
   assert( handle[0] );
   D3D12_CPU_DESCRIPTOR_HANDLE firstHandle = handle[0]->GetCPUDescriptorHandleForHeapStart();
   firstHandle.ptr += (descriptorSize * index); 
   return firstHandle;
   }

   D3D12_GPU_DESCRIPTOR_HANDLE DescriptorsD3D12::pointerToDescriptorOnGPU(uint32 index)
   {
   assert( handle[0] );
   D3D12_GPU_DESCRIPTOR_HANDLE firstHandle = handle[0]->GetGPUDescriptorHandleForHeapStart();
   firstHandle.ptr += (descriptorSize * index);
   return firstHandle;
   }

   D3D12_CPU_DESCRIPTOR_HANDLE DescriptorsD3D12::pointerToSamplerDescriptorOnCPU(uint32 index)
   {
   assert( handle[1] );
   D3D12_CPU_DESCRIPTOR_HANDLE firstHandle = handle[1]->GetCPUDescriptorHandleForHeapStart();
   firstHandle.ptr += (samplerDescriptorSize * index);
   return firstHandle;
   }

   D3D12_GPU_DESCRIPTOR_HANDLE DescriptorsD3D12::pointerToSamplerDescriptorOnGPU(uint32 index)
   {
   assert( handle[1] );
   D3D12_GPU_DESCRIPTOR_HANDLE firstHandle = handle[1]->GetGPUDescriptorHandleForHeapStart();
   firstHandle.ptr += (samplerDescriptorSize * index);
   return firstHandle;
   }

   std::shared_ptr<DescriptorSet> DescriptorsD3D12::allocate(const std::shared_ptr<SetLayout> _layout)
   {
   std::shared_ptr<DescriptorSetD3D12> result = nullptr;

   assert( _layout );
   
   SetLayoutD3D12* layout = reinterpret_cast<SetLayoutD3D12*>(_layout.get());

   // In Vulkan there are 11 different types of Descriptors. Each type has separate range in pool, 
   // from which it is allocated, but they all share single pool/heap. In D3D12 there are 4 different 
   // types of Descriptors. Descriptors Pool is backed by two Heaps, one for Samplers, and one for all 
   // other types of resource descriptors:
   //
   // D3D12_DESCRIPTOR_RANGE_TYPE_SRV
   // D3D12_DESCRIPTOR_RANGE_TYPE_UAV
   // D3D12_DESCRIPTOR_RANGE_TYPE_CBV

   // Index of first Descriptor Slot for given allocation
   uint64 descriptorIndex[2];
   descriptorIndex[0] = 0;
   descriptorIndex[1] = 0;

   // Allocates Descriptors from the General and Samplers pools depending on actual need.
   // If any of those allocations will fail, partial allocations are released and whole
   // creation terminated.
   bool allocated = true;
   for(uint32 i=0; i<2; ++i)
      {
      if (layout->descriptors[i])
         if (!allocator[i]->allocate(layout->descriptors[i], 0, descriptorIndex[i]))
            {
            if (i == 1 && layout->descriptors[0] > 0)
               allocator[0]->deallocate(descriptorIndex[0], layout->descriptors[0]);
            allocated = false;
            break;
            }
      }

   if (!allocated)
      return result;

   uint32 rangesCount = layout->mappingsCount;

   // Copy array of RangeMappings from SetLayout
   RangeMapping* mappings = en::allocate<RangeMapping>(rangesCount, cacheline);
   memcpy(mappings, layout->mappings, rangesCount * sizeof(RangeMapping));

   // Patch copied array with newly allocated heapOffsets.
   // Relative offsets in sub-allocations are computed during
   // layout creation time, so now, they just need to get 
   // added global starting offset in given Heap.
   for(uint32 i=0; i<rangesCount; ++i)
      mappings[i].heapOffset += descriptorIndex[mappings[i].heap];

   result = std::make_shared<DescriptorSetD3D12>(gpu,
                                            std::dynamic_pointer_cast<DescriptorsD3D12>(shared_from_this()),
                                            &descriptorIndex[0],
                                            &layout->descriptors[0],
                                            mappings,
                                            rangesCount);

   return result;
   }
   
   bool DescriptorsD3D12::allocate(const uint32 count,
                                   const std::shared_ptr<SetLayout>(&layouts)[],
                                   std::shared_ptr<DescriptorSet>** sets)
   {
   // Allocate group of Descriptor Sets from Desriptor Pool
   *sets = new std::shared_ptr<DescriptorSet>[count];
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


   void CommandBufferD3D12::setDescriptors(const PipelineLayout& _layout,
                                           const DescriptorSet& _set,
                                           const uint32 index)
   {
   assert( started );

   const PipelineLayoutD3D12& layout = reinterpret_cast<const PipelineLayoutD3D12&>(_layout);
   const DescriptorSetD3D12&  set    = reinterpret_cast<const DescriptorSetD3D12&>(_set);

   assert( layout.sets > index );

   // TODO: Support Compute!!!
   ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

   // There are only two Heaps that can be set at a time, general and for Samplers.
   // Each change of those Heaps, removes previously bound ones.
   ID3D12DescriptorHeap* ppHeaps[] = {set.parent->handle[0], set.parent->handle[1] };
   ValidateComNoRet( command->SetDescriptorHeaps(2, ppHeaps ) )  // (ID3D12DescriptorHeap*const*)(&set->parent->handle[0])

   // Each DescriptorSet may be backed by up to two D3D12 Descriptor Tables pointing
   // into two separate D3D12 Descriptor Heaps, depending on the fact if it is keeping 
   // Sampler descriptors mixed with different ones, just Sampler Descriptors or just
   // all other ones.
   // Thus for each logical Set creating layout, it's binding index is computed at
   // layout creation time.
   uint32 bindIndex = 0;
   if (index > 0)
      bindIndex = layout.setBindingIndex[index];

   if (set.slots[0])
      {
      ValidateComNoRet( command->SetGraphicsRootDescriptorTable(bindIndex, set.parent->pointerToDescriptorOnGPU(set.offset[0])) )
      bindIndex++;
      }
   if (set.slots[1])
      ValidateComNoRet( command->SetGraphicsRootDescriptorTable(bindIndex, set.parent->pointerToSamplerDescriptorOnGPU(set.offset[1])) )
   }

   void CommandBufferD3D12::setDescriptors(const PipelineLayout& _layout,
                                           const uint32 count,
                                           const std::shared_ptr<DescriptorSet>(&sets)[],
                                           const uint32 firstIndex)
   {
   assert( started );

   assert( count );

   const PipelineLayoutD3D12& layout = reinterpret_cast<const PipelineLayoutD3D12&>(_layout);

   assert( layout.sets >= firstIndex + count );

   // TODO: Support Compute!!!
   ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

   // Each DescriptorSet may be backed by up to two D3D12 Descriptor Tables pointing
   // into two separate D3D12 Descriptor Heaps, depending on the fact if it is keeping 
   // Sampler descriptors mixed with different ones, just Sampler Descriptors or just
   // all other ones.
   // Thus for each logical Set creating layout, it's binding index is computed at
   // layout creation time.
   uint32 bindIndex = 0;
   for(uint32 i=0; i<count; ++i)
      {
      DescriptorSetD3D12* set = reinterpret_cast<DescriptorSetD3D12*>(sets[i].get());

      uint32 index = firstIndex + i;
      if (index > 0)
         bindIndex = layout.setBindingIndex[index];
      
      if (set->slots[0])
         {
         ValidateComNoRet( command->SetGraphicsRootDescriptorTable(bindIndex, set->parent->pointerToDescriptorOnGPU(set->offset[0])) )
         bindIndex++;
         }
      if (set->slots[1])
         ValidateComNoRet( command->SetGraphicsRootDescriptorTable(bindIndex, set->parent->pointerToSamplerDescriptorOnGPU(set->offset[1])) )
      }
   }


   // DEVICE
   //////////////////////////////////////////////////////////////////////////


   //// Optimisation: This table is not needed. Frontend type can be directly cast to D3D12 type.
   //// https://msdn.microsoft.com/en-us/library/windows/desktop/dn879482(v=vs.85).aspx
   //const D3D12_SHADER_VISIBILITY TranslateShaderStage[underlyingType(ShaderStage::Count)] =
   //   {
   //   D3D12_SHADER_VISIBILITY_ALL                 ,  // All
   //   D3D12_SHADER_VISIBILITY_VERTEX              ,  // Vertex
   //   D3D12_SHADER_VISIBILITY_HULL                ,  // Control
   //   D3D12_SHADER_VISIBILITY_DOMAIN              ,  // Evaluation
   //   D3D12_SHADER_VISIBILITY_GEOMETRY            ,  // Geometry
   //   D3D12_SHADER_VISIBILITY_PIXEL                  // Fragment
   //   };
         
   std::shared_ptr<SetLayout> Direct3D12Device::createSetLayout(const uint32 count, 
                                                           const ResourceGroup* group,
                                                           const ShaderStages stageMask)
   {
   assert( count );
   assert( group );

   std::shared_ptr<SetLayoutD3D12> result = std::make_shared<SetLayoutD3D12>();

   // Total count of descriptors per Descriptor Table
   uint32 descriptors[2];
   descriptors[0] = 0;
   descriptors[1] = 0;

   // Each DescriptorSet may be backed by up to two D3D12 Descriptor Tables pointing
   // into two separate D3D12 Descriptor Heaps, depending on the fact if it is keeping 
   // Sampler descriptors mixed with different ones, just Sampler Descriptors or just
   // all other ones.
   // Thus all Sampler groups need to be separated from other types of descriptors,
   // and placed in separate Descriptor Table. To maintain logical Set layout matching
   // Vulkan, both Descriptor Tables will share the same HLSL space.
   //
   // To perform quick translation of logical index in Set, to internal index in one
   // of the two Heap sub-allocations, descriptors backed by the same heap are grouped
   // into mapping range. Each time desriptors change between general and samplers,
   // new mapping range is describing that.
   uint32 generalGroups = 0;
   uint32 samplerGroups = 0;
   uint32 rangesCount   = 1;
   bool   general       = true;
   if (group[0].type == ResourceType::Sampler)
      general = false;
   for(uint32 i=0; i<count; ++i)
      {
      // Calculate amount of required Descriptor Tables
      if (group[i].type == ResourceType::Sampler)
         {
         descriptors[1] += group[i].count;
         samplerGroups++;

         if (general)
            {
            general = false;
            rangesCount++;
            }
         }
      else
         {
         descriptors[0] += group[i].count;
         generalGroups++;

         if (!general)
            {
            general = true;
            rangesCount++;
            }
         }
      }

   // Allocate mapping ranges 
   result->mappingsCount = rangesCount;
   result->mappings      = new RangeMapping[rangesCount];

   // Images and Storage buffers are both handled as UAV's so there should be 
   // exactly 4 types of resource to be backed. Each type of resource has it's
   // own separate logical register space inside of single HLSL "space".
   // See: https://msdn.microsoft.com/en-us/library/windows/desktop/dn899207(v=vs.85).aspx  
   //
   // Metal and Vulkan have shared register spaces inside of their Descriptor Sets,
   // thus even though in HLSL they are separate per type, assigned register indexes
   // will be unique, mimicking shared register space.
   //
   // TODO: Check what impact this type of emulation has on performance!
   //       If it will be terrible, need to figure out different abstraction
   //        model that fits all three API's.
#if EN_SEPARATE_REGISTER_SPACES
   // For separate register spces:
   uint32 slot[4];
      
   static_assert(sizeof(slot) == ((underlyingType(ResourceType::Count) - 1) * sizeof(uint32)), "Amount of ResourceTypes is not matching!");

   memset(&slot, 0, sizeof(slot));
#else
   uint32 slot = 0;
#endif

   // Ranges array for general Descriptor Table
   uint32 generalRangeIndex = 0;
   D3D12_DESCRIPTOR_RANGE* generalRange = nullptr;              //D3D12_DESCRIPTOR_RANGE1* v1.1
   if (generalGroups)
      generalRange = new D3D12_DESCRIPTOR_RANGE[generalGroups]; //new D3D12_DESCRIPTOR_RANGE1[generalGroups]; v1.1

   // Ranges array for Sampler Descriptor Table
   uint32 samplerRangeIndex = 0;
   D3D12_DESCRIPTOR_RANGE* samplerRange = nullptr;              //D3D12_DESCRIPTOR_RANGE1* v1.1
   if (samplerGroups)
      samplerRange = new D3D12_DESCRIPTOR_RANGE[samplerGroups]; //new D3D12_DESCRIPTOR_RANGE1[samplerGroups]; v1.1

   // RangeMappings gather together all descriptors backed by the same Heap, 
   // thus they are not matching ResourceGroups, and make search for binding faster.
   uint32 rangeIndex = 0;
   result->mappings[rangeIndex].layoutOffset = 0;  // First Descriptor offset in Layout
   result->mappings[rangeIndex].heapOffset   = 0;  // First Descriptor offset in Heap (layoutOffset + heapStartingOffset)
   result->mappings[rangeIndex].elements     = 0;  // Size of range in Descriptors
   result->mappings[rangeIndex].heap         = 0;  // 0 - General, 1 - Samplers

   general = true;
   if (group[0].type == ResourceType::Sampler)
      {
      general = false;
      result->mappings[rangeIndex].heap = 1;
      }

   uint32 generalDescriptors = 0;
   uint32 samplerDescriptors = 0;

   for(uint32 i=0; i<count; ++i)
      {
      uint32 resourceType = underlyingType(group[i].type);

      if (group[i].type == ResourceType::Sampler)
         {
         samplerRange[samplerRangeIndex].RangeType          = TranslateResourceType[resourceType];
         samplerRange[samplerRangeIndex].NumDescriptors     = group[i].count;     // UINT - -1 or UINT_MAX to specify unbounded size (only last entry)

         // Register Slots are assigned in order of Resource Group declarations.
         // Register Space is set during Root Signature creation, in order in
         // which Descriptor Sets are assigned to Pipeline Layout.
#if EN_SEPARATE_REGISTER_SPACES
         samplerRange[samplerRangeIndex].BaseShaderRegister = slot[resourceType];
#else
         samplerRange[samplerRangeIndex].BaseShaderRegister = slot;
#endif
         samplerRange[samplerRangeIndex].RegisterSpace      = 0u;                 // To be patched.

         // Defines offset from first Descriptor in the Descriptors Heap, used by 
         // this Descriptors Table (Set Layout). Each range can have it's own offset
         // if separately sub-allocated from the Heap, or can have it set to:
         // D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND - immediatelly follow previous range.
         //
         // Because each sub-allocation of Descriptor Ranges for DescriptorSet 
         // matching this SetLayout would result in different offsets between Ranges,
         // and at the same time, those offsets need to be known during Root Signature
         // creation, all CBV, SRV and UAV descriptors need to be allocated on the
         // Heap as single range, and all Sampler descriptors on Sampler Heap in the
         // same way. Thus Vulkan sub-allocation cannot be perfectly matched.
         samplerRange[samplerRangeIndex].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

         samplerRangeIndex++;

         // Switch between backing heaps enforces new mapping range description
         if (general)
            {
            general = false;
            rangeIndex++;

            result->mappings[rangeIndex].layoutOffset = generalDescriptors + samplerDescriptors;
            result->mappings[rangeIndex].heapOffset   = samplerDescriptors;
            result->mappings[rangeIndex].elements     = 0;
            result->mappings[rangeIndex].heap         = 1;
            }

         // Increase count of Samplers in this mapping
         result->mappings[rangeIndex].elements += group[i].count;
 
         // Count total amount of Sampler descriptors to use as offset for next mapping after break
         samplerDescriptors += group[i].count;
         }
      else
         {
         generalRange[generalRangeIndex].RangeType          = TranslateResourceType[resourceType];
         generalRange[generalRangeIndex].NumDescriptors     = group[i].count;     // UINT - -1 or UINT_MAX to specify unbounded size (only last entry)

         // Storage buffers share local register space with Images (as UAV's, u(x))
         if (group[i].type == ResourceType::Storage)
            resourceType = underlyingType(ResourceType::Image);

         // Register Slots are assigned in order of Resource Group declarations.
         // Register Space is set during Root Signature creation, in order in
         // which Descriptor Sets are assigned to Pipeline Layout.
#if EN_SEPARATE_REGISTER_SPACES
         generalRange[generalRangeIndex].BaseShaderRegister = slot[resourceType];
#else
         generalRange[generalRangeIndex].BaseShaderRegister = slot;
#endif
         generalRange[generalRangeIndex].RegisterSpace      = 0u;                 // To be patched.

         // Defines offset from first Descriptor in the Descriptors Heap, used by 
         // this Descriptors Table (Set Layout). Each range can have it's own offset
         // if separately sub-allocated from the Heap, or can have it set to:
         // D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND - immediatelly follow previous range.
         //
         // Because each sub-allocation of Descriptor Ranges for DescriptorSet 
         // matching this SetLayout would result in different offsets between Ranges,
         // and at the same time, those offsets need to be known during Root Signature
         // creation, all CBV, SRV and UAV descriptors need to be allocated on the
         // Heap as single range, and all Sampler descriptors on Sampler Heap in the
         // same way. Thus Vulkan sub-allocation cannot be perfectly matched.
         generalRange[generalRangeIndex].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

         generalRangeIndex++;

         // Switch between backing heaps enforces new mapping range description
         if (!general)
            {
            general = true;
            rangeIndex++;

            result->mappings[rangeIndex].layoutOffset = generalDescriptors + samplerDescriptors;
            result->mappings[rangeIndex].heapOffset   = generalDescriptors;
            result->mappings[rangeIndex].elements     = 0;
            result->mappings[rangeIndex].heap         = 0;
            }

         // Increase count of descriptors in this mapping
         result->mappings[rangeIndex].elements += group[i].count;

         // Count total amount of General descriptors to use as offset for next mapping after break
         generalDescriptors += group[i].count;
         }

      // Increase index by amount of slots used
#if EN_SEPARATE_REGISTER_SPACES
      slot[resourceType] += group[i].count;
#else
      slot += group[i].count;
#endif

      // RootSignature 1.0:
      //
      // - Assumes Descriptors and Data are both Volatile
      //
      // RootSignature 1.1 new feature:
      // (see for details: https://msdn.microsoft.com/en-us/library/windows/desktop/mt709473(v=vs.85).aspx )
      //
      //generalRange[generalRangeIndex].Flags              = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;  // Descriptors Static, all Data static at execute (except UAV's)
      //
      // D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE              
      // D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE                     // Data changes on the fly (UAV's)
      // D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE  // Data changes when not executing
      // D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC                       // Data never changes
      }

   // General Descriptors Table
   result->table[0].NumDescriptorRanges = generalGroups;
   result->table[0].pDescriptorRanges   = generalRange;
   result->descriptors[0]               = descriptors[0];

   // Sampler Descriptors Table
   result->table[1].NumDescriptorRanges = samplerGroups;
   result->table[1].pDescriptorRanges   = samplerRange;
   result->descriptors[1]               = descriptors[1];

   // Calculate amount of backing Descriptor Tables
   if (generalGroups) result->tablesCount++;
   if (samplerGroups) result->tablesCount++;

   // Shader visibility
   //
   // D3D12 is not supporting visibility selection for multiple shaders
   // stages, except of all of them. Therefore in case that more than
   // one (but not all) shader stages are selected, we emulate this
   // behavior by falling back to "All" option.
   result->visibility = D3D12_SHADER_VISIBILITY_ALL;
   switch(underlyingType(stageMask))
      {
      case ShaderStages::Vertex:
         result->visibility = D3D12_SHADER_VISIBILITY_VERTEX;
         break;

      case ShaderStages::Control:
         result->visibility = D3D12_SHADER_VISIBILITY_HULL;
         break;
         
      case ShaderStages::Evaluation:
         result->visibility = D3D12_SHADER_VISIBILITY_DOMAIN;
         break;
         
      case ShaderStages::Geometry:
         result->visibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
         break;
         
      case ShaderStages::Fragment:
         result->visibility = D3D12_SHADER_VISIBILITY_PIXEL;
         break;
         
      default:
         break;
      };
      
   return result;
   }

   std::shared_ptr<PipelineLayout> Direct3D12Device::createPipelineLayout(const uint32 sets,
                                                              const std::shared_ptr<SetLayout>* set,
                                                              const uint32 immutableSamplers,
                                                              const std::shared_ptr<Sampler>* sampler,
                                                              const ShaderStages stageMask)
   {
   std::shared_ptr<PipelineLayoutD3D12> result = nullptr; 

   // Root Signature 1.1:
   // https://msdn.microsoft.com/en-us/library/windows/desktop/mt709473(v=vs.85).aspx
   // Specifies when descriptors are static, and when their data is static. And when they are volatile.

   bool allStages = false;
   bool stageUsesDescriptors[5] = { false, false, false, false, false };

   // Gather immutable sampler states.
   D3D12_STATIC_SAMPLER_DESC* samplers = nullptr;
   if (immutableSamplers)
      {
      // D3D12 is not supporting visibility selection for multiple shaders
      // stages, except of all of them. Therefore in case that more than
      // one (but not all) shader stages are selected, we emulate this
      // behavior by falling back to "All" option.
      D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;
      switch(underlyingType(stageMask))
         {
         case ShaderStages::Vertex:
            visibility = D3D12_SHADER_VISIBILITY_VERTEX;
            stageUsesDescriptors[0] = true;
            break;

         case ShaderStages::Control:
            visibility = D3D12_SHADER_VISIBILITY_HULL;
            stageUsesDescriptors[1] = true;
            break;
            
         case ShaderStages::Evaluation:
            visibility = D3D12_SHADER_VISIBILITY_DOMAIN;
            stageUsesDescriptors[2] = true;
            break;
            
         case ShaderStages::Geometry:
            visibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
            stageUsesDescriptors[3] = true;
            break;
            
         case ShaderStages::Fragment:
            visibility = D3D12_SHADER_VISIBILITY_PIXEL;
            stageUsesDescriptors[4] = true;
            break;
            
         default:
            allStages = true;
            break;
         };
         
      samplers = allocate<D3D12_STATIC_SAMPLER_DESC>(immutableSamplers);
      for(uint32 i=0; i<immutableSamplers; ++i)
         {
         SamplerD3D12* ptr = reinterpret_cast<SamplerD3D12*>(sampler[i].get());

         // While Vulkan uses the same descriptor for dynamic Samplers and
         // immutable Samplers, D3D12 uses two separate structures for those.
         // D3D12_SAMPLER_DESC        - dynamic Sampler
         // D3D12_STATIC_SAMPLER_DESC - immutable Sampler
         // So we need to copy data from one to another :/. Alternative would
         // be to introduce separate immutable sampler objects on API level,
         // but that sounds awkward.
         //
         samplers[i].Filter           = ptr->state.Filter;
         samplers[i].AddressU         = ptr->state.AddressU;
         samplers[i].AddressV         = ptr->state.AddressV;
         samplers[i].AddressW         = ptr->state.AddressW;
         samplers[i].MipLODBias       = ptr->state.MipLODBias;
         samplers[i].MaxAnisotropy    = ptr->state.MaxAnisotropy;
         samplers[i].ComparisonFunc   = ptr->state.ComparisonFunc;
         samplers[i].BorderColor      = ptr->border;
         samplers[i].MinLOD           = ptr->state.MinLOD;
         samplers[i].MaxLOD           = ptr->state.MaxLOD;
         samplers[i].ShaderRegister   = i; // All Immutable Samplers are enumerated in the order they are provided
         samplers[i].RegisterSpace    = sets; // Immutable Samplers always in last HLSL Space after all other Sets
         samplers[i].ShaderVisibility = visibility;
         }
      }
   
   // Gather Descriptor Tables
   uint32 tablesCount = 0;
   D3D12_ROOT_PARAMETER* tables = nullptr;
 //D3D12_ROOT_PARAMETER1* tables = nullptr; // v1.1
   uint32* setBindingIndex = nullptr;
   if (sets)
      {
      // Each DescriptorSet may be backed by up to two D3D12 Descriptor Tables pointing
      // into two separate D3D12 Descriptor Heaps, depending on the fact if it is keeping 
      // Sampler descriptors mixed with different ones, just Sampler Descriptors or just
      // all other ones. Thus for each logical Set creating layout, it's binding index 
      // needs to be computed at layout creation time.
      if (sets > 1)
         {
         setBindingIndex = new uint32[sets];
         setBindingIndex[0] = 0;
         }

      // Count amount of backing Descriptor Tables
      for(uint32 i=0; i<sets; ++i)
         {
         SetLayoutD3D12* ptr = reinterpret_cast<SetLayoutD3D12*>(set[i].get());
         tablesCount += ptr->tablesCount;
         }

      uint32 tableIndex = 0;
      tables = allocate<D3D12_ROOT_PARAMETER>(tablesCount);
    //tables = new D3D12_ROOT_PARAMETER1[tablesCount]; // v1.1
      for(uint32 i=0; i<sets; ++i)
         {
         SetLayoutD3D12* ptr = reinterpret_cast<SetLayoutD3D12*>(set[i].get());
         
         if (i > 0)
            setBindingIndex[i] = tableIndex;

         for(uint32 j=0; j<2; ++j)
            {
            uint32 ranges = ptr->table[j].NumDescriptorRanges;
            if (ranges)
               {
               tables[tableIndex].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
               tables[tableIndex].DescriptorTable  = ptr->table[j];   // It copies descriptor, but pointer in it, still shares pointed array!
               tables[tableIndex].ShaderVisibility = ptr->visibility;
            
               for(uint32 k=0; k<ranges; ++k)
                  {
                  // Patch Descriptor Table HLSL Spaces
                  // HLSL Register Space matches SPIRV Set indexing.
                  // pDescriptorRanges is pointing to const, so need to cast to patch.
                  // Patched "Space" assignments are still laying in the given SetLayout
                  // so they are valid only for current operation of RootSignature creation,
                  // each consecuting RootSignature creation will overwrite them - thus 
                  // Pipeline Layouts sharing Set Layouts cannot be created at the same
                  // time on different threads.
                  D3D12_DESCRIPTOR_RANGE& range = (D3D12_DESCRIPTOR_RANGE&)(tables[tableIndex].DescriptorTable.pDescriptorRanges[k]);
                  range.RegisterSpace = i;
                  }
            
             //tables[i].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
             //tables[i].Constants        = //D3D12_ROOT_CONSTANTS;
            
             //tables[i].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_CBV;
             //tables[i].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_SRV;
             //tables[i].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_UAV;
             //tables[i].Descriptor       = //D3D12_ROOT_DESCRIPTOR;
             //   UINT ShaderRegister;
             //   UINT RegisterSpace;

               tableIndex++;
               }
            }

         // Record shader stages that refer to any descriptors
         if (ptr->visibility == D3D12_SHADER_VISIBILITY_ALL)
            allStages = true;
         else
            stageUsesDescriptors[static_cast<uint32>(ptr->visibility) - 1] = true;
         }
      }

  
   D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc;
   desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
   desc.Desc_1_0.NumParameters     = tablesCount;
   desc.Desc_1_0.pParameters       = tables;
   desc.Desc_1_0.NumStaticSamplers = immutableSamplers;
   desc.Desc_1_0.pStaticSamplers   = samplers;
   desc.Desc_1_0.Flags             = D3D12_ROOT_SIGNATURE_FLAG_NONE;
   
   // Don't use this flag, to get small optimization on Programmable Vertex Fetch shaders
   desc.Desc_1_0.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

   if (!allStages)
      {
      // Optimize pipeline layout by preventin wiring down descriptors
      // to stages that never access them in any way. It will also deny
      // access to all shader stages in "default" Root Signature.
      if (!stageUsesDescriptors[0]) desc.Desc_1_0.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
      if (!stageUsesDescriptors[1]) desc.Desc_1_0.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
      if (!stageUsesDescriptors[2]) desc.Desc_1_0.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
      if (!stageUsesDescriptors[3]) desc.Desc_1_0.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
      if (!stageUsesDescriptors[4]) desc.Desc_1_0.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
      }

   // TODO: Add support for StreamOut
   // D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT

   // Serialize Root Signature
   ID3DBlob* signature = nullptr;
   ID3DBlob* error     = nullptr;
   ValidateCom( D3D12SerializeVersionedRootSignature(&desc, &signature, &error) )
   if (error)
      {
      // TODO: Log Serialization error blob
      }
      
   ID3D12RootSignature* handle = nullptr;
   Validate( this, CreateRootSignature(0u,
                                      signature->GetBufferPointer(),
                                      signature->GetBufferSize(),
                                      IID_PPV_ARGS(&handle)) ) // __uuidof(ID3D12RootSignature), reinterpret_cast<void**>(&handle)
   if (SUCCEEDED(lastResult[currentThreadId()]))
      result = std::make_shared<PipelineLayoutD3D12>(handle, sets, setBindingIndex);

   if (sets)
      deallocate<D3D12_ROOT_PARAMETER>(tables);
   if (immutableSamplers)
      deallocate<D3D12_STATIC_SAMPLER_DESC>(samplers);
   
   return result;
   }

   std::shared_ptr<Descriptors> Direct3D12Device::createDescriptorsPool(const uint32 maxSets, const uint32 (&count)[underlyingType(ResourceType::Count)])
   {
   std::shared_ptr<DescriptorsD3D12> result = nullptr;
   
   // All resources except of Samplers share the same heap in D3D12.
   // We set D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE to indicate that
   // Descriptors are not staged in CPU memory, but directly set in
   // shader visible space.
   //
   // See: Shader Visible Descriptor Heaps
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dn899211(v=vs.85).aspx
   //
   // See: Non Shader Visible Descriptor Heaps
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dn899199(v=vs.85).aspx
   
   // Create Samplers Heap if requested
   ID3D12DescriptorHeap* samplersHandle = nullptr;
   if (count[underlyingType(ResourceType::Sampler)] > 0)
      {
      D3D12_DESCRIPTOR_HEAP_DESC desc;
      desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
      desc.NumDescriptors = count[underlyingType(ResourceType::Sampler)];
      desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
      desc.NodeMask       = 0u; // TODO: Set bit to current GPU index in SLI
   
      Validate( this, CreateDescriptorHeap(&desc, IID_PPV_ARGS(&samplersHandle)) ) // __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&handleSamplers)
      if (!SUCCEEDED(lastResult[currentThreadId()]))
         {
         return result;
         }
      }
      
   uint32 resourceTypesCount = underlyingType(ResourceType::Count);
   
   // Calculate amount of slots required for Textures, Images, Uniforms and Storage Buffers
   uint32 slots = 0;
   for(uint32 i=1; i<resourceTypesCount; ++i)
      slots += count[i];
   
   ID3D12DescriptorHeap* handle = nullptr;
   if (slots > 0)
      {
      D3D12_DESCRIPTOR_HEAP_DESC desc;
      desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
      desc.NumDescriptors = slots;
      desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
      desc.NodeMask       = 0u; // TODO: Set bit to current GPU index in SLI
   
      Validate( this, CreateDescriptorHeap(&desc, IID_PPV_ARGS(&handle)) ) // __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&handle)
      if (!SUCCEEDED(lastResult[currentThreadId()]))
         {
         return result;
         }
      }

   // D3D12 has no limit of Descriptor Sets that can be allocated from it.
   result = std::make_shared<DescriptorsD3D12>(this);
   result->handle[0]    = handle;
   result->handle[1]    = samplersHandle;
   result->allocator[0] = new BasicAllocator(slots);
   result->allocator[1] = new BasicAllocator(count[underlyingType(ResourceType::Sampler)]);

   return result;
   }

   }
}
#endif
