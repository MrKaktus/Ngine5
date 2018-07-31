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
   
   
   // Current model assumes encoding and decoding of data transfers on tile
   // granularity. Reconsider separating "streamer" thread, and allowing
   // passing to it async transfers directly (so screenshots and other special
   // case transfers can be done on a side from "Streamer" class abstraction
   // that currently also manages resources allocation in RAM and have some
   // residency control in place (and thus shifts into direction of Resource
   // Manager more than simple streamer).
   //
   // Transfers are encoded in tile resolution granularity, to allow support
   // for large virtual textures. This creates some disadvantage for classic
   // textures where updates of data may be wasteful (due to rounding up
   // transfer area to tile size).
   //
   // Need to measure if there is gain, from storing textures pre-tiled in
   // system memory, and uploading them on tile granularity. Theoretically
   // this way transfer reads from consecutive adress range in RAM, and writes
   // to consecutive adress range in VRAM. (VRAM swizzling may still occur).
   
   // Uploads single allocation from system to GPU dedicated memory
   void uploadResource(Streamer* streamer, const TransferResource transfer)
   {
   // Determine upload queue
   // TODO: This should be done once, on Streamer start
   QueueType queueForTransfers = QueueType::Universal;
   if (streamer->gpu->queues(gpu::QueueType::Transfer) > 0u)
      queueForTransfers = QueueType::Transfer;
   
   // TODO: In future distribute transfers to different queues in the same queue type family

   // Determine transfer data size
   uint32 transferSize = transfer.buffer.size;
   if (transfer.type == underlyingType(TransferType::Surface))
      {
      transferSize = transfer.surface.region.count.x *
                     transfer.surface.region.count.y * 64 * KB;
      }
   if (transfer.type == underlyingType(TransferType::Volume))
      {
      transferSize = transfer.volume.width  *
                     transfer.volume.height *
                     transfer.volume.depth  * 64 * KB;
      }
   
   // TODO: Handle splitting workload?
   
   // TODO: Handle asynchronous bi-directional transfers
   
   // Buffer transfer
   if (transfer.type == underlyingType(TransferType::Buffer))
      {
      BufferAllocation*         desc         = streamer->bufferResourcesPool->entry(transfer.resourceId);
      BufferAllocationInternal* descInternal = streamer->bufferResourcesInternalPool->entry(transfer.resourceId);

      // TODO: When pushing resource for upload, verify it's uploading flag,
      //       to ensure it wasn't already pushed, then set it.
      assert( descInternal->uploading );
      
      BufferCache* sysCache = streamer->cpuHeap->entry(descInternal->sysHeapIndex);
      BufferCache* gpuCache = streamer->gpuBufferHeap->entry(descInternal->gpuHeapIndex);
      
      shared_ptr<CommandBuffer> command = streamer->gpu->createCommandBuffer(queueForTransfers);
      
      // TODO: Upload data in batches, taking into notice available bandwidth.
      //       Time budget of N miliseconds for data upload should be specified.
      //       Ensuring that it will not collide with rendering thread / Compositor?
      command->start();
      command->copy(*sysCache->buffer,
                    *gpuCache->buffer,
                    desc->size,
                    descInternal->sysOffset,
                    desc->gpuOffset);
      command->commit();
      
      // Sleep until transfer is done
      command->waitUntilCompleted();
      
      desc->resident = true;
      descInternal->uploading = false;
      }
   
   // Vulkan doesn't support transfers between Buffer and MSAA surfaces:
   //
   // See: 18.4. Copying Data Between Buffers and Images
   // "dstImage must have a sample count equal to VK_SAMPLE_COUNT_1_BIT"
   // "srcImage must have a sample count equal to VK_SAMPLE_COUNT_1_BIT"
   
   // TODO: Image Transfer Granularity:
   //
   // "The imageOffset and imageExtent members of each element of pRegions must
   //  respect the image transfer granularity requirements of commandBuffer’s
   //  command pool’s queue family, as described in VkQueueFamilyProperties"
   
   // Texture surface
   if (transfer.type == underlyingType(TransferType::Surface))
      {
      TextureAllocation*         desc         = streamer->textureResourcesPool->entry(transfer.resourceId);
      TextureAllocationInternal* descInternal = streamer->textureResourcesInternalPool->entry(transfer.resourceId);

      BufferCache* sysCache = streamer->cpuHeap->entry(descInternal->sysHeapIndex);

      shared_ptr<CommandBuffer> command = streamer->gpu->createCommandBuffer(queueForTransfers);
   
      command->start();
   
      // Init resource in VRAM
      if (!desc->resident)
         {
         // TODO: Separate init from uploadResource, move to make Resident
       
         // Transition all surfaces in texture
         command->barrier(*desc->gpuTexture, TextureAccess::CopyDestination | TextureAccess::Read);
         }
   

      uint32v2 mipResolution   = desc->state.mipResolution(transfer.surface.mipmap);
      uint16v2 tileResolution  = tileResolution2D(desc->state.format, desc->state.samples);
      uint16v2 blockResolution = texelBlockResolution(desc->state.format);
   
      // Determine memory layout in which surface is stored in system memory
      uint32 surfaceGroup = transfer.surface.mipmap * desc->state.planes() +
                            transfer.surface.plane;
      
      MipMemoryLayout* mipLayout = &descInternal->mipLayout[surfaceGroup];
   
      // Multi-plane surfaces can only be stored in Linear layout
      assert( !(mipLayout->layout != underlyingType(SurfaceLayout::Linear) && (transfer.surface.plane > 0)) );
      
      if (mipLayout->layout == underlyingType(SurfaceLayout::Linear))
         {
         // (A) Linear layout - Surface region is transfered at once
         //                     (Surface is tightly packed, without padding
         //                      on right and bottom edge that is needed in
         //                      tiled layout).

         // Offset to surface in system memory allocation
         uint64 surfaceOffset = mipLayout->offset +
                                transfer.surface.layer * mipLayout->size;

         uint32v2 texelOrigin;
         texelOrigin.x = transfer.surface.region.origin.x * tileResolution.width;
         texelOrigin.y = transfer.surface.region.origin.y * tileResolution.height;
         
         uint32v2 texelRegion;
         texelRegion.width  = transfer.surface.region.count.width  * tileResolution.width;
         texelRegion.height = transfer.surface.region.count.height * tileResolution.height;

         // Clamp tile-based granularity region to surface resolution
         if (texelOrigin.x + texelRegion.width > mipResolution.width)
            texelRegion.width  = mipResolution.width  - texelOrigin.x;
         if (texelOrigin.y + texelRegion.height > mipResolution.height)
            texelRegion.height = mipResolution.height - texelOrigin.y;

         // Fast path for transferring whole surface at once
         if (texelOrigin.x      == 0 &&
             texelOrigin.y      == 0 &&
             texelRegion.width  == mipResolution.width &&
             texelRegion.height == mipResolution.height)
            {
            command->copy(*sysCache->buffer,
                          descInternal->sysOffset + surfaceOffset,
                          mipLayout->alignment.rowPitch(texelRegion.width),
                          *desc->gpuTexture,
                          transfer.surface.mipmap,
                          transfer.surface.layer);
            }
         else // Transfer sub-region of surface
            {
            // Area origin will always be aligned to tile boundary and thus block boundary
            assert( texelOrigin.x % blockResolution.width == 0 );
            assert( texelOrigin.y % blockResolution.height == 0 );
            
            // Take into account possibility of surface storing compressed texture
            uint16 mipWidthInBlocks = intDivUp(mipResolution.width, blockResolution.width);
               
            // Surface is stored in Linear layout, so passed row pitch is for whole surface
            uint32 srcRowPitch = mipLayout->alignment.rowPitch(mipWidthInBlocks);
               
            // Calculates offset in surface, taking into notice block compression
            uint32 blockX = texelOrigin.x / blockResolution.width;
            uint32 blockY = texelOrigin.y / blockResolution.height;
            uint32 offset = (blockY * srcRowPitch) +
                            (blockX * mipLayout->alignment.texelPitch());
            
            // Clamp region back to block granularity (if stored as compressed)
            texelRegion.width  = roundUp(texelRegion.width,  blockResolution.width);
            texelRegion.height = roundUp(texelRegion.height, blockResolution.height);

            // Transfer region of given surface
            command->copyRegion2D(
               *sysCache->buffer,
               descInternal->sysOffset + surfaceOffset + offset,
               srcRowPitch,
               *desc->gpuTexture,
               transfer.surface.mipmap,
               transfer.surface.layer,
               texelOrigin,
               texelRegion,
               transfer.surface.plane);
            }
         }
      else
      if (mipLayout->layout == underlyingType(SurfaceLayout::Tiled2D))
         {
         // (B) Tiled layout - Source surface region is transferred in tiles
         
         // Offset to surface in system memory allocation
         uint64 surfaceOffset = mipLayout->offset +
                                transfer.surface.layer * (mipLayout->size * 64 * KB);

         uint16 mipWidthInTiles = intDivUp(mipResolution.width, tileResolution.width);
            
         // Take into account possibility of tiles storing compressed texture
         uint16 tileWidthInBlocks = intDivUp(tileResolution.width, blockResolution.width);

         // Surface is stored in Tiled2D layout, so passed row pitch is for tile
         uint32 srcRowPitch = mipLayout->alignment.rowPitch(tileWidthInBlocks);

         // Row after row, tile after tile in each row
         for(uint32 i=0; i<transfer.surface.region.count.height; ++i)
            for(uint32 j=0; j<transfer.surface.region.count.width; ++j)
               {
               uint32v2 tileOrigin;
               tileOrigin.x = transfer.surface.region.origin.x + j;
               tileOrigin.y = transfer.surface.region.origin.y + i;

               uint32 tileIndex  = (mipWidthInTiles * tileOrigin.y) + tileOrigin.x;
               uint64 tileOffset = tileIndex * 64 * KB;
               
               uint32v2 texelOrigin;
               texelOrigin.x = tileOrigin.x * tileResolution.width;
               texelOrigin.y = tileOrigin.y * tileResolution.height;
               
               uint32v2 texelRegion(tileResolution.x, tileResolution.y);

               // Copy one tile at a time
               command->copyRegion2D(
                  *sysCache->buffer,
                  descInternal->sysOffset + surfaceOffset + tileOffset,
                  srcRowPitch,
                  *desc->gpuTexture,
                  transfer.surface.mipmap,
                  transfer.surface.layer,
                  texelOrigin,
                  texelRegion,
                  transfer.surface.plane);
               }
         }
      else
         {
         // Unsupported tiled layout
         assert( 0 );
         }

      command->commit();
      
      // Sleep until transfer is done
      command->waitUntilCompleted();
      
      desc->resident = true;
      descInternal->uploading--; // TODO: Atomic
      }
         
         
   // Volume transfer
   if (transfer.type == underlyingType(TransferType::Volume))
      {
      TextureAllocation*         desc         = streamer->textureResourcesPool->entry(transfer.resourceId);
      TextureAllocationInternal* descInternal = streamer->textureResourcesInternalPool->entry(transfer.resourceId);
      
      BufferCache* sysCache = streamer->cpuHeap->entry(descInternal->sysHeapIndex);
      
      shared_ptr<CommandBuffer> command = streamer->gpu->createCommandBuffer(queueForTransfers);
      
      command->start();
      
      // Init resource in VRAM
      if (!desc->resident)
         {
         // TODO: Separate init from uploadResource, move to make Resident
         
         // Transition all surfaces in texture
         command->barrier(*desc->gpuTexture, TextureAccess::CopyDestination | TextureAccess::Read);
         }
         
      // 3D MSAA textures are currently not supported
      assert( desc->state.samples == 1 );
      
      // 3D Multi-Plane Volumes are currently not supported
      assert( desc->state.planes() == 1 );
      
      // Decode transferred volume mipmap level
      uint8 mipmap = (transfer.volume.mipmap2 << 4) | transfer.volume.mipmap;
      
      // Decode transferred volume size in tiles
      uint16 tilesWidth  = transfer.volume.width + 1;
      uint16 tilesHeight = transfer.volume.height + 1;
      uint16 tilesDepth  = transfer.volume.depth + 1;

      uint32v3 mipVolume       = desc->state.mipVolume(mipmap);
      uint16v4 tileResolution  = tileResolution3D(desc->state.format);
      uint16v2 blockResolution = texelBlockResolution(desc->state.format);

      // Tiles are always composed of blocks and thus have multiple
      // of block resolution. Area origin is always aligned to tile
      // boundary, and thus it's aligned to block boundary as well.
      assert( tileResolution.width  % blockResolution.width  == 0 );
      assert( tileResolution.height % blockResolution.height == 0 );
      
      // Take into account possibility storing block-compressed texture
      uint16 mipWidthInBlocks  = intDivUp(mipVolume.width, blockResolution.width);
      uint16 tileWidthInBlocks = tileResolution.width / blockResolution.width;
         
      // Determine memory layout in which surface/volume is stored in system memory
      MipMemoryLayout* mipLayout = &descInternal->mipLayout[mipmap];
   
      // Offset to volume in system memory allocation
      uint64 volumeOffset = mipLayout->offset;

      // Transferred volume depth range in texels
      uint32v2 depthRange;
      depthRange.base  = transfer.volume.z * tileResolution.depth;
      depthRange.count = tilesDepth * tileResolution.depth;
         
      // Clamp tile-based depth granularity to volume depth (Linear & Tiled2D)
      if (depthRange.base + depthRange.count > mipVolume.depth)
         depthRange.count = mipVolume.depth - depthRange.base;
         


      
      // TODO: Transfer Volume (Linear and Tiled2D, Tiled3D?)
      
      // Volume is stored line after line, depth plane after depth plane
      if (mipLayout->layout == underlyingType(SurfaceLayout::Linear))
         {
         uint32v2 texelOrigin;
         texelOrigin.x = transfer.volume.x * tileResolution.width;
         texelOrigin.y = transfer.volume.y * tileResolution.height;

         uint32v2 texelRegion;
         texelRegion.width  = tilesWidth  * tileResolution.width;
         texelRegion.height = tilesHeight * tileResolution.height;
         
         // Clamp tile-based granularity region to volume resolution
         if (texelOrigin.x + texelRegion.width > mipVolume.width)
            texelRegion.width  = mipVolume.width  - texelOrigin.x;
         if (texelOrigin.y + texelRegion.height > mipVolume.height)
            texelRegion.height = mipVolume.height - texelOrigin.y;

         // Fast path for transferring whole depth planes at once
         if (texelOrigin.x      == 0 &&
             texelOrigin.y      == 0 &&
             texelRegion.width  == mipVolume.width &&
             texelRegion.height == mipVolume.height)
            {
            // Clamp region back to block granularity (if stored as compressed)
            texelRegion.width  = roundUp(texelRegion.width,  blockResolution.width);
            texelRegion.height = roundUp(texelRegion.height, blockResolution.height);

            for(uint32 i=0; i<depthRange.count; ++i)
               {
               uint32 depthPlane  = depthRange.base + i;
               uint64 planeOffset = depthPlane * mipLayout->alignment.surfacePitch(mipVolume.width, mipVolume.height);

               command->copy(*sysCache->buffer,
                             descInternal->sysOffset + volumeOffset + planeOffset,
                             mipLayout->alignment.rowPitch(texelRegion.width),
                             *desc->gpuTexture,
                             mipmap,
                             depthPlane);
               }
            }
         else // Transfer sub-volume
            {
            // Volume is stored in Linear layout, so passed row pitch is for depth plane
            uint32 srcRowPitch = mipLayout->alignment.rowPitch(mipWidthInBlocks);
               
            // Calculates offset in depth plane, taking into notice block compression
            uint32 blockX = texelOrigin.x / blockResolution.width;
            uint32 blockY = texelOrigin.y / blockResolution.height;
            uint32 offset = (blockY * srcRowPitch) +
                            (blockX * mipLayout->alignment.texelPitch());
            
            // Clamp region back to block granularity (if stored as compressed)
            texelRegion.width  = roundUp(texelRegion.width,  blockResolution.width);
            texelRegion.height = roundUp(texelRegion.height, blockResolution.height);

            for(uint32 i=0; i<depthRange.count; ++i)
               {
               uint32 depthPlane  = depthRange.base + i;
               uint64 planeOffset = depthPlane * mipLayout->alignment.surfacePitch(mipVolume.width, mipVolume.height);

               // Transfer region of given depth plane surface
               command->copyRegion2D(
                  *sysCache->buffer,
                  descInternal->sysOffset + volumeOffset + planeOffset + offset,
                  srcRowPitch,
                  *desc->gpuTexture,
                  mipmap,
                  depthPlane,
                  texelOrigin,
                  texelRegion,
                  0);
               }
            }
         }
      else
      if (mipLayout->layout == underlyingType(SurfaceLayout::Tiled2D))
         {
         // (B) Tiled layout - Source surface region is transferred in tiles
         
         uint16v2 tileSize = tileSize2D(desc->state.format, desc->state.samples);

         uint16 mipWidthInTiles  = intDivUp(mipVolume.width,  tileResolution.width);
         uint16 mipHeightInTiles = intDivUp(mipVolume.height, tileResolution.height);
         uint32 mipSizeInTiles   = mipWidthInTiles * mipHeightInTiles;

         // Row after row, tile after tile in each row, plane after plane
         for(uint32 i=0; i<depthRange.count; ++i)
            for(uint32 j=0; j<tilesHeight; ++j)
               for(uint32 k=0; k<tilesWidth; ++k)
                  {
                  // Calculate offset in source buffer
                  uint32v3 tileOrigin;
                  tileOrigin.x = transfer.volume.x + k;
                  tileOrigin.y = transfer.volume.y + j;
                  tileOrigin.z = transfer.volume.z + i;

                  uint32 tileIndex  = (mipSizeInTiles * tileOrigin.z) +
                                      (mipWidthInTiles * tileOrigin.y) +
                                      tileOrigin.x;

                  uint64 tileOffset = tileIndex * 64 * KB;

                  // Volume is stored in Tiled2D layout, so passed row pitch is for tile
                  uint32 srcRowPitch = mipLayout->alignment.rowPitch(tileWidthInBlocks);

                  uint16 depthPlane = depthRange.base + i;

                  // Calculate sub-region in depth plane
                  uint32v2 texelOrigin;
                  texelOrigin.x = tileOrigin.x * tileResolution.width;
                  texelOrigin.y = tileOrigin.y * tileResolution.height;
               
                  uint32v2 texelRegion(tileResolution.x, tileResolution.y);

                  // Clamp tile-based granularity to 2D plane resolution
                  if (texelOrigin.x + texelRegion.width > mipVolume.width)
                     texelRegion.width  = mipVolume.width  - texelOrigin.x;
                  if (texelOrigin.y + texelRegion.height > mipVolume.height)
                     texelRegion.height = mipVolume.height - texelOrigin.y;

                  // Clamp region back to block granularity (if stored as compressed)
                  texelRegion.width  = roundUp(texelRegion.width,  blockResolution.width);
                  texelRegion.height = roundUp(texelRegion.height, blockResolution.height);

                  // Copy one tile at a time
                  command->copyRegion2D(
                     *sysCache->buffer,
                     descInternal->sysOffset + volumeOffset + tileOffset,
                     srcRowPitch,
                     *desc->gpuTexture,
                     mipmap,
                     depthPlane,
                     texelOrigin,
                     texelRegion,
                     0);          // Plane 0
                  }
         }
      else
      if (mipLayout->layout == underlyingType(SurfaceLayout::Tiled3D))
         {
         // TODO!
         }
      else
         {
         // Unsupported tiled layout
         assert( 0 );
         }
      
      command->commit();
      
      // Sleep until transfer is done
      command->waitUntilCompleted();
      
      desc->resident = true; // TODO: Now resident doesn't mean up-to-date, uploading == 0 is equal to up to date state.
      descInternal->uploading--; // TODO: Atomic
      }
      
   // Open questions:
   //
   // Currently streamer thread commits single Command Buffer per transfer
   // operation (which may include several blits in case of tiled resources).
   // As streamer thread needs to wait for Command Buffer completion, before
   // next transfer can be submitted, making this operation non-blocking by
   // using CB completion callback handler may not be benefitial.
   //
   // Consider:
   // - Elevating Command Buffer above, to batch several transfer operations,
   //   and to allow interleaving several transfer operations as partial blits.
   //   This may prevent fragmentation in Kernel, as single transfer Command
   //   Buffer with batched sub-transfers would be commited each frame.
   //   Such model requires also some kind of caching of status of several
   //   transfers in flight, so that streamer thread can switch between them
   //   and gradually progress their transfers with interleaving of those.
   // - Is there CPU/driver overhead from issuing multiple 64KB blits instead
   //   of few big ones?
   // Metal: Is there perf impact from encoding each blit in separate blit encoder?
   //
   }
   

 
   // textureID - unique ID of texture resource
   // surfaceID - unique ID of surface to transfer, can be also stored as:
   //             - offset (source linear)
   //             - size   (source linear)
   //             - mipmap (destination)
   //             - layer  (destination 3D: depth, Array: layer, CubeArray: layer*6+face, Cube: face)
   //             as each surface is stored linearly (at least currently)
   //
   // OR
   //
   // surfaceId + tileID - id of tile inside of one of surfaces
   //                      - offset to tile in texture
   //                      - tile size (this one is const, unless mip tail)
   //                      -
   //

   // Create resource
   // Make it resident
   //  - allocate in in GPU VRAM
   //  - init state -> (Command Buffer needed only in D3D12 and Vulkan)
   //  - transfer content
   //
   // System Adress
   //  - get adress of sub-resource
   //
   // Update content
   //  - transfer content
   //
   
   

   
   tileRegion2D::tileRegion2D()
   {
   }
   
   tileRegion3D::tileRegion3D()
   {
   }
  
   struct region2D
      {
      uint32v2 origin;
      union{ uint32v2 size; uint32v2 count; };
      
      region2D(void);
      };
   
   static_assert(sizeof(region2D) == 16, "en::region2D size mismatch!");
  
   region2D::region2D()
   {
   }
   
   struct region3D
      {
      uint32v3 origin;
      union{ uint32v3 size; uint32v3 count; };
      
      region3D(void);
      };
   
   static_assert(sizeof(region3D) == 24, "en::region3D size mismatch!");
  
   region3D::region3D()
   {
   }
   
   TransferResource::TransferResource(void)
   {
   buffer.offset     = 0;
   buffer.size       = 0;
   buffer.resourceId = 0;
   buffer.direction  = underlyingType(TransferDirection::DeviceUpload);
   buffer.type       = underlyingType(TransferType::Buffer);
   }
   
   
   // Upload any arbitrary size of data (buffer range, or texture surface sub-region).
   // Based on GPU upload bandwith divide updates into separate blit operations.
   //
   
   // Future proof streamer would handle transfers in batches of 64KB.
   
   
   // texture, mipmap, layer, plane - unique identifier of 2D surface (plane: depth or stencil)
   //           offset, size - unique block description in Linear layout
   //            tile, count - unique block description in Tiled layout (each tile having fixed size)
   //
   // offset, size - linear region to transfer (in bytes)
   // origin, size - region in 2D or 3D to transfer (in tiles or texels)
   



  
  
  
   //  64x64  - min 2D tile resolution
   // 768x768 - max 2D tile resolution

   // Minimum supported 3D volume size: 524288^3
   // Maximum supported 3D volume size: 6422528 * 6422528 * 524288
   // Volume size in texels depends on used Format.
   // Maximum supported 3D volume size is 32768^3 tiles, 64KB each.
   
   //  16x16x16 - min 3D tile
   //  32x32x16 - avg 3D tile
   //  64x64x16 - most common 3D tile
   //196x196x16 - max 3D tile

   //                                                         4bit|
   // 0,1,2,3,4 ,5 ,6 ,7  ,8  ,9  ,10  ,11  ,12  ,13  ,14   ,15
   // 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8096,16384,32768
   //
   // 16   ,17    ,18    ,19    ,20     ,21     ,22      .. ,31
   // 65536,131072,262144,524288,1048576,2097152,4194304 .. ,2147483648

   // 32768 x 128 = 4194304



  
  

