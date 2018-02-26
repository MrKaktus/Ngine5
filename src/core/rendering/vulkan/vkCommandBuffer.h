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

#ifndef ENG_CORE_RENDERING_VULKAN_COMMAND_BUFFER
#define ENG_CORE_RENDERING_VULKAN_COMMAND_BUFFER

#include "core/rendering/vulkan/vulkan.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/rendering/device.h"
#include "core/rendering/commandBuffer.h"
#include "core/rendering/vulkan/vkBuffer.h"
#include "core/rendering/vulkan/vkSynchronization.h"

namespace en
{
   namespace gpu
   {
   class VulkanDevice;

   class CommandBufferVK : public CommandBuffer
      {
      public:
      VulkanDevice*    gpu;       // Vulkan Device (for Device function calls)
      VkQueue          queue;
      QueueType        queueType;
      VkCommandBuffer  handle;
      const SemaphoreVK* semaphore; // Execution order synchronization    
      VkFence          fence;     // Completion notification
      bool             started;
      bool             encoding;
      bool             commited;

      // State cache

      const BufferVK* currentIndexBuffer;

      // Internal methods

      CommandBufferVK(VulkanDevice*         gpu,
                      const VkQueue         queue,
                      const QueueType       queueType,
                      const VkCommandBuffer handle,
                      const VkFence         fence);

      void barrier(const Buffer& buffer,
                   const uint64 offset,
                   const uint64 size,
                   const VkAccessFlags currentAccess,
                   const VkAccessFlags newAccess,
                   const VkPipelineStageFlags afterStage,   // Transition after this stage
                   const VkPipelineStageFlags beforeStage); // Transition before this stage

      void barrier(const Texture& texture,
                   const uint32v2 mipmaps, 
                   const uint32v2 layers,
                   const VkAccessFlags currentAccess,
                   const VkAccessFlags newAccess,
                   const VkImageLayout currentLayout,
                   const VkImageLayout newLayout,
                   const VkPipelineStageFlags afterStage,   // Transition after this stage
                   const VkPipelineStageFlags beforeStage); // Transition before this stage

      // Vulkan specific API (thus private for now)
      shared_ptr<Event> signal(void);                      // Event will be signaled, once execution reaches this point in Command Buffer
      void       wait(shared_ptr<Event> eventToWaitFor);   // Commnad Buffer execution will be stalled until given event won't be signaled

      bool isCompleted(void);

      // Interface methods
                      
      virtual ~CommandBufferVK();
      
      virtual void start(const Semaphore* waitForSemaphore = nullptr);

      virtual void copy(const Buffer& source,
                        const Buffer& destination);

      virtual void copy(const Buffer& source,
                        const Buffer& destination,
                        const uint64 size,
                        const uint64 srcOffset = 0u,
                        const uint64 dstOffset = 0u);
         
      virtual void copy(const Buffer& source,
                        const uint64 srcOffset,
                        const Texture& texture,
                        const uint32 mipmap,
                        const uint32 layer);

      virtual void copy(const Buffer& source,
                        const Texture& texture,
                        const uint32 mipmap,
                        const uint32 layer);

      virtual void startRenderPass(const shared_ptr<RenderPass> pass, 
                                   const shared_ptr<Framebuffer> framebuffer);

      virtual void setDescriptors(const PipelineLayout& layout,
                                  const DescriptorSet& set,
                                  const uint32 index = 0u);

      virtual void setDescriptors(const PipelineLayout& layout, 
                                  const uint32 count,
                                  const shared_ptr<DescriptorSet>(&sets)[],
                                  const uint32 firstIndex = 0u);

      virtual void setPipeline(const Pipeline& pipeline);

      virtual void setVertexBuffers(const uint32 firstSlot,
                                    const uint32 count, 
                                    const shared_ptr<Buffer>(&buffers)[],
                                    const uint64* offsets = nullptr) const;

      virtual void setInputBuffer(
         const uint32  firstSlot,
         const uint32  slots,
         const Buffer& buffer,
         const uint64* offsets = nullptr) const;

      virtual void setVertexBuffer(const uint32 slot, 
                                   const Buffer& buffer, 
                                   const uint64 offset = 0u) const;

      virtual void setIndexBuffer(
         const Buffer& buffer,
         const Attribute type,
         const uint32 offset = 0u) const;
                        
      virtual void draw(
         const uint32  elements,
         const uint32  instances     = 1,
         const uint32  firstVertex   = 0,
         const uint32  firstInstance = 0) const;

      virtual void drawIndexed(
         const uint32  elements,
         const uint32  instances     = 1,
         const uint32  firstIndex    = 0,
         const sint32  firstVertex   = 0,
         const uint32  firstInstance = 0) const;
         
      virtual void drawIndirect(
         const Buffer& indirectBuffer,
         const uint32  firstEntry   = 0,
         const Buffer* indexBuffer  = nullptr,
         const uint32  firstElement = 0); 

      virtual void endRenderPass(void);

      virtual void barrier(const Buffer& buffer,
                           const BufferAccess initAccess);

      virtual void barrier(const Buffer& buffer,
                           const uint64 offset,
                           const uint64 size,
                           const BufferAccess currentAccess,
                           const BufferAccess newAccess);

      virtual void barrier(const Texture& texture,
                           const TextureAccess initAccess);

      virtual void barrier(const Texture& texture,
                           const TextureAccess currentAccess,
                           const TextureAccess newAccess);

      virtual void barrier(const Texture& texture,
                           const uint32v2 mipmaps,
                           const uint32v2 layers,
                           const TextureAccess currentAccess,
                           const TextureAccess newAccess);

      virtual void commit(Semaphore* signalSemaphore = nullptr);
      virtual void waitUntilCompleted(void);
      };
   }
}
#endif

#endif
