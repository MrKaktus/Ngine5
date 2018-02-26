/*

 Ngine v5.0
 
 Module      : GPU Resource Streamer
 Requirements: none
 Description : Exposes simple interface for management of resource residency in 
               GPU dedicated memory.

*/

#ifndef ENG_RENDERING_STREAMER
#define ENG_RENDERING_STREAMER

#include "core/algorithm/allocator.h"
#include "core/utilities/poolAllocator.h"
#include "core/rendering/buffer.h"
#include "core/rendering/texture.h"
#include "core/rendering/device.h"

namespace en
{
   //// Single backing allocation 
   //struct BackingAllocation
   //   {
   //   shared_ptr<Heap>   heap;       // Heap backing this memory block
   //
   //
   //                                  // Used only for sysMem allocations:
   //
   //
   //   shared_ptr<Buffer> buffer;     // Linear buffer of heap size
   //   Allocator*         allocator;  // Algorithm used for sub-allocations in buffer
   //   void*              sysAddress; // Adress of system memory mapped, linear buffer
   //                                  // that spans across whole heap address range.
   //
   //   ~BackingAllocation();
   //   };
   
   
   // Asset type:
   //
   // surface            - surface
   // surface (LOD)      - given Level of Detail of surface
   // surfaceArray       - array of surfaces 
   // surfaceArray (LOD) - array of surfaces for given Level Of Detail
   // volume             - array of depth planes composing volume
   // volume (LOD)       - array of depth planes composing given Level of Detail of volume
   //
   // GPU resource, CPU handle based on storing rules:
   //
   // surface            - [Texture2D]
   // surface (LOD)      - Separate: [Texture2D, mipmap] 
   //                    - Cached:   [Texture2DArray, mipmap, layer]
   // surfaceArray       - Separate: [Texture2DArray, 0, 0] + layer count
   //                    - Cached:   [Texture2DArray, 0, firstLayer] + layer count
   // surfaceArray (LOD) - Separate: [Texture2DArray, mipmap, layer] + layer count
   //                    - Cached:   [Texture2DArray, mipmap, firstLayer] + layer count
   // cube               - [TextureCubeMap] 
   // cube (LOD)         - Separate: [TextureCubeMap] + mipmapRange
   //                    - Cached:   [TextureCubeMapArray, layer] + mipmapRange
   
   // cubeArray          - Separate: [TextureCubeMapArray] + mipmaps + layers
   //                    - Cached:   [TextureCubeMapArray, mipmap, firstLayer] + layer count
   // cubeArray (LOD)    - Separate: [TextureCubeMapArray, mipmap, layer] + layer count
   //                    - Cached:   [TextureCubeMapArray, mipmap, firstLayer] + layer count
   
   // volume             - [Texture3D]
   // volume (LOD)       - [Texture3D, mipmap]
   //
   
   
   
   // Resource access on CPU:
   
   // Resource access on GPU (with and without LOD's):
   //
   // Separate - unique resource per asset
   // Cached   - array resource, storing group of assets of the same type
   // Set Separate - array of resource descriptors, each resource storing asset 
   // Set Cached   - array of resource descriptors, each storing group of assets of the same type
   //
   // surface              - Separate:     [Texture2D] 
   //                        Cached:       [Texture2DArray] + surfaceId
   //                      * Set Separate: array[Texture2D] + surfaceId
   //                        Set Cached:   array[Texture2DArray] + arrayId + surfaceId
   // surfaceArray         - Separate:     [Texture2DArray]
   //                        Cached:       [Texture2DArray]      + firstSurfaceId
   //                        Set Separate: array[Texture2D]      + firstSurfaceId  (surface in array, each have separate descriptor)
   //                      * Set Separate: array[Texture2DArray] + arrayId         (surface array have unique descriptor)
   //                        Set Cached:   array[Texture2DArray] + firstArrayId + firstSurfaceId  (surface array is sub range of shared surfaceArray object in descriptors array)
   // cube                 - Separate:     [TextureCubeMap]
   //                        Cached:       [TextureCubeMapArray] + cubeId
   //                      * Set Separate: array[TextureCubeMap] + cubeId
   // cubeArray            - Separate:     [TextureCubeMapArray]
   //                        Cached:       [TextureCubeMapArray]      + firstCube
   //                        Set Separate: array[TextureCubeMap]      + firstCubeId  (cube in array, each have separate descriptor)
   //                      * Set Separate: array[TextureCubeMapArray] + arrayId      (cube array have unique descriptor)
   //                        Set Cached:   array[TextureCubeMapArray] + firstArrayId + firstCubeId  (cube array is sub range of shared cubeArray object in descriptors array)
   // volume               - Separate:     [Texture3D]
   //                      * Set Separate: array[Texture3D] + volumeId
   //
   // * Preffered for automatic upload. Each logical asset, can be referenced in 
   //   shader based on identifier, pointing to proper array of descriptor sets, and
   //   index in that array. Identifier may be based on TextureType as those cannot
   //   be mixed in Descriptor arrays.
   //   Descriptor sets allows grouping resources with different formats and sizes,
   //   while shared arrays don't.
   //
   enum class AssetType : uint32
      {
      Buffer               = 0,
      Surface                 ,
      SurfaceArray            ,
      Cube                    ,
      CubeArray               ,
      Volume                  ,
      Count                   ,
      };
   
