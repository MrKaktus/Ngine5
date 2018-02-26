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

#include "core/rendering/vulkan/vkHeap.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/vulkan/vkDevice.h"    // TODO: We only want Device class, not all subsystems
#include "core/rendering/vulkan/vkBuffer.h" 
#include "core/rendering/vulkan/vkTexture.h"   

namespace en
{
   namespace gpu
   { 
   #define KB 1024
   #define MB 1024*1024
   
   // Memory organization:
   //
   // Discreete GPU:
   // CPU <---> CPU CACHE <---> RAM <---> GPU <---> VRAM
   // 
   // Mobile GPU:
   // CPU <---> CPU CACHE <---> RAM <---> GPU
   //
   // Intel integrated GPU (CPU and GPU can both access the same cache)
   // CPU, GPU <---> SHARED CACHE <---> L4 EDRAM <---> RAM 
   //
   // Allowed Memory Types:

   #define SystemEviction              0
   #define System                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
   #define SystemCached                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT
   #define SystemWriteThrough          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
   #define Dedicated                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
   #define DedicatedMapped             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
   #define DedicatedWithCopy           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT
   #define DedicatedSharedCache        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
   #define DedicatedMemorylessFallback VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT

   // Description:
   //
   // a) SystemEviction              - System memory region visible only by driver.
   //                                  Used for temporary eviction of resources from VRAM (on over commitment).
   // b) System                      - System memory, CPU writes directly to memory without caching.
   //                                  Access is slower but explicit synchronization is not needed.
   //                                  (GPU writes are immediately visible as well). Equivalent of Write-Around cache.
   // c) SystemCached                - System memory, CPU caches it's writes. Memory access is faster, but
   //                                  requires mapping and explicit flushing to make changes visible for GPU.
   // d) SystemWriteThrough          - System memory, CPU caches it's writes and sends data to RAM at the same time.
   //                                  Data is coherent but writes confirmation take more time. Write-Through cache.
   // e) Dedicated                   - GPU dedicated memory (VRAM on Discreete or BIOS mapped region of RAM on Integrated).
   //                                  Can be populated only through staging buffers and transfer operations.
   // f) DedicatedMapped             - GPU dedicated memory. It's adress space is directly mapped to CPU adress space
   //                                  (through PCI-E for Discreete), so synchronisation is immediate and flushing is not needed.
   // g) DedicatedWithCopy           - GPU dedicated memory. CPU keeps local copy of resource through
   //                                  Mapping mechanism. Requires explicit flushing.
   //                                  (equivalent of MTLStorageModeManaged in Metal API)
   // h) DedicatedSharedCache        - CPU and GPU can share common cahe on integrated GPU's like in case of Intel.
   // i) DedicatedMemorylessFallback - Memory used as drivers fallback alocation area for intermediate render targets
   //                                  (that on mobile could be processed in tile cache).
   //                                  Equivalent of Memoryless Textures / Render Targets in Metal API.
   //
   // Access patterns:
   //
   // SystemEviction
   // System                      map/unmap
   // SystemCached                map/unmap flush/invalidate
   // SystemWriteThrough          map/unmap
   // Dedicated                   blit
   // DedicatedMapped             map/unmap
   // DedicatedWithCopy           map/unmap flush/invalidate
   // DedicatedSharedCache        map/unmap
   // DedicatedMemorylessFallback
   //
   // Because Engine currently is not using "SystemCached" nor "DedicatedWithCopy" memory types,
   // (as they doesn't occur in the actual drivers), it doesn't need to use below Vulkan calls:
   //
   // vkFlushMappedMemoryRanges      - non-coherent
   // vkInvalidateMappedMemoryRanges - non-coherent , after barrier and fence



   //AMD R9 390:
   //
   //Dedicated
   //DedicatedMapped
   //System
   //SystemWriteThrough
   //
   //NV GTX 980 Ti:
   //
   //Dedicated
   //System
   //SystemWriteThrough
   //
   //Intel HD 520:

