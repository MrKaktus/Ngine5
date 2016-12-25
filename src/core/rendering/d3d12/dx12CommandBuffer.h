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

namespace en
{
   namespace gpu
   {
   class CommandBufferD3D12 : public CommandBuffer
      {
      public:
      Direct3D12Device* gpu;     // GPU owning this Command Buffer (for temporary staging buffers creation)
      ID3D12CommandQueue* queue;
      ID3D12CommandList* handle;
      ID3D12Fence*    fence;
      bool            started;
      bool            encoding;
      bool            commited;
      
      virtual void bind(const Ptr<RasterState> raster);
      //virtual void bind(const Ptr<ViewportScissorState> viewportScissor);
      virtual void bind(const Ptr<DepthStencilState> depthStencil);
      virtual void bind(const Ptr<BlendState> blend);
      
      virtual bool startRenderPass(const Ptr<RenderPass> pass, 
                                   const Ptr<Framebuffer> framebuffer);
                                   
      virtual void setPipeline(const Ptr<Pipeline> pipeline);
      



      virtual void setVertexBuffers(const uint32 count, 
                                    const uint32 firstSlot, 
                                    const Ptr<Buffer>* buffers, 
                                    const uint64* offsets = nullptr) const;

      virtual void setVertexBuffer(const uint32 slot, 
                                   const Ptr<Buffer> buffer, 
                                   const uint64 offset = 0u) const;

      virtual void copy(Ptr<Buffer> source,
                        Ptr<Buffer> buffer);
                        
      virtual void copy(Ptr<Buffer> source,
                        Ptr<Texture> texture,
                        const uint32 mipmap,
                        const uint32 layer);
         
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
   
      CommandBufferD3D12(const id<MTLDevice> _device);
      virtual ~CommandBufferMTL();
      };
   }
}
#endif

#endif