   class Asset
      {
      AssetType type;     // Type of asset
      bool      hasLOD;   // Stores different data for different Levels of Detail
      uint32    layers;   // surfaceArray range size
      };
   
   struct AssetHandle
      {
      shared_ptr<gpu::Texture> handle;
      
      // surface -> Texture, mipmap, layer
      };
   
   
   
   
   
   
   
   #define InvalidHeapID 0xFFFFFFFF
   
   
   
   
   
   //// Resource is always CPU RAM backed. It's GPU VRAM residency is dynamic.
   //struct Resource
   //   {
   //   uint32 size;      // Allocation size
   //   uint32 cpuOffset; // Offset in RAM allocation
   //   uint32 gpuOffset; // Offset in VRAM allocation
   //   uint16 cpuHeapId; // Index of backing Heap in RAM
   //   uint16 gpuHeapId; // Index of backing Heap in VRAM
   //   // 16 bytes
   //
   //   Resource(const uint64 size);
   //   };
   //
   //
   //
   //Resource::Resource(const uint64 _size) :
   //   size(_size),
   //   cpuOffset(0),
   //   gpuOffset(0),
   //   cpuHeapId(InvalidHeapID),
   //   gpuHeapId(InvalidHeapID)
   //{
   //}
   
   
   
   
   
   
   
   
   
   
   
   
   
   struct ResourceCache
      {
      ResourceCache* next;       // Pointer to next Heap descriptor
      };
   
   struct BufferCache : public ResourceCache // Buffer & Texture
      {
      shared_ptr<gpu::Buffer> buffer; // Linear buffer covering whole heap (it keeps reference to parent Heap, so Heap pointer doesn't need to be stored)
      Allocator*     allocator;       // Algorithm used for sub-allocations in heap
      volatile void* sysAddress;      // Adress of system memory mapped, linear buffer
                                      // that spans across whole heap address range.
      };
   
   static_assert(sizeof(BufferCache) == 40, "BufferChache size mismatch!");
   
   struct TextureCache : public ResourceCache
      {
      shared_ptr<gpu::Heap> heap;
      };
   
   static_assert(sizeof(TextureCache) == 24, "TextureChache size mismatch!");
   
   // GPU Buffer public structure for referencing resource
   struct BufferAllocation
      {
      volatile void*          cpuPointer;     // Pointer to memory location in system memory (offset in mapped buffer)
      shared_ptr<gpu::Buffer> gpuBuffer;      // Updated when made resident
      uint32                  gpuOffset;      // Updated when made resident
      uint32                  size     : 31;  // Max supported size of single GPU allocation is 2GB
      uint32                  resident : 1;   // Set by Streamer when resource is resident in GPU dedicated memory
      };
   
   static_assert(sizeof(BufferAllocation) == 32, "BufferAllocation size mismatch!");
   
   // GPU Buffer internal structure needed only during allocation/deallocation
   struct BufferAllocationInternal
      {
      // Pointers to descriptors in Pool Allocator are calculate on the fly as its rare operation.
      uint32 sysHeapIndex;  // Index to system Heap descriptor
      uint32 gpuHeapIndex;  // Index to dedicated Heap descriptor
      uint32 sysOffset;     // Starting offset in CPU Heap
      uint32 locked : 1;    // Cannot be evicted when locked (lock is set on resident call)
      uint32        : 31;   // Padding to 16 bytes
      };
   
