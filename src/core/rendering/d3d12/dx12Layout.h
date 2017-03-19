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

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/layout.h"

namespace en
{
   namespace gpu
   { 
   class SetLayoutD3D12 : public SetLayout
      {
      public:
      D3D12_ROOT_DESCRIPTOR_TABLE table;
    //D3D12_ROOT_DESCRIPTOR_TABLE1 table; // v1.1
      D3D12_SHADER_VISIBILITY visibility;
      
      SetLayoutD3D12() {};
      virtual ~SetLayoutD3D12();
      };

   class PipelineLayoutD3D12 : public PipelineLayout
      {
      public:
      ID3D12RootSignature* handle;

      PipelineLayoutD3D12(ID3D12RootSignature* handle);
      virtual ~PipelineLayoutD3D12();
      };

   class DescriptorsD3D12 : public Descriptors
      {
      public:
      ID3D12DescriptorHeap* handle;
      ID3D12DescriptorHeap* handleSamplers;
      
      virtual Ptr<DescriptorSet> allocate(const Ptr<SetLayout> layout);
      virtual bool allocate(const uint32 count,
                            const Ptr<SetLayout>* layouts,
                            Ptr<DescriptorSet>* sets);
         
      DescriptorsD3D12(ID3D12DescriptorHeap* handle, ID3D12DescriptorHeap* handleSamplers);
      virtual ~DescriptorsD3D12();
      };

   class DescriptorSetD3D12 : public DescriptorSet
      {
      public:
      Ptr<DescriptorsD3D12> parent; // Reference to Descriptors Pool
      //VkDescriptorSet       handle;
      
      DescriptorSetD3D12(Ptr<DescriptorsD3D12> parent, VkDescriptorSet handle);
      virtual ~DescriptorSetD3D12();
      };
   }
}
#endif

#endif