   //DedicatedMapped
   //DedicatedSharedCache
   //
   //Intel Ivybridge / Haswell / Skylake:
   //
   //DedicatedSharedCache   
   //
   //Adreno 430:
   //
   //Dedicated             ( reports shared system memory as Dedicated )
   //DedicatedWithCopy
   //DedicatedSharedCache
   //
   //Tegra K1, X1:
   //
   //Dedicated
   //DedicatedMapped
   //DedicatedWithCopy

   // Description:


   //
   //    There may be two separate Memory Heaps marked with VK_MEMORY_HEAP_DEVICE_LOCAL_BIT. 
   //    If one is of size 256MB then it represents PCI-E mapped VRAM.
   //    It may be used for fast streaming of uniforms / push constants (view matrix, model transforms etc.).
   //



// Possible Memory Types:
// 
// Memory types backed by GPU VRAM:
// 
// (they only match to Heaps of type `VK_MEMORY_HEAP_DEVICE_LOCAL_BIT`, and can be populated only with Copy operations)
// 
// `VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT` | 
// `VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT` | 
// `MEMORY_PROPERTY_HOST_COHERENT_BIT`
// 
// Looks like most efficient memory type. It's VRAM backed memory, that can be mapped through CPU, but with explicit copy operation from our source to GPU VRAM (no local CPU RAM copy is keept for caching purposes / there probably can still be CPU RAM copy for PageOff purposes).
// 
// `VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT` | 
// `VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT` | 
// `MEMORY_PROPERTY_HOST_CACHED_BIT`
// 
// Like "MTLStorageModeManaged". VRAM backed memory, keeping CPU RAM copy for caching. Require explicit syncing of modified sections.
// 
// - need explicit `vkInvalidateMappedMemoryRanges` call before CPU Read (sync from GPU)
// - need explicit `vkFlushMappedMemoryRanges` call after CPU Write (sync to GPU)
// 
// 
// `VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT` | 
// `VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT` | 
// `MEMORY_PROPERTY_HOST_CACHED_BIT` | 
// `MEMORY_PROPERTY_HOST_COHERENT_BIT`
// 
// Looks like universal memory pool capable of handling both above types.
// It's like mix of "MTLStorageModePrivate" and "MTLStorageModeManaged" where it can decide dynamically per resource if it will keep CPU RAM copy for caching or not.
//  
// `VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT` | 
// `VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT`
// 
// Allows handling Memoryless Framebuffers by the Driver. Render Targets backed by it, need to have such bit set in their Image: `VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT`
// 
// Memory types backed by CPU RAM:
// 
// `VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT` | 
// `MEMORY_PROPERTY_HOST_COHERENT_BIT`
// 
// System memory only (CPU RAM only), like "MTLStorageModeShared". GPU reads directly from it, until it can explicitly page it in for better performance. Then it performs whole copy for sync on it's onw and manages coherency. 
// 
// `VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT` | 
// `MEMORY_PROPERTY_HOST_CACHED_BIT`
// 
// System memory only (CPU RAM only), like "MTLStorageModeShared". If I understand this mode correctly, if GPU decided that it wants to keep it locally in VRAM after all, it needs to be manually synced by the app? It doesn't make a lot of sence.
// Need to explain this one.
// 
// 
// 0
// 
// Memory used for paging off resources created in dedicated memory.
// 




// 
// Query memory layout for given resource:
// 
// 	void VKAPI vkGetBufferMemoryRequirements(
// 	    VkDevice                                    device,
// 	    VkBuffer                                    buffer,
// 	    VkMemoryRequirements*                       pMemoryRequirements);
// 	
// 	void VKAPI vkGetImageMemoryRequirements(
// 	    VkDevice                                    device,
// 	    VkImage                                     image,
// 	    VkMemoryRequirements*                       pMemoryRequirements);
// 	
// 	void VKAPI vkGetImageSparseMemoryRequirements(
// 	    VkDevice                                    device,
// 	    VkImage                                     image,
// 	    uint32_t*                                   pNumRequirements,
// 	    VkSparseImageMemoryRequirements*            pSparseMemoryRequirements);
// 
// Returns simple size/offset + bits :
// 
// 	typedef struct {
// 	    VkDeviceSize                                size;
// 	    VkDeviceSize                                alignment;
// 	    uint32_t                                    memoryTypeBits;
// 	} VkMemoryRequirements;
// 	
// 	typedef struct {
// 	    VkSparseImageFormatProperties               formatProps;
// 	    uint32_t                                    imageMipTailStartLOD;
// 	    VkDeviceSize                                imageMipTailSize;
// 	    VkDeviceSize                                imageMipTailOffset;
// 	    VkDeviceSize                                imageMipTailStride;
// 	} VkSparseImageMemoryRequirements;
// 
// 
// Then we can bind it to resource description/handle to given memory block range:
// 
// 	VkResult VKAPI vkBindBufferMemory(
// 	    VkDevice                                    device,
// 	    VkBuffer                                    buffer,
// 	    VkDeviceMemory                              mem,
// 	    VkDeviceSize                                memOffset);
// 	
// 	VkResult VKAPI vkBindImageMemory(
// 	    VkDevice                                    device,
// 	    VkImage                                     image,
// 	    VkDeviceMemory                              mem,
// 	    VkDeviceSize                                memOffset);
// 
// 
// So it looks like in Vulkan (VkDeviceMemory == Heap in D3D12/Metal) while Vulkan Heaps are even bigger concept.
// We do allocations on the Heap, but then we can freely bind Resource Handles to different Memory regions.
// There are also functions for mapping/unmapping/invalidating memory etc.


