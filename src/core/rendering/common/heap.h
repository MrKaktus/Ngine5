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
    MemoryUsage _usage;
    uint32 _size;

    CommonHeap(const MemoryUsage usage, const uint32 size);

    virtual uint32 size(void) const;

    /// Create formatted Vertex buffer that can be bound to InputLayout.
    virtual Buffer* createBuffer(const uint32 elements,
                                 const Formatting& formatting,
                                 const uint32 step = 0u);
      
    /// Create formatted Index buffer that can be bound to InputLayout.
    virtual Buffer* createBuffer(const uint32 elements,
                                 const Attribute format);

    /// Create unformatted generic buffer of given type and size.
    /// This method can still be used to create Vertex or Index buffers,
    /// but it's adviced to use ones with explicit formatting.
    virtual Buffer* createBuffer(const BufferType type,
                                 const uint32 size);

    virtual ~CommonHeap() {};
};

// CompileTimeSizeReporting( CommonHeap );
static_assert(sizeof(CommonHeap) == 16, "en::gpu::CommonHeap size mismatch!");

} // en::gpu
} // en
#endif
