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

#ifndef ENG_CORE_RENDERING_D3D12_RESOURCE_LAYOUT
#define ENG_CORE_RENDERING_D3D12_RESOURCE_LAYOUT

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/utilities/basicAllocator.h"
#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/layout.h"

namespace en
{
   namespace gpu
   { 
   // Type of bound Descriptor is not validated against Set Layout
   struct RangeMapping
      {
      uint32 layoutOffset;  // First Descriptor offset in Layout
      uint32 heapOffset;    // First Descriptor offset in Heap (layoutOffset + heapStartingOffset)
      uint32 elements;      // Size of range in Descriptors
      uint32 heap;          // Backing heap index
      };

   class SetLayoutD3D12 : public SetLayout
      {
      public:
      D3D12_ROOT_DESCRIPTOR_TABLE table[2]; //D3D12_ROOT_DESCRIPTOR_TABLE1 v1.1
      D3D12_SHADER_VISIBILITY visibility;
      uint32                  descriptors[2]; // Precomputed, total amount of descriptors in each table
      ResourceType*           type;
      uint32                  tablesCount;
      RangeMapping*           mappings;       // Mappings of Layout slots to backing Heap sub-allocations (its copied and patched on allocation)
      uint32                  mappingsCount;

      SetLayoutD3D12();
      virtual ~SetLayoutD3D12();
      };

   class PipelineLayoutD3D12 : public PipelineLayout
      {
      public:
      ID3D12RootSignature* handle;
      uint32               sets;
      uint32*              setBindingIndex;

      PipelineLayoutD3D12(ID3D12RootSignature* handle, const uint32 _sets, uint32* setBindingIndex);
      virtual ~PipelineLayoutD3D12();
      };

   class Direct3D12Device;

   class DescriptorsD3D12 : public Descriptors
      {
      public:
      Direct3D12Device*     gpu;
      ID3D12DescriptorHeap* handle[2];    // Descriptors heap handle ( 0 - General , 1 - Samplers )
      BasicAllocator*       allocator[2]; // Descriptors allocator per backing Heap
      UINT64                descriptorSize;
      UINT64                samplerDescriptorSize;

      // Internal helper methods
      D3D12_CPU_DESCRIPTOR_HANDLE pointerToDescriptorOnCPU(uint32 index);
      D3D12_GPU_DESCRIPTOR_HANDLE pointerToDescriptorOnGPU(uint32 index);
      D3D12_CPU_DESCRIPTOR_HANDLE pointerToSamplerDescriptorOnCPU(uint32 index);
      D3D12_GPU_DESCRIPTOR_HANDLE pointerToSamplerDescriptorOnGPU(uint32 index);

      virtual DescriptorSet* allocate(const SetLayout& layout);
      virtual bool allocate(const uint32 count,
                            const SetLayout*(&layouts)[],
                            DescriptorSet**& sets);
         
      DescriptorsD3D12(Direct3D12Device* gpu);
      virtual ~DescriptorsD3D12();
      };

   class DescriptorSetD3D12 : public DescriptorSet
      {
      public:
      Direct3D12Device*   gpu;
      DescriptorsD3D12*   parent;    // Reference to Descriptors Pool
      uint64              offset[2]; // Index of first Descriptor Slot for given allocation
      uint32              slots[2];
      RangeMapping* const mappings;  // Mappings of Layout slots to backing Heap sub-allocations
      uint32              mappingsCount;

      DescriptorSetD3D12(Direct3D12Device* gpu, DescriptorsD3D12* parent, const uint64* offsets, const uint32* slots, RangeMapping* const mappings, uint32 mappingsCount);
      virtual ~DescriptorSetD3D12();

      // Helper method translating layout slot index into backing heap index
      bool translateSlot(const uint32 slot, uint32& heapSlot);

      virtual void setBuffer(const uint32 slot, const Buffer& buffer);
      virtual void setSampler(const uint32 slot, const Sampler& sampler);
      virtual void setTextureView(const uint32 slot, const TextureView& view);
      };
   }
}
#endif

#endif
