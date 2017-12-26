/*
 
 Ngine v5.0
 
 Module      : Metal Heap.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.
 
 */

#ifndef ENG_CORE_RENDERING_METAL_HEAP
#define ENG_CORE_RENDERING_METAL_HEAP

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/common/heap.h"
#include "core/utilities/basicAllocator.h"

namespace en
{
   namespace gpu
   {
   class MetalDevice;
   
   class HeapMTL : public CommonHeap
      {
      public:
      shared_ptr<MetalDevice> gpu;
#if defined(EN_PLATFORM_IOS)
      id <MTLHeap> handle;
#else
      // On macOS Upload/Download/Immediate heaps are emulated with
      // MTLBuffer directly allocated on MTLDevice, as MTLHeaps with
      // MTLStorageModeShared are not allowed (as Textures can have
      // only Private backing, such Heaps would need to disallow
      // Texture allocation).
      id         handle;    // id<MTLHeap> or id<MTLBuffer>
      Allocator* allocator; // Allocation algorithm used to place resources on the backing buffer
#endif

      HeapMTL(shared_ptr<MetalDevice> gpu, id handle, const MemoryUsage _usage, const uint32 _size);
      virtual ~HeapMTL();

      // Return parent device
      virtual shared_ptr<GpuDevice> device(void) const;
      
      // Create unformatted generic buffer of given type and size.
      // This method can still be used to create Vertex or Index buffers,
      // but it's adviced to use ones with explicit formatting.
      virtual shared_ptr<Buffer> createBuffer(const BufferType type,
                                              const uint32 size);
      
      virtual shared_ptr<Texture> createTexture(const TextureState state);
      };
   }
}
#endif

#endif
