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

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/utilities/TintrusivePointer.h"
#include "core/rendering/vulkan/vkDevice.h"    // TODO: We only want Device class, not all subsystems
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

   // Allowed Memory Types:

   #define SystemEviction              0
   #define System                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
   #define SystemCached                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT
   #define SystemSharedCache           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
   #define Dedicated                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
   #define DedicatedMappedPCIE         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
   #define DedicatedWithCopy           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT
   #define DedicatedSharedCache        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
   #define DedicatedMemorylessFallback VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT

   // Description:

   // a) System memory region visible only by driver. Used for temporary eviction of resources from VRAM (on over commitment).
   // b) System memory, CPU writes directly to memory without caching. Access is slower but explicit synchronization is not needed. (GPU writes are immediately visible as well).
   // c) System memory, CPU caches it's writes. Memory access is faster, but requires mapping and explicit flushing, to make changes visible for GPU.
   // d) System memory accessed by Intel integrated GPU. CPU and GPU share common cache.   
   //    ? Why AMD and  NVidia reports this memory ?
   // e) GPU dedicated memory (VRAM). Can be populated only through staging buffers and transfer operations.
   // f) GPU dedicated memory (VRAM). It's adress space is directly mapped to CPU adress space through PCI-E, so synchronisation is immediate and flushing is not needed.
   // g) GPU dedicated memory (VRAM). CPU keeps local copy of resource through Mapping mechanism. Requires explicit flushing. (equivalent of MTLStorageModeManaged in Metal API)
   // h) Equivalent of (d). Reported by Intel GPU's. ( Can it be EDRAM?)
   // i) VRAM used as drivers fallback alocation area for intermediate render targets (that on mobile could be processed in tile cache).
   //    Equivalent of Memoryless Textures / Render Targets in Metal API.
   //
   //    There may be two separate Memory Heaps marked with VK_MEMORY_HEAP_DEVICE_LOCAL_BIT. 
   //    If one is of size 256MB then it represents PCI-E mapped VRAM. It may be used for fast streaming of uniforms / push constants (view matrix, model transforms etc.).
   //
   // Memory Types hierarchy (Discreete):
   // - general allocations - e) > g) > f) > d) or h) > c) > b)
   // - UBO's / push const. - f) > g) > e) > d) or h) > c) > b)
   //
   // Memory Types hierarchy (Integrated/Mobile):
   // - general allocations - d) or h) > c) > b)
   // - intermediate RT's   - i) > d) or h) > c) > b)


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


//typedef enum VkMemoryPropertyFlagBits {
//    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT = 0x00000001,
//    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT = 0x00000002,
//    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT = 0x00000004,
//    VK_MEMORY_PROPERTY_HOST_CACHED_BIT = 0x00000008,
//    VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT = 0x00000010,
//} VkMemoryPropertyFlagBits;
//typedef VkFlags VkMemoryPropertyFlags;


// typedef struct {
//     VkDeviceSize                                size;
//     VkDeviceSize                                alignment;
//     uint32_t                                    memoryTypeBits;
// } VkMemoryRequirements;








