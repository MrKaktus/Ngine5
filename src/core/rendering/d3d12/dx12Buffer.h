/*

 Ngine v5.0
 
 Module      : D3D12 Buffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_BUFFER
#define ENG_CORE_RENDERING_D3D12_BUFFER

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/common/buffer.h"
#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/d3d12/dx12Heap.h"

namespace en
{
   namespace gpu
   {
   class BufferD3D12 : public CommonBuffer
      {
      public:
      ID3D12Resource* handle;
      Ptr<HeapD3D12>  heap;       // Memory backing heap
      uint64          offset;     // Offset in the heap
      D3D12_RANGE     range;      // Mapped range
      
      BufferD3D12(Ptr<HeapD3D12> heap,
                  ID3D12Resource* handle,
                  const BufferType type,
                  const uint64 offset,
                  const uint64 size);
         
      virtual ~BufferD3D12();
      
      virtual void* map(void);
      virtual void* map(const uint64 offset, const uint64 size);
      virtual void  unmap(void);
      };
   }
}
#endif

#endif
