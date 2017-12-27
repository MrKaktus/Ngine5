/*

 Ngine v5.0
 
 Module      : Metal Command Buffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_COMMAND_BUFFER
#define ENG_CORE_RENDERING_METAL_COMMAND_BUFFER

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/metal.h"
#include "core/rendering/commandBuffer.h"
#include "core/rendering/metal/mtlDevice.h"

namespace en
{
   namespace gpu
   {
   class CommandBufferMTL : public CommandBuffer
      {
      public:
      MetalDevice* gpu;  // GPU owning this Command Buffer (for temporary staging buffers creation)
      id <MTLCommandBuffer> handle;
      id <MTLRenderCommandEncoder> renderEncoder;
      bool commited;
      
      // State cache
      
      MTLPrimitiveType primitive;
         
      virtual void start(const shared_ptr<Semaphore> waitForSemaphore = nullptr);

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
      



      virtual void setVertexBuffers(const uint32 count, 
                                    const uint32 firstSlot, 
                                    const shared_ptr<Buffer>(&buffers)[],
                                    const uint64* offsets = nullptr) const;

      virtual void setVertexBuffer(const uint32 slot, 
                                   const Buffer& buffer, 
                                   const uint64 offset = 0u) const;

      virtual void copy(const Buffer& source,
                        const Buffer& destintion);
         
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
         
      virtual void draw(
         const uint32  elements,
         const Buffer* indexBuffer   = nullptr,
         const uint32  instances     = 1,
         const uint32  firstElement  = 0,
         const sint32  firstVertex   = 0,
         const uint32  firstInstance = 0);
         
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

      virtual void commit(const shared_ptr<Semaphore> signalSemaphore = nullptr);
      
      virtual void waitUntilCompleted(void);
   
      CommandBufferMTL(MetalDevice* gpu);
      virtual ~CommandBufferMTL();
      };
   }
}
#endif

#endif
