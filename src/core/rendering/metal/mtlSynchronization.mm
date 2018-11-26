/*

 Ngine v5.0
 
 Module      : Vulkan Synchronization.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/metal/mtlCommandBuffer.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/log/log.h"
#include "core/rendering/metal/mtlDevice.h"
#include "core/rendering/metal/mtlBuffer.h"
#include "core/rendering/metal/mtlTexture.h"
#include "core/rendering/metal/mtlRenderPass.h"

namespace en
{
   namespace gpu
   {
   std::shared_ptr<Semaphore> MetalDevice::createSemaphore(void)
   {
   // TODO: Analyze how Metal synchronizes Command Buffers execution and Swap-Chain
   return std::shared_ptr<Semaphore>(nullptr);
   }

   void CommandBufferMTL::barrier(const Buffer& buffer,
                                  const BufferAccess initAccess)
   {
   // Direct3D12 and Vulkan requires explicit transition barriers
   // specified by the application. Metal handles internal storage
   // management automaticly, thus this is a no-op.
   }

   void CommandBufferMTL::barrier(const Buffer& buffer,
                                  const uint64 offset,
                                  const uint64 size,
                                  const BufferAccess currentAccess,
                                  const BufferAccess newAccess)
   {
   // Direct3D12 and Vulkan requires explicit transition barriers
   // specified by the application. Metal handles internal storage
   // management automaticly, thus this is a no-op.
   }

   void CommandBufferMTL::barrier(const Texture& texture,
                                  const TextureAccess initAccess)
   {
   // Direct3D12 and Vulkan requires explicit transition barriers
   // specified by the application. Metal handles internal storage
   // management automaticly, thus this is a no-op.
   }
      
   void CommandBufferMTL::barrier(const Texture& texture,
                                  const TextureAccess currentAccess,
                                  const TextureAccess newAccess)
   {
   // Direct3D12 and Vulkan requires explicit transition barriers
   // specified by the application. Metal handles internal storage
   // management automaticly, thus this is a no-op.

   // TODO: Check textureBarrier call.
   }

   void CommandBufferMTL::barrier(const Texture& texture,
                                  const uint32v2 mipmaps,
                                  const uint32v2 layers,
                                  const TextureAccess currentAccess,
                                  const TextureAccess newAccess)
   {
   // Direct3D12 and Vulkan requires explicit transition barriers
   // specified by the application. Metal handles internal storage
   // management automaticly, thus this is a no-op.

   // TODO: Check textureBarrier call.
   }

   }
}
#endif