   HeapVK::HeapVK(shared_ptr<VulkanDevice> _gpu,
                  const VkDeviceMemory _handle,
                  const uint32 _memoryType,
                  const MemoryUsage _usage,
                  const uint32 size) :
      gpu(_gpu),
      handle(_handle),
      memoryType(_memoryType),
      allocator(new BasicAllocator(size)),
      mappingsCount(0),
      mappedPtr(nullptr),
      CommonHeap(_usage, size)
   {
   }

   HeapVK::~HeapVK()
   {
   if (mappingsCount)
      ValidateNoRet( gpu, vkUnmapMemory(gpu->device, handle) )

   ValidateNoRet( gpu, vkFreeMemory(gpu->device, handle, nullptr) )
   
   delete allocator;
   }

   // Return parent device
   shared_ptr<GpuDevice> HeapVK::device(void) const
   {
   return gpu;
   }

   // Create unformatted generic buffer of given type and size.
   // This method can still be used to create Vertex or Index buffers,
   // but it's adviced to use ones with explicit formatting.
   shared_ptr<Buffer> HeapVK::createBuffer(const BufferType type, const uint32 size)
   {
   // Create buffer descriptor
   shared_ptr<BufferVK> buffer = gpu::createBuffer(this, type, size);
   if (!buffer)
      return nullptr;

   // Check if created buffer can be backed by this Heap memory
   if (!checkBit(buffer->memoryRequirements.memoryTypeBits, memoryType))
      {
      // Destroy texture descriptor
      buffer = nullptr;
      return nullptr;
      }

   // Find memory region in the Heap where this buffer can be placed.
   // If allocation succeeded, buffer is mapped to given offset.
   uint64 offset = 0u;
   if (!allocator->allocate(buffer->memoryRequirements.size,
                            buffer->memoryRequirements.alignment,
                            offset))
      {
      // Destroy buffer descriptor
      buffer = nullptr;
      return nullptr;
      }

   Validate( gpu, vkBindBufferMemory(gpu->device, buffer->handle, handle, offset) )
   buffer->heap   = dynamic_pointer_cast<HeapVK>(shared_from_this());
   buffer->offset = offset;
   
   return buffer;
   }
   
