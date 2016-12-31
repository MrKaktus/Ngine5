/*

 Ngine v5.0
 
 Module      : D3D12 Heap.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_HEAP
#define ENG_CORE_RENDERING_D3D12_HEAP

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/common/heap.h"
#include "core/utilities/basicAllocator.h"

namespace en
{
   namespace gpu
   {
   class Direct3D12Device;

   class HeapD3D12 : public CommonHeap
      {
      public:
      Direct3D12Device* gpu;
      ID3D12Heap*       handle;
      Allocator*        allocator;    // Allocation algorithm used to place resources on the Heap

      HeapD3D12(Direct3D12Device* gpu,
                ID3D12Heap* handle,
                const MemoryUsage usage,
                const uint32 size);

      // Return parent device
      virtual Ptr<GpuDevice> device(void) const;

      // Create unformatted generic buffer of given type and size.
      // This method can still be used to create Vertex or Index buffers,
      // but it's adviced to use ones with explicit formatting.
      virtual Ptr<Buffer> createBuffer(const BufferType type,
                                       const uint32 size);
      
      virtual Ptr<Texture> createTexture(const TextureState state);

      virtual ~HeapD3D12();
      };
      
   }
}
#endif

#endif
