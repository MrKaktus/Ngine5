/*

 Ngine v5.0
 
 Module      : GPU Resource Streamer
 Requirements: none
 Description : Exposes simple interface for management of resource residency in 
               GPU dedicated memory.

*/

#include "rendering/streamer.h"
#include "core/utilities/basicAllocator.h" // TODO: This should be moved out of core as is platform independent
#include "utilities/timer.h"

#define MB 1024*1024

// Size of single allocation in system memory in MB
// May be bigger than resident allocation size, as it's not voulnurable to
// eviction from dedicated memory and thus transfer through PCI-E bus.
// Maximum allocation size in it, cannot exceed ResidentAllocationSize.
#define SystemAllocationSize   128

// Size of single allocation in dedicated memory in MB
#define ResidentAllocationSize 128

// Should be enough to alow 4K 32bpp screenshot transfer
#define DownloadAllocationSize 32

// Maximum allowed memory budget for buffers (in resident allocations count)
#define BufferResidentMemorySize 1

// Maximum allowed memory budget for textures (in resident allocations count)
#define TextureResidentMemorySize 3

// Starting count of GPU resources (assumed average GPU resources count used by apps)
#define DefaultResourcesCount 1024

// Maximum allowed count of GPU resources (1 milion matching 1 milion GPU descriptors)
#define MaximumResourcesCount 1*1024*1024

using namespace en::gpu;

namespace en
{
   //
   //// Allocates RAM for populating resource
   //volatile void* allocateBuffer(const hash resource, const uint64 size);
   //volatile void* allocateTexture(const hash resource, const TextureState settings);
   //
   //// Get handle to resident resource in VRAM
   //BufferView getBufferView(hash resource);
   //<Texture, descriptor set, index in descriptor set> getTexture(hash resource);
   
   // Resource may not be resident
   
   // hash - unique identifier of allocation 
   //        [Heap, offset, size] - unique identifier of allocation on GPU
   //        Buffer - unique iden
   
   // hash -> size (for releasing)
   //      -> Heap, offset (CPU side allocation)
   //      -> Heap/Buffer, offset (GPU side allocation) -> produces BufferView    
   //      -> last time used, livability, metric for deciding residency
   
   
   
   
   // Model - has single skeleton, can have multiple meshes.
   // Single skeleton animation can span across several separate meshes.
   // Each mesh having different set of textures.
   // 
   // mesh - geometry + program + textures
   
   
   
   
   
   
   
   
   // Q: Do we need per-Heap list of resources / residency tracking? 
   
   // Q: Asynchronous connection between Renderer and ResourceStreamers is required.
   
   // Renderer wants to use resource. 
   //
   // Naive aproach:
   // - renderer -> streamer : is resource resident?
   // - streamer -> renderer : yes / no
   // - renderer -> streamer : load resource, notify when ready
   // - renderer - use alternative resource
   // - renderer - log error if no alternative
   // - renderer - mark that resources are not ready (this may be picked up by state machine, loading screen may be displayed)
   // - streamer -> renderer : notifies about resource being ready
   //
   // How automate above interaction to make it transparent without any messages and queeus?
   //
   // Semahpore aproach:
   // - renderer -> streamer : is resource resident? 
   //     >> bool resident == true ? 
   // - streamer -> renderer : yes / no
   //     >> . 
   // - renderer -> streamer : load resource, notify when ready
   //     >> bool locked = true;    // Renderer locks resource for usage, if not loaded streamer needs to make it resident, it cannot remove it until resource is released.
   //     >> if (!locked) { makeResident(hash resource); } // Rendered signals streamer but only once. Then resource is placed on the list of resources to stream.
   //     >> makeResident(hash resource); // May be called without locking resource by renderer (level loader) it will be then twice on residency list, but will quit early when will see that it's already resident.
   // - renderer - use alternative resource
   // - renderer - log error if no alternative
   // - renderer - mark that resources are not ready (this may be picked up by state machine, loading screen may be displayed)
   // - streamer -> renderer : notifies about resource being ready
   //     >> resident = true;   // Renderer will touch needed resources anyway, so signal is not needed
   
   // DescriptorSet's are managed outside of resource streamer.
   
