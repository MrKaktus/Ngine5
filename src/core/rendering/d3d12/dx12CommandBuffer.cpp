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

#include "core/rendering/d3d12/dx12CommandBuffer.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/log/log.h"
#include "core/rendering/d3d12/dx12Device.h"
#include "core/rendering/d3d12/dx12Buffer.h"
#include "core/rendering/d3d12/dx12Texture.h"
#include "core/rendering/d3d12/dx12Pipeline.h"
#include "core/rendering/d3d12/dx12RenderPass.h"

namespace en
{
   namespace gpu
   {
   CommandBufferD3D12::CommandBufferD3D12(Direct3D12Device* _gpu, 
                                          ID3D12CommandQueue* _queue, 
                                          ID3D12CommandList* _handle) :
      gpu(_gpu),
      queue(_queue),
      handle(_handle),
      started(false),
      encoding(false),
      commited(false),
      CommandBuffer()
   {
   // TODO: Finish!
   }

   CommandBufferD3D12::~CommandBufferD3D12()
   {
   // Finish encoded tasks
   if (!commited)
      commit();

   // TODO: Finish!
   
   ProfileCom( handle->Release() )
   handle = nullptr;
   queue = nullptr;
   gpu = nullptr;
   }


   // RENDER PASS
   //////////////////////////////////////////////////////////////////////////
   

   void CommandBufferD3D12::start(const Ptr<Semaphore> waitForSemaphore)
   {
   assert( !started );

   // TODO: Finish !
   }

   void CommandBufferD3D12::startRenderPass(const Ptr<RenderPass> pass, const Ptr<Framebuffer> _framebuffer)
   {
   assert( started );
   assert( !encoding );
    
   assert( pass );
   assert( _framebuffer );
   
   RenderPassD3D12*  renderPass  = raw_reinterpret_cast<RenderPassD3D12>(&pass);
   FramebufferD3D12* framebuffer = raw_reinterpret_cast<FramebufferD3D12>(&_framebuffer);

   // Descriptor for empty Color Attachment output
   D3D12_RENDER_TARGET_VIEW_DESC nullDesc;

   // TODO: Do we need to set Format, or view Dimmension ?

   // Create new Views that bind resources to Descriptors on the fly
   uint32 lastUsedIndex = 0;
   uint32 index = 0;
   for(uint32 i=0; i<8; ++i)
      if (checkBit(renderPass->usedAttachments, i))
         {
         lastUsedIndex = i;
         ProfileNoRet( gpu, CreateRenderTargetView(framebuffer->colorHandle[i]->texture->handle,
                                                  &framebuffer->colorDesc[i],
                                                   gpu->handleRTV[i]) )
         ++index;
         }
      else
         {
         // Clear binding. To ensure proper shader behavior (reading 0's, writes are discarded)
         ProfileNoRet( gpu, CreateRenderTargetView(nullptr,
                                                  &nullDesc,
                                                   gpu->handleRTV[i]) )
         }
   
   // Create Depth-Stencil View
   if (renderPass->depthStencil)
      ProfileNoRet( gpu, CreateDepthStencilView(framebuffer->depthHandle->texture->handle,
                                               &framebuffer->depthDesc,
                                                gpu->handleDSV) )
   
   // RTV and DSV handles could be sourced from Framebuffer (if preallocated).
   
   // TODO: Ensure this Command Buffer is Graphic one !
   ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

   // Clear Color Attachments
   // Engine currently doesn't support clearing sub-areas of render targets
   // (there are some uses for it like cinematic mode, but not all API's support it).
   for(uint32 i=0; i<=lastUsedIndex; ++i)
      if (checkBit(renderPass->usedAttachments, i))
         if (renderPass->colorState[i].loadOp == LoadOperation::Clear)
            ProfileComNoRet( command->ClearRenderTargetView(gpu->handleRTV[i],
                                                     reinterpret_cast<const FLOAT*>(&renderPass->colorState[i].clearValue),
                                                     0, nullptr) )
      
   // Clear Depth-Stencil Attachment
   if (renderPass->depthStencil)
      if (renderPass->depthState.clearFlags > 0)
         ProfileComNoRet( command->ClearDepthStencilView(gpu->handleDSV,
                                                         renderPass->depthState.clearFlags,
                                                         renderPass->depthState.clearDepth,
                                                         renderPass->depthState.clearStencil,
                                                         0, nullptr) )

   // Always fill up all consecutive descriptors, from first one to
   // the last one that is used, thus TRUE can be passed in third parameter.
   ProfileComNoRet( command->OMSetRenderTargets((lastUsedIndex + 1), gpu->handleRTV, TRUE, &gpu->handleDSV) )
   }
   

   // TRANSFER COMMANDS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferD3D12::copy(Ptr<Buffer> source,
                                 Ptr<Buffer> destination)
   {
   assert( started );
   assert( source );
   assert( destination );

   copy(source, destination, source->length());
   }

   void CommandBufferD3D12::copy(Ptr<Buffer> source,
                                 Ptr<Buffer> destination,
                                 uint64 size,
                                 uint64 srcOffset,
                                 uint64 dstOffset)
   {
   assert( started );
   assert( source );
   assert( destination );

   BufferD3D12* src = raw_reinterpret_cast<BufferD3D12>(&source);
   BufferD3D12* dst = raw_reinterpret_cast<BufferD3D12>(&destination);

   // TODO: Finish!
   }
         