   shared_ptr<Texture> HeapVK::createTexture(const TextureState state)
   {
   // Do not create textures on Heaps designated for Streaming.
   // (Engine currently is not supporting Linear Textures).
   assert( _usage == MemoryUsage::Tiled ||
           _usage == MemoryUsage::Renderable );
   
   // Create texture descriptor
   shared_ptr<TextureVK> texture = gpu::createTexture(gpu.get(), state);
   if (!texture)
      return nullptr;

   // Check if created texture can be backed by this Heap memory
   if (!checkBit(texture->memoryRequirements.memoryTypeBits, memoryType))
      {
      // Destroy texture descriptor
      texture = nullptr;
      return nullptr;
      }
  
   // Find memory region in the Heap where this texture can be placed.
   // If allocation succeeded, texture is mapped to given offset.
   uint64 offset = 0;
   if (!allocator->allocate(texture->memoryRequirements.size,
                            texture->memoryRequirements.alignment,
                            offset))
      {
      // Destroy texture descriptor
      texture = nullptr;
      return nullptr;
      }

   Validate( gpu, vkBindImageMemory(gpu->device, texture->handle, handle, offset) )
   texture->heap   = dynamic_pointer_cast<HeapVK>(shared_from_this());
   texture->offset = offset;
   
   return texture;
   }
  
  
  
