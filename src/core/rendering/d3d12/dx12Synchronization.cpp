/*

 Ngine v5.0
 
 Module      : D3D12 Synchronization.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12Synchronization.h"

#if defined(EN_MODULE_RENDERER_VULKAN)

#include "core/log/log.h"
#include "core/rendering/d3d12/dx12Device.h"
#include "core/rendering/d3d12/dx12Buffer.h"
#include "core/rendering/d3d12/dx12Texture.h"
#include "core/rendering/d3d12/dx12RenderPass.h"
#include "core/rendering/d3d12/dx12CommandBuffer.h"

namespace en
{
   namespace gpu
   {
   void CommandBufferD3D12::barrier(const Ptr<Buffer> buffer, 
                                    const uint64 offset,
                                    const uint64 size,
                                    const BufferAccess currentAccess,
                                    const BufferAccess newAccess)
   {
   assert( buffer );

   BufferD3D12* ptr = raw_reinterpret_cast<BufferD3D12>(&buffer);

   // TODO: Finish!
   }

   void CommandBufferD3D12::barrier(const Ptr<Texture>  texture, 
                                    const uint32v2      mipmaps, 
                                    const uint32v2      layers,
                                    const TextureAccess currentAccess,
                                    const TextureAccess newAccess) 
   {
   assert( texture );

   TextureD3D12* ptr = raw_reinterpret_cast<TextureD3D12>(&texture);

   // TODO: Finish!
   }

   // SEMAPHORES
   //////////////////////////////////////////////////////////////////////////


   SemaphoreD3D12::SemaphoreD3D12(Direct3D12Device* _gpu) :
      gpu(_gpu),
      handle(nullptr)
   {
   Profile( gpu, CreateFence(0, 
                             D3D12_FENCE_FLAG_SHARED, 
                             IID_PPV_ARGS(&handle)) ) // __uuidof(ID3D12Fence), reinterpret_cast<void**>(&fence)
   }

   SemaphoreD3D12::~SemaphoreD3D12()
   {
   assert( handle );
   ProfileCom( handle->Release() )
   handle = nullptr;
   }

   Ptr<Semaphore> Direct3D12Device::createSemaphore(void)
   {
   return Ptr<Semaphore>(new SemaphoreD3D12(this));
   }

   // Semaphores - can be used to control resource access across multiple queues.
   //              Command Buffers synchronization on the queue.
   // 
   //              D3D12 Fences - used to synchronize queues.
   //              CommandQueue::Signal(ID3D12Fence *pFence, UINT64 Value) - signals execution reaching given fence
   //              CommandQueue::Wait(ID3D12Fence *pFence, UINT64 Value) - waits for given fence to be signaled
   //



   }
}
#endif