   // Instance -> Model -> Nx [Buffer + offset, Formatting]
   
   
   
   
   // Those two calls are needed only on beginning, or when transferring between GPU's:
   
      //// Creates resource backing memory, visible by GPU
      //volatile void* allocate(const AssetType type, const en::hash resource, const uint64 size);
   
      //// Returns pointer to resource backing memory
      //volatile void* memory(const hash resource);
   
   
   
   // Those two calls are made from time to time:
   
      // Notifies resource streamer that given resource should be made resident. 
      // This call can be done several times for the same resource. It will add 
      // event to the queue of residency requests. After first such event will be 
      // finished, resource "resident" flag will be set, and all consecutive events 
      // for it will be discarded. If resource is requested to be resident by 
      // Renderer, it may mark it as locked. In such case, Streamer is forbidden 
      // to evict resource until Renderer will unlock that resource (this ensures 
      // resource won't be evicted while used for rendering).
      //void makeResident(const hash resource, const bool lock = false);
   
      // Notifies Streamer that given resource may be unlocked. When needed Streamer
      // will lazily evict it from GPU dedicated memory.
      //void release(const hash resource);
   
   
   // Those calls are made constantly:
   //
   //  - isResident? 
   //    no? -> makeResident
   //    yes? -> lock -> use GPU Buffer + offset (those data are already there, in Resource, pointed by instance? YES for primary streamer! No for secondary, as may be resident only on some GPU's?)
   //
   // - streamer is instructer per resource, if it's that resource primary streamer or not.
   
   
   
   // - renderer checks resource status each time it wants to use it
   // - if bool resident == false, and renderer needs it, it sets flag: -makeResident
   // - this means renderer doesn't need any callback, once resource is resident,
   //   
   // Similar async interaction will happen between Renderer and ResourceManager
   //
   // ResourceStreamer may be populated with resources from ResourceManager without Renderer knowledge (spawned by level loader?)
   // Once it's done, logic decides to start render a level?
   // Resources are not stored by Renderer, but by Resource Manager.
   // Renderer uses resources.
   
   
   
   
   
   
   
   
   BufferCache* releaseCache(BufferCache* cache)
   {
   delete cache->allocator;
   
   cache->buffer->unmap();
   cache->buffer = nullptr;
   
   return reinterpret_cast<BufferCache*>(cache->next);
   }
   
   void releaseCacheList(BufferCache* cache)
   {
   while(cache)
      cache = releaseCache(cache);
   }
   
   void releaseTextureCacheList(TextureCache* cache)
   {
   while(cache)
      {
      cache->heap = nullptr;
   
      cache = reinterpret_cast<TextureCache*>(cache->next);
      }
   }
   
   
   // Idle time upload thread is spinning before going to sleep (in seconds)
   #define IdleSpinTime 0.5
   
   // Uploads single allocation from system to GPU dedicated memory
   void uploadResource(Streamer* streamer, const uint32 resourceId)
   {
   // Determine upload queue
   QueueType queueForTransfers = QueueType::Universal;
   if (streamer->gpu->queues(gpu::QueueType::Transfer) > 0u)
      queueForTransfers = QueueType::Transfer;
   
   BufferAllocation*         desc         = streamer->resource->entry(resourceId);
   BufferAllocationInternal* descInternal = streamer->resourceRef->entry(resourceId);

   // TODO: When pushing resource for upload, verify it's uploading flag,
   //       to ensure it wasn't already pushed, then set it.
   assert( descInternal->uploading );
   
   BufferCache* sysCache = streamer->cpuHeap->entry(descInternal->sysHeapIndex);
   BufferCache* gpuCache = streamer->gpuBufferHeap->entry(descInternal->gpuHeapIndex);
   
   shared_ptr<CommandBuffer> command = streamer->gpu->createCommandBuffer(queueForTransfers);
   
   // TODO: Upload data in batches, taking into notice available bandwidth.
   command->copy(*sysCache->buffer,
                 *gpuCache->buffer,
                 desc->size,
                 descInternal->sysOffset,
                 desc->gpuOffset);
   
   command->waitUntilCompleted();
   
   desc->resident = true;
   descInternal->uploading = false;
   }
   
