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
      const RenderPassD3D12*      renderPass;   // Render Pass and Framebuffer used for currently encoded Render Pass
      const FramebufferD3D12*     framebuffer;
      bool                  started;
      bool                  encoding;
      bool                  commited;
      
      // State cache

      uint32                bufferStride[MaxInputLayoutAttributesCount];
      uint32                buffersCount;


      // Internal methods

      CommandBufferD3D12(Direct3D12Device* _gpu, ID3D12CommandQueue* _queue, uint32 queueIndex, ID3D12CommandList* _handle);

      bool isCompleted(void);

      // Interface methods

      virtual ~CommandBufferD3D12();

      virtual void start(const Semaphore* waitForSemaphore = nullptr);

      virtual void copy(const Buffer& source,
                        const Buffer& destination);

      virtual void copy(const Buffer& source,
                        const Buffer& destination,
                        const uint64 size,
                        const uint64 srcOffset = 0u,
                        const uint64 dstOffset = 0u);
         
      virtual void copy(const Buffer&  source,
                        const uint64   srcOffset,
                        const uint32   srcRowPitch,
                        const Texture& texture,
                        const uint32   mipmap,
                        const uint32   layer);

      virtual void copyRegion2D(
         const Buffer&  source,
         const uint64   srcOffset,
         const uint32   srcRowPitch,
         const Texture& texture,
         const uint32   mipmap,
         const uint32   layer, // Array layer, face, layer+face
         const uint32v2 origin,
         const uint32v2 region,
         const uint8    plane);
         
      virtual void startRenderPass(const RenderPass& pass, 
                                   const Framebuffer& framebuffer);

      virtual void setDescriptors(const PipelineLayout& layout,
                                  const DescriptorSet& set,
                                  const uint32 index = 0u);

      virtual void setDescriptors(const PipelineLayout& layout, 
                                  const uint32 count,
                                  const std::shared_ptr<DescriptorSet>(&sets)[],
                                  const uint32 firstIndex = 0u);

      virtual void setPipeline(const Pipeline& pipeline);
      



      virtual void setVertexBuffers(const uint32 firstSlot,
                                    const uint32 count,
                                    const std::shared_ptr<Buffer>(&buffers)[],
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
         const uint32 offset = 0u);


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
         const uint32  firstEntry    = 0) const;

      virtual void drawIndirectIndexed(
         const Buffer& indirectBuffer,
         const uint32  firstEntry    = 0,
         const uint32  firstIndex    = 0) const;
         
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
