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
#include "core/parallel/thread.h"
#include "core/rendering/buffer.h"
#include "core/rendering/texture.h"
#include "core/rendering/device.h"

#include "memory/circularQueue.h"

namespace en
{
   //// Single backing allocation 
   //struct BackingAllocation
   //   {
   //   std::shared_ptr<Heap>   heap;       // Heap backing this memory block
   //
   //
   //                                  // Used only for sysMem allocations:
   //
   //
   //   std::shared_ptr<Buffer> buffer;     // Linear buffer of heap size
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
      std::shared_ptr<gpu::Texture> handle;
      
      // surface -> Texture, mipmap, layer
      };
   
   
   
   struct tileRegion2D
      {
      uint16v2 origin;
      uint16v2 count;
      
      tileRegion2D(void);
      };

   static_assert(sizeof(tileRegion2D) == 8, "en::tileRegion2D size mismatch!");
   
   struct tileRegion3D
      {
      struct Origin
         {
         uint16 x;
         uint16 y;
         uint16 z;
         } origin;
       
      struct Count
         {
         uint16 width;
         uint16 height;
         uint16 depth;
         } count;

      tileRegion3D(void);
      };

   static_assert(sizeof(tileRegion3D) == 12, "en::tileRegion3D size mismatch!");
   
   enum class TransferType : uint32
      {
      Buffer   = 0,
      Surface     ,
      Volume      ,
      };
   
   enum class TransferDirection : uint8
      {
      DeviceUpload   = 0,  // System Memory -> Dedicated Memory
      DeviceDownload    ,  // Dedicated Memory -> System Memory
      };
   
   struct TransferBuffer
      {
      uint32 offset;          // Offset in buffer (max buffer size is 4GB)
      uint32 size;            // Size of data transfer (max transfer size is 4GB)
      uint32            : 32; // Padding
      
      uint32 resourceId : 20; // Resource ID (separate per type)
      uint32            : 9;  // Reserved
      uint32 direction  : 1;  // TransferDirection - 0 - Upload to GPU, 1 - Download to System Memory
      uint32 type       : 2;  // TransferType      - 0 - Buffer, 1 - Surface, 2 - Volume
      };
   
   static_assert(sizeof(TransferBuffer) == 16, "en::TransferBuffer size mismatch!");
  
   struct TransferSurface
      {
      tileRegion2D region;    // Origin and size of 2D region to transfer in tiles
      uint32 mipmap     : 5;  // mipmap in range [0..31], allows practically unlimited surface size
      uint32 layer      : 12; // Up to 2048 surfaces on given mipmap level
                              // 2DArray: layer, CubeArray: (layer*6)+face, Cube: face
      uint32            : 14; // Padding
      uint32 plane      : 1;  // 0 - Default plane (Color or Depth), 1 - Secondary plane (Stencil from Depth-Stencil texture)

      uint32 resourceId : 20; // Resource ID (separate per type)
      uint32            : 9;  // Reserved
      uint32 direction  : 1;  // TransferDirection - 0 - Upload to GPU, 1 - Download to System Memory
      uint32 type       : 2;  // TransferType      - 0 - Buffer, 1 - Surface, 2 - Volume
      };

   static_assert(sizeof(TransferSurface) == 16, "en::TransferSurface size mismatch!");
  
   struct TransferVolume
      {
      uint64 x          : 15; // Region origin X in tiles
      uint64 y          : 15; // Region origin Y in tiles
      uint64 z          : 15; // Region origin Z in tiles
      uint64 width      : 15; // Region width in tiles (minus 1)
      uint64 mipmap     : 4;  // mipmap in range [0..15] - up to 32768^3
      uint32 height     : 15; // Region height in tiles (minus 1)
      uint32 depth      : 15; // Region depth in tiles (minus 1)
      uint32 mipmap2    : 1;  // High MipMap [16..31] - up to engine limit (2147483648^3)
      uint32 plane      : 1;  // 0 - Default plane (Color or Depth), 1 - Secondary plane (Stencil from Depth-Stencil texture)
 
      uint32 resourceId : 20; // Resource ID (separate per type)
      uint32            : 9;  // Reserved
      uint32 direction  : 1;  // TransferDirection - 0 - Upload to GPU, 1 - Download to System Memory
      uint32 type       : 2;  // TransferType      - 0 - Buffer, 1 - Surface, 2 - Volume
      };

   static_assert(sizeof(TransferVolume) == 16, "en::TransferVolume size mismatch!");
  
   // Describes transfer of any arbitrary size of data
   // (buffer range, or texture surface sub-region, sub-volume).
   struct TransferResource
      {
      union
         {
         TransferBuffer  buffer;
         TransferSurface surface;
         TransferVolume  volume;
         
         // Common part directly accessible
         struct
            {
            uint64            : 64; // Padding (specialized part)
            uint32            : 32; //
            
            uint32 resourceId : 20; // Resource ID (separate per type)
            uint32            : 9;  // Reserved
            uint32 direction  : 1;  // TransferDirection - 0 - Upload to GPU, 1 - Download to System Memory
            uint32 type       : 2;  // TransferType      - 0 - Buffer, 1 - Surface, 2 - Volume
            };
         };
         
      TransferResource();
      };

   static_assert(sizeof(TransferResource) == 16, "en::TransferResource size mismatch!");



   
   
   
   
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
    std::unique_ptr<gpu::Buffer> buffer; // Linear buffer covering whole heap (it keeps reference to parent Heap, so Heap pointer doesn't need to be stored)
    Allocator*     allocator;         // Algorithm used for sub-allocations in heap
    volatile void* sysAddress;        // Adress of system memory mapped, linear buffer
                                      // that spans across whole heap address range.
};
   
   static_assert(sizeof(BufferCache) == 32, "BufferChache size mismatch!");
   
   struct TextureCache : public ResourceCache
      {
      gpu::Heap* heap;
      };
   
   static_assert(sizeof(TextureCache) == 16, "TextureChache size mismatch!");
   
   // GPU Buffer public structure for referencing resource
   struct BufferAllocation
      {
      volatile void* cpuPointer;     // Pointer to memory location in system memory (offset in mapped buffer)
      gpu::Buffer*   gpuBuffer;      // Updated when made resident
      uint32         gpuOffset;      // Updated when made resident
      uint32         size     : 31;  // Max supported size of single GPU allocation is 2GB
      uint32         resident : 1;   // Set by Streamer when resource is resident in GPU dedicated memory
      };
   
   static_assert(sizeof(BufferAllocation) == 24, "BufferAllocation size mismatch!");
   
   // GPU Buffer internal structure needed only during allocation/deallocation
   struct BufferAllocationInternal
      {
      // Pointers to descriptors in Pool Allocator are calculate on the fly as its rare operation.
      uint32 sysHeapIndex;  // Index to system Heap descriptor
      uint32 gpuHeapIndex;  // Index to dedicated Heap descriptor
      uint32 sysOffset;     // Allocation offset in system Heap
      uint32 locked    : 1; // Cannot be evicted when locked (lock is set on resident call)
      uint32 uploading : 1; // Set, before sending for upload (later checked to ensure it won't be pushed several times)
      uint32           : 30;// Padding to 16 bytes
      };
   
   // TODO:
   // Locking against eviction could be optimized by changing it to locking as
   // a mutex for any type of operation and using Atomic operations to
   // set it, and clear it. That will introduce big overhead for rendering thread
   // or streamer thread thou (each time when resource is used). There need to be
   // easier way to ensure thread safety and prevent unwanted eviction.
   
   static_assert(sizeof(BufferAllocationInternal) == 16, "BufferAllocationInternal size mismatch!");
   
   
   // Textures should allow sparse storage on both ends.
   // Generic buffer suballocation is used in RAM, as multiple of tile size.
   // From it tiles are streamed to VRAM, which also has Heap/Pool of tiles to use.
   // Then those tiles are bound to texture object used during rendering.
   // This may not be supported for all texture types, thus both modes should be
   // handled: fully resident textures, and sparsely resident textures.
   // From CPU side, it shouldn't matter, and should always be handled as
   // Pool of tiles (that may back whole texture if it's small).
   // Mipmaps are assumed to be generated offline, or provided by CPU.
   struct TextureAllocation
      {
      gpu::Texture*     gpuTexture; // Updated when made resident
      gpu::TextureState state;      // Texture state (should be read only)
      uint64 size     : 63;         // Max supported size of single GPU allocation
      uint64 resident : 1;          // Set by Streamer when resource is resident in GPU dedicated memory
      };
    
   // TODO: Above struct should be Resource Manager / Streamer interface.
   //       Correct way would be to hide it's properties behind inlined getters.
   //       MipLayout / Alignments should be exposed as well.
   //       Texture state is duplicated, as texture object can be evicted/destroyed and only RAM copy being available.
   //
   // Asset -> (placed in) -> TextureAllocation -> (streamed to) -> Texture
    
    
   static_assert(sizeof(TextureAllocation) == 32, "TextureAllocation size mismatch!");
   
   enum class SurfaceLayout : uint8
      {
      Linear    = 0,  // Stored linearly in memory texel after texel, line after line
      Tiled2D      ,  // Stored in 2D tiles, 64KB each. Inside of tile, stored linearly.
      Tiled3D      ,  // Stored in 3D tiles, 64KB each. Inside of tile, stored linearly.
      };
   
   // texelSize    - texel data size
   // texelPadding - padding between texels (0 means that texels are tightly packed), power of two
   // rowSize      - size of row data without row padding (but with included paddings between texels)
   // rowPadding   - (0 means tightly packed, otherwise
   // surfaceAlignment - surface data starting adress alignment (dictates padding between


      // rowAlignment - alignment of row starting adress (dictates row pitch)
   
      // width            -
      // height           -
      // depth            - 1 for surfaces, 1 or block depth for volumes
      // texelSize        - texel or texel block data size
      // texelAlignment   - power of two, data alignment (if 0 or equal in size to texel data, then texels are tightly packed)
      // rowAlignment     - power of two, data alignment (if 0 or rowSize % rowAlignment, then rows are tightly packed)
      // surfaceAlignment - power of two, data alignment (if 0 or surfaceSize % surfaceAlignment, then surfaces are tightly packed)
      //
   