   // Thread performing asynchronous data transfers between CPU and GPU memories
   // (uploads from CPU RAM to GPU dedicated memory, as well as data downloads).
   void* threadAsyncStreaming(Thread* thread)
   {
   Streamer* streamer = static_cast<Streamer*>(thread->state());
   
   // Runs until receives signal from parent Streamer to terminate
   Timer idleTime;
   idleTime.start();
   while(!streamer->terminating)
      {
      // If time since last resource upload, is bigger than N frames, go to
      // sleep, until new resouces will need to be uploaded.
      Time dT = idleTime.elapsed();
      if (dT.seconds() > IdleSpinTime)
         thread->sleep();

      // If thread was sleeping waiting for resource to upload, it was woken up
      // here and will now pick first resource from the list.

      // TODO: In future handle "downloadQueue" and downloading data from dedicated memory to RAM
      //       It is possible that to not collide with draw commands,
      //       Constant Buffers may need to be transferred as well?
      
      // Pick 'resourceId' from queue of resources to make resident
      uint32 resourceId = 0;
      if (streamer->uploadQueue->pop(&resourceId))
         {
         uploadResource(streamer, resourceId);
         idleTime.start();
         }
      }
   
   // Terminate thread
   thread->exit(0);
   return nullptr;
   }

   Streamer::Streamer(shared_ptr<GpuDevice> _gpu, const StreamerSettings* settings) :
      gpu(_gpu),
      queueForTransfers(QueueType::Universal),
      downloadAllocationSize(DownloadAllocationSize*MB),
      systemAllocationSize(SystemAllocationSize*MB),
      residentAllocationSize(ResidentAllocationSize*MB),
      maxBufferResidentSize(BufferResidentMemorySize*residentAllocationSize),
      maxTextureResidentSize(TextureResidentMemorySize*residentAllocationSize),
      downloadBuffer(nullptr),
      downloadAdress(0) 
   {
   // Sanity check of default settings (in case they will be tweaked in future)
   static_assert( powerOfTwo(DownloadAllocationSize) &&
                  (SystemAllocationSize > 0) &&
                  powerOfTwo(SystemAllocationSize) &&
                  (ResidentAllocationSize > 0) &&
                  powerOfTwo(ResidentAllocationSize), 
                  "GPU Resource Streamer default values are invalid!" ); 
   
   if (settings)
      {
      if (powerOfTwo(settings->downloadAllocationSize) &&
          (settings->systemAllocationSize > 0) &&
          powerOfTwo(settings->systemAllocationSize) &&
          (settings->residentAllocationSize > 0) &&
          powerOfTwo(settings->residentAllocationSize) &&
          (settings->maxBufferResidentSize  % settings->residentAllocationSize == 0) &&
          (settings->maxTextureResidentSize % settings->residentAllocationSize == 0))
         {
         downloadAllocationSize = settings->downloadAllocationSize;
         systemAllocationSize   = settings->systemAllocationSize;
         residentAllocationSize = settings->residentAllocationSize;
         maxBufferResidentSize  = settings->maxBufferResidentSize;
         maxTextureResidentSize = settings->maxTextureResidentSize;
         }
      } 
   
   // Available memory
   dedicatedMemorySize = gpu->dedicatedMemorySize();
   systemMemorySize    = gpu->systemMemorySize();
   
   // Bandwidth
   // TODO: Determine maxBandwithPerSecond in bytes.
   
   // Verify memory budget
   uint64 totalRequiredSize = maxBufferResidentSize + maxTextureResidentSize;
   if (dedicatedMemorySize < totalRequiredSize)
      {
      systemMemorySize       = 0;
      maxBufferResidentSize  = 0;
      maxTextureResidentSize = 0;
      }
   
   availableSystemMemory  = systemMemorySize;
   availableBufferMemory  = maxBufferResidentSize;
   availableTextureMemory = maxTextureResidentSize;
   
   // Early end constructor if memory requirements were not meet
   if (!availableSystemMemory ||
       !availableBufferMemory ||
       !availableTextureMemory)
      return;
   
   // Count of system Heap allocations is limited to visible system memory
   uint32 maxSystemHeapCount  = static_cast<uint32>(systemMemorySize / systemAllocationSize);
   
   // Count of Heaps backing resident allocations is constant
   uint32 maxBufferHeapCount  = static_cast<uint32>(maxBufferResidentSize  / residentAllocationSize);
   uint32 maxTextureHeapCount = static_cast<uint32>(maxTextureResidentSize / residentAllocationSize);
   
   // Pre-allocate pools of Heap descriptors 
   cpuHeap        = new PoolAllocator<BufferCache>(64, maxSystemHeapCount);
   gpuBufferHeap  = new PoolAllocator<BufferCache>(64, maxBufferHeapCount);
   gpuTextureHeap = new PoolAllocator<TextureCache>(64, maxTextureHeapCount);
   
   // Heaps are lazily allocated
   cpuHeapCount    = 0;
   gpuHeapCount[0] = 0;
   gpuHeapCount[1] = 0;
   
   cpuHeapList        = nullptr;
   gpuBufferHeapList  = nullptr;
   gpuTextureHeapList = nullptr;
   
   // Pre-allocate pools of Resource descriptors 
   // (due to different entry sizes, even thou allocation patterns will be identical,
   //  reallocatioon will happen at different moment in time for each of the two).
   resource    = new PoolAllocator<BufferAllocation>(DefaultResourcesCount, MaximumResourcesCount);
   resourceRef = new PoolAllocator<BufferAllocationInternal>(DefaultResourcesCount, MaximumResourcesCount);
   
   // Determine which GPU queue is best for data transfers
   if (gpu->queues(gpu::QueueType::Transfer) > 0u)
      queueForTransfers = QueueType::Transfer;
   
   // Allocate dedicated download memory
   if (downloadAllocationSize)
      {
      // Create staging Heap, that can be accessed through linear Buffer.
      shared_ptr<Heap> heap = gpu->createHeap(MemoryUsage::Download, downloadAllocationSize);
      assert( heap );
      shared_ptr<Buffer> buffer = heap->createBuffer(gpu::BufferType::Transfer, downloadAllocationSize);
      assert( buffer );
   
      // Buffer will be always used as source of transfers to GPU VRAM
      shared_ptr<CommandBuffer> command = gpu->createCommandBuffer(queueForTransfers);
      command->start();
      command->barrier(*buffer, BufferAccess::TransferDestination);
      command->commit();
      
      // CPU-GPU synchronization point. 
      // Streamer shouldn't be constructed on the fly, but at application start.
      command->waitUntilCompleted();
   
      // Buffer is always mapped, so that several resources can be downloaded at the same time.
      downloadBuffer = buffer;
      downloadAdress = buffer->map();
      }
      
   uploadQueue = new CircularQueue<uint32>(1024, 4);
   
   // Spawn thread handling asynchronous data transfers
   // (TODO: in future get back to Task-Pool)
   streamingThread = startThread(threadAsyncStreaming, this);
   }
   
