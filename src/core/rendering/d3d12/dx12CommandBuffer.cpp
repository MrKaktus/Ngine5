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
#include "core/rendering/d3d12/dx12RenderPass.h"

namespace en
{
   namespace gpu
   {
   CommandBufferD3D12::CommandBufferD3D12(Direct3D12Device* _gpu, ID3D12CommandQueue* _queue, ID3D12CommandList* _handle) :
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
   
   handle->Release();
   handle = nullptr;
   queue = nullptr;
   gpu = nullptr;
   }

   void CommandBufferD3D12::commit(void)
   {
   assert( started );
   assert( !encoding );
   assert( !commited );
   
   handle->Close();
   
   ID3D12CommandList* lists[] = { handle };

   queue->ExecuteCommandLists(1, lists);
      
   commited = true;
   }
   
   void CommandBufferVK::waitUntilCompleted(void)
   {
   assert( started );
   assert( !encoding );
   assert( commited );

   // Wait maximum 1 second, then assume GPU hang. // TODO: This should be configurable global
   DWORD gpuWatchDog = 1000;
   
   // Alloc unique value for Fence to signal on this thread
   uint32 thread = Scheduler.core();
   UINT64 acquiredValue = AtomicIncrease(gpu->fenceCurrentValue[thread]);



    // https://msdn.microsoft.com/en-us/library/windows/desktop/dn899171(v=vs.85).aspx

// Create signaling Event
gpu->fenceSignalingEvent[thread] = CreateEvent(nullptr, FALSE, FALSE, nullptr);

   queue->Signal(fence, acquiredValue);
   fence->SetEventOnCompletion(acquiredValue, gpu->fenceSignalingEvent[thread]);

// Destroy signaling Event
CloseHandle(gpu->fenceSignalingEvent[thread]);
   
   DWORD result = WaitForSingleObject(gpu->fenceSignalingEvent[thread], gpuWatchDog);
   if (result == WAIT_TIMEOUT)
      {
      }

   }
   
 
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
   if ( SUCCEDED(lastResult[thread]) )
      {
      result = CommandBufferD3D12(this, queue[queueType], handle, type);
      }

   // ID3D12GraphicsCommandList extends ID3D12CommandList
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dn903537(v=vs.85).aspx
   // and alows all types of operations (draws, dispatches & copies)

   return ptr_reinterpret_cast<CommandBuffer>(&result);
   }
   
   }
}
#endif
