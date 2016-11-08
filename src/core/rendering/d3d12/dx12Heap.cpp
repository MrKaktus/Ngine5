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

#include "core/rendering/d3d12/dx12Heap.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12Device.h"

namespace en
{
   namespace gpu
   {
   #define KB 1024
   #define MB 1024*1024
   
   HeapD3D12::HeapD3D12(Direct3D12Device* _gpu,
                        ID3D12Heap* _handle,
                        const MemoryUsage usage,
                        const uint32 size) :
      gpu(_gpu),
      handle(_handle),
      allocator(new BasicAllocator(size)),
      CommonHeap(usage, size)
   {
   }
   
   HeapD3D12::~HeapD3D12()
   {
   assert( handle );
   handle->Release();
   handle = nullptr;
   gpu = nullptr;
   
   delete allocator;
   }

   Ptr<GpuDevice> HeapD3D12::device(void) const
   {
   return Ptr<GpuDevice>(gpu);
   }
   
   Ptr<Heap> Direct3D12Device::createHeap(const MemoryUsage usage, const uint32 size)
   {
   Ptr<HeapD3D12> result = nullptr;
   
   uint32 roundedSize = roundUp(size, 4096u);

   // Heap properties (GPU VRAM by default)
   D3D12_HEAP_PROPERTIES properties;
   properties.Type                 = D3D12_HEAP_TYPE_DEFAULT;
   properties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
   properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L1;
   properties.CreationNodeMask     = 0u;  // TODO: Set bit equivalent to GPU index !
   properties.VisibleNodeMask      = 0u;  // TODO: Set all bits to make it visible everywhere.
                                          // Multi-GPU is currently not supported.

   // TODO: If Intel/UMA architecture
   if (usage == MemoryUsage::Streamed ||
       usage == MemoryUsage::Temporary)
      {
      // Both directions so no D3D12_HEAP_TYPE_READBACK
      properties.Type                 = D3D12_HEAP_TYPE_CUSTOM;
      properties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
      properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
      }
      
   // Dedicated to uploading data to GPU as fast as possible.
   if (usage == MemoryUsage::Immediate)
      {
      properties.Type                 = D3D12_HEAP_TYPE_CUSTOM; // D3D12_HEAP_TYPE_UPLOAD ?
      properties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
      properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
      }

   // Heap descriptor
   D3D12_HEAP_DESC desc;
   desc.SizeInBytes = static_cast<UINT64>(roundedSize);
   desc.Properties  = properties;
   desc.Alignment   = 4 * MB;                // 4KB, 64K or 4MB for MSAA
	desc.Flags       = D3D12_HEAP_FLAG_NONE;
      
   // TODO: If Intel/UMA architecture
   if (usage == MemoryUsage::Streamed  ||
       usage == MemoryUsage::Immediate)
      desc.Flags |= D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
      
   if (usage == MemoryUsage::Temporary)
      desc.Flags |= D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;

   // Flags bitfield:
   //
   // 0 bit - shared         <- Allows sharing between processes. Currently not supported.
   // 1
   // 2 bit - no buffers
   // 3 bit - allow display  <- This flag should be used only by Swap-Chain for it's internal Heap allocation.
   //
   // 4
   // 5 bit - cross adapter  <- Allows sharing between GPU's. Currently not supported.
   // 6 bit -  non RT/DS textures + buffers   \__ together: only buffers
   // 7 bit - only RT/DS textures + buffers   /
   
   // TODO: GPU possible internal optimizations:
   //
   // Use linear memory instead of tiled pattern
   // - D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS
   //
   // Use tiled memory patterns.
   // - D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES
   //
   // Allows compression of RT's in VRAM
   // - D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES

   ID3D12Heap* handle;

	Profile( this, CreateHeap(&desc, __uuidof(ID3D12Heap), IID_PPV_ARGS(&handle)) )
   if ( SUCCEDED(lastResult[Scheduler.core()]) )
      result = new HeapD3D12(this, handle, usage, roundedSize);
   
   return ptr_reinterpret_cast<Heap>(&result);
   }
   
   }
}
#endif