   Streamer::~Streamer()
   {
   // Notify streaming thread to terminate itself
   terminating = true;
   streamingThread->waitUntilCompleted();
   
   delete uploadQueue;
   
   downloadBuffer->unmap();
   downloadBuffer = nullptr;
   
   // TODO: Release all resource handles
   
   delete resource;
   delete resourceRef;
   
   releaseCacheList(cpuHeapList);
   releaseCacheList(gpuBufferHeapList);
   releaseTextureCacheList(gpuTextureHeapList);
   
   delete cpuHeap;
   delete gpuBufferHeap;
   delete gpuTextureHeap;
   }
   
   
   
   
   
   //// PCIe 3.0 x16 - 16GB/sec - 
   //
   //// Single:
   //// Upload   - System Memory, single linear buffer with sub-allocations (for texture upload)
   ////            
   //// Download - System Memory, single linear buffer with sub-allocations (for texture download)
   //
   //// Multiple:
   //// Upload   - System Memory, single linear buffer with sub-allocations (buffers upload and eviction backing)
   //// ??       - System Memory, texture allocations (texture eviction backing)
   //
   //// Linear - Dedicated Memory, single linear buffer with sub-allocations (buffers)
   ////          - Heap per buffer type (single cache buffer on Heap -> it's only Vertex and Index streaming)
   ////          - Buffer + sub-allocator (avoid creation of multiple API objects, and thus internal sub-allocator)
   ////           
   //
   //// Tiled  - Dedicated Memory, separate allocations (textures)
   ////          - Heap (+ internal sub-allocator)
   //
   
   bool Streamer::initSystemHeap(BufferCache& systemCache)
   {
   // TODO: There should be always one extra separate system Heap, that can be 
   //       immediately used, without waiting for its Buffer transition to happen.
   //       Such Heap is added to HEaps list when there is no Heap left, and
   //       new backing Heap allocation is spawned in the background.
   
   // Create staging Heap, that can be accessed through linear Buffer.
   shared_ptr<Heap> heap = gpu->createHeap(MemoryUsage::Upload, systemAllocationSize);
   assert( heap );
   shared_ptr<Buffer> buffer = heap->createBuffer(gpu::BufferType::Transfer, systemAllocationSize);
   assert( buffer );
   
   // Buffer will be always used as source of transfers to GPU VRAM
   shared_ptr<CommandBuffer> command = gpu->createCommandBuffer(queueForTransfers);
   command->start();
   command->barrier(*buffer, BufferAccess::TransferSource);
   command->commit();
   command->waitUntilCompleted();  // TODO: Command completion may be delayed by whole frame!!!!!
   
   systemCache.next       = nullptr;  
   systemCache.buffer     = buffer; 
   systemCache.allocator  = new BasicAllocator(systemAllocationSize);
   systemCache.sysAddress = buffer->map(); // Buffer is always mapped, so that several resources can be uploaded at the same time.
   
   return true;
   }
   
   bool Streamer::initBufferHeap(BufferCache& bufferCache)
   {
   // Create linear Heap, that can be accessed through linear Buffer.
   shared_ptr<Heap> heap = gpu->createHeap(MemoryUsage::Linear, residentAllocationSize);
   assert( heap );
   shared_ptr<Buffer> buffer = heap->createBuffer(gpu::BufferType::Vertex, residentAllocationSize);   // TODO: Buffer bitmask! Buffer for everything!
   assert( buffer );
   
   // Buffer will be always used as source of transfers to GPU VRAM
   shared_ptr<CommandBuffer> command = gpu->createCommandBuffer(queueForTransfers);
   command->start();
   command->barrier(*buffer, BufferAccess::TransferDestination | BufferAccess::Read);
   command->commit();
   command->waitUntilCompleted();  // TODO: Command completion may be delayed by whole frame!!!!!
   
   bufferCache.next       = nullptr;  
   bufferCache.buffer     = buffer; 
   bufferCache.allocator  = new BasicAllocator(residentAllocationSize);
   bufferCache.sysAddress = nullptr; // Not used by resident buffers
   
   return true;
   }
      