   void CommandBufferD3D12::copy(Ptr<Buffer> transfer, 
                                 const uint64 srcOffset, 
                                 Ptr<Texture> texture, 
                                 const uint32 mipmap, 
                                 const uint32 layer)
   {
   assert( started );
   assert( transfer );
   assert( transfer->type() == BufferType::Transfer );
   assert( texture );
   assert( texture->mipmaps() > mipmap );
   assert( texture->layers() > layer );
   
   BufferD3D12*  source      = raw_reinterpret_cast<BufferD3D12>(&transfer);
   TextureD3D12* destination = raw_reinterpret_cast<TextureD3D12>(&texture);

   assert( source->size >= destination->size(mipmap) );
   
   // TODO: Finish ! 
   }

   void CommandBufferD3D12::copy(Ptr<Buffer> transfer,
                                 Ptr<Texture> texture,
                                 const uint32 mipmap,
                                 const uint32 layer)
   {
   copy(transfer, 0u, texture, mipmap, layer);
   }


   // PIPELINE COMMANDS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferD3D12::setPipeline(const Ptr<Pipeline> pipeline)
   {
   assert( started );
   assert( pipeline );

   PipelineD3D12* ptr = raw_reinterpret_cast<PipelineD3D12>(&pipeline);

   // TODO: Finish !
   }


   // DRAW COMMANDS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferD3D12::draw(const DrawableType primitiveType,
                                 const uint32       elements,
                                 const Ptr<Buffer>  indexBuffer,
                                 const uint32       instances,
                                 const uint32       firstElement,
                                 const sint32       firstVertex,
                                 const uint32       firstInstance)
   {
   assert( started );
   assert( encoding );

   // TODO: Finish!
   }

   void CommandBufferD3D12::draw(const DrawableType primitiveType,
                                 const Ptr<Buffer>  indirectBuffer,
                                 const uint32       firstEntry,
                                 const Ptr<Buffer>  indexBuffer,
                                 const uint32       firstElement)
   {
   assert( started );
   assert( encoding );
   assert( indirectBuffer );
   
   BufferD3D12* indirect = raw_reinterpret_cast<BufferD3D12>(&indirectBuffer);
   assert( indirect->apiType == BufferType::Indirect );
   
   // TODO: Finish!
   }


   // FINISHING
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferD3D12::commit(const Ptr<Semaphore> signalSemaphore)
   {
   assert( started );
   assert( !encoding );
   assert( !commited );
   
   // TODO: Check if graphics or compute!  
   reinterpret_cast<ID3D12GraphicsCommandList*>(handle)->Close();
   
   ID3D12CommandList* lists[] = { handle };

   queue->ExecuteCommandLists(1, lists);
      
   if (signalSemaphore)
      {
      // TODO: Is there a way to sync on GPU side CB execution with Swap-Chain ?
      }

   commited = true;
   }
   
//   void CommandBufferVK::waitUntilCompleted(void)
//   {
//   assert( started );
//   assert( !encoding );
//   assert( commited );
//
//   // Wait maximum 1 second, then assume GPU hang. // TODO: This should be configurable global
//   DWORD gpuWatchDog = 1000;
//   
//   // Alloc unique value for Fence to signal on this thread
//   uint32 thread = Scheduler.core();
//   UINT64 acquiredValue = AtomicIncrease(gpu->fenceCurrentValue[thread]);
//
//
//
//    // https://msdn.microsoft.com/en-us/library/windows/desktop/dn899171(v=vs.85).aspx
//
//// Create signaling Event
//gpu->fenceSignalingEvent[thread] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
//
//   queue->Signal(fence, acquiredValue);
//   fence->SetEventOnCompletion(acquiredValue, gpu->fenceSignalingEvent[thread]);
//
//// Destroy signaling Event
//CloseHandle(gpu->fenceSignalingEvent[thread]);
//   
//   DWORD result = WaitForSingleObject(gpu->fenceSignalingEvent[thread], gpuWatchDog);
//   if (result == WAIT_TIMEOUT)
//      {
//      }
//
//   }
   

 
   // DEVICE
   //////////////////////////////////////////////////////////////////////////


   Ptr<CommandBuffer> Direct3D12Device::createCommandBuffer(const QueueType type, const uint32 parentQueue)
   {
   // Each thread creates it's Command Buffers from separate Command Allocator
   uint32 thread    = Scheduler.core();
   uint32 queueType = underlyingType(type);
   
   assert( queuesCount[queueType] > parentQueue );
   
   Ptr<CommandBufferD3D12> result = nullptr;

   ID3D12CommandList* handle;
   
   Profile( this, CreateCommandList(0u,      /* No Multi-GPU support for now */
                                    TranslateQueueType[queueType],
                                    commandAllocator[thread][queueType],
                                    nullptr, /* No initial PipelineState for now */
                                    IID_PPV_ARGS(&handle)) )
   if ( SUCCEEDED(lastResult[thread]) )
      {
      result = new CommandBufferD3D12(this, queue[queueType], handle);
      }

   // ID3D12GraphicsCommandList extends ID3D12CommandList
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dn903537(v=vs.85).aspx
   // and alows all types of operations (draws, dispatches & copies)

   return ptr_reinterpret_cast<CommandBuffer>(&result);
   }
   
   }
}
#endif
