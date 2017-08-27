/*

 Ngine v5.0
 
 Module      : D3D12 Command Buffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_D3D12_COMMAND_BUFFER
#define ENG_CORE_RENDERING_D3D12_COMMAND_BUFFER

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12.h"
#include "core/rendering/commandBuffer.h"
#include "core/rendering/d3d12/dx12Buffer.h"
#include "core/rendering/d3d12/dx12RenderPass.h"
#include "core/rendering/d3d12/dx12Synchronization.h"

namespace en
{
   namespace gpu
   {
   class Direct3D12Device;

   class CommandBufferD3D12 : public CommandBuffer
      {
      public:
      Direct3D12Device*     gpu;         // GPU owning this Command Buffer (for temporary staging buffers creation)
      ID3D12CommandQueue*   queue;
      uint32                queueIndex;  // Direct index to queue in device's array (also used for queue fence indexing) 
      ID3D12CommandList*    handle;
      ID3D12Fence*          fence;        // Fence to wait for before executing this CommandBuffer
      uint64                waitForValue; // Accompanying value representing unique moment in time
      uint64                commitValue;  // Value representing moment in time, when this CommandBuffer is executed
      Ptr<RenderPassD3D12>  renderPass;  // Render Pass and Framebuffer used for currently encoded Render Pass
      Ptr<FramebufferD3D12> framebuffer;
      bool                  started;
      bool                  encoding;
      bool                  commited;
      
      // State cache

      BufferD3D12*          currentIndexBuffer;

      // Internal methods

      CommandBufferD3D12(Direct3D12Device* _gpu, ID3D12CommandQueue* _queue, uint32 queueIndex, ID3D12CommandList* _handle);

      bool isCompleted(void);

      // Interface methods

      virtual ~CommandBufferD3D12();

      virtual void start(const Ptr<Semaphore> waitForSemaphore = nullptr);

      virtual void copy(Ptr<Buffer> source,
                        Ptr<Buffer> destination);

      virtual void copy(Ptr<Buffer> source,
                        Ptr<Buffer> destination,
                        uint64 size,
                        uint64 srcOffset = 0u,
                        uint64 dstOffset = 0u);
         
      virtual void copy(Ptr<Buffer> source,
                        const uint64 srcOffset,
                        Ptr<Texture> texture,
                        const uint32 mipmap,
                        const uint32 layer);

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

      virtual void draw(const uint32       elements,            // Elements to process (they are assembled into Primitives)
                        const Ptr<Buffer>  indexBuffer   = nullptr, // Optional Index buffer
                        const uint32       instances     = 1,   // Instances to draw
                        const uint32       firstElement  = 0,   // First element to process (or index in Index buffer if specified)
                        const sint32       firstVertex   = 0,   // VertexID from which numeration should start (can be negative)
                        const uint32       firstInstance = 0);  // InstanceID from which numeration should start
         
      virtual void draw(const Ptr<Buffer>  indirectBuffer,      // Buffer from which Draw parameters are sourced
                        const uint32       firstEntry   = 0,    // First entry to process in Indirect buffer
                        const Ptr<Buffer>  indexBuffer  = nullptr, // Optional Index buffer
                        const uint32       firstElement = 0);   // First index to process in Index buffer (if specified)


      virtual void endRenderPass(void);

      virtual void barrier(const Ptr<Buffer> buffer, 
                           const uint64 offset,
                           const uint64 size,
                           const BufferAccess currentAccess,
                           const BufferAccess newAccess);

      virtual void barrier(const Ptr<Texture>  _texture, 
                           const TextureAccess currentAccess,
                           const TextureAccess newAccess);

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