   // Buffers are kept in both system memory and resident memory, and are accessible
   // by GPU from both locations. Load balancing textures residency will require
   // storing them in system memory but already in tiled form accessible by GPU
   // (and quickly blittable without format conversion). As this is currently not
   // implemented textures share system memory with buffers and are uploaded from
   // there to resident memory. 
   bool Streamer::allocateMemory(BufferAllocation*& desc, const uint32 size)
   {
   // Early return if no memory left
   if (availableSystemMemory < size)
      return false;
   
   // Early return if requested size is bigger than max size of single allocation
   if (systemAllocationSize < size ||
       residentAllocationSize < size)
      return false;
   
   BufferCache** cache = &cpuHeapList;
   
   bool   allocated = false;
   uint64 sysOffset = 0;
   uint32 alignment = 256; // TODO: Platform specific Buffer/Texture alignment!
   
   // Currently it is simplest possible allocation algorithm, iterating over all
   // available heaps to find one with enough free memory. This needs to be redone
   // to proper allocation algorithm (optimal one). For e.g. heaps could be sorted
   // by remaining free space, max and min free block size could be taken into 
   // notice as well.
   while(*cache)
      {
      // Try to allocate memory from current Heap
      allocated = (*cache)->allocator->allocate(size, alignment, sysOffset);
      if (!allocated)
         {
         *cache = reinterpret_cast<BufferCache*>(&((*cache)->next));
         continue;
         }
   
      // Allocate resource
      desc = resource->allocate();
      BufferAllocationInternal* descInternal = resourceRef->allocate();
   
      // Resource Streamer state of buffer allocation (sub-allocated from Heap)
      desc->cpuPointer = reinterpret_cast<volatile void*>(reinterpret_cast<volatile uint8*>((*cache)->sysAddress) + sysOffset);
      desc->gpuBuffer  = nullptr;
      desc->gpuOffset  = 0;
      desc->size       = size;
      desc->resident   = false;
   
      // Resource Streamer internal state of buffer allocation 
      descInternal->locked    = false;
      bool result = cpuHeap->index(**cache, descInternal->sysHeapIndex);
      assert( result );
      descInternal->sysOffset = static_cast<uint32>(sysOffset);
      descInternal->gpuHeapIndex = 0;  // TODO: Distinguish index 0 from unused index!
   
      availableSystemMemory -= size;
      break;
      }
   
   // If reached end of list, allocate new Heap
   if (!allocated && !*cache)
      {
      BufferCache* systemCache = cpuHeap->allocate();
      if (systemCache)
         {
         initSystemHeap(*systemCache);
         *cache = systemCache;
         cpuHeapCount++;
   
         // Allocate memory from new Heap
         allocated = (*cache)->allocator->allocate(size, alignment, sysOffset);
   
         // This is new empty Heap so allocation should always succeed
         assert( allocated );
   
         // Allocate resource
         desc = resource->allocate();
         BufferAllocationInternal* descInternal = resourceRef->allocate();
   
   #ifdef EN_DEBUG
         // Both allocators need to remain in sync
         uint32 resourceA = 0;
         uint32 resourceB = 0;
         resource->index(*desc, resourceA);
         resourceRef->index(*descInternal, resourceA);
         assert( resourceA == resourceB );
   #endif
   
         // Resource Streamer state of buffer allocation (sub-allocated from Heap)
         desc->cpuPointer = reinterpret_cast<volatile void*>(reinterpret_cast<volatile uint8*>((*cache)->sysAddress) + sysOffset);
         desc->gpuBuffer  = nullptr;
         desc->gpuOffset  = 0;
         desc->size       = size;
         desc->resident   = false;
   
         // Resource Streamer internal state of buffer allocation 
         descInternal->locked    = false;
         bool result = cpuHeap->index(**cache, descInternal->sysHeapIndex);
         assert( result );
         descInternal->sysOffset = static_cast<uint32>(sysOffset);
         descInternal->gpuHeapIndex = 0;  // TODO: Distinguish index 0 from unused index!
   
         availableSystemMemory -= size;
         }
      }
   
   return allocated;
   }
   

   // TODO: Texture variant needed!
   void Streamer::evictBuffer(BufferAllocation& desc, BufferAllocationInternal& descInternal)
   {
   // TODO: Make it thread safe
   assert( !descInternal.locked );
   
   // Release resource from GPU
   BufferCache* gpuHeap = gpuBufferHeap->entry(descInternal.gpuHeapIndex);
   gpuHeap->allocator->deallocate(desc.gpuOffset, desc.size);

   desc.resident  = false;
   desc.gpuBuffer = nullptr;
   desc.gpuOffset = 0;
   
   availableBufferMemory += desc.size;
   }

