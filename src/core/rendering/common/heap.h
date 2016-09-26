/*

 Ngine v5.0
 
 Module      : Common Heap.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_COMMON_HEAP
#define ENG_CORE_RENDERING_COMMON_HEAP

#include "core/rendering/heap.h"

namespace en
{
   namespace gpu
   {
   class CommonHeap : public Heap
      {
      public:
      uint32 _size;

      CommonHeap(const uint32 size);

      virtual uint32 size(void) const;

      // Create formatted Vertex buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> createBuffer(const uint32 elements,
                                       const Formatting& formatting,
                                       const uint32 step = 0u);
        
      // Create formatted Index buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> createBuffer(const uint32 elements,
                                       const Attribute format);

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

      virtual ~CommonHeap() {};                           // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif
