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
#include "core/rendering/heap.h"

namespace en
{
   namespace gpu
   {
   class HeapMTL : public Heap
      {
      public:
#if defined(EN_PLATFORM_IOS)
      id <MTLHeap> handle;
      
      HeapMTL(id<MTLHeap> handle);
#endif
#if defined(EN_PLATFORM_OSX)
      HeapMTL();      
#endif

      // Create formatted Vertex buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> create(const uint32 elements,
                                 const Formatting& formatting,
                                 const uint32 step = 0);
        
      // Create formatted Index buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> create(const uint32 elements,
                                 const Attribute format);

      // Create unformatted generic buffer of given type and size.
      // This method can still be used to create Vertex or Index buffers,
      // but it's adviced to use ones with explicit formatting.
      virtual Ptr<Buffer> create(const BufferType type,
                                 const uint32 size);
      
      // Create unformatted generic buffer of given type and size.
      // This is specialized method, that allows passing pointer
      // to data, to directly initialize buffer content.
      // It is allowed on mobile devices conforming to UMA architecture.
      // On Discreete GPU's with NUMA architecture, only Transient buffers
      // can be created and populated with it.
      virtual Ptr<Buffer> create(const BufferType type,
                                 const uint32 size,
                                 const void* data);

      virtual Ptr<Texture> create(const TextureState state);

      virtual ~HeapMTL();
      };
   }
}
#endif

#endif
