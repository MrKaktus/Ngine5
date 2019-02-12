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
HeapD3D12::HeapD3D12(
        std::shared_ptr<Direct3D12Device> _gpu,
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
    ValidateCom( handle->Release() )
    handle = nullptr;
    gpu = nullptr;
   
    delete allocator;
}

std::shared_ptr<GpuDevice> HeapD3D12::device(void) const
{
    return gpu;
}
   
Heap* Direct3D12Device::createHeap(const MemoryUsage usage, const uint32 size)
{
    HeapD3D12* result = nullptr;
   
    uint32 roundedSize = roundUp(size, 4096u);

    // Heap descriptor (by default assume GPU VRAM on NUMA architectures)
    D3D12_HEAP_DESC desc;
    desc.SizeInBytes                     = static_cast<UINT64>(roundedSize);
    desc.Properties.Type                 = D3D12_HEAP_TYPE_CUSTOM;
    desc.Properties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
    desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L1;
    desc.Properties.CreationNodeMask     = 0u;  // TODO: Set bit equivalent to GPU index !
    desc.Properties.VisibleNodeMask      = 0u;  // TODO: Set all bits to make it visible everywhere.
                                                // Multi-GPU is currently not supported.
    desc.Alignment                       = 0;   // Will automatically choose between 4KB and 64K
    desc.Flags                           = D3D12_HEAP_FLAG_NONE;

    // For default Heap:
    // 
    // desc.Properties.Type                 = D3D12_HEAP_TYPE_DEFAULT;
    // desc.Properties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    // desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    // TODO: Consider Intel/UMA architecture
    //       To differentiate NUMA from UMA adapters, see D3D12_FEATURE_ARCHITECTURE and D3D12_FEATURE_DATA_ARCHITECTURE.

    if (usage == MemoryUsage::Linear)
        desc.Flags |= D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
    else
    if (usage == MemoryUsage::Tiled)
        desc.Flags |= D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
    else
    if (usage == MemoryUsage::Renderable)
    {
        desc.Alignment = 4 * MB;  // 4MB for MSAA
        desc.Flags |= D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
    }
    else
    if (usage == MemoryUsage::Upload)
    {
        // More on Write-Combine memory for VRAM uploads:
        // https://fgiesen.wordpress.com/2013/01/29/write-combining-is-not-your-friend/
        desc.Properties.Type                 = D3D12_HEAP_TYPE_UPLOAD;          // D3D12_HEAP_TYPE_CUSTOM;
        desc.Properties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; // D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
        desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;       // D3D12_MEMORY_POOL_L0;

        // TODO: Consider Intel/UMA architecture
        desc.Flags |= D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
    }
    else
    if (usage == MemoryUsage::Download)
    {
        desc.Properties.Type                 = D3D12_HEAP_TYPE_READBACK;        // D3D12_HEAP_TYPE_CUSTOM;
        desc.Properties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; // D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
        desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;       // D3D12_MEMORY_POOL_L0;

        // TODO: Consider Intel/UMA architecture
        desc.Flags |= D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
    }
    else // Dedicated to uploading data to GPU as fast as possible.
    if (usage == MemoryUsage::Immediate)
    {
        desc.Properties.Type                 = D3D12_HEAP_TYPE_UPLOAD;          // D3D12_HEAP_TYPE_CUSTOM;
        desc.Properties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; // D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
        desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;       // D3D12_MEMORY_POOL_L0;

        // TODO: Consider Intel/UMA architecture
        desc.Flags |= D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
    }

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
   
    ID3D12Heap* handle = nullptr;

    Validate( this, CreateHeap(&desc, IID_PPV_ARGS(&handle)) ) // __uuidof(ID3D12Heap), reinterpret_cast<void**>(&handle)
    if ( SUCCEEDED(lastResult[currentThreadId()]) )
    {
        result = new HeapD3D12(std::dynamic_pointer_cast<Direct3D12Device>(shared_from_this()),
                               handle,
                               usage,
                               roundedSize);
    }

    return result;
}

} // en::gpu
} // en
#endif