   static_assert(sizeof(BufferAllocationInternal) == 16, "BufferAllocationInternal size mismatch!");
   
   // Upload and download allocation sizes need to be power of two (download allocation
   // can be set to 0, if there will be no transfers from GPU to CPU memory). 
   // Resident allocation size needs to be power of two, and both buffer and texture
   // maximum residency sizes need to be multiple of resident allocation size. If 
   // this structure is not provided, or those conditions are not met, default 
   // values are used:
   //
   //  32MB - download allocation size
   // 128MB - resident allocation size
   // 128MB - max buffer resident size
   // 384MB - max texture resident size
   //
   // By default GPU with minimum 768MB of memory is required (as streamer will use 
   // 512MB). If GPU accessible memory will be less than default requirements or
   // less than provided settings, streamer creation will fail, which will be 
   // indicated by all allocations requests failing. Application should verify that
   // those conditions are met by GPU ahead of time.
   struct StreamerSettings
      {
      uint32 downloadAllocationSize; // Download memory size
      uint32 systemAllocationSize;   // Size of single memory allocation in system memory
      uint32 residentAllocationSize; // Size of single memory allocation in dedicated memory
      uint64 maxBufferResidentSize;  // Maximum allowed usage of dedicated memory for buffers
      uint64 maxTextureResidentSize; // Maximum allowed usage of dedicated memory for textures
      };
   
   class Streamer
      {
      private:
      shared_ptr<gpu::GpuDevice> gpu;
      gpu::QueueType queueForTransfers;
   
      uint64 dedicatedMemorySize;    // Total size of GPU dedicated memory
      uint64 systemMemorySize;       // Total size of system memory GPU can access
      uint32 downloadAllocationSize; // Download memory size
      uint32 systemAllocationSize;   // Size of single memory allocation in system memory
      uint32 residentAllocationSize; // Size of single memory allocation in dedicated memory
      uint64 maxBufferResidentSize;  // Maximum allowed usage of dedicated memory for buffers
      uint64 maxTextureResidentSize; // Maximum allowed usage of dedicated memory for textures
      uint64 availableSystemMemory;
      uint64 availableBufferMemory;
      uint64 availableTextureMemory;
   
      // Backing Heaps data structures 
   
      uint32 cpuHeapCount;    // Count of heaps allocated in RAM
      uint32 gpuHeapCount[2]; // Count of heaps allocated in dedicated memory (separate for Buffers and Textures)
      PoolAllocator<BufferCache>*  cpuHeap;
      PoolAllocator<BufferCache>*  gpuBufferHeap;
      PoolAllocator<TextureCache>* gpuTextureHeap; // Those are just pools of objects! Still need data structure to iterate over them!
   
      // Data structure managing order in which Heaps are ordered and iterated
   
      BufferCache*  cpuHeapList;        // Head of linked list, of RAM allocated Heap descriptors
      BufferCache*  gpuBufferHeapList;  // Head of linked list, of dedicated memory allocated Heap descriptors
      TextureCache* gpuTextureHeapList; // Head of linked list, of dedicated memory allocated Heap descriptors
   
      // Resource allocation data structures
   
      PoolAllocator<BufferAllocation>* resource;
      PoolAllocator<BufferAllocationInternal>* resourceRef;
   
      // Dedicated Heap for downloading results of GPU operations from dedicated to system memory
      shared_ptr<gpu::Buffer> downloadBuffer;
      volatile void*          downloadAdress;
   
   
   
   
      // Helper method allocating memeory
      bool initSystemHeap(BufferCache& systemCache);
      bool initBufferHeap(BufferCache& bufferCache);
      void evictBuffer(BufferAllocation& desc, BufferAllocationInternal& descInternal);
   
      public:
      Streamer(shared_ptr<gpu::GpuDevice> gpu, const StreamerSettings* settings);
     ~Streamer();
    
      bool allocateMemory(BufferAllocation*& desc, const uint32 size);
      void makeResident(BufferAllocation& desc);
      void evictMemory(BufferAllocation& desc);
      void deallocateMemory(BufferAllocation& desc);
      };
}

#endif