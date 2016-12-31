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

#include "core/rendering/d3d12/dx12Buffer.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12Device.h"

namespace en
{
   namespace gpu
   {
   BufferD3D12::BufferD3D12(Ptr<HeapD3D12> _heap,
                            ID3D12Resource* _handle,
                            const BufferType _type,
                            const uint64 _offset,
                            const uint64 _size) :
      heap(_heap),
      handle(_handle),
      offset(_offset),
      CommonBuffer(_type, _size)
   {
   }

   BufferD3D12::~BufferD3D12()
   {
   assert( handle );
   ProfileCom( handle->Release() )
   handle = nullptr;
   
   // Deallocate from the Heap (let Heap allocator know that memory region is available again)
   raw_reinterpret_cast<HeapD3D12>(&heap)->allocator->deallocate(offset, size);
   heap = nullptr;
   }
      
   void* BufferD3D12::map(void)
   {
   return map(0u, size);
   }

   void* BufferD3D12::map(const uint64 _offset, const uint64 _size)
   {
   assert( _offset + _size <= size );
   
   // Buffers can be mapped only on Streamed and Immediate Heaps.
   assert( heap->_usage == MemoryUsage::Streamed ||
           heap->_usage == MemoryUsage::Immediate );
      
   // TODO: SIZE_T is 32bit -> settings.maxBufferSize < 4GB !!!
   assert( _offset + _size <= 0xFFFFFFFF );

   // Mapped range
   range.Begin = static_cast<SIZE_T>(_offset); 
   range.End   = static_cast<SIZE_T>(_offset + _size);

   void* pointer = nullptr;
   ProfileCom( handle->Map(0, &range, &pointer) )
   
   return pointer;
   }
   
   void BufferD3D12::unmap(void)
   {
   ProfileComNoRet( handle->Unmap(0, &range) )
   }
   
   // Create unformatted generic buffer of given type and size.
   // This method can still be used to create Vertex or Index buffers,
   // but it's adviced to use ones with explicit formatting.
   Ptr<Buffer> HeapD3D12::createBuffer(const BufferType type, const uint32 size)
   {
   Ptr<BufferD3D12> result = nullptr;
   
   // Buffer descriptor
   D3D12_RESOURCE_DESC desc;
   desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
   desc.Alignment          = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
   desc.Width              = static_cast<UINT64>(size);
   desc.Height             = 1u;
   desc.DepthOrArraySize   = 1u;
   desc.MipLevels          = 1u;
   desc.Format             = DXGI_FORMAT_UNKNOWN;
   desc.SampleDesc.Count   = 1u;
   desc.SampleDesc.Quality = 0u;
   desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
   desc.Flags              = D3D12_RESOURCE_FLAG_NONE;

   // Acquire buffer alignment and size
   D3D12_RESOURCE_ALLOCATION_INFO allocInfo;
   allocInfo = gpu->device->GetResourceAllocationInfo(0u /* currently no multi-GPU support */, 1u, &desc);

   // Find memory region in the Heap where this texture can be placed.
   // If allocation succeeded, texture is mapped to given offset.
   uint64 offset = 0;
   if (!allocator->allocate(static_cast<uint64>(allocInfo.SizeInBytes),
                            static_cast<uint64>(allocInfo.Alignment),
                            offset))
      {
      return Ptr<Buffer>(nullptr);
      }

   // Can be used by all stages, and needs to be populated first.
   D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE |
                                     D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
                                     D3D12_RESOURCE_STATE_COPY_DEST;
   
   ID3D12Resource* bufferHandle = nullptr;

   Profile( gpu, CreatePlacedResource(handle,
                                      static_cast<UINT64>(offset),
                                      &desc,
                                      initState,
                                      nullptr,                       // Clear value - currently not supported
                                      IID_PPV_ARGS(&bufferHandle)) ) // __uuidof(ID3D12Resource), reinterpret_cast<void**>(&bufferHandle)
      
   if ( SUCCEEDED(gpu->lastResult[Scheduler.core()]) )
      result = new BufferD3D12(Ptr<HeapD3D12>(this),
                               bufferHandle,
                               type,
                               offset,
                               static_cast<uint64>(allocInfo.SizeInBytes) );

   return ptr_reinterpret_cast<Buffer>(&result);
   }
   
   }
}
#endif
