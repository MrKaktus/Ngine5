/*

 Ngine v5.0
 
 Module      : Metal Buffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlBuffer.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/mtlDevice.h"

namespace en
{
   namespace gpu
   {
   BufferMTL::BufferMTL(Ptr<HeapMTL> _heap,
                        id<MTLBuffer> _handle,
                        const BufferType type,
                        const uint32 size,
                        const uint32 _offset) :
      handle(_handle),
      heap(_heap),
      offset(_offset),
      CommonBuffer(type, size)
   {
   }


   // UMA (iOS):
   // Private - system memory ->
   //           (!) explicit CommandBuffer data copy from "Transfer" resource to destination
   //               would mean unnecessary copy of data
   //           BUT data will be probably converted / compressed anyway to match the GPU ?
   // Shared  - system memory -> "contents" to directly modify data (ok for buffers, not for textures)
   //
   // NUMA (macOS):
   // Private - GPU optimal, requires copy with possible internal conversion ->
   //           explicit CommandBuffer data copy from "Transfer" resource to destination
   // Shared  - system memory -> "contents" to directly modify data (ok for buffers, not for textures)
   // Managed - two copies ->
   //            map: calls "contents" to allow modification CPU copy data
   //          unmap: calls "didModifyRange" but have no information about when transfer to GPU copy is done?
   //
   // map / unmap
   //
   
   // GPU resources moslty should have their RAM copy anyway as intermediate state between VRAM and HDD.
   //
   // Memory Heaps can be allocated with different backing memory:
   // MemoryUsage
   //
   // Static   = 0,   // Resources will be allocated once and used only by GPU (Static textures, Render Targets)
   // Streamed    ,   // Resources will be updated every few frames (Geometry Cache's, Sparse Textures)
   // Immediate   ,   // Data that ideally should be updated in the same frame (Uniform's, Push Constants)
   // Temporary       // Special memory type for Intermediate Render Targets.
      
   // Metal Heaps -> Private by default, can be Shared
   
   // Metal Transfer resources need to be Shared -> CANNOT BE SUBALLOCATED FROM Private Heap !!
   
   // Vulkan -> depending which type of Memory will be choosen by the engine as backing memory for given
   //           Heap type, it may, or may not support "Transfer" buffers. Thus the same engine API calls
   //           can sometimes succeedd and sometimes fail on Transfer buffer creation on given heap.
   //
   // Transfer buffers creation should be allowed only OUTSIDE of Heaps, and even outside of GPU,
   // but in scope of Graphics API. As they are GPU independent and reside in system memory.
   //
   // BUT THEN:
   //
   // If we will create "Immediate" heap, does app still need to "populate" it ?
   // Cannot we write directly to the buffers from such Heap?
   
   
   // if we would:
   // MAP -> create transfer buffer if needed, otherwise directly map destination resource from shared heap
   // updated
   // UNMAP -> blit transfer resource to destination on private, do nothing on backed by shared heap
   //          <- this blit operation should be under app control!!
   // unmap completion handler ?!
   // <- this assumes API provides memory to populate
   
   // API provides memory pointer:
   //  - may save extra mem copy if provided pointer is direct pointer to shared memory backing heap
   //    Metal: Shared Heaps, Vulkan: CPU mapped Heaps 
   // APP provides memory pointer:
   //  - may save extra mem copy if provided pointer is to memory mapped file for compressed resource
   //    <- this one is more likely (file format must be mmap friendly, data page size aligned!)
   
   // load from HDD to system temp -> skip system temp by MMap'ing file
   // copy system temp to RAM
   // * do any op on buffer    -> destination:
   // copy RAM to gpu resource
   //   - copy and recompress to VRAM
   //   - just copy to RAM
   
   


   // Vulkan:
   // - special Heap for staging (prefer system memory storage without VRAM copy)
   // - preffered one at a time, but multiple staging buffers can coegsist at the same time
   // - always mapped
   // - Heap::map(Upload, 16MB);  <- creates temporary upload buffer on the heap and maps it
   //                                WRONG: Cannot use it later for blit's !


      
   //   Upload to GPU (fastest possible CPU write, delayed sync to GPU):
   //
   //   SystemWriteThrough
   //   DedicatedSharedCache
   //   SystemCached         requires flushing/Invalidaiton
   //   System
   //   DedicatedWithCopy    requires flushing/Invalidaiton
   //
   //   Download from GPU (fastest possible GPU write):

   //   DedicatedSharedCache
   //   SystemWriteThrough
   //   SystemCached          requires flushing/Invalidaiton
   //   System
   //   DedicatedWithCopy     requires flushing/Invalidaiton


   // UMA (iOS):
   // Shared - direct write to buffers
   // Private - allow textures tiling, etc. -> use staging buffers
   
   // We always want textures to be in Private storage to enable GPU specific optimizations of resources (UMA & NUMA).
   // So "Staging Buffers" are a must for textures always (except of compressed textures on UMA).
   // "Staging" is done on the application level (above Frontend).
   // We don't need "staging" for buffers on UMA.
   // We don't need "staging" for "streaming" buffers on NUMA.
   
//   VkMappedMemoryRange mappedRange;
//   mappedRange.sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
//   mappedRange.pNext  = nullptr;
//   mappedRange.memory = heap->handle;
//   mappedRange.offset = offset + mappedOffset;
//   mappedRange.size   = mappedSize;
//
//   // Sync CPU writes -> visible to GPU VRAM
//   VkResult vkFlushMappedMemoryRanges(
//    VkDevice                                    device,
//    uint32_t                                    memoryRangeCount,
//    const VkMappedMemoryRange*                  pMemoryRanges);
//   
//   // Sync GPU VRAM writes -> visible to CPU RAM copy
//   VkResult vkInvalidateMappedMemoryRanges(
//    VkDevice                                    device,
//    uint32_t                                    memoryRangeCount,
//    const VkMappedMemoryRange*                  pMemoryRanges);
   

   
   

   
//            // Copy loaded texture to temporary staging buffer
//            Ptr<gpu::Buffer> transfer = en::ResourcesContext.defaults.enHeap->createBuffer(gpu::BufferType::Transfer,
//                                                                          static_cast<uint32>(surfaceSize));
//            
//            void* ptr = transfer->content(); // like map (texture, mipmap, slice)
//      
//            file->read(offset, static_cast<uint32>(surfaceSize), ptr);
//      
//            // like unmap
//            // Copy data from staging buffer to final texture
//            Ptr<gpu::CommandBuffer> command = Graphics->primaryDevice()->createCommandBuffer();
//            command->populate(transfer, texture, mipmap, slice); // blit
//            command->commit();
//      
//            // here return completion handler callback !!!
//      
//            // dont wait:
//            // command->waitUntilCompleted();
//            command = nullptr;
//      
      
            

   BufferMTL::~BufferMTL()
   {
   handle = nil;
   
   if (offset)
      heap->allocator->deallocate(offset, size);
      
   heap = nullptr;
   }

   void* BufferMTL::map(void)
   {
   return map(0u, size);
   }
   
   void* BufferMTL::map(const uint64 _offset, const uint64 _size)
   {
   assert( _offset + _size <= size );
   
   // Buffers can be mapped only on Upload, Download and Immediate Heaps.
   assert( heap->_usage == MemoryUsage::Upload   ||
           heap->_usage == MemoryUsage::Download ||
           heap->_usage == MemoryUsage::Immediate );
      
   // Just return pointer to buffer data
   return (void*)((uint64)[handle contents] + (uint64)offset + (uint64)_offset);
   }

   void BufferMTL::unmap(void)
   {
   // Don't need to do nothing.
   }

   BufferViewMTL::BufferViewMTL()
   {
   }
   
   BufferViewMTL::~BufferViewMTL()
   {
   }
   
   }
}
#endif