   bool Streamer::makeResident(BufferAllocation& desc, const bool lock)
   {
   // Requested size need to be smaller than max size of single allocation,
   // and that should be already verified during memory allocation.
   assert( desc.size <= residentAllocationSize );

   // TODO: Lock critical section here
   
   // If resource is already resident quit
   if (desc.resident)
      return true;

   // Verify if descriptor is valid
   uint32 resourceId = 0;
   if (!resource->index(desc, resourceId))
      return false;
   
   // Verify if this resource is not already marked for upload
   BufferAllocationInternal* descInternal = resourceRef->entry(resourceId);
   assert( descInternal );
   if (descInternal->uploading)
      return true;
   
   // (A) Allocate destination in dedicated memory
   
   // If not enoough dedicated memory, evict least used one
   while(availableBufferMemory < desc.size)
      {
      // TODO: Evict old allocations until enough memory is freed
      assert( 0 );
      };

   BufferCache** cache = &gpuBufferHeapList;
   
   bool   allocated = false;
   uint64 gpuOffset = 0;
   uint32 alignment = 256; // TODO: Platform specific Buffer/Texture alignment!
   uint32 size      = desc.size;
   
   // Currently it is simplest possible allocation algorithm, iterating over all
   // available heaps to find one with enough free memory. This needs to be redone
   // to proper allocation algorithm (optimal one). For e.g. heaps could be sorted
   // by remaining free space, max and min free block size could be taken into 
   // notice as well.
   while(*cache)
      {
      // Try to allocate memory from current Heap
      allocated = (*cache)->allocator->allocate(size, alignment, gpuOffset);
      if (!allocated)
         {
         *cache = reinterpret_cast<BufferCache*>(&((*cache)->next));
         continue;
         }
   
      // Resource Streamer state of buffer allocation (sub-allocated from Heap)
      desc.gpuBuffer = (*cache)->buffer;
      desc.gpuOffset = static_cast<uint32>(gpuOffset);

      // Resource Streamer internal state of buffer allocation
      bool result = gpuBufferHeap->index(**cache, descInternal->gpuHeapIndex);
      assert( result );

      availableBufferMemory -= size;
      break;
      }
   
   // If reached end of list, allocate new Heap
   if (!allocated && !*cache)
      {
      BufferCache* gpuCache = gpuBufferHeap->allocate();
      if (gpuCache)
         {
         initBufferHeap(*gpuCache);
         *cache = gpuCache;
         gpuHeapCount[0]++;
   
         // Allocate memory from new Heap
         allocated = (*cache)->allocator->allocate(size, alignment, gpuOffset);
   
         // This is new empty Heap so allocation should always succeed
         assert( allocated );

         // Resource Streamer state of buffer allocation (sub-allocated from Heap)
         desc.gpuBuffer = (*cache)->buffer;
         desc.gpuOffset = static_cast<uint32>(gpuOffset);

         // Resource Streamer internal state of buffer allocation
         bool result = gpuBufferHeap->index(**cache, descInternal->gpuHeapIndex);
         assert( result );

         availableBufferMemory -= size;
         }
      }
   
   // If after all couldn't allocate dedicated memory for resource, it looks like
   // we're in bad place (VRAM is fragmented?).
   if (!allocated)
      return false;

   // (B) Stream asynchronously from RAM

   // Push 'resourceId' on a queue of resources to upload
   if (!uploadQueue->push(resourceId))
      return false;
      
   descInternal->locked    = lock;
   descInternal->uploading = true;
   
   // If streaming thread was idle, it went to sleep, wake it up for upload
   if (streamingThread->sleeping())
      streamingThread->wakeUp();

   return true;
   }
   
   bool Streamer::lockMemory(BufferAllocation& desc)
   {
   uint32 resourceId = 0;
   if (!resource->index(desc, resourceId))
      return false;
   
   // TODO: Between if check, and switch of "locked" there may be race condition
   BufferAllocationInternal* descInternal = resourceRef->entry(resourceId);
   assert( descInternal );
   if (desc.resident || descInternal->uploading)
      {
      descInternal->locked = true;
      return true;
      }
      
   return false;
   }

   void Streamer::unlockMemory(BufferAllocation& desc)
   {
   uint32 resourceId = 0;
   if (!resource->index(desc, resourceId))
      return;
   
   // Resource can always be unlocked (made evictable by streamer)
   BufferAllocationInternal* descInternal = resourceRef->entry(resourceId);
   assert( descInternal );
   descInternal->locked = false;
   }

   void Streamer::evictMemory(BufferAllocation& desc)
   {
   uint32 resourceId = 0;
   if (!resource->index(desc, resourceId))
      return;
   
   BufferAllocationInternal* descInternal = resourceRef->entry(resourceId);
   assert( descInternal );
   descInternal->locked = false;
   
   // Evict from dedicated memory if resident
   if (desc.resident)
      evictBuffer(desc, *descInternal);
   }

