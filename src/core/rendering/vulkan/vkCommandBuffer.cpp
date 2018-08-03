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
   ValidateNoRet( gpu, vkDestroyFence(gpu->device, fence, nullptr) )
   
   // Release Command Buffer
   uint32 thread = Scheduler.core();
   ValidateNoRet( gpu, vkFreeCommandBuffers(gpu->device, gpu->commandPool[thread][underlyingType(queueType)], 1, &handle) )
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
   

   void CommandBufferVK::start(const Semaphore* waitForSemaphore)
   {
   assert( !started );

   if (waitForSemaphore)
      semaphore = reinterpret_cast<const SemaphoreVK*>(waitForSemaphore);

   // In Metal API we need to create Render Command Encoder.
   // In Vulkan API CommandBuffer needs to be started first,
   // before encoding content to it. It only needs to be done
   // once, and multiple RenderPasses can be encoded afterwards.
   VkCommandBufferBeginInfo info;
   info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
   info.pNext            = nullptr;
   info.flags            = 0;
   info.pInheritanceInfo = nullptr; // We don't support secondary Command Buffers for now

   Validate( gpu, vkBeginCommandBuffer(handle, &info) )
   
   started = true;
   }
   
   void CommandBufferVK::startRenderPass(const shared_ptr<RenderPass> pass, const shared_ptr<Framebuffer> _framebuffer)
   {
   assert( started );
   assert( !encoding );

   assert( pass );
   assert( _framebuffer );

   RenderPassVK*  renderPass  = reinterpret_cast<RenderPassVK*>(pass.get());
   FramebufferVK* framebuffer = reinterpret_cast<FramebufferVK*>(_framebuffer.get());

   // Begin encoding commands for this Render Pass
   VkRenderPassBeginInfo beginInfo;
   beginInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
   beginInfo.pNext           = nullptr;
   beginInfo.renderPass      = renderPass->handle;
   beginInfo.framebuffer     = framebuffer->handle;
   beginInfo.renderArea      = { 0, 0, framebuffer->resolution.width, framebuffer->resolution.height };
   beginInfo.clearValueCount = renderPass->surfaces;
   beginInfo.pClearValues    = renderPass->clearValues;

   ValidateNoRet( gpu, vkCmdBeginRenderPass(handle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE) )
   
   encoding = true;
   }
   
   void CommandBufferVK::endRenderPass(void)
   {
   assert( started );
   assert( encoding );

   // End encoding commands for this Render Pass
   ValidateNoRet( gpu, vkCmdEndRenderPass(handle) )
    
   encoding = false;
   }
   

   // SETTING INPUT ASSEMBLER VERTEX BUFFERS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferVK::setVertexBuffers(const uint32 firstSlot,
                                          const uint32 count,
                                          const shared_ptr<Buffer>(&buffers)[],
                                          const uint64* offsets) const
   {
   assert( started );
   assert( count );
   assert( (firstSlot + count) <= gpu->support.maxInputLayoutBuffersCount );

   // Extract Vulkan buffer handles
   VkBuffer* handles = new VkBuffer[count];   // TODO: Optimize by allocating on the stack maxBuffersCount sized fixed array.
   for(uint32 i=0; i<count; ++i)
      {
      assert( buffers[i] );
      handles[i] = reinterpret_cast<BufferVK*>(buffers[i].get())->handle;
      }

   // Generate default zero offsets array if none is passed
   uint64* finalOffsets = (uint64*)(offsets);
   if (!offsets)
      {
	  finalOffsets = new uint64[count];
	  memset(finalOffsets, 0, sizeof(uint64)*count);
	  }

   ValidateNoRet( gpu, vkCmdBindVertexBuffers(handle, 
                                              firstSlot,
                                              count,
                                              handles,
                                              static_cast<const VkDeviceSize*>(finalOffsets)) )
   delete [] handles;
   if (!offsets)
      delete [] finalOffsets;
   }

   void CommandBufferVK::setInputBuffer(const uint32  firstSlot,
                                        const uint32  slots,
                                        const Buffer& _buffer,
                                        const uint64* offsets) const
   {
   assert( started );
   assert( slots );
   assert( (firstSlot + slots) <= gpu->support.maxInputLayoutBuffersCount );

   const BufferVK& buffer = reinterpret_cast<const BufferVK&>(_buffer);

   // Extract Vulkan buffer handles
   VkBuffer* handles = new VkBuffer[slots];   // TODO: Optimize by allocating on the stack maxBuffersCount sized fixed array.
   for(uint32 i=0; i<slots; ++i)
      handles[i] = buffer.handle;

   // Generate default zero offsets array if none is passed
   uint64* finalOffsets = (uint64*)(offsets);
   if (!offsets)
      {
	  finalOffsets = new uint64[slots];
	  memset(finalOffsets, 0, slots * sizeof(uint64));
	  }

   ValidateNoRet( gpu, vkCmdBindVertexBuffers(handle, 
                                              firstSlot,
                                              slots,
                                              handles,
                                              static_cast<const VkDeviceSize*>(finalOffsets)) )
   delete [] handles;
   if (!offsets)
      delete [] finalOffsets;
   }

   void CommandBufferVK::setVertexBuffer(const uint32 slot,
                                         const Buffer& buffer,
                                         const uint64 offset) const
   {
   assert( started );
   assert( slot < gpu->support.maxInputLayoutBuffersCount );

   ValidateNoRet( gpu, vkCmdBindVertexBuffers(handle, 
                                              slot, 1,
                                              &reinterpret_cast<const BufferVK&>(buffer).handle,
                                              static_cast<const VkDeviceSize*>(&offset)) )
   }

   void CommandBufferVK::setIndexBuffer(
      const Buffer& buffer,
      const Attribute type,
      const uint32 offset)
   {
   assert( started );
   assert( encoding );
   assert( type == Attribute::u16 ||
           type == Attribute::u32 );

   // Elements are assembled into Primitives.
   const BufferVK& index = reinterpret_cast<const BufferVK&>(buffer);
   
   assert( index.apiType == BufferType::Index );
   
   VkIndexType indexType = VK_INDEX_TYPE_UINT16;
   if (type == Attribute::u32)
      indexType = VK_INDEX_TYPE_UINT32;
   
   // Index Buffer remains bound to Command Buffer after this call,
   // but because there is no other way to do indexed draw than to
   // go through this API, it's safe to leave it bounded.
   ValidateNoRet( gpu, vkCmdBindIndexBuffer(handle,
                                            index.handle,
                                            offset,             // Offset In Index Buffer is calculated at draw call.
                                            indexType) )
   }


   // TRANSFER COMMANDS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferVK::copy(const Buffer& source, const Buffer& destination)
   {
   assert( started );
   assert( source.length() <= destination.length() );

   copy(source, destination, source.length());
   }
   
   void CommandBufferVK::copy(
      const Buffer& source,
      const Buffer& destination,
      const uint64 size,
      const uint64 srcOffset,
      const uint64 dstOffset)
   {
   assert( started );
   assert( source.type() == BufferType::Transfer );
   assert( (srcOffset + size) <= source.length() );
   assert( (dstOffset + size) <= destination.length() );
   
   const BufferVK& src = reinterpret_cast<const BufferVK&>(source);
   const BufferVK& dst = reinterpret_cast<const BufferVK&>(destination);

   VkBufferCopy region;
   region.srcOffset = srcOffset;
   region.dstOffset = dstOffset;
   region.size      = size;

   ValidateNoRet( gpu, vkCmdCopyBuffer(handle,
                                      src.handle,
                                      dst.handle,
                                      1u,
                                      &region) )
   }

   void CommandBufferVK::copy(
       const Buffer&  transfer,
       const uint64   srcOffset,
       const uint32   srcRowPitch,
       const Texture& texture,
       const uint32   mipmap,
       const uint32   layer)
   {
   assert( started );
   assert( transfer.type() == BufferType::Transfer );
   assert( texture.mipmaps() > mipmap );
   assert( texture.layers() > layer );
   
   const BufferVK&  source      = reinterpret_cast<const BufferVK&>(transfer);
   const TextureVK& destination = reinterpret_cast<const TextureVK&>(texture);

   assert( source.size >= srcOffset + destination.size(mipmap) );
   
   VkImageSubresourceLayers layersInfo;
   layersInfo.aspectMask     = TranslateImageAspect(destination.state.format);
   layersInfo.mipLevel       = mipmap;
   layersInfo.baseArrayLayer = layer;
   layersInfo.layerCount     = 1u;
   
   assert( source.size < 0xFFFFFFFF );

   VkBufferImageCopy regionInfo;
   regionInfo.bufferOffset      = srcOffset;
   regionInfo.bufferRowLength   = srcRowPitch;
   regionInfo.bufferImageHeight = 0u;
   regionInfo.imageSubresource  = layersInfo;
   regionInfo.imageOffset       = { 0u, 0u, 0u };
   regionInfo.imageExtent       = { destination.width(mipmap), destination.height(mipmap), 1 };
   
   ValidateNoRet( gpu, vkCmdCopyBufferToImage(handle,
                                              source.handle,
                                              destination.handle,
                                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                              1, &regionInfo) )

   }

   void CommandBufferVK::copyRegion2D(
      const Buffer&  _source,
      const uint64   srcOffset,
      const uint32   srcRowPitch,
      const Texture& texture,
      const uint32   mipmap,
      const uint32   layer,
      const uint32v2 origin,
      const uint32v2 region,
      const uint8    plane)
   {
   const BufferVK&  source      = reinterpret_cast<const BufferVK&>(_source);
   const TextureVK& destination = reinterpret_cast<const TextureVK&>(texture);

   assert( started );
   assert( source.type() == BufferType::Transfer );
   assert( mipmap < destination.state.mipmaps );
   assert( layer < destination.state.layers );
   assert( origin.x + region.width  <= destination.width(mipmap) );
   assert( origin.y + region.height <= destination.height(mipmap) );
   //assert( source.size >= srcOffset + layout.size );

   VkImageSubresourceLayers layersInfo;
   layersInfo.aspectMask     = TranslateImageAspect(destination.state.format);
   layersInfo.mipLevel       = mipmap;
   layersInfo.baseArrayLayer = layer;
   layersInfo.layerCount     = 1u;
   
   // Specify planes to blit
   if (isDepthStencil(destination.state.format))
      {
      layersInfo.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
      if (plane == 1)
         layersInfo.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
      }

   assert( source.size < 0xFFFFFFFF );

   // TODO: What about plane selection?
   
   VkBufferImageCopy regionInfo;
   regionInfo.bufferOffset      = srcOffset;
   regionInfo.bufferRowLength   = srcRowPitch;
   regionInfo.bufferImageHeight = 0;               // Tightly packed, see spec: 18.4. Copying Data Between Buffers and Images
   regionInfo.imageSubresource  = layersInfo;
   regionInfo.imageOffset       = { static_cast<sint32>(origin.x), static_cast<sint32>(origin.y), 0 };
   regionInfo.imageExtent       = { region.width, region.height, 1 };
   
   ValidateNoRet( gpu, vkCmdCopyBufferToImage(handle,
                                              source.handle,
                                              destination.handle,
                                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                              1, &regionInfo) )
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


   void CommandBufferVK::setPipeline(const Pipeline& _pipeline)
   {
   assert( started );
   
   const PipelineVK& pipeline = reinterpret_cast<const PipelineVK&>(_pipeline);

   VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
   if (!pipeline.graphics)
      bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

   ValidateNoRet( gpu, vkCmdBindPipeline(handle, bindPoint, pipeline.handle) )
   }


   // DRAW COMMANDS
   //////////////////////////////////////////////////////////////////////////

   void CommandBufferVK::draw(const uint32  elements,
                              const uint32  instances,
                              const uint32  firstVertex,
                              const uint32  firstInstance) const
   {
   assert( started );
   assert( encoding );
   assert( elements );

   ValidateNoRet( gpu, vkCmdDraw(handle,
                                 elements,
                                 max(1U, instances),
                                 firstVertex,       // Index of first vertex to draw (offset in buffer, VertexID == 0)
                                 firstInstance) )
   }

   void CommandBufferVK::drawIndexed(const uint32  elements,
                                     const uint32  instances,
                                     const uint32  firstIndex,
                                     const sint32  firstVertex,
                                     const uint32  firstInstance) const
   {
   assert( started );
   assert( encoding );
   assert( elements );

   ValidateNoRet( gpu, vkCmdDrawIndexed(handle,
                                        elements,
                                        max(1U, instances),
                                        firstIndex,    // Index of first index to start (multiplied by elementSize will give starting offset in IBO). There can be several buffers with separate indexes groups in one GPU Buffer.
                                        firstVertex,     // VertexID of first processed vertex
                                        firstInstance) ) // InstanceID of first processed instance
   }

   void CommandBufferVK::drawIndirect(
      const Buffer& indirectBuffer,
      const uint32  firstEntry) const
   {
   assert( started );
   assert( encoding );

   const BufferVK& indirect = reinterpret_cast<const BufferVK&>(indirectBuffer);
   assert( indirect.apiType == BufferType::Indirect );
   
   // IndirectDrawArgument can be directly cast to VkDrawIndirectCommand.
   
   // TODO: Currently draw count is equal to amount of entries from first entry to the end of the indirect buffer.
   ValidateNoRet( gpu, vkCmdDrawIndirect(handle,
                                         indirect.handle,
                                         (firstEntry * sizeof(VkDrawIndirectCommand)),
                                         (indirect.size / sizeof(VkDrawIndirectCommand)) - firstEntry,
                                         sizeof(VkDrawIndirectCommand)) )
   }

   void CommandBufferVK::drawIndirectIndexed(
      const Buffer& indirectBuffer,
      const uint32  firstEntry,
      const uint32  firstIndex) const
   {
   assert( started );
   assert( encoding );

   const BufferVK& indirect = reinterpret_cast<const BufferVK&>(indirectBuffer);
   assert( indirect.apiType == BufferType::Indirect );
   
   // IndirectIndexedDrawArgument can be directly cast to VkDrawIndexedIndirectCommand.

   // TODO: Currently draw count is equal to amount of entries from first entry to the end of the indirect buffer.
   ValidateNoRet( gpu, vkCmdDrawIndexedIndirect(handle,
                                                indirect.handle,
                                                (firstEntry * sizeof(VkDrawIndexedIndirectCommand)),
                                                (indirect.size / sizeof(VkDrawIndexedIndirectCommand)) - firstEntry,
                                                sizeof(VkDrawIndexedIndirectCommand)) )
   }
   

   // FINISHING
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferVK::commit(Semaphore* signalSemaphore)
   {
   assert( started );
   assert( !encoding );
   assert( !commited );
   
   // Finish Command Buffer encoding.
   Validate( gpu, vkEndCommandBuffer(handle) )
   
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
      SemaphoreVK* signal = reinterpret_cast<SemaphoreVK*>(signalSemaphore);

      submitInfo.signalSemaphoreCount = 1u;   
      submitInfo.pSignalSemaphores    = &signal->handle;        
      }

   // Submit single batch of work to the queue.
   // Each batch can consist of multiple command buffers.
   // Internal fence will be signaled when this work is done.
   Validate( gpu, vkQueueSubmit(queue, 1, &submitInfo, fence) ) 

   // Try to clear any CommandBuffers that are no longer executing.
   gpu->clearCommandBuffersQueue();

   // Add this CommandBuffer to device's array of CB's in flight.
   // This will ensure that CommandBuffer won't be destroyed until
   // fence is not signaled.
   gpu->addCommandBufferToQueue(shared_from_this());

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
   
   Validate( gpu, vkWaitForFences(gpu->device, 1, &fence, VK_TRUE, gpuWatchDog) )
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
   
   shared_ptr<CommandBuffer> VulkanDevice::createCommandBuffer(const QueueType type, const uint32 parentQueue)
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
   
   Validate( this, vkAllocateCommandBuffers(device, &commandInfo, &handle) )

   // Create Fence that will be signaled when the Command Buffer execution is finished.
   VkFence fence = VK_NULL_HANDLE;
   
   VkFenceCreateInfo fenceInfo;
   fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
   fenceInfo.pNext = nullptr;
   fenceInfo.flags = 0; // VK_FENCE_CREATE_SIGNALED_BIT if want to create it in signaled state from start
   
   Validate( this, vkCreateFence(device, &fenceInfo, nullptr, &fence) )

   // Acquire queue handle (queues are created at device creation time)
   VkQueue queue;
   ValidateNoRet( this, vkGetDeviceQueue(device, queueTypeToFamily[underlyingType(type)], parentQueue, &queue) )

   return make_shared<CommandBufferVK>(this, queue, type, handle, fence);
   }

   void VulkanDevice::addCommandBufferToQueue(shared_ptr<CommandBuffer> command)
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
      CommandBufferVK* command = reinterpret_cast<CommandBufferVK*>(commandBuffers[thread][i].get());
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
