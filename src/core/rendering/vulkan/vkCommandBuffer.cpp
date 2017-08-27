/*

 Ngine v5.0
 
 Module      : Vulkan Command Buffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/vulkan/vkCommandBuffer.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/log/log.h"
#include "core/rendering/vulkan/vkDevice.h"
#include "core/rendering/vulkan/vkBuffer.h"
#include "core/rendering/vulkan/vkTexture.h"
#include "core/rendering/vulkan/vkRenderPass.h"

namespace en
{
   namespace gpu
   {

   CommandBufferVK::CommandBufferVK(VulkanDevice* _gpu, 
         const VkQueue _queue, 
         const QueueType type, 
         const VkCommandBuffer _handle, 
         const VkFence _fence) :
      gpu(_gpu),
      queue(_queue),
      queueType(type),
      handle(_handle),
      semaphore(nullptr),
      fence(_fence),
      started(false),
      encoding(false),
      commited(false),
      currentIndexBuffer(nullptr),
      CommandBuffer()
   {
   }
   
   CommandBufferVK::~CommandBufferVK()
   {
   // Finish encoded tasks
   if (!commited)
      commit();
      
   // Don't wait for completion
   
   // Add yourself's fence and CB handle to Garbage Collector
   
   //gpu->add
   
   
   // TODO: Release buffer
   
   // We need to wait until Command Buffer is finished, before we can release the Fence.
   // Therefore this Command Buffer need to be added to Device's Garbage Collector,
   // which will remove it automatically, when completion is signaled through Fence.

   // Release fence
   ProfileNoRet( gpu, vkDestroyFence(gpu->device, fence, nullptr) )
   
   // Release Command Buffer
   uint32 thread = Scheduler.core();
   ProfileNoRet( gpu, vkFreeCommandBuffers(gpu->device, gpu->commandPool[thread][underlyingType(queueType)], 1, &handle) )
   }




/* COMMAND QUEUES */



   // Each Thread:
   // - acquire handles to all queues from all families
   // - create CP for each family
   // - create CB's from that CP's based on Family type
   // - begin CB's and encode operations to them
   // - end CB's and commit them to CP
   
   // Expose above Metal CB's:
   // - secondary CB's as optional feature
   // - CB's reuse as optional feature
   // - specialized command buffers (Metal get only Universal)
   


   // Command Buffer type depends from Queue Family type.

   // Application can query how many Queues of given type it has.
   // It can then create on each thread CB's from that queues.
   // CB's are automatically commited to their parent queues when done.




   // RENDER PASS
   //////////////////////////////////////////////////////////////////////////
   

   void CommandBufferVK::start(const Ptr<Semaphore> waitForSemaphore)
   {
   assert( !started );

   if (waitForSemaphore)
      semaphore = ptr_reinterpret_cast<SemaphoreVK>(&waitForSemaphore);

   // In Metal API we need to create Render Command Encoder.
   // In Vulkan API CommandBuffer needs to be started first,
   // before encoding content to it. It only needs to be done
   // once, and multiple RenderPasses can be encoded afterwards.
   VkCommandBufferBeginInfo info;
   info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
   info.pNext            = nullptr;
   info.flags            = 0;
   info.pInheritanceInfo = nullptr; // We don't support secondary Command Buffers for now

   Profile( gpu, vkBeginCommandBuffer(handle, &info) )
   
   started = true;
   }
   
   void CommandBufferVK::startRenderPass(const Ptr<RenderPass> pass, const Ptr<Framebuffer> _framebuffer)
   {
   assert( started );
   assert( !encoding );

   assert( pass );
   assert( _framebuffer );

   RenderPassVK*  renderPass  = raw_reinterpret_cast<RenderPassVK>(&pass);
   FramebufferVK* framebuffer = raw_reinterpret_cast<FramebufferVK>(&_framebuffer);

   // Begin encoding commands for this Render Pass
   VkRenderPassBeginInfo beginInfo;
   beginInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
   beginInfo.pNext           = nullptr;
   beginInfo.renderPass      = renderPass->handle;
   beginInfo.framebuffer     = framebuffer->handle;
   beginInfo.renderArea      = { 0, 0, framebuffer->resolution.width, framebuffer->resolution.height };
   beginInfo.clearValueCount = renderPass->surfaces;
   beginInfo.pClearValues    = renderPass->clearValues;

   ProfileNoRet( gpu, vkCmdBeginRenderPass(handle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE) )
   
   encoding = true;
   }
   
   void CommandBufferVK::endRenderPass(void)
   {
   assert( started );
   assert( encoding );

   // End encoding commands for this Render Pass
   ProfileNoRet( gpu, vkCmdEndRenderPass(handle) )
    
   encoding = false;
   }
   

   // SETTING INPUT ASSEMBLER VERTEX BUFFERS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferVK::setVertexBuffers(const uint32 count, const uint32 firstSlot, const Ptr<Buffer>* buffers, const uint64* offsets) const
   {
   assert( started );
   assert( count );
   assert( buffers );
   assert( (firstSlot + count) <= gpu->support.maxInputLayoutBuffersCount );

   // Extract Vulkan buffer handles
   VkBuffer* handles = new VkBuffer[count];   // TODO: Optimize by allocating on the stack maxBuffersCount sized fixed array.
   for(uint32 i=0; i<count; ++i)
      {
      assert( buffers[i] );
      handles[i] = ptr_dynamic_cast<BufferVK, Buffer>(buffers[i])->handle;
      }

   // Generate default zero offsets array if none is passed
   uint64* finalOffsets = (uint64*)(offsets);
   if (!offsets)
      {
	  finalOffsets = new uint64[count];
	  memset(finalOffsets, 0, sizeof(uint64)*count);
	  }

   ProfileNoRet( gpu, vkCmdBindVertexBuffers(handle, 
                                             firstSlot, 
                                             count, 
                                             handles, 
                                             static_cast<const VkDeviceSize*>(finalOffsets)) )
   delete [] handles;
   if (!offsets)
      delete [] finalOffsets;
   }

   void CommandBufferVK::setVertexBuffer(const uint32 slot, const Ptr<Buffer> buffer, const uint64 offset) const
   {
   assert( started );
   assert( buffer );
   assert( slot < gpu->support.maxInputLayoutBuffersCount );

   ProfileNoRet( gpu, vkCmdBindVertexBuffers(handle, 
                                             slot, 1,
                                             &raw_reinterpret_cast<BufferVK>(&buffer)->handle, 
                                             static_cast<const VkDeviceSize*>(&offset)) )
   }


   // TRANSFER COMMANDS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferVK::copy(Ptr<Buffer> source, Ptr<Buffer> destination)
   {
   assert( started );
   assert( source );
   assert( destination );

   copy(source, destination, source->length());
   }
   
   void CommandBufferVK::copy(Ptr<Buffer> source,
      Ptr<Buffer> destination,
      uint64 size,
      uint64 srcOffset,
      uint64 dstOffset)
   {
   assert( started );
   assert( source );
   assert( destination );

   BufferVK* src = raw_reinterpret_cast<BufferVK>(&source);
   BufferVK* dst = raw_reinterpret_cast<BufferVK>(&destination);

   VkBufferCopy region;
   region.srcOffset = srcOffset;
   region.dstOffset = dstOffset;
   region.size      = size;

   ProfileNoRet( gpu, vkCmdCopyBuffer(handle,
                                      src->handle,
                                      dst->handle,
                                      1u,
                                      &region) )
   }

   void CommandBufferVK::copy(Ptr<Buffer> transfer, const uint64 srcOffset, Ptr<Texture> texture, const uint32 mipmap, const uint32 layer)
   {
   assert( started );
   assert( transfer );
   assert( transfer->type() == BufferType::Transfer );
   assert( texture );
   assert( texture->mipmaps() > mipmap );
   assert( texture->layers() > layer );
   
   BufferVK*  source      = raw_reinterpret_cast<BufferVK>(&transfer);
   TextureVK* destination = raw_reinterpret_cast<TextureVK>(&texture);

   assert( source->size >= srcOffset + destination->size(mipmap) );
   
   VkImageSubresourceLayers layersInfo;
   layersInfo.aspectMask     = TranslateImageAspect(destination->state.format);
   layersInfo.mipLevel       = mipmap;
   layersInfo.baseArrayLayer = layer;
   layersInfo.layerCount     = 1u;
   
   assert( source->size < 0xFFFFFFFF );

   VkBufferImageCopy regionInfo;
   regionInfo.bufferOffset      = srcOffset;
   regionInfo.bufferRowLength   = 0u;
   regionInfo.bufferImageHeight = 0u;
   regionInfo.imageSubresource  = layersInfo;
   regionInfo.imageOffset       = { 0u, 0u, 0u };
   regionInfo.imageExtent       = { destination->width(mipmap), destination->height(mipmap), 1 };
   
   ProfileNoRet( gpu, vkCmdCopyBufferToImage(handle,
                                             source->handle,
                                             destination->handle,
                                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                             1, &regionInfo) )

   }

   void CommandBufferVK::copy(Ptr<Buffer> transfer, Ptr<Texture> texture, const uint32 mipmap, const uint32 layer)
   {
   copy(transfer, 0u, texture, mipmap, layer);
   }
      
