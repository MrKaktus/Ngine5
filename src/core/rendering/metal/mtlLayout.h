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

#ifndef ENG_CORE_RENDERING_METAL_RESOURCE_LAYOUT
#define ENG_CORE_RENDERING_METAL_RESOURCE_LAYOUT

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/layout.h"
#include "core/rendering/shader.h"

#include <vector>
using namespace std;

namespace en
{
   namespace gpu
   {
   class MetalDevice;

   class SetLayoutMTL : public SetLayout
      {
      public:
      id<MTLArgumentEncoder> handle;
      ShaderStages stageMask;
      uint32 descriptors;

      SetLayoutMTL(id<MTLArgumentEncoder> encoder, const ShaderStages stageMask, const uint32 descriptors);
      virtual ~SetLayoutMTL();
      };

   class PipelineLayoutMTL : public PipelineLayout
      {
      public:
      MetalDevice* gpu;
      uint32 setsCount;  // Count of descriptor sets
      shared_ptr<SetLayoutMTL>* setLayout;
      
      PipelineLayoutMTL(MetalDevice* gpu, const uint32 _setsCount);
      virtual ~PipelineLayoutMTL();
      };
      
   class DescriptorsMTL : public Descriptors
      {
      public:
      MetalDevice* gpu;
      
      DescriptorsMTL(MetalDevice* gpu);
      virtual ~DescriptorsMTL();
      
      virtual shared_ptr<DescriptorSet> allocate(const shared_ptr<SetLayout> layout);
      virtual bool allocate(const uint32 count,
                            const shared_ptr<SetLayout>(&layouts)[],
                            shared_ptr<DescriptorSet>** sets);
      };
      
   class DescriptorSetMTL : public DescriptorSet
      {
      public:
      MetalDevice*      gpu;        // Device backing this buffer memory
      shared_ptr<SetLayoutMTL> layout; // Reference to Layout, that will be used to encode descriptors in backing buffer
      id<MTLBuffer>     handle;     // MTLBuffer storing descriptors
      
      uint8*            heapId;     // Index to Heap pointer, that backs resource currently bound to given Descriptor Slot
      id<MTLHeap>*      heapsUsed;  // Array of pointers to Heaps used by bound resources
      uint32*           heapsRefs;  // Count of desriptors referencing each Heap
      uint32            heapsCount; // Describes range of slots in Heaps array, used by valid Heaps pointers
      
      DescriptorSetMTL(MetalDevice* gpu, shared_ptr<SetLayoutMTL> layout);
      virtual ~DescriptorSetMTL();
      
      void updateResidencyTracking(const uint32 slot, const id<MTLHeap> heap);
      
      virtual void setBuffer(const uint32 slot, const shared_ptr<Buffer> buffer);
      virtual void setSampler(const uint32 slot, const shared_ptr<Sampler> sampler);
      virtual void setTextureView(const uint32 slot, const shared_ptr<TextureView> view);
      };
   }
}
#endif

#endif