   void VulkanDevice::initMemoryManager()
   {
   // Engine Default preferences
   //----------------------------
   //
   // Each allowed combination of Memory Properties, describes possible Vulkan Memory Type.
   // Engine declares which possible Vulkan Memory Types it would want to use, to back 
   // allocations for each Engine Memory Usage. It declares their count and order, so that, 
   // if some Vulkan Memory Type is not available at runtime (or Vulkan Memory Heap used by 
   // it is full) it can fall back to use other one. Preffered Vulkan Memory Types for each 
   // Engine Memory Usage are ordered from most suitable to least suitable. Engine can
   // exclude some Vulkan Memory Types for some usages, if it decides that it's better not
   // to allocate, rather than allocate on memory with unsuitable memory.

   // TODO: Run-Time detect GPU architecture type ( Discreete, Integrated, Mobile )

   // Discrete

   memoryTypePerUsageCount[underlyingType(MemoryUsage::Linear)]     = 1;
   memoryTypePerUsageCount[underlyingType(MemoryUsage::Tiled)]      = 1; 
   memoryTypePerUsageCount[underlyingType(MemoryUsage::Renderable)] = 1; 
   memoryTypePerUsageCount[underlyingType(MemoryUsage::Upload)]     = 3; // Map-unmap (only for buffers, staging)
   memoryTypePerUsageCount[underlyingType(MemoryUsage::Download)]   = 3; // Map-unmap (only for buffers, staging)
   memoryTypePerUsageCount[underlyingType(MemoryUsage::Immediate)]  = 3; // Immediately visible

   uint32 prefferedMemoryTypePerUsage[6][8] = {
      { Dedicated },                                    // Fastest possible GPU read and write (linear)
      { Dedicated },                                    // Fastest possible GPU read and write (tiled/compressed for size)
      { Dedicated },                                    // Fastest possible GPU read and write (linear/compressed for bandwith)
      { SystemWriteThrough, System, DedicatedMapped },  // Fastest possible CPU write for upload
      { SystemCached, System, DedicatedMapped },        // Fastest possible CPU read for download
      { DedicatedMapped, SystemWriteThrough, System }   // Fastest possible CPU write, immediate upload to GPU memory if possible
      };
      
   // // Integrated 
   // 
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Static)]    = 3; // Staging in using Immediate or Streamed
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Streamed)]  = 4; // Map-unmap (only for buffers, staging)
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Immediate)] = 4; // Map-unmap (only for buffers, staging)
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Temporary)] = 1; // Staging in using Immediate or Streamed
   // uint32 prefferedMemoryTypePerUsage[4][8] = {
   //    { DedicatedSharedCache, DedicatedMapped, Dedicated },                   // Fastest possible GPU read and write
   //    { SystemWriteThrough, System, DedicatedSharedCache, DedicatedMapped },  // Fastest possible CPU read and write
   //    { DedicatedSharedCache, DedicatedMapped, SystemWriteThrough, System },  // Fastest possible CPU write, immediate upload to GPU memory if possible
   //    { DedicatedMemorylessFallback }                                         // Fastest possible allocation, GPU read and write
   //    };
   // 
   // // Mobile 
   // 
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Static)]    = 3; // Staging in using Immediate or Streamed
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Streamed)]  = 4; // Map-unmap (only for buffers, staging)
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Immediate)] = 4; // Map-unmap (only for buffers, staging)
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Temporary)] = 1; // Staging in using Immediate or Streamed
   // uint32 prefferedMemoryTypePerUsage[4][8] = {
   //    { DedicatedSharedCache, DedicatedMapped, Dedicated },                   // Fastest possible GPU read and write
   //    { SystemWriteThrough, System, DedicatedSharedCache, DedicatedMapped },  // Fastest possible CPU read and write
   //    { DedicatedSharedCache, DedicatedMapped, SystemWriteThrough, System },  // Fastest possible CPU write, immediate upload to GPU memory if possible
   //    { DedicatedMemorylessFallback }                                         // Fastest possible allocation, GPU read and write
   //    };

   // Then engine preffered memory types are validated at runtime with available memory types reported by device.
   
   // Iterate over lists of engine preffered Vulkan Memory Types.
   // For each element in each list, verify if such type is supported by current device.
   // If it is store it's index for future heaps allocations and move to the next one.
   for(uint32 usage=0; usage<6; ++usage)
      {
      uint32 index = 0;
      for(uint32 i=0; i<memoryTypePerUsageCount[usage]; ++i)
         for(uint32 j=0; j<memory.memoryTypeCount; ++j)
            if (memory.memoryTypes[j].propertyFlags == prefferedMemoryTypePerUsage[usage][i])
               {
               memoryTypePerUsage[usage][index] = j;
               index++;
               break;
               }

      // For each memory usage there need to be at least one memory type.
      assert( index > 0u );
      
      // Set final count of Vulkan Memory Type indexes stored in array.
      memoryTypePerUsageCount[usage] = index;
      }
   }

   shared_ptr<Heap> VulkanDevice::createHeap(const MemoryUsage usage, const uint32 size)
   {
   uint32 roundedSize = roundUp(size, 4096u);
   
   // How many different Memory Types this device support for given Memory Usage,
   // this many times app will try to allocate memory iterating from best matching 
   // and available memory type to worst matching one. 
   uint32 usageIndex = underlyingType(usage);
   for(uint32 i=0; i<memoryTypePerUsageCount[usageIndex]; ++i)
      {
      // Try to allocate memory on Vulkan Heap supporting this memory type
      VkMemoryAllocateInfo allocInfo;
      allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.pNext           = nullptr;
      allocInfo.allocationSize  = roundedSize;
      allocInfo.memoryTypeIndex = memoryTypePerUsage[usageIndex][i];
      
      VkDeviceMemory handle;
      Validate( this, vkAllocateMemory(device, &allocInfo, nullptr, &handle) )
      if (lastResult[Scheduler.core()] == VK_SUCCESS)
         {
         return make_shared<HeapVK>(dynamic_pointer_cast<VulkanDevice>(shared_from_this()),
                                    handle,
                                    memoryTypePerUsage[usageIndex][i],
                                    usage,
                                    roundedSize);
         }
      }

   return shared_ptr<Heap>(nullptr);
   }

   }
}
#endif
