/*

 Ngine v5.0
 
 Module      : Metal Buffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_BUFFER
#define ENG_CORE_RENDERING_METAL_BUFFER

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/metal/mtlHeap.h"
#include "core/rendering/common/buffer.h"

namespace en
{
   namespace gpu
   {
   class BufferMTL : public CommonBuffer
      {
      public:
      id<MTLBuffer> handle;
      shared_ptr<HeapMTL> heap; // Memory backing heap
      uint32 offset;            // Offset in parent MTLBuffer
      
      BufferMTL(shared_ptr<HeapMTL> heap, id<MTLBuffer> handle, const BufferType type, const uint32 size, const uint32 offset);

      virtual void* map(void);
      virtual void* map(const uint64 offset, const uint64 size);
      virtual void  unmap(void);
      
      virtual ~BufferMTL();
      };
      
   class BufferViewMTL : public BufferView
      {
      public:
      shared_ptr<Buffer> parent;  // Buffer from which this View is created
      
      BufferViewMTL();
      virtual ~BufferViewMTL();
      };

   }
}
#endif

#endif
