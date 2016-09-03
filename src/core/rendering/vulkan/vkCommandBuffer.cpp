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

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/log/log.h"
#include "core/rendering/vulkan/vkDevice.h"
#include "core/rendering/vulkan/vkBuffer.h"

namespace en
{
   namespace gpu
   {

   CommandBufferVK::CommandBufferVK(const VulkanDevice* _gpu, VkQueue _queue, VkCommandBuffer _handle, VkFence _fence) :
      gpu(_gpu),
      queue(_queue),
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
   
   
   bool CommandBufferVK::startRenderPass(const Ptr<RenderPass> pass)
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
      
   // Begin encoding commands for this Render Pass
   VkRenderPassBeginInfo beginInfo;
   beginInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
   beginInfo.pNext           = nullptr;
   beginInfo.renderPass      = pass->passHandle;
   beginInfo.framebuffer     = pass->framebufferHandle;
   beginInfo.renderArea      = { width, height };   // TODO: Source width & height
   beginInfo.clearValueCount = pass->attachments;
   beginInfo.pClearValues    = pass->clearValues;

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
      ProfileNoRet( gpu, vkCmdDrawIndexedIndirect(handle
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
      
      SemaphoreVK();
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
   Profile( gpu, vkDestroySemaphore(gpu->device, handle, nullptr) )
   }
   



   
   
   void CommandBufferVK::commit(void)
   {
   assert( started );
   assert( !encoding );
   assert( !commited );
   
   // Finish Command Buffer encoding.
   Profile( gpu, vkEndCommandBuffer(handle) )
   
   
   
   
   
   typedef enum VkPipelineStageFlagBits {
    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT                    = 0x00000001,
    VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT                  = 0x00000002,
    VK_PIPELINE_STAGE_VERTEX_INPUT_BIT                   = 0x00000004,
    VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                  = 0x00000008,
    VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT    = 0x00000010,
    VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT = 0x00000020,
    VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT                = 0x00000040,
    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT                = 0x00000080,
    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT           = 0x00000100,
    VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT            = 0x00000200,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT        = 0x00000400,
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                 = 0x00000800,
    VK_PIPELINE_STAGE_TRANSFER_BIT                       = 0x00001000,
    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT                 = 0x00002000,
    VK_PIPELINE_STAGE_HOST_BIT                           = 0x00004000,
    VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT                   = 0x00008000,
    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT                   = 0x00010000,
} VkPipelineStageFlagBits;
   

   //
   uint32 waitEvents = 0u;
   VkPipelineStageFlags* waitFlags = nullptr;

   // TODO: Add support for waiting for previous job to be finished through use of Semaphores.
   //       Point out for which Pipeline Stages we wait for.
   if (waitEvents)
      {
      waitFlags = new VkPipelineStageFlags[waitEvents];
      for(uint32 i=0; i<waitEvent; ++i)
         {
         waitFlags[i] = 0u;
         for(uint32 j=0; j<waitStagesInEvent[i]; ++j)
            waitFlags[i] |= TranslatePipelineStage[waitStageInEvent[i][j]];
         }
      }
   
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
   
   submitInfo.waitSemaphoreCount   = waitEvents;
    const VkSemaphore* pWaitSemaphores;              // Which semaphores we wait for.
   submitInfo.pWaitDstStageMask    = waitFlags;      // Before which pipeline stage we wait blocked, before they are signaled.
    
   submitInfo.commandBufferCount   = 1u;
   submitInfo.pCommandBuffers      = &handle;
   
   // Amount of Semaphores we want to signal when this batch of job is done.
   // (more than one so we can unblock several queues at the same time)
   submitInfo.signalSemaphoreCount = signalEvents;
    const VkSemaphore* pSignalSemaphores = signalHandles;

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
      Log << "GPU Hang! Engine file: " << __FILE__ << " line: " << __LINE__ << endl;
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
   //   and srcAccessMask members set to guarantee completion of the writes. If the
   //   memory was allocated without the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT set,
   //   then vkInvalidateMappedMemoryRanges must be called after the fence is
   //   signaled in order to ensure the writes are visible to the host, as described
   //   in Host Access to Device Memory Objects. "
   //
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
   ProfileNoRet( gpu, vkFreeCommandBuffers(gpu->device, commandPool[thread][underlyingType(type)], 1, &handle) )


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
   commandPool                    = commandPool[thread][parentQueue];
   commandInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Secondary CB's need VK_COMMAND_BUFFER_LEVEL_SECONDARY
   commandInfo.commandBufferCount = 1; // Can create multiple CB's at once
   
   Profile( this, vkAllocateCommandBuffers(device, &commandInfo, &handle) )

   // Create Fence that will be signaled when the Command Buffer execution is finished.
   VkFence fence;
   
   VkFenceCreateInfo fenceInfo;
   fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
   fenceInfo.pNext = nullptr;
   fenceInfo.flags = 0; // VK_FENCE_CREATE_SIGNALED_BIT if want to create it signaled from start
   
   Profile( this, vkCreateFence(device, &fenceInfo, nullptr, &fence) )

   // Acquire queue handle (queues are created at device creation time)
   VkQueue queue;
   Profile( this, vkGetDeviceQueue(device, queueTypeToFamily[underlyingType(type)], parentQueue, &queue) )

   return ptr_dynamic_cast<CommandBuffer, CommandBufferVK>(Ptr<CommandBufferVK>(new CommandBufferVK(this, queue, handle, fence)));
   }

   }
}
#endif
