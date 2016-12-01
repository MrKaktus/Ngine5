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

   CommandBufferVK::CommandBufferVK(VulkanDevice* _gpu, const VkQueue _queue, const QueueType type, const VkCommandBuffer _handle, const VkFence _fence) :
      gpu(_gpu),
      queue(_queue),
      queueType(type),
      handle(_handle),
      fence(_fence),
      started(false),
      encoding(false),
      commited(false),
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
   
   
   bool CommandBufferVK::startRenderPass(const Ptr<RenderPass> pass, const Ptr<Framebuffer>_framebuffer)
   {
   if (encoding)
      return false;
  
   if (!started)
      {
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
   return true;
   }
   
   bool CommandBufferVK::endRenderPass(void)
   {
   if (!encoding)
      return false;
   
   // End encoding commands for this Render Pass
   ProfileNoRet( gpu, vkCmdEndRenderPass(handle) )
    
   encoding = false;
   return true;
   }
   

   // SETTING INPUT ASSEMBLER VERTEX BUFFERS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferVK::setVertexBuffers(const uint32 count, const uint32 firstSlot, const Ptr<Buffer>* buffers, const uint64* offsets) const
   {
   assert( count );
   assert( (firstSlot + count) <= gpu->properties.limits.maxVertexInputBindings );

   // Extrack Vulkan buffer handles
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
   ProfileNoRet( gpu, vkCmdBindVertexBuffers(handle, 
                                             slot, 1,
                                             &ptr_dynamic_cast<BufferVK, Buffer>(buffer)->handle, 
                                             static_cast<const VkDeviceSize*>(&offset)) )
   }


   // TRANSFER COMMANDS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferVK::copy(Ptr<Buffer> source, Ptr<Buffer> buffer)
   {
   // TODO:
   
//   void vkCmdCopyBuffer(
//    VkCommandBuffer                             commandBuffer,
//    VkBuffer                                    srcBuffer,
//    VkBuffer                                    dstBuffer,
//    uint32_t                                    regionCount,
//    const VkBufferCopy*                         pRegions);
   }
   
   void CommandBufferVK::copy(Ptr<Buffer> transfer, Ptr<Texture> texture, const uint32 mipmap, const uint32 layer)
   {
   assert( transfer );
   assert( transfer->type() == BufferType::Transfer );
   assert( texture );
   assert( texture->mipmaps() > mipmap );
   assert( texture->layers() > layer );
   
   BufferVK*  source      = raw_reinterpret_cast<BufferVK>(&transfer);
   TextureVK* destination = raw_reinterpret_cast<TextureVK>(&texture);

   assert( source->size >= destination->size(mipmap) );
   
   VkImageSubresourceLayers layersInfo;
   layersInfo.aspectMask     = TranslateImageAspect(destination->state.format);
   layersInfo.mipLevel       = mipmap;
   layersInfo.baseArrayLayer = layer;
   layersInfo.layerCount     = 1u;
   
   VkBufferImageCopy regionInfo;
   regionInfo.bufferOffset      = 0u;
   regionInfo.bufferRowLength   = source->size;
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
   

   // DRAW COMMANDS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferVK::draw(const DrawableType primitiveType,
                              const uint32       elements,
                              const Ptr<Buffer>  indexBuffer,
                              const uint32       instances,
                              const uint32       firstElement,
                              const sint32       firstVertex,
                              const uint32       firstInstance)
   {
   // Vulkan cannot dynamically change drawn primitive type
   // TODO: Should we remove primitiveType from parameters ?
   
   // Elements are assembled into Primitives.
   if (indexBuffer)
      {
      Ptr<BufferVK> index = ptr_dynamic_cast<BufferVK, Buffer>(indexBuffer);
      
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
                                              (firstElement * elementSize), // Offset In Index Buffer, so that we can have several buffers with separate indexes groups in one GPU Buffer
                                              indexType) )
      // TODO: We probably should bind from offset 0, pass starting offset in draw call, and avoid binding at all if index buffer is the same.
      ProfileNoRet( gpu, vkCmdDrawIndexed(handle,
                                          elements,
                                          max(1U, instances),
                                          0,               // Index of first index to start (we've calculated final offset above)
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

   void CommandBufferVK::draw(const DrawableType primitiveType,
                              const Ptr<Buffer>  indirectBuffer,
                              const uint32       firstEntry,
                              const Ptr<Buffer>  indexBuffer,
                              const uint32       firstElement)
   {
   assert( indirectBuffer );
   
   Ptr<BufferVK> indirect = ptr_dynamic_cast<BufferVK, Buffer>(indirectBuffer);
   assert( indirect->apiType == BufferType::Indirect );
   
   if (indexBuffer)
      {
      Ptr<BufferVK> index = ptr_dynamic_cast<BufferVK, Buffer>(indexBuffer);
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
   
   
   // SEMAPHORES
   //////////////////////////////////////////////////////////////////////////


   // It's currently just a handle to synchronize command buffers execution.
   // Similar to MTLFence in Metal, but works between CB's submissions.
   class SemaphoreVK
      {
      public:
      VulkanDevice* gpu;
      VkSemaphore   handle;
      
      SemaphoreVK(VulkanDevice* _gpu);
     ~SemaphoreVK();
      };
   
   SemaphoreVK::SemaphoreVK(VulkanDevice* _gpu) :
      gpu(_gpu)
   {
   VkSemaphoreCreateInfo info;
   info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
   info.pNext = nullptr;
   info.flags = 0u;       // VkSemaphoreCreateFlags - reserved.

   Profile( gpu, vkCreateSemaphore(gpu->device, &info, nullptr, &handle) )
   }

   SemaphoreVK::~SemaphoreVK()
   {
   ProfileNoRet( gpu, vkDestroySemaphore(gpu->device, handle, nullptr) )
   }
   
   



   //enum class PipelineStage : uint32
   //   {
   //   // TODO: Do we mimic Vulkan Pipeline Stages granularity ?
   //   Count
   //   };
   //
   //static const VkPipelineStageFlagBits TranslatePipelineStage[underlyingType(PipelineStage::Count)] =
   //   {
   //   VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT                    ,
   //   VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT                  ,
   //   VK_PIPELINE_STAGE_VERTEX_INPUT_BIT                   ,
   //   VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                  ,
   //   VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT    ,
   //   VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT ,
   //   VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT                ,
   //   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT                ,
   //   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT           ,
   //   VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT            ,
   //   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT        ,
   //   VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                 ,
   //   VK_PIPELINE_STAGE_TRANSFER_BIT                       ,
   //   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT                 ,
   //   VK_PIPELINE_STAGE_HOST_BIT                           ,
   //   VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT                   ,
   //   VK_PIPELINE_STAGE_ALL_COMMANDS_BIT                   ,
   //   };
   
   void CommandBufferVK::commit(void)
   {
   assert( started );
   assert( !encoding );
   assert( !commited );
   
   // Finish Command Buffer encoding.
   Profile( gpu, vkEndCommandBuffer(handle) )
   

   
 
   //
   uint32 waitEvents = 0u;
   VkPipelineStageFlags* waitFlags = nullptr;

   // TODO: Add support for waiting for previous job to be finished through use of Semaphores.
   //       Point out for which Pipeline Stages we wait for. One Stage for One Semaphore.
   // if (waitEvents)
   //    {
   //    waitFlags = new VkPipelineStageFlags[waitEvents];
   //    for(uint32 i=0; i<waitEvents; ++i)
   //       {
   //       waitFlags[i] = 0u;
   //       for(uint32 j=0; j<waitStagesInEvent[i]; ++j)
   //          waitFlags[i] |= TranslatePipelineStage[  waitStageInEvent[i][j]  ];
   //       }
   //    }
   
   uint32 signalEvents = 0u;


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
   
   VkSubmitInfo submitInfo;
   submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
   submitInfo.pNext                = nullptr;
   submitInfo.waitSemaphoreCount   = waitEvents;     // 
   submitInfo.pWaitSemaphores      = nullptr;        // TODO: const VkSemaphore* - Which semaphores we wait for. One Semaphore for one Pipeline Stage of other queue executing CommandBuffer.
   submitInfo.pWaitDstStageMask    = waitFlags;      // pipeline stages at which each corresponding semaphore wait will occur (until this stage is done)
   submitInfo.commandBufferCount   = 1u;
   submitInfo.pCommandBuffers      = &handle;
   submitInfo.signalSemaphoreCount = signalEvents;   // Amount of Semaphores we want to signal when this batch of job is done (more than one so we can unblock several queues at the same time)
   submitInfo.pSignalSemaphores    = nullptr;        // TODO: const VkSemaphore* signalHandles

   // Fence is optional. TODO: Analyze this.
   
   // Submit single batch of work to the queue.
   // Each batch can consist of multiple command buffers.
   // Internal fence will be signaled when this work is done.
   Profile( gpu, vkQueueSubmit(queue, 1, &submitInfo, fence) )

   // TODO: Is ending command buffer equivalent to commiting it for execution ?
   
   // TODO: Commit to execution
   
   commited = true;
   }
    
   void CommandBufferVK::waitUntilCompleted(void)
   {
   // Wait maximum 1 second, then assume GPU hang.
   uint64 gpuWatchDog = 1000000000;
   
   Profile( gpu, vkWaitForFences(gpu->device, 1, &fence, VK_TRUE, gpuWatchDog) )
   if (gpu->lastResult[Scheduler.core()] == VK_TIMEOUT)
      {
      Log << "GPU Hang! Engine file: " << __FILE__ << " line: " << __LINE__ << endl;   // TODO: File / line doesn't make sense as it will always point this method!
      }

   // TODO:
   //
   // " The memory dependency defined by signaling a fence and waiting on the host
   //   does not guarantee that the results of memory accesses will be visible to
   //   the host, or that the memory is available. To provide that guarantee, the
   //   application must insert a memory barrier between the device writes and the
   //   end of the submission that will signal the fence, with dstAccessMask having
   //   the VK_ACCESS_HOST_READ_BIT bit set, with dstStageMask having the
   //   VK_PIPELINE_STAGE_HOST_BIT bit set, and with the appropriate srcStageMask
   //   and srcAccessMask members set to guarantee completion of the writes. 
   //
   //   If the memory was allocated without the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
   //   set, then vkInvalidateMappedMemoryRanges must be called after the fence is
   //   signaled in order to ensure the writes are visible to the host, as described
   //   in Host Access to Device Memory Objects. "
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
   commandInfo.commandPool        = commandPool[thread][parentQueue];
   commandInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Secondary CB's need VK_COMMAND_BUFFER_LEVEL_SECONDARY
   commandInfo.commandBufferCount = 1; // Can create multiple CB's at once
   
   Profile( this, vkAllocateCommandBuffers(device, &commandInfo, &handle) )

   // Create Fence that will be signaled when the Command Buffer execution is finished.
   VkFence fence;
   
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

   }
}
#endif