//   // CPU data -> Buffer (max 64KB, recorded on CommandBuffer itself, for UBO's ?)
//   void vkCmdUpdateBuffer(
//    VkCommandBuffer                             commandBuffer,
//    VkBuffer                                    dstBuffer,
//    VkDeviceSize                                dstOffset,
//    VkDeviceSize                                dataSize,
//    const void*                                 pData);
//   
//   // Fill buffer with 32bit pattern (like clearing)
//   void vkCmdFillBuffer(
//    VkCommandBuffer                             commandBuffer,
//    VkBuffer                                    dstBuffer,
//    VkDeviceSize                                dstOffset,
//    VkDeviceSize                                size,
//    uint32_t                                    data);
//    
//   // Buffer -> Buffer
//   void vkCmdCopyBuffer(
//    VkCommandBuffer                             commandBuffer,
//    VkBuffer                                    srcBuffer,
//    VkBuffer                                    dstBuffer,
//    uint32_t                                    regionCount,
//    const VkBufferCopy*                         pRegions);
//    
//   // Buffer -> Image
//   void vkCmdCopyBufferToImage(
//    VkCommandBuffer                             commandBuffer,
//    VkBuffer                                    srcBuffer,
//    VkImage                                     dstImage,
//    VkImageLayout                               dstImageLayout,
//    uint32_t                                    regionCount,
//    const VkBufferImageCopy*                    pRegions);
//    
//   // Image -> Buffer
//   void vkCmdCopyImageToBuffer(
//    VkCommandBuffer                             commandBuffer,
//    VkImage                                     srcImage,
//    VkImageLayout                               srcImageLayout,
//    VkBuffer                                    dstBuffer,
//    uint32_t                                    regionCount,
//    const VkBufferImageCopy*                    pRegions);
//    
//   // Image -> Image (no filtering)
//   void vkCmdCopyImage(
//    VkCommandBuffer                             commandBuffer,
//    VkImage                                     srcImage,
//    VkImageLayout                               srcImageLayout,
//    VkImage                                     dstImage,
//    VkImageLayout                               dstImageLayout,
//    uint32_t                                    regionCount,
//    const VkImageCopy*                          pRegions);
//    
//   // Texture -> Texture
//   void vkCmdBlitImage(
//    VkCommandBuffer                             commandBuffer,
//    VkImage                                     srcImage,
//    VkImageLayout                               srcImageLayout,
//    VkImage                                     dstImage,
//    VkImageLayout                               dstImageLayout,
//    uint32_t                                    regionCount,
//    const VkImageBlit*                          pRegions,
//    VkFilter                                    filter);
   

   // PIPELINE COMMANDS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferVK::setPipeline(const Ptr<Pipeline> pipeline)
   {
   assert( started );
   assert( pipeline );

   PipelineVK* ptr = raw_reinterpret_cast<PipelineVK>(&pipeline);

   VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
   if (!ptr->graphics)
      bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

   ProfileNoRet( gpu, vkCmdBindPipeline(handle, bindPoint, ptr->handle) )
   }


   // DRAW COMMANDS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferVK::draw(const uint32       elements,
                              const Ptr<Buffer>  indexBuffer,
                              const uint32       instances,
                              const uint32       firstElement,
                              const sint32       firstVertex,
                              const uint32       firstInstance)
   {
   assert( started );
   assert( encoding );
   assert( elements );

   // Elements are assembled into Primitives.
   if (indexBuffer)
      {
      BufferVK* index = raw_reinterpret_cast<BufferVK>(&indexBuffer);
      
      assert( index->apiType == BufferType::Index );
      
      // Prevent binding the same IBO several times in row
      if (index != currentIndexBuffer)
         {
         VkIndexType indexType = VK_INDEX_TYPE_UINT16;
         if (index->formatting.column[0] == Attribute::u32)
            indexType = VK_INDEX_TYPE_UINT32;
         
         // Index Buffer remains bound to Command Buffer after this call,
         // but because there is no other way to do indexed draw than to
         // go through this API, it's safe to leave it bounded.
         ProfileNoRet( gpu, vkCmdBindIndexBuffer(handle,
                                                 index->handle,
                                                 0,             // Offset In Index Buffer is calculated at draw call.
                                                 indexType) )

         currentIndexBuffer = index;
         }

      ProfileNoRet( gpu, vkCmdDrawIndexed(handle,
                                          elements,
                                          max(1U, instances),
                                          firstElement,    // Index of first index to start (multiplied by elementSize will give starting offset in IBO). There can be several buffers with separate indexes groups in one GPU Buffer.
                                          firstVertex,     // VertexID of first processed vertex
                                          firstInstance) ) // InstanceID of first processed instance
      }
   else
      {
      ProfileNoRet( gpu, vkCmdDraw(handle,
                                   elements,
                                   max(1U, instances),
                                   firstElement,       // Index of first vertex to draw (offset in buffer, VertexID == 0)
                                   firstInstance) )
      }
   }

   void CommandBufferVK::draw(const Ptr<Buffer>  indirectBuffer,
                              const uint32       firstEntry,
                              const Ptr<Buffer>  indexBuffer,
                              const uint32       firstElement)
   {
   assert( started );
   assert( encoding );
   assert( indirectBuffer );
   
   BufferVK* indirect = raw_reinterpret_cast<BufferVK>(&indirectBuffer);
   assert( indirect->apiType == BufferType::Indirect );
   
   if (indexBuffer)
      {
      BufferVK* index = raw_reinterpret_cast<BufferVK>(&indexBuffer);
      assert( index->apiType == BufferType::Index );
      
      uint32 elementSize = 2;
      VkIndexType indexType = VK_INDEX_TYPE_UINT16;
      if (index->formatting.column[0] == Attribute::u32)
         {
         elementSize = 4;
         indexType = VK_INDEX_TYPE_UINT32;
         }

      // Index Buffer remains bound to Command Buffer after this call,
      // but because there is no other way to do indexed draw than to
      // go through this API, it's safe to leave it bounded.
      ProfileNoRet( gpu, vkCmdBindIndexBuffer(handle,
                                              index->handle,
                                              (firstElement * elementSize), // Offset In Index Buffer, so that we can have several buffers with separate indeges groups in one GPU Buffer
                                              indexType) )

      // IndirectIndexedDrawArgument can be directly cast to VkDrawIndexedIndirectCommand.

      // TODO: Currently draw count is equal to amount of entries from first entry to the end of the indirect buffer.
      ProfileNoRet( gpu, vkCmdDrawIndexedIndirect(handle,
                                                  indirect->handle,
                                                  (firstEntry * sizeof(VkDrawIndexedIndirectCommand)),
                                                  (indirect->size / sizeof(VkDrawIndexedIndirectCommand)) - firstEntry,
                                                  sizeof(VkDrawIndexedIndirectCommand)) )
      }
   else
      {
      // IndirectDrawArgument can be directly cast to VkDrawIndirectCommand.
      
      // TODO: Currently draw count is equal to amount of entries from first entry to the end of the indirect buffer.
      ProfileNoRet( gpu, vkCmdDrawIndirect(handle,
                                           indirect->handle,
                                           (firstEntry * sizeof(VkDrawIndirectCommand)),
                                           (indirect->size / sizeof(VkDrawIndirectCommand)) - firstEntry,
                                           sizeof(VkDrawIndirectCommand)) )
      }
   }
   

   // FINISHING
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferVK::commit(const Ptr<Semaphore> signalSemaphore)
   {
   assert( started );
   assert( !encoding );
   assert( !commited );
   
   // Finish Command Buffer encoding.
   Profile( gpu, vkEndCommandBuffer(handle) )
   
   VkSubmitInfo submitInfo;
   submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
   submitInfo.pNext                = nullptr;
   submitInfo.waitSemaphoreCount   = 0u;      
   submitInfo.pWaitSemaphores      = nullptr;   // Which semaphores we wait for. One Semaphore for one Pipeline Stage of other queue executing CommandBuffer.
   submitInfo.pWaitDstStageMask    = nullptr;   // Pipeline stages at which each corresponding semaphore wait will occur (until pointed stage is done).
   submitInfo.commandBufferCount   = 1u;
   submitInfo.pCommandBuffers      = &handle;
   submitInfo.signalSemaphoreCount = 0u;        // Amount of Semaphores we want to signal when this batch of job is done (more than one so we can unblock several queues at the same time).
   submitInfo.pSignalSemaphores    = nullptr;   

   // Example:
   //
   // Execute everything except of final drawing to render targets, until previous frame is presented.
   // (assumes direct write to framebuffer, reuse of the same surface).
   // waitEvents   = 1;
   // waitFlags[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

   // Common case:
   //
   // Wait for previous job to finish before starting this one.
   // waitEvents   = 1;
   // waitFlags[0] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

   // Wait for completion of previous Command Buffer synced with starting semaphore.
   VkPipelineStageFlags waitFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
   if (semaphore)
      {
      submitInfo.waitSemaphoreCount = 1u;
      submitInfo.pWaitSemaphores    = &semaphore->handle;
      submitInfo.pWaitDstStageMask  = &waitFlags;
      }

   // Signal semaphore for future synchronization.
   if (signalSemaphore)
      {
      SemaphoreVK* signal = raw_reinterpret_cast<SemaphoreVK>(&signalSemaphore);

      submitInfo.signalSemaphoreCount = 1u;   
      submitInfo.pSignalSemaphores    = &signal->handle;        
      }

   // Submit single batch of work to the queue.
   // Each batch can consist of multiple command buffers.
   // Internal fence will be signaled when this work is done.
   Profile( gpu, vkQueueSubmit(queue, 1, &submitInfo, fence) ) 

   // Try to clear any CommandBuffers that are no longer executing.
   gpu->clearCommandBuffersQueue();

   // Add this CommandBuffer to device's array of CB's in flight.
   // This will ensure that CommandBuffer won't be destroyed until
   // fence is not signaled.
   gpu->addCommandBufferToQueue(Ptr<CommandBuffer>(this));

   commited = true;
   }

   bool CommandBufferVK::isCompleted(void)
   {
   // Unrolled "Profile" macro, to prevent outputting of false Warning messages.
   uint32 thread = Scheduler.core();
#ifdef EN_DEBUG
   #ifdef EN_PROFILER_TRACE_GRAPHICS_API
   Log << "[" << setw(2) << thread << "] ";
   Log << "Vulkan GPU " << setbase(16) << gpu << ": vkWaitForFences(gpu->device, 1, &fence, VK_TRUE, 0u)" << endl;
   gpu->lastResult[thread] = gpu->vkWaitForFences(gpu->device, 1, &fence, VK_TRUE, 0u);
   if (en::gpu::IsError(gpu->lastResult[thread]))
      assert( 0 );
   #else 
   gpu->lastResult[thread] = gpu->vkWaitForFences(gpu->device, 1, &fence, VK_TRUE, 0u);
   if (en::gpu::IsError(gpu->lastResult[thread]))
      assert( 0 );
   #endif
#else 
   gpu->lastResult[thread] = gpu->vkWaitForFences(gpu->device, 1, &fence, VK_TRUE, 0u);
#endif
   return gpu->lastResult[thread] == VK_SUCCESS ? true : false;
   }
    
   void CommandBufferVK::waitUntilCompleted(void)
   {
   // Wait maximum 1 second, then assume GPU hang.
   uint64 gpuWatchDog = 1000000000; // TODO: This should be configurable global
   
   Profile( gpu, vkWaitForFences(gpu->device, 1, &fence, VK_TRUE, gpuWatchDog) )
   if (gpu->lastResult[Scheduler.core()] == VK_TIMEOUT)
      {
      Log << "GPU Hang! Engine file: " << __FILE__ << " line: " << __LINE__ << endl;   // TODO: File / line doesn't make sense as it will always point this method!
      }



   // TODO:

   // v1.0.36 p97
   //
   // " Note
   //   Signaling a fence and waiting on the host does not guarantee that the results of memory accesses will be visible
   //   to the host. To provide that guarantee, the application must insert a memory barrier between the device writes
   //   and the end of the submission that will signal the fence, with dstAccessMask having the VK_ACCESS_HOST_
   //   READ_BIT bit set, with dstStageMask having the VK_PIPELINE_STAGE_HOST_BIT bit set, and with the
   //   appropriate srcStageMask and srcAccessMask members set to guarantee completion of the writes. If the
   //   memory was allocated without the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set, then vkInval
   //   idateMappedMemoryRanges must be called after the fence is signaled in order to ensure the writes are
   //   visible to the host, as described in Host Access to Device Memory Objects. "
   //

   // v1.0.36 p219
   // 
   // " vkInvalidateMappedMemoryRanges must be used to guarantee that device writes to non-coherent memory are
   //   visible to the host. It must be called after command buffers that execute and flush (via memory barriers) the device writes
   //   have completed, and before the host will read or write any of those locations. If a range of non-coherent memory is
   //   written by the host and then invalidated without first being flushed, its contents are undefined.
   //   
   //   Note
   //   Mapping non-coherent memory does not implicitly invalidate the mapped memory, and device writes that have
   //   not been invalidated must be made visible before the host reads or overwrites them. "
   // 
   }
   
   Ptr<CommandBuffer> VulkanDevice::createCommandBuffer(const QueueType type, const uint32 parentQueue)
   {
   assert( queuesCount[underlyingType(type)] > parentQueue );
   
   // CommandBuffers can be recycled / reused (in Metal Buffers and Encoders are single time use)
   // Multiple buffers can be created simultaneously for one queue
   // Buffers are executed in order in queue


   // Each thread creates it's Command Buffers from separate Command Pool
   uint32 thread = Scheduler.core();

   VkCommandBuffer handle;
 
   VkCommandBufferAllocateInfo commandInfo;
   commandInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
   commandInfo.pNext              = nullptr;
   commandInfo.commandPool        = commandPool[thread][underlyingType(type)];
   commandInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Secondary CB's need VK_COMMAND_BUFFER_LEVEL_SECONDARY
   commandInfo.commandBufferCount = 1; // Can create multiple CB's at once
   
   Profile( this, vkAllocateCommandBuffers(device, &commandInfo, &handle) )

   // Create Fence that will be signaled when the Command Buffer execution is finished.
   VkFence fence = VK_NULL_HANDLE;
   
   VkFenceCreateInfo fenceInfo;
   fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
   fenceInfo.pNext = nullptr;
   fenceInfo.flags = 0; // VK_FENCE_CREATE_SIGNALED_BIT if want to create it in signaled state from start
   
   Profile( this, vkCreateFence(device, &fenceInfo, nullptr, &fence) )

   // Acquire queue handle (queues are created at device creation time)
   VkQueue queue;
   ProfileNoRet( this, vkGetDeviceQueue(device, queueTypeToFamily[underlyingType(type)], parentQueue, &queue) )

   return ptr_dynamic_cast<CommandBuffer, CommandBufferVK>(Ptr<CommandBufferVK>(new CommandBufferVK(this, queue, type, handle, fence)));
   }

   void VulkanDevice::addCommandBufferToQueue(Ptr<CommandBuffer> command)
   {
   uint32 thread    = Scheduler.core();
   uint32 executing = commandBuffersExecuting[thread];

   assert( executing < MaxCommandBuffersExecuting );

   commandBuffers[thread][executing] = command;
   commandBuffersExecuting[thread]++;
   }

   void VulkanDevice::clearCommandBuffersQueue(void)
   {
   // Iterate over list of Command Buffers submitted for execution by this thread.
   uint32 thread    = Scheduler.core();
   uint32 executing = commandBuffersExecuting[thread];
   for(uint32 i=0; i<executing; ++i)
      {
      CommandBufferVK* command = raw_reinterpret_cast<CommandBufferVK>(&commandBuffers[thread][i]);
      if (command->isCompleted())
         {
         // Safely release Command Buffer object
         commandBuffers[thread][i] = nullptr;
         if (i < (executing - 1))
            {
            commandBuffers[thread][i] = commandBuffers[thread][executing - 1];
            commandBuffers[thread][executing - 1] = nullptr;
            }

         executing--;
         commandBuffersExecuting[thread]--;
         }
      }
   }

   }
}
#endif
