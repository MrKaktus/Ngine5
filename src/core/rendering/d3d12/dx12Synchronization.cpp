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

   }
}
#endif
