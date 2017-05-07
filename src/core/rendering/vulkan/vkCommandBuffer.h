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
      Ptr<SemaphoreVK> semaphore; // Execution order synchronization    
      VkFence          fence;     // Completion notification
      bool             started;
      bool             encoding;
      bool             commited;

      // Internal methods

      CommandBufferVK(VulkanDevice*         gpu,
                      const VkQueue         queue,
                      const QueueType       queueType,
                      const VkCommandBuffer handle,
                      const VkFence         fence);

      void barrier(const Ptr<Texture> _texture, 
                   const uint32v2 mipmaps, 
                   const uint32v2 layers,
                   const VkAccessFlags currentAccess,
                   const VkAccessFlags newAccess,
                   const VkImageLayout currentLayout,
                   const VkImageLayout newLayout,
                   const VkPipelineStageFlags afterStage,   // Transition after this stage
                   const VkPipelineStageFlags beforeStage); // Transition before this stage

      bool isCompleted(void);

      // Interface methods
                      
      virtual ~CommandBufferVK();
      
      virtual void start(const Ptr<Semaphore> waitForSemaphore = nullptr);

      virtual void copy(Ptr<Buffer> source,
                        Ptr<Buffer> destination);

      virtual void copy(Ptr<Buffer> source,
                        Ptr<Buffer> destination,
                        uint64 size,
                        uint64 srcOffset = 0u,
                        uint64 dstOffset = 0u);
         
      virtual void copy(Ptr<Buffer> source,
                        Ptr<Texture> texture,
                        const uint32 mipmap,
                        const uint32 layer);

      virtual void startRenderPass(const Ptr<RenderPass> pass, 
                                   const Ptr<Framebuffer> framebuffer);

      virtual void setDescriptors(const Ptr<PipelineLayout> layout, 
                                  const Ptr<DescriptorSet> set,
                                  const uint32 index = 0u);

      virtual void setDescriptors(const Ptr<PipelineLayout> layout, 
                                  const uint32 count,
                                  const Ptr<DescriptorSet>* sets,
                                  const uint32 firstIndex = 0u);

      virtual void setPipeline(const Ptr<Pipeline> pipeline);

      virtual void setVertexBuffers(const uint32 count, 
                                    const uint32 firstSlot, 
                                    const Ptr<Buffer>* buffers, 
                                    const uint64* offsets = nullptr) const;

      virtual void setVertexBuffer(const uint32 slot, 
                                   const Ptr<Buffer> buffer, 
                                   const uint64 offset = 0u) const;
                        
      virtual void draw(const DrawableType primitiveType,
                        const uint32       elements      = 1,   // Elements to process (they are assembled into Primitives)
                        const Ptr<Buffer>  indexBuffer   = nullptr, // Optional Index buffer
                        const uint32       instances     = 1,   // Instances to draw
                        const uint32       firstElement  = 0,   // First element to process (or index in Index buffer if specified)
                        const sint32       firstVertex   = 0,   // VertexID from which numeration should start (can be negative)
                        const uint32       firstInstance = 0);  // InstanceID from which numeration should start
         
      virtual void draw(const DrawableType primitiveType,
                        const Ptr<Buffer>  indirectBuffer,      // Buffer from which Draw parameters are sourced
                        const uint32       firstEntry   = 0,    // First entry to process in Indirect buffer
                        const Ptr<Buffer>  indexBuffer  = nullptr, // Optional Index buffer
                        const uint32       firstElement = 0);   // First index to process in Index buffer (if specified)


      virtual void endRenderPass(void);

      virtual void barrier(const Ptr<Texture>  texture, 
                           const uint32v2      mipmaps, 
                           const uint32v2      layers,
                           const TextureAccess currentAccess,
                           const TextureAccess newAccess);

      virtual void commit(const Ptr<Semaphore> signalSemaphore = nullptr);
      virtual void waitUntilCompleted(void);
      };
   }
}
#endif

#endif
