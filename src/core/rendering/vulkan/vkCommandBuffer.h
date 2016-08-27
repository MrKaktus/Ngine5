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

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_WINDOWS)

#include "core/rendering/commandBuffer.h"

namespace en
{
   namespace gpu
   {
   class CommandBufferVK : public CommandBuffer
      {
      public:
      VulkanDevice*   gpu;       // Vulkan Device (for Device function calls)
      VKQueue         queue;
      VkCommandBuffer handle;
      VkFence         fence;     // Completion notification
      bool            encoding;
      bool            commited;
      
      virtual void bind(const Ptr<RasterState> raster);
      //virtual void bind(const Ptr<ViewportScissorState> viewportScissor);
      virtual void bind(const Ptr<DepthStencilState> depthStencil);
      virtual void bind(const Ptr<BlendState> blend);
      
      virtual bool startRenderPass(const Ptr<RenderPass> pass);
      virtual void set(const Ptr<Pipeline> pipeline);
      
      // TEMP: Until Descriptor Tables abstraction is not done
      virtual void setVertexBuffer(Ptr<Buffer> buffer, uint32 slot);
      virtual bool populate(Ptr<Buffer> buffer, const void* data);
  
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


      virtual bool endRenderPass(void);
      virtual void commit(void);
      virtual void waitUntilCompleted(void);
   
      CommandBufferVK(const VulkanDevice*   gpu
                      const VKQueue         queue,
                      const VkCommandBuffer handle,
                      const VkFence         fence);
                      
      virtual ~CommandBufferVK();
      };
   }
}
#endif

#endif