//      // Texture surface identifier
//      uint32 textureId : 20; // ID of texture resource in system (not in GPU), 1mln limit
//      uint32 layer     : 11; // Up to 2048 surfaces on given mipmap level
//                             // 3D: depth, 2DArray: layer, CubeArray: (layer*6)+face, Cube: face
//      uint32 plane     : 1;  // 0 - Default plane (Color or Depth), 1 - Secondary plane (Stencil from Depth-Stencil texture)
//      uint32 mipmap    : 5;  // mipmap in range [0..31], allows practically unlimited surface size

//      // Linear region - 8 bytes
//      // Tile region 2D - 8 bytes
//      // Tile region 3D - 12 bytes
//      // Texel region 2D - 16 bytes
//      // Texel region 3D - 24 bytes

//    Even if surface is stored in Linear layout, it can be transferred or updated
//    as set of 2D/3D region transfers. Those can be of size of single tile in
//    VRAM (after adress swizzling), or size of multiple tiles.
//    Transfer of such region is still described on tile granularity, as textures
//    are stored in tiled manner in VRAM anyway.
//

      // Convert any arbitrary 2D rect area, into set of 2D tiles, 64KB in size
      // each. Taking into notice surface size and pixel format.
      // Even if surface is stored in linear layout, it may be updated in blocks?
   
   
      // Sparse textures:
      //
      // D3D12 Residency - https://msdn.microsoft.com/en-us/library/windows/desktop/mt186622(v=vs.85).aspx
      // D3D12 Tiled Resources - https://msdn.microsoft.com/en-us/library/windows/desktop/dn786480(v=vs.85).aspx
      // D3D12, Tier3, 3D: https://msdn.microsoft.com/en-us/library/windows/desktop/dn903951(v=vs.85).aspx
      //
      // Vulkan Sparse Images - https://vulkan.lunarg.com/doc/view/1.0.30.0/windows/vkspec.chunked/ch28s04.html
   
   
   
      // There are two types of backing memory layout:
      // - Linear - whole surface content is stored in linear manner
      //            (with optional padding per row)
      // - TiledLinear - whole surface is divided into 64KB tiles, inside of each
      //                 storage remains linear
      //
      // Linear storage treats whole surface as single big tile to unify algorithm.




