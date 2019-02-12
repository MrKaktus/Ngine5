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

#include "core/rendering/metal/mtlHeap.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/mtlBuffer.h"
#include "core/rendering/metal/mtlDevice.h"

namespace en
{
   namespace gpu
   {
   HeapMTL::HeapMTL(std::shared_ptr<MetalDevice> _gpu,
                    id handle,
                    const MemoryUsage _usage,
                    const uint32 _size) :
      gpu(_gpu),
      handle(handle),
#if defined(EN_PLATFORM_OSX)
      allocator(new BasicAllocator(_size)),
#endif
      CommonHeap(_usage, _size)
   {
   }
      
   HeapMTL::~HeapMTL()
   {
#if defined(EN_PLATFORM_OSX)
   delete allocator;
#endif

   deallocateObjectiveC(handle);
   }
   
   // Return parent device
   std::shared_ptr<GpuDevice> HeapMTL::device(void) const
   {
   return gpu;
   }
      
// Create unformatted generic buffer of given type and size.
// This method can still be used to create Vertex or Index buffers,
// but it's adviced to use ones with explicit formatting.
std::unique_ptr<Buffer> HeapMTL::createBuffer(const BufferType type, const uint32 size)
{
    assert( _usage != MemoryUsage::Tiled &&
            _usage != MemoryUsage::Renderable );

    BufferMTL* result = nullptr;

#if defined(EN_PLATFORM_OSX)
    // Sub-allocate region from backing MTLBuffer
    if (_usage == MemoryUsage::Upload   ||
        _usage == MemoryUsage::Download ||
        _usage == MemoryUsage::Immediate)
    {
        // Find memory region in the buffer where new buffer can be placed.
        // If allocation succeeded, buffer is mapped to given offset.
        uint64 offset = 0u;
        if (allocator->allocate(size, 0, offset))
        {
            [handle retain];
            result = new BufferMTL(*this,
                                   handle,
                                   type,
                                   size,
                                   offset);
        }
    }
    else
#endif
    {
        MTLResourceOptions options = (MTLCPUCacheModeDefaultCache << MTLResourceCPUCacheModeShift);
   
        // Based on buffer type, it's located in CPU RAM or GPU VRAM on NUMA architectures.
#if defined(EN_PLATFORM_IOS)
        options |= (MTLStorageModeShared << MTLResourceStorageModeShift);
#else
        options |= (MTLStorageModePrivate << MTLResourceStorageModeShift);
#endif

        id<MTLBuffer> buffer = [handle newBufferWithLength:(NSUInteger)size
                                                   options:options];
         
        if (buffer)
		{
            result = new BufferMTL(*this,
                                   buffer,
                                   type,
                                   size,
                                   0);
        }
    }

    return std::unique_ptr<Buffer>(result);
}

Texture* HeapMTL::createTexture(const TextureState state)
{
    // Textures can be only created on Tiled or Renderable Heaps.
    // (Engine currently is not supporting Linear Textures).
    assert( _usage == MemoryUsage::Tiled ||
            _usage == MemoryUsage::Renderable );
   
    TextureMTL* ptr = new TextureMTL(handle, state);
    if (ptr)
	{
        ptr->heap = *this;
    }

    return ptr;
}
   
Heap* MetalDevice::createHeap(const MemoryUsage usage, const uint32 size)
{
    HeapMTL* heap = nullptr;

    uint32 roundedSize = roundUp(size, 4096u);
  
#if defined(EN_PLATFORM_OSX)
    // CPU accessible (with MTLStorageModeShared) staging Heaps,
    // are emulated on macOS with MTLBuffers allocated directly
    // from MTLDevice. Depending on Heap usage, it's cache model
    // may be Default for Download, or WriteCombine for
    // Upload and Immediate usages.
    if (usage == MemoryUsage::Upload   ||
        usage == MemoryUsage::Download ||
        usage == MemoryUsage::Immediate)
    {
        MTLResourceOptions options = (MTLStorageModeShared << MTLResourceStorageModeShift);
      
        id<MTLBuffer> handle = nil;
        if (usage == MemoryUsage::Download)
        {
            options |= (MTLCPUCacheModeDefaultCache << MTLResourceCPUCacheModeShift);
      
            handle = [device newBufferWithLength:(NSUInteger)roundedSize
                                         options:options];
        }
        else
        {
            options |= (MTLCPUCacheModeWriteCombined << MTLResourceCPUCacheModeShift);
      
            handle = [device newBufferWithLength:(NSUInteger)roundedSize
                                         options:options];
         
        }
         
        if (handle)
		{
            heap = new HeapMTL(std::dynamic_pointer_cast<MetalDevice>(shared_from_this()),
                               handle,
                               usage,
                               roundedSize);
        }

        return heap;
    }
#endif

    MTLHeapDescriptor* desc = allocateObjectiveC(MTLHeapDescriptor);
    desc.size         = roundedSize;
    desc.cpuCacheMode = MTLCPUCacheModeDefaultCache;
#if defined(EN_PLATFORM_IOS)
    desc.storageMode  = MTLStorageModeShared;
#else
    // At this point we know that usage is one of
    // Linear, Tiled or Renderable
    desc.storageMode  = MTLStorageModePrivate;
#endif
   
    id<MTLHeap> handle = nil;
    handle = [device newHeapWithDescriptor:desc];
    if (handle)
	{
        heap = new HeapMTL*(std::dynamic_pointer_cast<MetalDevice>(shared_from_this()),
                            handle,
                            usage,
                            roundedSize);
    }
    deallocateObjectiveC(desc);

    return heap;
}
      
} // en::gpu
} // en
#endif