//      uint32 texelSize     = 0; // texel or texel block data size
//      uint32 width         = 0; // surface width, or tile width in texels or blocks
//      uint32 height        = 0; // surface height, or tile height in texels or blocks
//      ImageMemoryLayout dataLayout;
//
//      // If texel data size is not multiple of required alignment, then extra padding between texels is required
//      bool iteratePerTexel = texelSize % (1 << texelAlignmentPower) != 0;
//
//      // If row data size is not multiple of required alignment, then extra padding between rows is required
//      bool iteratePerRow   = rowSize % (1 << rowAlignmentPower) != 0;

   
   
   

   // D3D12:
   //
   // rowPitch - The row pitch, or width, or physical size, in bytes, of the subresource data. This must be a multiple of D3D12_TEXTURE_DATA_PITCH_ALIGNMENT (256), and must be greater than or equal to the size of the data within a row.
   //          = roundUp(rowSize, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT); //256
   //
   // Metal:
   //
   // sourceBytesPerRow   - This specifies the stride in bytes between rows of the source buffer memory. The value must be a multiple of the destination texture's pixel size, in bytes. The value must be less than or equal to 32,767 multiplied by the destination texture’s pixel size.
   // sourceBytesPerImage - For 3D textures and 2D array textures, the stride in bytes between 2D images of the source buffer memory. The value must be a multiple of the destination texture's pixel size, in bytes.
   //
   // Vulkan:
   //
   // VkBufferImageCopy
   // regionInfo.bufferRowLength   - 0 means tightly packed
   // regionInfo.bufferImageHeight - 0 means tightly packed??
   
   
   // Sparse textures, being stored only partially in RAM (streamed from HDD or
   // Network), will have different memory layout than those fully backed in RAM.
   struct MipMemoryLayout
      {
      uint64 offset    : 50; // Offset, to first surface in mipmap (from beginning of memory block storing this texture)
      uint64 layers    : 12; // Count of surfaces in this mip level (multiplied by size gives total mipmap storage size). Max is 2048 (2^11).
      uint64 layout    : 2;  // SurfaceLayout - Layout in which surfaces are stored in RAM per mip level
                             // (so small mips are linear and can be loaded at once, while bigger ones are tiled).
      uint32 size;           // Size of single surface in mipmap
                             // Linear - size in bytes (after applying all paddings)
                             // Tiled  - size in tiles (includes right, bottom and back padding)
      gpu::ImageMemoryAlignment alignment; // Details of data layout in memory.
      };
   
   static_assert(sizeof(MipMemoryLayout) == 16, "MipMemoryLayout size mismatch!");
   
   struct TextureAllocationInternal
      {
      MipMemoryLayout* mipLayout; // Memory layout in which surfaces of all mipmaps are stored in system Heap (starting from allocation offset in Heap)
      volatile void* pointer;     // Pointer to data in system memory (global system pointer)

      // Pointers to descriptors in Pool Allocator are calculate on the fly as its rare operation.
      uint32 sysHeapIndex;        // Index to system Heap descriptor
      uint32 gpuHeapIndex;        // Index to dedicated Heap descriptor
      uint32 sysOffset;           // Allocation offset in system Heap

      // Should be accessed as atomic (as it's modified async by different threads)
      uint32 uploading : 31;      // Semaphore set before sending for upload each surface region, cleared when that upload is complete.
                                  // Allows verifying if there are queued upload/download operations on this texture.
      uint32 locked    : 1;       // Cannot be evicted when locked (lock is set on resident call)
      };
  
   static_assert(sizeof(TextureAllocationInternal) == 32, "TextureAllocationInternal size mismatch!");
   
   
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
   
   // Streamer keeps resources in RAM, immediately in GPU visible Buffers, so that
   // they can be immediately transferred to VRAM to one of resident Heaps.
   // Residency management for Buffers is simple, and limits itself to re-allocating
   // Buffers in VRAM Heaps on request. In terms of Textures this is more complicated.
   // 
   
   // TODO: Separate streamer interface from private implementation (this requires some kind of factory)
   class Streamer
      {
      public:
      std::shared_ptr<gpu::GpuDevice> gpu;
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
   
      PoolAllocator<BufferAllocation>* bufferResourcesPool;
      PoolAllocator<BufferAllocationInternal>* bufferResourcesInternalPool;

      PoolAllocator<TextureAllocation>* textureResourcesPool;
      PoolAllocator<TextureAllocationInternal>* textureResourcesInternalPool;
   

      // Dedicated Heap for downloading results of GPU operations from dedicated to system memory
      std::unique_ptr<gpu::Heap>   downloadHeap;
      std::unique_ptr<gpu::Buffer> downloadBuffer;
      volatile void*          downloadAdress;
   
      // Queue of transfers to perform
      CircularQueue<TransferResource>* transferQueue;
   
      // Thread managing asynchronous data streaming
      bool terminating;
      std::unique_ptr<Thread> streamingThread;
   
   
      // Helper methods allocating memory
      bool initSystemHeap(BufferCache& systemCache);
      bool initBufferHeap(BufferCache& bufferCache);
      bool initTextureHeap(TextureCache& textureCache);
      void evictBuffer(BufferAllocation& desc, BufferAllocationInternal& descInternal);
      void evictTexture(TextureAllocation& desc, TextureAllocationInternal& descInternal);

      // Helper methods encoding surface/volume transfers
      bool transferSurface(const TextureAllocation& desc,
                           const uint32 resourceId,
                           const uint8 mipmap,
                           const uint16 layer,
                           const uint8 plane,
                           const TransferDirection direction);
         
      bool transferSurface(const TextureAllocation& desc,
                           const uint32 resourceId,
                           const tileRegion2D region,
                           const uint8 mipmap,
                           const uint16 layer,
                           const uint8 plane,
                           const TransferDirection direction);

      bool transferVolume(const TextureAllocation& desc,
                          const uint32 resourceId,
                          const uint8 mipmap,
                          const TransferDirection direction);

      bool transferVolume(const TextureAllocation& desc,
                          const uint32 resourceId,
                          const tileRegion3D region,
                          const uint8 mipmap,
                          const TransferDirection direction);
         
         
   
      public:
      Streamer(std::shared_ptr<gpu::GpuDevice> gpu, const StreamerSettings* settings = nullptr);
     ~Streamer();
    
      bool allocateMemory(BufferAllocation*& desc, const uint32 size);
      bool makeResident(BufferAllocation& desc, const bool lock = false);
      bool lockMemory(BufferAllocation& desc);
      void unlockMemory(BufferAllocation& desc);
      void evictMemory(BufferAllocation& desc);
      void deallocateMemory(BufferAllocation& desc);
      
      
      // Creation and destruction:
      
      bool allocateMemory(TextureAllocation*& desc,
                          const gpu::TextureState& state);
      void deallocateMemory(TextureAllocation& desc);
      
      // Residency:

      // Only Sparse, single-plane resources can be partially resident or evicted.

      // Make given texture fully resident in GPU dedicated memory
      bool makeResident(TextureAllocation& desc, const bool lock);  // TODO: Remove lock

      // Make given surface resident in GPU dedicated memory
      bool makeResidentSurface(const TextureAllocation& desc,
                               const uint8 mipmap = 0,
                               const uint16 layer = 0,
                               const uint8 plane = 0);

      // Make given volume resident in GPU dedicated memory
      bool makeResidentVolume(const TextureAllocation& desc,
                              const uint8 mipmap = 0);
         
      // Make given surface 2D region resident in GPU dedicated memory
      bool makeResidentRegion2D(const TextureAllocation& desc,
                                const tileRegion2D region,
                                const uint8 mipmap = 0,
                                const uint16 layer = 0);

      // Make given volume 3D region resident in GPU dedicated memory
      bool makeResidentRegion3D(const TextureAllocation& desc,
                                const tileRegion3D region,
                                const uint8 mipmap = 0);


      // Eviction:


      // Evict given texture from GPU dedicated memory
      bool evict(TextureAllocation& desc);

      // Evict given surface from GPU dedicated memory
      bool evictSurface(const TextureAllocation& desc,
                        const uint8 mipmap = 0,
                        const uint16 layer = 0,
                        const uint8 plane = 0);

      // Evict given volume from GPU dedicated memory
      bool evictVolume(const TextureAllocation& desc,
                       const uint8 mipmap = 0);
         
      // Evict given surface 2D region from GPU dedicated memory
      bool evictRegion2D(const TextureAllocation& desc,
                         const tileRegion2D region,
                         const uint8 mipmap = 0,
                         const uint16 layer = 0);

      // Evict given volume 3D region from GPU dedicated memory
      bool evictRegion3D(const TextureAllocation& desc,
                         const tileRegion3D region,
                         const uint8 mipmap = 0);


      // Data access:


      // Surfaces can be stored:
      // - Linear
      // - Tiled-Linear
      // Volumes can be stored:
      // - Planar-Linear
      // - Planar-2D Tiled-Linear
      // - 3D Tiled

      // Returns pointer to system memory backing this texture resource specific surface.
      // Layer property pases specific information, for different texture types:
      // - for 3D it represents "depth" slice
      // - for Array it represents "layer" surface
      // - for CubeMap it represents "face" surface
      // - for CubeMapArray it represents "layer-face" surface
      void* systemMemory(const TextureAllocation& desc,
                         const uint8 mipmap = 0,
                         const uint16 layer = 0,
                         const uint8 plane = 0);  // Plane of multi-plane surface

      // - get storage layout type
      // - get system memory pointer to (mip/slice) or (mip/slice/tile)



      // Submitting data transfer requests:


      // Transfer whole texture between system and GPU dedicated memory
      bool transfer(const TextureAllocation& desc,
                    const TransferDirection direction = TransferDirection::DeviceUpload);
         
      // Transfer surface data between system and GPU dedicated memory
      bool transferSurface(const TextureAllocation& desc,
                           const uint8 mipmap = 0,
                           const uint16 layer = 0,
                           const uint8 plane = 0,
                           const TransferDirection direction = TransferDirection::DeviceUpload);
         
      // Transfer volume data between system and GPU dedicated memory
      bool transferVolume(const TextureAllocation& desc,
                          const uint8 mipmap = 0,
                          const TransferDirection direction = TransferDirection::DeviceUpload);
         
      // Transfer 2D region data between system and GPU dedicated memory
      // (at tile granularity, even if stored in Linear layout)
      bool transferRegion2D(const TextureAllocation& desc,
                            const tileRegion2D region,
                            const uint8 mipmap = 0,
                            const uint16 layer = 0,
                            const uint8 plane = 0,
                            const TransferDirection direction = TransferDirection::DeviceUpload);

      // Transfer 3D region data between system and GPU dedicated memory
      // (at tile granularity, even if stored in Linear layout)
      bool transferRegion3D(const TextureAllocation& desc,
                            const tileRegion3D region,
                            const uint8 mipmap = 0,
                            const TransferDirection direction = TransferDirection::DeviceUpload);
         
      // Convenience menthods:
         
      // Uploads to GPU dedicated memory:
      
      // Update texture in dedicated memory
      bool upload(const TextureAllocation& desc);

      // Update surface in GPU dedicated memory
      bool uploadSurface(const TextureAllocation& desc,
                         const uint8 mipmap = 0,
                         const uint16 layer = 0,
                         const uint8 plane = 0);

      // Update volume in GPU dedicated memory
      bool uploadVolume(const TextureAllocation& desc,
                        const uint8 mipmap = 0);
         
      // Update surface 2D region in GPU dedicated memory (at tile granularity)
      bool uploadRegion2D(const TextureAllocation& desc,
                          const tileRegion2D region,
                          const uint8 mipmap = 0,
                          const uint16 layer = 0,
                          const uint8 plane = 0);

      // Update volume 3D region in GPU dedicated memory (at tile granularity)
      bool uploadRegion3D(const TextureAllocation& desc,
                          const tileRegion3D region,
                          const uint8 mipmap = 0);

      // Downloads from GPU dedicated memory to System memory:

      // Download texture to system memory
      bool download(const TextureAllocation& desc);

      // Download surface to system memory
      bool downloadSurface(const TextureAllocation& desc,
                           const uint8 mipmap = 0,
                           const uint16 layer = 0,
                           const uint8 plane = 0);

      // Download volume to system memory
      bool downloadVolume(const TextureAllocation& desc,
                          const uint8 mipmap = 0);
         
      // Download surface 2D region to system memory (at tile granularity)
      bool downloadRegion2D(const TextureAllocation& desc,
                            const tileRegion2D region,
                            const uint8 mipmap = 0,
                            const uint16 layer = 0,
                            const uint8 plane = 0);

      // Download volume 3D region to system memory (at tile granularity)
      bool downloadRegion3D(const TextureAllocation& desc,
                            const tileRegion3D region,
                            const uint8 mipmap = 0);
      };
}

#endif