#include "utilities/utilities.h"

   // Count of tiles needed to store 2D surface of given resolution in texels
   forceinline uint16v2 tilesCount2D(const uint16v2 tileResolution, const uint32v2 surfaceResolution)
   {
   assert( tileResolution.width > 0 );
   assert( tileResolution.height > 0 );
   assert( surfaceResolution.width > 0 );
   assert( surfaceResolution.height > 0 );
   
   // Round-Up to closest tile boundary
   uint16v2 tileCount;
   tileCount.width  = intDivUp(surfaceResolution.width,  tileResolution.width);
   tileCount.height = intDivUp(surfaceResolution.height, tileResolution.height);

   return tileCount;
   }
   
   // Count of tiles needed to store 3D volume of given resolution in texels
   forceinline uint16v4 tilesCount3D(const uint16v4 tileResolution, const uint32v3 volumeResolution)
   {
   assert( tileResolution.width > 0 );
   assert( tileResolution.height > 0 );
   assert( tileResolution.depth > 0 );
   assert( volumeResolution.width > 0 );
   assert( volumeResolution.height > 0 );
   assert( volumeResolution.depth > 0 );
   
   // Round-Up to closest tile boundary
   uint16v4 tileCount;
   tileCount.width  = intDivUp(volumeResolution.width,  tileResolution.width);
   tileCount.height = intDivUp(volumeResolution.height, tileResolution.height);
   tileCount.depth  = intDivUp(volumeResolution.depth,  tileResolution.depth);
   
   return tileCount;
   }
   
   // Region in tiles, containing given 2D region in texels
   tileRegion2D tilesRegion2D(const uint16v2 tileResolution, const region2D texels)
   {
   tileRegion2D tiles;
   
   // Round-Down to closest tile boundary
   tiles.origin.x = texels.origin.x / tileResolution.width;
   tiles.origin.y = texels.origin.y / tileResolution.height;

   // Round-Up to closest tile boundary
   tiles.count.width  = intDivUp(texels.size.width  + (texels.origin.x % tileResolution.width),  tileResolution.width);
   tiles.count.height = intDivUp(texels.size.height + (texels.origin.y % tileResolution.height), tileResolution.height);

   return tiles;
   };
   
   // Region in tiles, containing given 3D region in texels
   tileRegion3D tilesRegion3D(const uint16v4 tileResolution, const region3D texels)
   {
   tileRegion3D tiles;
   
   // Round-Down to closest tile boundary
   tiles.origin.x = texels.origin.x / tileResolution.width;
   tiles.origin.y = texels.origin.y / tileResolution.height;
   tiles.origin.z = texels.origin.z / tileResolution.depth;

   // Round-Up to closest tile boundary
   tiles.count.width  = intDivUp(texels.size.width  + (texels.origin.x % tileResolution.width),  tileResolution.width);
   tiles.count.height = intDivUp(texels.size.height + (texels.origin.y % tileResolution.height), tileResolution.height);
   tiles.count.depth  = intDivUp(texels.size.depth  + (texels.origin.z % tileResolution.depth),  tileResolution.depth);
   
   return tiles;
   };

   // uint16v2 tilesCount = tilesCount2D(tileResolution, surfaceResolution);
   // uint16 tilesInRow = tilesCount.width;
   
   // Convert texel position to tile index
   forceinline uint32 tileIndex(const uint32v2 texel, const uint16 tilesInRow, const uint16v2 tileResolution)
   {
   uint16v2 tileOrigin;
   tileOrigin.x = texel.x / tileResolution.width;
   tileOrigin.y = texel.y / tileResolution.height;
   
   return tileOrigin.y * tilesInRow + tileOrigin.x;
   }

   // Convert tile origin to tile index
   forceinline uint32 tileIndex(const uint16v2 tileOrigin, const uint16 tilesInRow)
   {
   return tileOrigin.y * tilesInRow + tileOrigin.x;
   }

   // srcOffset = tileIndex * 64 * KB;

   // Convert tile index in given surface to texel position
   forceinline uint32v2 tileOrigin(const uint32 tile, const uint16 tilesInRow, const uint16v2 tileResolution)
   {
   uint32v2 origin;

   uint32 tileRow = tile / tilesInRow;
   uint32 tileCol = tile % tilesInRow;
   
   origin.x = tileCol * tileResolution.width;
   origin.y = tileRow * tileResolution.height;
   
   return origin;
   }
   
   // Smallest linearly stored block of memory in tile:
   // - resolution in texels
   // - total size in bytes
   //
   uint16v4 tileLine2D(const Format format)
   {
   uint16v2 tileSize        = tileSize2D(format);
   uint16v2 tileResolution  = tileResolution2D(format);
   uint16v2 blockResolution = texelBlockResolution(format);
   
   return uint16v4(tileResolution.width, blockResolution.height, tileSize.width, 0);
   }
   
   // Copies surface of any size, stored in Linear layout, to Tiled layout storage.
   // (tiled layout storage needs to have enough memory to take into notice tile
   // padding in both bottom and right side of texture).
   // Multi-Plane surfaces are always stored in Linear layout.
   void copyLinearToTiled(uint8* src, uint8* dst, const Format format, const uint32v2 surfaceResolution)
   {
   uint16v2 tileSize        = tileSize2D(format);
   uint16v2 tileResolution  = tileResolution2D(format);
   uint16v2 tilesCount      = tilesCount2D(tileResolution, surfaceResolution);
   uint8    blockSize       = texelSize(format);
   uint16v2 blockResolution = texelBlockResolution(format);
   uint32   rowSize         = intDivUp(surfaceResolution.width, blockResolution.width) * blockSize;
   
   assert( surfaceResolution.width  % blockResolution.width  == 0 );
   assert( surfaceResolution.height % blockResolution.height == 0 );
   
   uint32 tile = 0;
   uint32 row  = 0;
   for(uint16 tilesRow=0; tilesRow<tilesCount.height; ++tilesRow)
      {
      // Calculate amount of rows of data to copy (takes into notice bottom row padding)
      uint32 rowsToCopy = tileResolution.height;
      uint32 remainingLines = surfaceResolution.height - tilesRow * tileResolution.height;
      if (remainingLines < tileResolution.height)
         rowsToCopy = remainingLines;
      
      // If texture is compressed, rows count is smaller than lines count in tile
      rowsToCopy = intDivUp(rowsToCopy, blockResolution.height);
      
      // Split N rows, into set of row fragments each, and distribute them among tiles
      for(uint16 tileLine=0; tileLine<rowsToCopy; ++tileLine)
         {
         uint16 localTileIndex = tile;
         for(uint16 tileInRow=0; tileInRow<tilesCount.width; ++tileInRow)
            {
            uint32 dstOffset = (localTileIndex * 64 * KB) + (tileLine * tileSize.width);
            uint32 srcOffset = (row * rowSize) + (tileInRow * tileSize.width);
            
            // Takes into notice tile padding, in last tile at the end of row.
            uint32 size = tileSize.width;
            uint32 remainingTexels = surfaceResolution.width - (tileInRow * tileResolution.width);
            if (remainingTexels < tileResolution.width)
               size = intDivUp(remainingTexels, blockResolution.width) * blockSize;
            
            // Copy tile content one tile line at a time
            memcpy(static_cast<void*>(dst + dstOffset),
                   static_cast<void*>(src + srcOffset),
                   size);

            localTileIndex++;
            }
         row++;
         }
      tile += tilesCount.width;
      }
   }
   
   
   
   


   
      // Engine needs to know in which layout data is stored in system memory.
      // (tiled for most, but linear for streaming Render Targets, even when they
      //  are not part of Streamer memory pool)
      //
      // - make given region2D/region3D resident (for sparse resources)
      // - evict given region2D/region3D
      // - get storage layout type
      // - get system memory pointer to (mip/slice) or (mip/slice/tile)
      // - update region2D/region3D in resident copy (or system copy)
      //   transferRegion2D(), transferRegion3D()
   
   
   // Tiles backing given surface are stored in system memory in linear order,
   // row after row, plane after plane.
   

   


   

   

   
   
   // Thread performing asynchronous data transfers between CPU and GPU memories
   // (uploads from CPU RAM to GPU dedicated memory, as well as data downloads).
   void* threadAsyncStreaming(Thread* thread)
   {
   Streamer* streamer = static_cast<Streamer*>(thread->state());
   
   // Runs until receives signal from parent Streamer to terminate
   // (needs to be woken up first!)
   while(!streamer->terminating)
      {
      // TODO: In future handle "downloadQueue" and downloading data from dedicated memory to RAM
      
      // TODO: In future, have different queues based on size of resource transfer.
      //       For e.g. big uploads could be interleaved with small ones.
      //       Upload priority could be driven by resource % contribution to
      //       object (mesh, surface, volume). This way lowest mip-maps will
      //       have highest contribution as single 64KB tile can store several
      //       surfaces from mip-tail. The bigger the mip surface, it's tiles
      //       contribution will drop (so mipmap value in transfer descriptor
      //       is some metric of contribution already).
      
      // Pick 'resourceId' from queue of resources to make resident
      
      TransferResource transfer;
      if (streamer->transferQueue->pop(&transfer))
         {
         if (transfer.direction == underlyingType(TransferDirection::DeviceUpload))
            uploadResource(streamer, transfer);
            
         // TODO: Parallel download
         }
      else // Go to sleep, until new resouces will need to be uploaded.
         thread->sleep();
         
      // If thread was sleeping waiting for resource to upload, it was woken up
      // here. It will first check, if it shouldn't terminate, and if not, it
      // will try to pick first transfer for execution from the list.
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
      downloadAdress(0),
      terminating(false)
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
   bufferResourcesPool         = new PoolAllocator<BufferAllocation>(DefaultResourcesCount, MaximumResourcesCount);
   bufferResourcesInternalPool = new PoolAllocator<BufferAllocationInternal>(DefaultResourcesCount, MaximumResourcesCount);
   
   // Pre-allocate pool of texture resource descriptors
   textureResourcesPool         = new PoolAllocator<TextureAllocation>(DefaultResourcesCount, MaximumResourcesCount);
   textureResourcesInternalPool = new PoolAllocator<TextureAllocationInternal>(DefaultResourcesCount, MaximumResourcesCount);

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
   
      // Buffer will be always used as source of transfers from GPU VRAM
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
      
   transferQueue = new CircularQueue<TransferResource>(1024, 4);
   
   // Spawn thread handling asynchronous data transfers
   // (TODO: in future get back to Task-Pool)
   streamingThread = startThread(threadAsyncStreaming, this);
   }
   
   Streamer::~Streamer()
   {
   // Notify streaming thread to terminate itself
   terminating = true;
   streamingThread->waitUntilCompleted();
   
   delete transferQueue;
   
   downloadBuffer->unmap();
   downloadBuffer = nullptr;
   
   // TODO: Release all resource handles
   
   delete textureResourcesPool;
   delete textureResourcesInternalPool;
   delete bufferResourcesPool;
   delete bufferResourcesInternalPool;
   
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
   
   bool Streamer::initTextureHeap(TextureCache& textureCache)
   {
   textureCache.next = nullptr;
   textureCache.heap = gpu->createHeap(MemoryUsage::Tiled, residentAllocationSize);
   assert( textureCache.heap );
   
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
      desc = bufferResourcesPool->allocate();
      BufferAllocationInternal* descInternal = bufferResourcesInternalPool->allocate();
   
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
         desc = bufferResourcesPool->allocate();
         BufferAllocationInternal* descInternal = bufferResourcesInternalPool->allocate();
   
   #ifdef EN_DEBUG
         // Both allocators need to remain in sync
         uint32 resourceA = 0;
         uint32 resourceB = 0;
         bufferResourcesPool->index(*desc, resourceA);
         bufferResourcesInternalPool->index(*descInternal, resourceA);
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
   if (!bufferResourcesPool->index(desc, resourceId))
      return false;
   
   // Verify if this resource is not already marked for upload
   BufferAllocationInternal* descInternal = bufferResourcesInternalPool->entry(resourceId);
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


   // Transfer whole buffer to VRAM
   TransferResource transfer;
   transfer.type          = underlyingType(TransferType::Buffer);
   transfer.direction     = underlyingType(TransferDirection::DeviceUpload);
   transfer.resourceId    = resourceId;
   transfer.buffer.offset = 0;
   transfer.buffer.size   = desc.size;

   if (!transferQueue->push(transfer))
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
   if (!bufferResourcesPool->index(desc, resourceId))
      return false;
   
   // TODO: Between if check, and switch of "locked" there may be race condition
   BufferAllocationInternal* descInternal = bufferResourcesInternalPool->entry(resourceId);
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
   if (!bufferResourcesPool->index(desc, resourceId))
      return;
   
   // Resource can always be unlocked (made evictable by streamer)
   BufferAllocationInternal* descInternal = bufferResourcesInternalPool->entry(resourceId);
   assert( descInternal );
   descInternal->locked = false;
   }

   void Streamer::evictMemory(BufferAllocation& desc)
   {
   uint32 resourceId = 0;
   if (!bufferResourcesPool->index(desc, resourceId))
      return;
   
   BufferAllocationInternal* descInternal = bufferResourcesInternalPool->entry(resourceId);
   assert( descInternal );
   descInternal->locked = false;
   
   // Evict from dedicated memory if resident
   if (desc.resident)
      evictBuffer(desc, *descInternal);
   }

   void Streamer::deallocateMemory(BufferAllocation& desc)
   {
   uint32 resourceId = 0;
   if (!bufferResourcesPool->index(desc, resourceId))
      return;
   
   BufferAllocationInternal* descInternal = bufferResourcesInternalPool->entry(resourceId);
   assert( descInternal );
   
   // Evict from dedicated memory if resident
   if (desc.resident)
      evictBuffer(desc, *descInternal);
   
   BufferCache* sysHeap = cpuHeap->entry(descInternal->sysHeapIndex);
   sysHeap->allocator->deallocate(descInternal->sysOffset, desc.size);
   
   availableSystemMemory += desc.size;
   
   // Release resource descriptor
   bufferResourcesPool->deallocate(desc);
   bufferResourcesInternalPool->deallocate(*descInternal);
   }
   
   

   // TEXTURES LAYOUT IN MEMORY
   /////////////////////////////////////////////////////////////////////////////
   


   #include "core/utilities/memory.h"
   #include "utilities/utilities.h"
   
   /*
   
   // TODO: Doesn't take into account sparse textures
   // TODO: Who specifies size of tile pool for given sparse texture?
   //       0 - Whole texture resident
   //       N - amount of 64KB tiles

   // Render targets are always stored in memory in Linear layout
   bool isSparse = underlyingType(state.usage) & underlyingType(TextureUsage::Sparse);
   
   const uint32 systemBacking;    // Amount of RAM dedicated to back tiles pool for this sparse texture
   const uint32 dedicatedBacking; // Amount of VRAM dedicated to back tiles pool for this sparse texture

   */

   // Texture Memory Layout:
   // - Multi-plane textures are always stored in Linear layout (Depth32-Stencil8, YCbCr, etc.)
   // - Render Targets are always stored in Linear layout
   // - MSAA textures are tiled (unless specified as Render-Targets)
   // - other textures are stored in linear layout for mip-tail
   // - mip-tail is defined by mipmap resolution, once mipmap resolution is
   //   bigger than tile resolution for given format, it's stored as tiled
   //
   // TODO: 3D Textures stored as 3D Tiles !!!
   //
   MipMemoryLayout* generateTextureMemoryLayout(const gpu::TextureState& state, const GpuDevice* gpu)
   {
   bool hasSecondaryPlane = state.planes() > 1;
   
   // Render targets are always stored in memory in Linear layout
   bool isRenderTarget = underlyingType(state.usage) & underlyingType(TextureUsage::RenderTargetWrite);
   
   // 16 bytes per mip level, up to 512 bytes (for 32 mipmaps)
   MipMemoryLayout* mipLayout = en::allocate<MipMemoryLayout>(sizeof(MipMemoryLayout), state.mipmaps * state.planes());
   
   // Mipmaps are stored on disk from smallest ones to biggest one, so when they
   // are loaded from disk, read is sequential and seek is one directional.
   //
   // In memory mipmaps are stored from biggest one, to smallest one (data in
   // destination memory may be offseted between surfaces to take into notice
   // necessary alignment for GPU reads).
   //
   // Mipmaps are loaded to GPU dedicated memory (VRAM) from smallest ones, so
   // that textures can be used as soon as possible (and their more detailed mip
   // levels can be later streamed in parallel). Smallest mip levels may not be
   // present (as 1x1, 2x2 textures aren't very usefull and at this point object
   // should be culled anyway).
   uint64 offset = 0;
   for(uint32 i=0; i<state.mipmaps; ++i)
      {
      for(uint32 j=0; j<state.planes(); ++j)
         {
         // TODO: In future handle YCbCr different resolutions per plane
         //       (mipResolution() should accept plane as parameter)
         uint32v2 mipResolution  = state.mipResolution(i);
         uint16v2 tileResolution = tileResolution2D(state.format, state.samples);

         // Default memory alignment for texture data
         // Can be overriden by specific GPU requirements if optional GPU handle is provided.
         mipLayout[i].alignment.sampleSize = gpu::texelSize(state.format, j);
         mipLayout[i].alignment.samplesCount(state.samples);
         mipLayout[i].alignment.surfaceAlignment(cacheline);
   
         // If memory layout is calculated for specific GPU, optimal row pading,
         // and surface address alignment is taken into notice.
         if (gpu)
            mipLayout[i].alignment = gpu->textureMemoryAlignment(state, i, 0);

         // Surface is tiled, if it's width and height are both bigger or equal to
         // tile width and height. It's a tradeoff between keeping surfaces in optimal
         // layout for GPU uploads (tiled), and wasting memory on unneccessary padding
         // (for e.g. when storing very long and thin textures).
         mipLayout[i].layout = underlyingType(SurfaceLayout::Linear);
         if (!hasSecondaryPlane && !isRenderTarget)
            if ((mipResolution.width  >= tileResolution.width ) &&
                (mipResolution.height >= tileResolution.height))
               mipLayout[i].layout = underlyingType(SurfaceLayout::Tiled2D);
         
         // TODO: Determine for 3D textures, if want to use Tiled3D layout (needs to be supported by GPU / API).
         
         if (mipLayout[i].layout == underlyingType(SurfaceLayout::Linear))
            {
            uint16v2 blockResolution = texelBlockResolution(state.format);
            uint32   blockWidth      = intDivUp(mipResolution.width,  blockResolution.width);
            uint32   blockHeight     = intDivUp(mipResolution.height, blockResolution.height);
            
            // Size of single surface after applying all paddings
            mipLayout[i].size = mipLayout[i].alignment.surfaceSize(blockWidth, blockHeight);
            }
         else
         if (mipLayout[i].layout == underlyingType(SurfaceLayout::Tiled2D))
            {
            // Mip surface resolution, after rounding up to multiple of tile resolution.
            uint32v2 mipTiledResolution;
            mipTiledResolution.width  = roundUp(mipResolution.width,  tileResolution.width);
            mipTiledResolution.height = roundUp(mipResolution.height, tileResolution.height);
            
            // Size in tiles, of surface stored in Tiled layout (including right and bottom paddings)
            uint16v2 tilesCount = tilesCount2D(tileResolution, mipTiledResolution);
            mipLayout[i].size = tilesCount.width * tilesCount.height;
            }
         else
         if (mipLayout[i].layout == underlyingType(SurfaceLayout::Tiled3D))
            {
            uint32v3 mipResolution  = state.mipVolume(i);
            uint16v4 tileResolution = tileResolution3D(state.format);
            
            // Mip volume resolution, after rounding up to multiple of tile resolution.
            uint32v3 mipTiledResolution;
            mipTiledResolution.width  = roundUp(mipResolution.width,  tileResolution.width);
            mipTiledResolution.height = roundUp(mipResolution.height, tileResolution.height);
            mipTiledResolution.depth  = roundUp(mipResolution.depth,  tileResolution.depth);
            
            // Size in tiles, of volume stored in Tiled layout (including right, bottom and back paddings)
            uint16v4 tilesCount = tilesCount3D(tileResolution, mipTiledResolution);
            mipLayout[i].size = tilesCount.width * tilesCount.height * tilesCount.depth;
            }

         // Starting offset in memory allocation. As mipmap surfaces are stored
         // from bigger to smaller one, first mipmap defines memory alignment to
         // which whole texture memory allocation should be aligned. Then
         // consecutive mipmap surfaces will have the same or smaller memory
         // alignment, that will be already taken into notice in their global
         // offset.
         mipLayout[i].offset = 0;
         if (i > 0)
            mipLayout[i].offset = roundUp(offset, static_cast<uint64>(mipLayout[i].alignment.surfaceAlignment()));
         
         // Surfaces stored in this mip level
         if (state.type == TextureType::Texture3D)
            {
            // If 3D texture is stored in Linear or Tiled2D layout, amount of
            // "surfaces" in given mip level is equal to that mip depth planes
            // count. If it's stored in Tiled3D layout, then there is no notion
            // of "surfaces", but it's single volume.
            uint32 depthPlanes = max(1U, static_cast<uint32>(state.layers >> i));
            if (mipLayout[i].layout == underlyingType(SurfaceLayout::Linear) ||
                mipLayout[i].layout == underlyingType(SurfaceLayout::Tiled2D))
               mipLayout[i].layers = depthPlanes;
            else
            if (mipLayout[i].layout == underlyingType(SurfaceLayout::Tiled3D))
               {
               // ... each "surface" is composed of 3D tiles, that gather N depth
               // planes (where N is depth of 3D tile).
               //
               // uint16v4 tileResolution = tileResolution3D(state.format);
               // mipLayout[i].layers = intDivUp(depthPlanes, tileResolution.depth);
               
               mipLayout[i].layers = 1;
               }
            }
         else
            mipLayout[i].layers = state.layers;
            
         // Mipmap size in memory is equal to size of all it's surfaces padded
         // (in case of tiled surfaces, this size needs to be multiplied by 64KB).
         uint64 mipSize = 0;
         if (mipLayout[i].layout == underlyingType(SurfaceLayout::Linear))
            mipSize = mipLayout[i].layers * mipLayout[i].size;
         else
         if (mipLayout[i].layout == underlyingType(SurfaceLayout::Tiled2D) ||
             mipLayout[i].layout == underlyingType(SurfaceLayout::Tiled3D))
            mipSize = mipLayout[i].layers * (mipLayout[i].size << 16);
            
         // Next mipmap starting offset
         offset += mipSize;
         }
      }
      
   return mipLayout;
   }



   // TEXTURES
   /////////////////////////////////////////////////////////////////////////////



   // Mipmaps in container are stored from smallest one to biggest one.
   // uint32 surfaceOffset[2048]; // If each surface is separately compressed using variable length
   //                             // compression, then it may be beneficial to keep max 8KB offset table
   //                             // specifying each surface offset from given mip adress in container.
   
   // Intel - tiles based on byte size
   // AMD   - tiles based on texel size (co different tile layouts based on bpp)
   //         can cast textures with the same bpp only?
   // NV    - stores texture format info in Page Table Entries, so cannot cast easily
   
   
   
   bool Streamer::allocateMemory(TextureAllocation*& desc, const gpu::TextureState& state)
   {
   // Calculate texture layout in system memory
   MipMemoryLayout* mipLayout = generateTextureMemoryLayout(state, gpu.get());
   
   // Calculate total amount of memory needed to store texture in CPU RAM, taking
   // into account GPU specific padding per "surface", "line", "texel" or "block".
   uint32 surfaceGroups = state.mipmaps * state.planes();
   uint32 lastGroup = surfaceGroups - 1;
   uint64 size = mipLayout[lastGroup].offset +
                 mipLayout[lastGroup].layers * roundUp(mipLayout[lastGroup].size, mipLayout[lastGroup].alignment.surfaceAlignment());
   
   // Early return if no memory left
   if (availableSystemMemory < size)
      return false;
   
   // Early return if requested size is bigger than max size of single allocation
   if (systemAllocationSize < size ||
       residentAllocationSize < size)
      return false;
   
   BufferCache** cache = &cpuHeapList;
   
   bool   allocated = false;
   uint64 sysOffset = 0;  // Offset at which memory region is allocated in Buffer

   // Currently it is simplest possible allocation algorithm, iterating over all
   // available heaps to find one with enough free memory. This needs to be redone
   // to proper allocation algorithm (optimal one). For e.g. heaps could be sorted
   // by remaining free space, max and min free block size could be taken into
   // notice as well. Ideally, CPU side Heaps, should be replaced with one huge
   // Heap, and residency would be managed on per resource, per tile basis instead
   // of paging in and out whole Heaps.
   while(*cache)
      {
      // Try to allocate memory from current Heap
      allocated = (*cache)->allocator->allocate(size, mipLayout[0].alignment.surfaceAlignment(), sysOffset);
      if (!allocated)
         {
         *cache = reinterpret_cast<BufferCache*>(&((*cache)->next));
         continue;
         }
   
      // Allocate resource
      desc = textureResourcesPool->allocate();
      TextureAllocationInternal* descInternal = textureResourcesInternalPool->allocate();
   
      // Resource Streamer state of texture allocation (sub-allocated from Heap)
      desc->gpuTexture = nullptr;
      desc->state      = state;
      desc->size       = size;
      desc->resident   = false;
   
      // Resource Streamer internal state of buffer allocation
      descInternal->mipLayout    = mipLayout;
      descInternal->pointer      = reinterpret_cast<volatile void*>(reinterpret_cast<volatile uint8*>((*cache)->sysAddress) + sysOffset);
      bool result = cpuHeap->index(**cache, descInternal->sysHeapIndex);
      assert( result );
      descInternal->sysOffset    = static_cast<uint32>(sysOffset);
      descInternal->gpuHeapIndex = 0;  // TODO: Distinguish index 0 from unused index!
      descInternal->uploading    = 0;
      descInternal->locked       = false;

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
         allocated = (*cache)->allocator->allocate(size, mipLayout[0].alignment.surfaceAlignment(), sysOffset);
   
         // This is new empty Heap so allocation should always succeed
         assert( allocated );
   
         // Allocate resource
         desc = textureResourcesPool->allocate();
         TextureAllocationInternal* descInternal = textureResourcesInternalPool->allocate();
   
   #ifdef EN_DEBUG
         // Both allocators need to remain in sync
         uint32 resourceA = 0;
         uint32 resourceB = 0;
         textureResourcesPool->index(*desc, resourceA);
         textureResourcesInternalPool->index(*descInternal, resourceA);
         assert( resourceA == resourceB );
   #endif

         // Resource Streamer state of texture allocation (sub-allocated from Heap)
         desc->gpuTexture = nullptr;
         desc->state      = state;
         desc->size       = size;
         desc->resident   = false;
   
         // Resource Streamer internal state of buffer allocation
         descInternal->mipLayout    = mipLayout;
         descInternal->pointer      = reinterpret_cast<volatile void*>(reinterpret_cast<volatile uint8*>((*cache)->sysAddress) + sysOffset);
         bool result = cpuHeap->index(**cache, descInternal->sysHeapIndex);
         assert( result );
         descInternal->sysOffset    = static_cast<uint32>(sysOffset);
         descInternal->gpuHeapIndex = 0;  // TODO: Distinguish index 0 from unused index!
         descInternal->uploading    = 0;
         descInternal->locked       = false;
      
         availableSystemMemory -= size;
         }
      }
   
   return allocated;
   }
   
   void Streamer::deallocateMemory(TextureAllocation& desc)
   {
   uint32 resourceId = 0;
   if (!textureResourcesPool->index(desc, resourceId))
      return;
   
   TextureAllocationInternal* descInternal = textureResourcesInternalPool->entry(resourceId);
   assert( descInternal );
   
   // Evict from dedicated memory if resident
   if (desc.resident)
      evictTexture(desc, *descInternal);
   
   BufferCache* sysHeap = cpuHeap->entry(descInternal->sysHeapIndex);
   sysHeap->allocator->deallocate(descInternal->sysOffset, desc.size);
   
   availableSystemMemory += desc.size;
   
   // Release resource descriptor
   textureResourcesPool->deallocate(desc);
   textureResourcesInternalPool->deallocate(*descInternal);
   }

   // Returns pointer to system memory backing this texture resource specific surface
   void* Streamer::systemMemory(const TextureAllocation& desc,
                                const uint8 mipmap,
                                const uint16 layer,  // Layer of Array resource
                                const uint8 plane)   // Plane of multi-plane surface
   {
   assert( mipmap < desc.state.mipmaps );
   assert( layer < desc.state.layers );
   assert( plane < desc.state.planes() );
   
   uint32 resourceId = 0;
   if (!textureResourcesPool->index(desc, resourceId))
      return nullptr;
   
   TextureAllocationInternal* descInternal = textureResourcesInternalPool->entry(resourceId);
   assert( descInternal );
   
   MipMemoryLayout* layout = &descInternal->mipLayout[mipmap * desc.state.planes() + plane];
   
   // Streamer may use different layouts in the future. Thus direct pointer to
   // system memory backing this texture cannot be returned.
   uint64 offset = layout->offset + layer * roundUp(layout->size, layout->alignment.surfaceAlignment());
   return reinterpret_cast<void*>((uint8*)(descInternal->pointer) + offset);
   }
   

   
   
   
   
   
   
   
   // Each texture can be described as set of mipmap levels, each having N surfaces.
   // Where by surface, it is assumed 2D plane (or few planes that compose together
   // logical pixel format), that can have both width and height in range of 1 to
   // implementation dependent maximum.
   //
   // Count of surfaces per mipmap level is calculated as follows:
   // Texture1D                 - 1
   // Texture1DArray            - layers
   // Texture2D                 - 1
   // Texture2DArray            - layers
   // Texture2DMultisample      - 1
   // Texture2DMultisampleArray - layers
   // TextureCubeMap            - 6
   // TextureCubeMapArray       - 6 * layers
   // Texture3D                 - depth of mipmap level
   //
   // Texture components are stored in such order in memory:
   // - All surfaces of given mipmap level, in order from first (biggest one)
   //   mipmap 0, to last (smallest available) mipmap. Texture may be missing
   //   it's mip tail.
   // - In given mipmap, all surfaces of that mipmap level, from surface 0, to
   //   surfaces count. If texture format is multi-plane, ten first all surfaces
   //   Primary plane is stored, followed by Secondary plane of all those surfaces.
   // - Each stored surface starting offset is aligned to 64 bytes address boundary
   //   for surfaces of mip tail stored in Linear layout (or other GPU provided
   //   address padding). If surface is expected to be stored in Linear layout,
   //   GPU provided address padding can be bigger (in most cases up to 4MB).
   //   Linearly stored surfaces may also have GPU provided line/row padding.
   //   All remaining surfaces are stored as groups of 64KB tiles, and thus each
   //   tile and surface adress is aligned at least to 64KB.
   //
   // Surfaces count can vary per mipmap level (for e.g. Texture3D case).
   //
   // Each surface texels are stored:
   // - for uncompressed pixel formats, and packed DS_24_8 - line by line, texel after texel
   // - for compressed pixel formats - row by row, compressed block after compressed block in each row
   // - for multi-plane DS_32_f_8 - plane after plane, then line by line and texel after texel in each plane
   //
   // Each surface data is stored in consecutive linear order without any padding
   // between samples, texels, lines, compressed blocks and their rows. If surface
   // is split to two separate planes like in case of multi-plane DS_32_f_8, those
   // planes are stored at starting offset aligned to 256 bytes address boundary
   // the same way as surfaces themselves.

   // Texture layout in RAM:
   // - smallest mips to biggest ones
   // - surfaces one after another (array layers, cube faces, cube faces/cubes in array, depth planes, depth volumes)
   // - surfaces padded to alignment, or tile size
   // - if surface is equal in size or bigger than tile its tiled (equal bigger in dimensions?)
   // - what about shadow maps and render targets? (those can be only read, read is always in Linear layout)
   
   
   



   
   
   // cpu pointer is not exposed directly, as there will be different memory layouts
   // based on fact if texture is allocated as whole, or sparse.
   
//   // Returns pointer to system memory backing this texture resource
//   volatile void* Streamer::systemMemory(const TextureAllocation& desc)
//   {
//   uint32 resourceId = 0;
//   if (!textureResourcesPool->index(desc, resourceId))
//      return nullptr;
//
//   TextureAllocationInternal* descInternal = textureResourcesInternalPool->entry(resourceId);
//   assert( descInternal );
//
//   return descInternal->cpuPointer;
//   }
   
   
   // TODO: Doesn't take into account sparse textures
   // TODO: Who specifies size of tile pool for given sparse texture?
   
   // There are two types of texture layouts:
   // - linear layout for textures with complete storage
   // - tile pool layout for textures with sparse storage, and for partial updates, streaming
   //   In this layout, each surface is split into set of 2D tiles of the same
   //   exact size. Storage is performed per-tile in the same manner as previously
   //   it was performed per-surface.
   // TODO: Check how block-compressed textures are stored in memory. Are they still
   //       swizzled?
   //    a: Yes!
   //    i: "Linear vs Tiled Storage"
   // TODO: Compare upload time of whole surface at once, vs bunch of tile updates
   //       in row, for block compressed textures. If there is no difference, then
   //       textures should always be stored as pool of tiles in memory.
   
   
   // Idea:
   // Implement "InstantAccess" or "Direct Resource Access (DRA)" for texture
   // memory:
   // https://software.intel.com/en-us/blogs/2013/03/27/cpu-texture-compositing-with-direct-resource-access
   //
   // See:
   //
   // General introduction to driver/hw texture tiling and swizzling:
   // https://fgiesen.wordpress.com/2011/01/17/texture-tiling-and-swizzling/
   //
   // Intel DX11 extension:
   // https://software.intel.com/sites/default/files/4th_Generation_Core_Graphics_Developers_Guide_Final.pdf
   // (section 5.3.2 CPU view of tiled resources)
   //
   // Optimizing texture mapper:
   // http://www.flipcode.com/documents/fatmap.txt
   //


   // Memory needs to be locked to achieve maximum upload/download bandwith
   // http://www.stephenconover.com/cuda-example-bandwidth-test/
   // https://stackoverflow.com/questions/5736968/why-is-cuda-pinned-memory-so-fast
   // https://www.mkssoftware.com/docs/man3/mlock.3.asp
   //
   // #include <sys/mman.h>
   // int   mlock(const void *, size_t);
   // int   munlock(const void *, size_t);
   //
   // Windows equivalent:
   // https://msdn.microsoft.com/en-us/library/windows/desktop/aa366895(v=vs.85).aspx
   // https://msdn.microsoft.com/en-us/library/windows/desktop/ms686234(v=vs.85).aspx
   //
   // D3D12/Vulkan (WWDM 2.0) and Metal should always have all source pages locked,
   // so this shouldn't be needed (looks like CUDA is different here).
   
   
   
   // Surfaces stored in Tiled layout, are just pools of tiles.
   // Sparse textures are backed by pools of tiles too (with extra binding map indirection).
   // Allocator can thus treat non-sparse textures as sparse ones with 100% backing in memory.
   // At the same time sparse textures will just have less than < 100% backing in RAM/VRAM.
   // Those backings will be different for RAM and VRAM!
   // The only special case is mip-tail.
   
   // Surface is tiled if it's width and height is bigger than one tile.
   // Below that size, it's mip levels are stored in linear way with given padding.


   



   
   
   
   bool Streamer::makeResident(TextureAllocation& desc, const bool lock)
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
   if (!textureResourcesPool->index(desc, resourceId))
      return nullptr;
   
   // Verify if this resource is not already marked for upload
   TextureAllocationInternal* descInternal = textureResourcesInternalPool->entry(resourceId);
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

   TextureCache** cache = &gpuTextureHeapList;
   
   shared_ptr<Texture> texture = nullptr;

   // Currently it is simplest possible allocation algorithm, iterating over all
   // available heaps to find one with enough free memory. This needs to be redone
   // to proper allocation algorithm (optimal one). For e.g. heaps could be sorted
   // by remaining free space, max and min free block size could be taken into
   // notice as well.
   while(*cache)
      {
      // Try to allocate memory from current Heap
      texture = (*cache)->heap->createTexture(desc.state);
      if (texture == nullptr)
         {
         *cache = reinterpret_cast<TextureCache*>(&((*cache)->next));
         continue;
         }
   
      // Resource Streamer state of texture allocation (sub-allocated from Heap)
      desc.gpuTexture = texture;

      // Resource Streamer internal state of buffer allocation
      bool result = gpuTextureHeap->index(**cache, descInternal->gpuHeapIndex);
      assert( result );

      availableTextureMemory -= desc.size;
      break;
      }
   
   // If reached end of list, allocate new Heap
   if (!texture && !*cache)
      {
      TextureCache* gpuCache = gpuTextureHeap->allocate();
      if (gpuCache)
         {
         initTextureHeap(*gpuCache);
         *cache = gpuCache;
         gpuHeapCount[1]++;
   
         // Allocate memory from new Heap
         texture = (*cache)->heap->createTexture(desc.state);

         // This is new empty Heap so allocation should always succeed
         assert( texture );

         // Resource Streamer state of texture allocation (sub-allocated from Heap)
         desc.gpuTexture = texture;

         // Resource Streamer internal state of buffer allocation
         bool result = gpuTextureHeap->index(**cache, descInternal->gpuHeapIndex);
         assert( result );

         availableTextureMemory -= desc.size;
         }
      }
   
   // If after all couldn't allocate dedicated memory for resource, it looks like
   // we're in bad place (VRAM is fragmented?).
   if (!texture)
      return false;
      
   descInternal->locked = lock;
   return true;
   }

   bool Streamer::makeResidentSurface(const TextureAllocation& desc,
                                      const uint8 mipmap,
                                      const uint16 layer,
                                      const uint8 plane)
   {
   // TODO: Finish, consider what level of interface is really needed.
   assert( 0 );
   return false;
   }

   bool Streamer::makeResidentVolume(const TextureAllocation& desc,
                                     const uint8 mipmap)
   {
   // TODO: Finish, consider what level of interface is really needed.
   assert( 0 );
   return false;
   }

   bool Streamer::makeResidentRegion2D(const TextureAllocation& desc,
                                       const tileRegion2D region,
                                       const uint8 mipmap,
                                       const uint16 layer)
   {
   // TODO: Finish, consider what level of interface is really needed.
   assert( 0 );
   return false;
   }

   bool Streamer::makeResidentRegion3D(const TextureAllocation& desc,
                                       const tileRegion3D region,
                                       const uint8 mipmap)
   {
   // TODO: Finish, consider what level of interface is really needed.
   assert( 0 );
   return false;
   }

   void Streamer::evictTexture(TextureAllocation& desc, TextureAllocationInternal& descInternal)
   {
   // TODO: Make it thread safe
   assert( !descInternal.locked );
   
   desc.gpuTexture = nullptr;
   desc.size       = 0;
   desc.resident   = false;
   
   availableTextureMemory += desc.size;
   }

   bool Streamer::evict(TextureAllocation& desc)
   {
   uint32 resourceId = 0;
   if (!textureResourcesPool->index(desc, resourceId))
      return false;
   
   TextureAllocationInternal* descInternal = textureResourcesInternalPool->entry(resourceId);
   assert( descInternal );
   
   // Evict from dedicated memory if resident
   if (desc.resident)
      evictTexture(desc, *descInternal);

   return true;
   }

   bool Streamer::evictSurface(const TextureAllocation& desc,
                               const uint8 mipmap,
                               const uint16 layer,
                               const uint8 plane)
   {
   // TODO: Finish, consider what level of interface is really needed.
   assert( 0 );
   return false;
   }

   bool Streamer::evictVolume(const TextureAllocation& desc,
                              const uint8 mipmap)
   {
   // TODO: Finish, consider what level of interface is really needed.
   assert( 0 );
   return false;
   }

   bool Streamer::evictRegion2D(const TextureAllocation& desc,
                                const tileRegion2D region,
                                const uint8 mipmap,
                                const uint16 layer)
   {
   // TODO: Finish, consider what level of interface is really needed.
   assert( 0 );
   return false;
   }

   bool Streamer::evictRegion3D(const TextureAllocation& desc,
                                const tileRegion3D region,
                                const uint8 mipmap)
   {
   // TODO: Finish, consider what level of interface is really needed.
   assert( 0 );
   return false;
   }

   bool Streamer::transferSurface(const TextureAllocation& desc,
                                  const uint32 resourceId,
                                  const uint8 mipmap,
                                  const uint16 layer,
                                  const uint8 plane,
                                  const TransferDirection direction)
   {
   assert( desc.state.type != TextureType::Texture3D );
   assert( desc.state.mipmaps > mipmap );
   assert( desc.state.layers > layer );
   assert( desc.state.planes() > plane );

   uint16v2 tileResolution = tileResolution2D(desc.state.format, desc.state.samples, plane);
   uint32v2 mipResolution  = desc.state.mipResolution(mipmap); // plane
   uint16v2 tilesCount     = tilesCount2D(tileResolution, mipResolution);

   TransferResource transfer;
   transfer.resourceId                  = resourceId;
   transfer.direction                   = underlyingType(direction);
   transfer.type                        = underlyingType(TransferType::Surface);
   transfer.surface.mipmap              = mipmap;
   transfer.surface.region.origin.x     = 0;
   transfer.surface.region.origin.y     = 0;
   transfer.surface.region.count.width  = tilesCount.width;
   transfer.surface.region.count.height = tilesCount.height;
   transfer.surface.layer               = layer;
   transfer.surface.plane               = plane;
            
   // Push transfer of single surface on a queue
   if (!transferQueue->push(transfer))
      return false;

   return true;
   }
   
   bool Streamer::transferSurface(const TextureAllocation& desc,
                                  const uint32 resourceId,
                                  const tileRegion2D region,
                                  const uint8 mipmap,
                                  const uint16 layer,
                                  const uint8 plane,
                                  const TransferDirection direction)
   {
   assert( desc.state.type != TextureType::Texture3D );
   assert( desc.state.mipmaps > mipmap );
   assert( desc.state.layers > layer );
   assert( desc.state.planes() > plane );

   TransferResource transfer;
   transfer.resourceId                  = resourceId;
   transfer.direction                   = underlyingType(direction);
   transfer.type                        = underlyingType(TransferType::Surface);
   transfer.surface.mipmap              = mipmap;
   transfer.surface.region.origin.x     = region.origin.x;
   transfer.surface.region.origin.y     = region.origin.y;
   transfer.surface.region.count.width  = region.count.width;
   transfer.surface.region.count.height = region.count.height;
   transfer.surface.layer               = layer;
   transfer.surface.plane               = plane;
      
   // Push transfer of single surface on a queue
   if (!transferQueue->push(transfer))
      return false;

   return true;
   }
   
   bool Streamer::transferVolume(const TextureAllocation& desc,
                                 const uint32 resourceId,
                                 const uint8 mipmap,
                                 const TransferDirection direction)
   {
   assert( desc.state.type == TextureType::Texture3D );
   assert( desc.state.planes() == 1 );
   assert( desc.state.mipmaps > mipmap );

   uint16v4 tileResolution = tileResolution3D(desc.state.format);
   uint32v3 mipVolume      = desc.state.mipVolume(mipmap);
   uint16v4 tilesVolume    = tilesCount3D(tileResolution, mipVolume);
   
   // Transfer whole texture to VRAM
   TransferResource transfer;
   transfer.resourceId     = resourceId;
   transfer.direction      = underlyingType(direction);
   transfer.type           = underlyingType(TransferType::Volume);
   transfer.volume.mipmap  = mipmap;
   transfer.volume.mipmap2 = (mipmap >> 4) & 1; // Highest bit
   transfer.volume.plane   = 0;
   transfer.volume.x       = 0;
   transfer.volume.y       = 0;
   transfer.volume.z       = 0;
   transfer.volume.width   = tilesVolume.width;
   transfer.volume.height  = tilesVolume.height;
   transfer.volume.depth   = tilesVolume.depth;
      
   // Push transfer of single mipmap volume on a queue
   if (!transferQueue->push(transfer))
      return false;
      
   return true;
   }

   bool Streamer::transferVolume(const TextureAllocation& desc,
                                 const uint32 resourceId,
                                 const tileRegion3D region,
                                 const uint8 mipmap,
                                 const TransferDirection direction)
   {
   assert( desc.state.type == TextureType::Texture3D );
   assert( desc.state.planes() == 1 );
   assert( desc.state.mipmaps > mipmap );

   // Transfer whole texture to VRAM
   TransferResource transfer;
   transfer.resourceId     = resourceId;
   transfer.direction      = underlyingType(direction);
   transfer.type           = underlyingType(TransferType::Volume);
   transfer.volume.mipmap  = mipmap;
   transfer.volume.mipmap2 = (mipmap >> 4) & 1; // Highest bit
   transfer.volume.plane   = 0;
   transfer.volume.x       = region.origin.x;
   transfer.volume.y       = region.origin.y;
   transfer.volume.z       = region.origin.z;
   transfer.volume.width   = region.count.width;
   transfer.volume.height  = region.count.height;
   transfer.volume.depth   = region.count.depth;
      
   // Push transfer of single mipmap volume on a queue
   if (!transferQueue->push(transfer))
      return false;
      
   return true;
   }
   
   bool Streamer::transfer(const TextureAllocation& desc,
                           const TransferDirection direction)
   {
   // Verify descriptor
   uint32 resourceId = 0;
   if (!textureResourcesPool->index(desc, resourceId))
      return false;

   TextureAllocationInternal* descInternal = textureResourcesInternalPool->entry(resourceId);
   assert( descInternal );
 
   bool result = true;
   if (desc.state.type == TextureType::Texture3D)
      {
      // Upload from smallest mipmap to most detailed one
      for(sint32 i=(desc.state.mipmaps-1); i>=0; --i)
         {
         if (!transferVolume(desc, i, direction))
            {
            result = false;
            break;
            }
            
         descInternal->uploading++;  // TODO: Atomic
         }
      }
   else // Transfer textures composed from surfaces, one surface at a time
      {
      // Upload from smallest mipmap to most detailed one
      for(sint32 i=(desc.state.mipmaps-1); i>=0; --i)
         for(uint32 j=0; j<desc.state.layers; ++j)
            for (uint32 k=0; k<desc.state.planes(); ++k)
                {
                if (!transferSurface(desc, i, j, k, direction))
                   {
                   result = false;
                   break;
                   }
                   
                descInternal->uploading++;  // TODO: Atomic
                }
      }
   
   // If streaming thread was idle, it is sleeping, so wake it up
   if (result && streamingThread->sleeping())
      streamingThread->wakeUp();

   return result;
   }
   
   bool Streamer::transferSurface(const TextureAllocation& desc,
                                  const uint8 mipmap,
                                  const uint16 layer,
                                  const uint8 plane,
                                  const TransferDirection direction)
   {
   // Verify descriptor
   uint32 resourceId = 0;
   if (!textureResourcesPool->index(desc, resourceId))
      return false;
      
   TextureAllocationInternal* descInternal = textureResourcesInternalPool->entry(resourceId);
   assert( descInternal );
 
   if (!transferSurface(desc, resourceId, mipmap, layer, plane, direction))
      return false;
      
   descInternal->uploading++;  // TODO: Atomic
   
   // If streaming thread was idle, it is sleeping, so wake it up
   if (streamingThread->sleeping())
      streamingThread->wakeUp();

   return true;
   }

   bool Streamer::transferVolume(const TextureAllocation& desc,
                                 const uint8 mipmap,
                                 const TransferDirection direction)
   {
   // Verify descriptor
   uint32 resourceId = 0;
   if (!textureResourcesPool->index(desc, resourceId))
      return false;
      
   TextureAllocationInternal* descInternal = textureResourcesInternalPool->entry(resourceId);
   assert( descInternal );
 
   if (!transferVolume(desc, resourceId, mipmap, direction))
      return false;
      
   descInternal->uploading++;  // TODO: Atomic
   
   // If streaming thread was idle, it is sleeping, so wake it up
   if (streamingThread->sleeping())
      streamingThread->wakeUp();

   return true;
   }
   
   bool Streamer::transferRegion2D(const TextureAllocation& desc,
                                   const tileRegion2D region,
                                   const uint8 mipmap,
                                   const uint16 layer,
                                   const uint8 plane,
                                   const TransferDirection direction)
   {
   // Verify descriptor
   uint32 resourceId = 0;
   if (!textureResourcesPool->index(desc, resourceId))
      return false;
      
   TextureAllocationInternal* descInternal = textureResourcesInternalPool->entry(resourceId);
   assert( descInternal );
 
   if (!transferSurface(desc, resourceId, region, mipmap, layer, plane, direction))
      return false;
      
   descInternal->uploading++;  // TODO: Atomic
   
   // If streaming thread was idle, it is sleeping, so wake it up
   if (streamingThread->sleeping())
      streamingThread->wakeUp();

   return true;
   }

   bool Streamer::transferRegion3D(const TextureAllocation& desc,
                                   const tileRegion3D region,
                                   const uint8 mipmap,
                                   const TransferDirection direction)
   {
   // Verify descriptor
   uint32 resourceId = 0;
   if (!textureResourcesPool->index(desc, resourceId))
      return false;
      
   TextureAllocationInternal* descInternal = textureResourcesInternalPool->entry(resourceId);
   assert( descInternal );
 
   if (!transferVolume(desc, resourceId, region, mipmap, direction))
      return false;
      
   descInternal->uploading++;  // TODO: Atomic
   
   // If streaming thread was idle, it is sleeping, so wake it up
   if (streamingThread->sleeping())
      streamingThread->wakeUp();

   return true;
   }
   
   
   
   
   // Do we really need those?
   
   bool Streamer::upload(const TextureAllocation& desc)
   {
   return transfer(desc, TransferDirection::DeviceUpload);
   }
   
   bool Streamer::uploadSurface(const TextureAllocation& desc,
                                const uint8 mipmap,
                                const uint16 layer,
                                const uint8 plane)
   {
   return transferSurface(desc, mipmap, layer, plane, TransferDirection::DeviceUpload);
   }

   bool Streamer::uploadVolume(const TextureAllocation& desc,
                               const uint8 mipmap)
   {
   return transferVolume(desc, mipmap, TransferDirection::DeviceUpload);
   }

   bool Streamer::uploadRegion2D(const TextureAllocation& desc,
                                 const tileRegion2D region,
                                 const uint8 mipmap,
                                 const uint16 layer,
                                 const uint8 plane)
   {
   return transferRegion2D(desc, region, mipmap, layer, plane, TransferDirection::DeviceUpload);
   }

   bool Streamer::uploadRegion3D(const TextureAllocation& desc,
                                 const tileRegion3D region,
                                 const uint8 mipmap)
   {
   return transferRegion3D(desc, region, mipmap, TransferDirection::DeviceUpload);
   }

   bool Streamer::download(const TextureAllocation& desc)
   {
   return transfer(desc, TransferDirection::DeviceDownload);
   }
   
   bool Streamer::downloadSurface(const TextureAllocation& desc,
                                  const uint8 mipmap,
                                  const uint16 layer,
                                  const uint8 plane)
   {
   return transferSurface(desc, mipmap, layer, plane, TransferDirection::DeviceDownload);
   }
   
   bool Streamer::downloadVolume(const TextureAllocation& desc,
                                 const uint8 mipmap)
   {
   return transferVolume(desc, mipmap, TransferDirection::DeviceDownload);
   }
   
   bool Streamer::downloadRegion2D(const TextureAllocation& desc,
                                   const tileRegion2D region,
                                   const uint8 mipmap,
                                   const uint16 layer,
                                   const uint8 plane)
   {
   return transferRegion2D(desc, region, mipmap, layer, plane, TransferDirection::DeviceDownload);
   }

   bool Streamer::downloadRegion3D(const TextureAllocation& desc,
                                   const tileRegion3D region,
                                   const uint8 mipmap)
   {
   return transferRegion3D(desc, region, mipmap, TransferDirection::DeviceDownload);
   }
}




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
//     - Layer (Face / Depth)





// Texture LOD's streaming:
// - Objects are Data Structures
// - Material is set of Texture references
// - Material textures can be streamed in async way
//   - Material async load:
//     - give streamer array of resource ID's to load, and they destination in data structure
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