   void Streamer::deallocateMemory(BufferAllocation& desc)
   {
   uint32 resourceId = 0;
   if (!resource->index(desc, resourceId))
      return;
   
   BufferAllocationInternal* descInternal = resourceRef->entry(resourceId);
   assert( descInternal );
   
   // Evict from dedicated memory if resident
   if (desc.resident)
      evictBuffer(desc, *descInternal);
   
   BufferCache* sysHeap = cpuHeap->entry(descInternal->sysHeapIndex);
   sysHeap->allocator->deallocate(descInternal->sysOffset, desc.size);
   
   availableSystemMemory += desc.size;
   
   // Release resource descriptor
   resource->deallocate(desc);
   resourceRef->deallocate(*descInternal);
   }
   
   

   

   
   
   
   //gpuHeapList[0] = gpuBufferHeap->allocate();
   //gpuHeapList[1] = gpuTextureHeap->allocate();
   
   // Alloc:
   // - iterate over CPU Heaps
   //   - in each Heap, try to find empty space
   //   - sub-allocate region
   //     - create sub-allocation descriptor
   //     - add it to the list
   
   // - allocation
   //   -> size
   //   -> CPU Heap, offset
   //   -> GPU Heap, offset
   
   // CPU Heaps array -> growing.
   // GPU Heaps array -> fixed size. <- can point directly to!
   
   
   
   
   /*
   BackingAllocation* result = new BackingAllocation();
   
   // Allocate heap
   result->heap = gpu->createHeap(MemoryUsage::Upload, size);
   if (!result->heap)
      {
      delete result;
      allocation = nullptr;
      return false;
      }
   
   // Allocate buffer
   result->buffer = result->heap->createBuffer(BufferType::Transfer, size);
   if (!result->buffer)
      {
      result->heap = nullptr;
      delete result;
      allocation = nullptr;
      return false;
      }
   
   result->allocator = new BasicAllocator(size)
   */
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   // Streamer 
   // - controls resource residency in VRAM (load balances between RAM and VRAM)
   // - one instance per GPU
   // - gets VRAM limit it can use, and backing RAM limit
   // - may request resource load from HDD when not enough RAM
   
   // - give it a resource to load
   // - tell it each time resource is used (so it can bookkeep who to evict)
   
   
   // Scene manager:
   // - has list of instances
   // - determines their LOD levels
   // - tells Streamer which resources it needs.
   
   // Instance -> LOD's -> Model -> Mesh -> Material -> Texture (mipmap count/texture quality depends on LOD level).
   
   // each surface has it's unique identifier:
   
   // HASH - unique filename
   
   
   // HDD accessible location is determined as:
   // - package
   //   - Hash / Filename
   //     * Mipmap
   //     * Layer / Face / Depth
   
   // CPU RAM location is determined as:
   
   // GPU accessible location is determined as:
   // - Heap
   //   - Texture
   //     - Mipmap
   //     - Layer / Face / Depth
   
   
 
   
   
   // Texture LOD's streaming:
   // - Objects are Data Structures
   // - Material is set of Texture references
   // - Material textures can be streamed in async way
   //   - Material async load:
   //     - give streamera array of resource ID's to load, and they destination in data structure
   //     - streamer loads them async, and updates data structure on it's own (data structure is passed to Streamer)
   //     - material can be used, and it's quality will automatically increase together with loaded LOD's
   
   // - Filename
   
   
   // Assets:
   //
   // - mesh
   // - gradient
   // - function
   // - surface
   // - volume
   // - cube
   //
   /// above asset types are then streamed to:
   // 
   // - mesh               -> Buffer
   // - gradient, function -> specific mipmap in Texture1D or layer in Texture1DArray
   // - surface            -> specific mipmap in Texture2D or in layer of Texture2DArray
   // - volume             -> specific mipmap in Texture3D
   // - cube               -> specific mipmap in TextureCube or range in TextureCubeMapArray
   // 
   
   // - lod:
   //   - texture
   //   - mipmap
   //   - layer
   //
   // Material:
   // - lod:
   // - texture
}
