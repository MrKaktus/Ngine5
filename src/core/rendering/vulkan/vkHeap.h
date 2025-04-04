/*

 Ngine v5.0
 
 Module      : Vulkan Heap.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_VULKAN_HEAP
#define ENG_CORE_RENDERING_VULKAN_HEAP

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/memory/alignedAllocator.h"
#include "core/rendering/common/heap.h"
#include "core/rendering/common/device.h"
#include "core/utilities/basicAllocator.h"
#include "core/parallel/mutex.h"

namespace en
{
namespace gpu
{

class VulkanDevice;

class HeapVK : public CommonHeap
{
    public:                            
    std::shared_ptr<VulkanDevice> gpu; 
    VkDeviceMemory handle;             
    Allocator*     allocator;          // Allocation algorithm used to place resources on the Heap
    Mutex          mutex;              // Guards critical section during mapping (at 64 byte offset to match alignment)
    uint32         memoryType;         
    uint32         mappingsCount;      
    void*          mappedPtr;          

    HeapVK(std::shared_ptr<VulkanDevice> gpu,
           const VkDeviceMemory handle, 
           const uint32 _memoryType, 
           const MemoryUsage _usage,
           const uint32 size);

    // Return parent device
    virtual std::shared_ptr<GpuDevice> device(void) const;
    
    // Create unformatted generic buffer of given type and size.
    // This method can still be used to create Vertex or Index buffers,
    // but it's adviced to use ones with explicit formatting.
    virtual Buffer* createBuffer(const BufferType type,
                                 const uint32 size);
    
    virtual Texture* createTexture(const TextureState state);
    
    void* operator new(size_t size)
    {
        // New and delete are overloaded to make sure that Mutex is always 
        // aligned at proper adress. This also allows application to use 
        // std::unique_ptr() to manage Heap lifecycle, without growing 
        // unique pointer size (as there is no custom deleter needed).
        return en::allocate<HeapVK>(1, cacheline);
    }

    void operator delete(void* pointer)
    {
        en::deallocate<HeapVK>(static_cast<HeapVK*>(pointer));
    }

    virtual ~HeapVK();
};

// CompileTimeSizeReporting( HeapVK );
static_assert(sizeof(HeapVK) == 72, "en::gpu::CommonHeap size mismatch!"); // 144 + padding

} // en::gpu
} // en

#endif
#endif