// typedef struct {
//     VkDeviceSize                                size;
//     VkDeviceSize                                alignment;
//     uint32_t                                    memoryTypeBits;
// } VkMemoryRequirements;
// 
// 
// - Heaps are fixed up front.
// - Their amount and types is vendor / gpu / driver dependent.
// - We alloc memory on given cheap by referring to Memory Type ID -> which points at given heap. (see below).
// 
// Acquire GPU heaps count an types available :
// 
// 	void vkGetPhysicalDeviceMemoryProperties(
// 	    VkPhysicalDevice physicalDevice, 
// 	    VkPhysicalDeviceMemoryProperties* pMemoryProperties);
// 
// 	#define VK_MAX_MEMORY_TYPES               32
// 	#define VK_MAX_MEMORY_HEAPS               16
// 
// 	typedef struct {
// 	    uint32_t                   memoryTypeCount;
// 	    VkMemoryType               memoryTypes[VK_MAX_MEMORY_TYPES];
// 	    uint32_t                   memoryHeapCount;
// 	    VkMemoryHeap               memoryHeaps[VK_MAX_MEMORY_HEAPS];
// 	} VkPhysicalDeviceMemoryProperties;
// 
// 
// 
// 
// 
// Memory Heap:
// 
// 	// size is uint64
// 	typedef struct {
// 	    VkDeviceSize               size;
// 	    VkMemoryHeapFlags          flags;
// 	} VkMemoryHeap;
// 
// 	typedef enum {
// 	    VK_MEMORY_HEAP_HOST_LOCAL_BIT = 0x00000001,
// 	} VkMemoryHeapFlagBits;
// 	typedef VkFlags VkMemoryHeapFlags;
// 
// 
// Memory Type:
// 
// 	// heapIndex - refers to memoryHeaps[] array above
// 	typedef struct {
// 	    VkMemoryPropertyFlags                       propertyFlags;
// 	    uint32_t                                    heapIndex;   
// 	} VkMemoryType;
// 
// 	typedef enum {
// 	    VK_MEMORY_PROPERTY_DEVICE_ONLY = 0,
// 	    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT = 0x00000001,
// 	    VK_MEMORY_PROPERTY_HOST_NON_COHERENT_BIT = 0x00000002,
// 	    VK_MEMORY_PROPERTY_HOST_UNCACHED_BIT = 0x00000004,
// 	    VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT = 0x00000008,
// 	} VkMemoryPropertyFlagBits;
// 	typedef VkFlags VkMemoryPropertyFlags;
// 
// When allocating resource, you pass MemoryType ID in the array, and through it, Driver picks Heap to use.
// There can be several Memory Types that have the same subset of properties, but uses different backing Heaps (?).
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


   uint32 roundUp(uint32 num, uint32 alignment)
   {
   if (alignment == 0)
      return num;
   
   uint32 remainder = num % alignment;
   if (remainder == 0)
      return num;
   
   return num + alignment - remainder;
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

   memoryTypePerUsageCount[underlyingType(MemoryUsage::Static)]    = 4;
   memoryTypePerUsageCount[underlyingType(MemoryUsage::Streamed)]  = 3;
   memoryTypePerUsageCount[underlyingType(MemoryUsage::Immediate)] = 4;
   memoryTypePerUsageCount[underlyingType(MemoryUsage::Temporary)] = 1;
   uint32 prefferedMemoryTypePerUsage[4][8] = {
      { Dedicated, DedicatedWithCopy, System, SystemCached },            // Fastest possible GPU read and write
      { SystemCached, System, DedicatedWithCopy },                       // Fastest possible CPU read and write, burst upload to GPU preffered
      { DedicatedMappedPCIE, System, SystemCached, DedicatedWithCopy },  // Fastest possible CPU read and write, immediate upload to GPU preffered
      { DedicatedMemorylessFallback }                                    // Fastest possible allocation, GPU read and write
      };

   // // Integrated 
   // 
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Static)]    = 5;
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Streamed)]  = 5;
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Immediate)] = 5;
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Temporary)] = 1;
   // memoryTypePerUsage = {
   //    { Dedicated, DedicatedSharedCache, SystemSharedCache, SystemCached, System },         // Fastest possible GPU read and write (with focus on cache)
   //    { DedicatedSharedCache, SystemSharedCache, SystemCached, System, DedicatedWithCopy }, // Fastest possible CPU read and write (with focus on cache)
   //    { DedicatedSharedCache, SystemSharedCache, SystemCached, System, DedicatedWithCopy },
   //    { DedicatedMemorylessFallback }
   //    };
   // 
   // // Mobile 
   // 
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Static)]    = 5;
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Streamed)]  = 5;
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Immediate)] = 4;
   // memoryTypePerUsageCount[underlyingType(MemoryUsage::Temporary)] = 1;
   // memoryTypePerUsage = {
   //    { Dedicated, DedicatedSharedCache, SystemSharedCache, SystemCached, System },         // Fastest possible GPU read and write (with focus on cache)
   //    { DedicatedSharedCache, SystemSharedCache, SystemCached, System, DedicatedWithCopy }, // Fastest possible CPU read and write (with focus on cache)
   //    { DedicatedSharedCache, SystemSharedCache, SystemCached, System, DedicatedWithCopy },
   //    { DedicatedMemorylessFallback }
   //    };

   // Then engine preffered memory types are validated at runtime with available memory types reported by device.
   
   // Iterate over lists of engine preffered Vulkan Memory Types.
   // For each element in each list, verify if such type is supported by current device.
   // If it is store it's index for future heaps allocations and move to the next one.
   for(uint32 usage=0; usage<4; ++usage)
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

      // Set final count of Vulkan Memory Type indexes stored in array.
      memoryTypePerUsageCount[usage] = index;
      }
   }

   Ptr<Heap> VulkanDevice::create(const MemoryUsage usage, const uint32 size)
   {
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
      allocInfo.allocationSize  = size;
      allocInfo.memoryTypeIndex = memoryTypePerUsage[usageIndex][i];
      
      VkDeviceMemory handle;
      Profile( this, vkAllocateMemory(device, &allocInfo, nullptr, &handle) )
	  if (lastResult[0] == VK_SUCCESS)   // FIXME!! Assumes first thread !
         {
         return ptr_dynamic_cast<Heap, HeapVK>(Ptr<HeapVK>(new HeapVK(this, handle, memoryTypePerUsage[usageIndex][i], size)));
         }
      }

   return Ptr<Heap>(nullptr);
   }

   HeapVK::HeapVK(VulkanDevice* _gpu, const VkDeviceMemory _handle, const uint32 _memoryType, const uint32 size) :
      gpu(_gpu),
      handle(_handle),
      memoryType(_memoryType),
      CommonHeap(size)
   {
   }

   HeapVK::~HeapVK()
   {
   ProfileNoRet( gpu, vkFreeMemory(gpu->device, handle, nullptr) )
   }

   bool allocate(uint64 size, uint64 alignment, uint64* offset)
   {
   // TODO: Alocating algorithm!
   // TODO: Requires minimum Heap size.
   // TODO: Generalize this allocator to be of an Engine Type. 
   //       This will allow it to be used also for CPU memory sub-allocations,
   //       or to hook-up user specific memory allocators.
   // TODO: Add "makeAliasable" call to the resources API.
   // TODO: Ensure resources "dealloc" on Heap's allocator on destruction.
   // TODO: General and Specified memory allocators are needed.
   return false;
   }

   // Create formatted Vertex buffer that can be bound to InputAssembler.
   Ptr<Buffer> HeapVK::create(const uint32 elements, const Formatting& formatting, const uint32 step)
   {
   assert( elements );
   assert( formatting.column[0] != Attribute::None );
   
   uint32 rowSize = formatting.rowSize();
   uint32 size    = elements * rowSize;
   Ptr<Buffer> buffer = create(BufferType::Vertex, size);
   if (buffer)
      {
      Ptr<BufferVK> ptr = ptr_dynamic_cast<BufferVK, Buffer>(buffer);
      
      // TODO: Which of those are later needed ?
      //ptr->size = size;
      //ptr->rowSize = rowSize;
      //ptr->elements = elements;
      //ptr->formatting = formatting;
      }

   return buffer;
   }
     
   // Create formatted Index buffer that can be bound to InputAssembler.
   Ptr<Buffer> HeapVK::create(const uint32 elements, const Attribute format)
   {
   assert( elements );
   assert( format == Attribute::R_8_u  ||
           format == Attribute::R_16_u ||
           format == Attribute::R_32_u );
      
   uint32 rowSize = TranslateAttributeSize[underlyingType(format)];
   uint32 size    = elements * rowSize;
   return create(BufferType::Index, size);
   }

   // Create unformatted generic buffer of given type and size.
   // This method can still be used to create Vertex or Index buffers,
   // but it's adviced to use ones with explicit formatting.
   Ptr<Buffer> HeapVK::create(const BufferType type, const uint32 size)
   {
   // Create buffer descriptor
   Ptr<Buffer> result = createBuffer(gpu, type, size);
   if (!result)
      return result;

   // Check if created buffer can be backed by this Heap memory
   Ptr<BufferVK> buffer = ptr_dynamic_cast<BufferVK, Buffer>(result);
   if (!checkBit(buffer->memoryRequirements.memoryTypeBits, memoryType))
      {
      // Destroy texture descriptor
      buffer = nullptr;
      result = nullptr;
      return result;
      }

   // Find memory region in the Heap where this buffer can be placed.
   // If allocation succeeded, buffer is mapped to given offset.
   uint64 offset = 0;
   if (!allocate(buffer->memoryRequirements.size,
                 buffer->memoryRequirements.alignment,
                 &offset))
      {
      // Destroy buffer descriptor
      buffer = nullptr;
      result = nullptr;
      return result;
      }

   Profile( gpu, vkBindBufferMemory(gpu->device, buffer->handle, handle, offset) )

   return result;
   }
   
   // Create unformatted generic buffer of given type and size.
   // This is specialized method, that allows passing pointer
   // to data, to directly initialize buffer content.
   // It is allowed on mobile devices conforming to UMA architecture.
   // On Discreete GPU's with NUMA architecture, only Transient buffers
   // can be created and populated with it.
   Ptr<Buffer> HeapVK::create(const BufferType type, const uint32 size, const void* data)
   {
   // TODO: FINISH !!!!
   return Ptr<Buffer>(nullptr);
   }
 
   Ptr<Texture> HeapVK::create(const TextureState state)
   {
   // Create texture descriptor
   Ptr<Texture> result = createTexture(gpu, state);
   if (!result)
      return result;

   // Check if created texture can be backed by this Heap memory
   Ptr<TextureVK> texture = ptr_dynamic_cast<TextureVK, Texture>(result);
   if (!checkBit(texture->memoryRequirements.memoryTypeBits, memoryType))
      {
      // Destroy texture descriptor
      texture = nullptr;
      result = nullptr;
      return result;
      }
  
   // Find memory region in the Heap where this texture can be placed.
   // If allocation succeeded, texture is mapped to given offset.
   uint64 offset = 0;
   if (!allocate(texture->memoryRequirements.size,
                 texture->memoryRequirements.alignment,
                 &offset))
      {
      // Destroy texture descriptor
      texture = nullptr;
      result = nullptr;
      return result;
      }

   Profile( gpu, vkBindImageMemory(gpu->device, texture->handle, handle, offset) )

   return result;
   }
  
   }
}
#endif
