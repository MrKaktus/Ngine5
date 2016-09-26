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

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/metal.h"
#include "core/rendering/common/heap.h"

namespace en
{
   namespace gpu
   {
   class HeapMTL : public CommonHeap
      {
      public:
#if defined(EN_PLATFORM_IOS)
      id <MTLHeap> handle;
      
      HeapMTL(id<MTLHeap> handle, const uint32 _size);
#endif
#if defined(EN_PLATFORM_OSX)
      id <MTLDevice> handle;
      
      HeapMTL(id<MTLDevice> device, const uint32 _size);
#endif

      // Create unformatted generic buffer of given type and size.
      // This method can still be used to create Vertex or Index buffers,
      // but it's adviced to use ones with explicit formatting.
      virtual Ptr<Buffer> createBuffer(const BufferType type,
                                       const uint32 size);
      
      // Create unformatted generic buffer of given type and size.
      // This is specialized method, that allows passing pointer
      // to data, to directly initialize buffer content.
      // It is allowed on mobile devices conforming to UMA architecture.
      // On Discreete GPU's with NUMA architecture, only Transient buffers
      // can be created and populated with it.
      virtual Ptr<Buffer> createBuffer(const BufferType type,
                                       const uint32 size,
                                       const void* data);

      virtual Ptr<Texture> createTexture(const TextureState state);

      virtual ~HeapMTL();
      };
   }
}
#endif

#endif
