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
#include "core/rendering/d3d12/dx12Validate.h"
#include "core/rendering/d3d12/dx12Device.h"
#include "core/rendering/d3d12/dx12Buffer.h"
#include "core/rendering/d3d12/dx12Texture.h"
#include "core/rendering/d3d12/dx12RenderPass.h"
#include "core/rendering/d3d12/dx12CommandBuffer.h"

namespace en
{
namespace gpu
{

// Mapping:
//
// This can be deduced during bind to Indirect draw command. (but then we will have bubble caused by late transition).
//
// IndirectCommand     (R buffer)   VK_ACCESS_INDIRECT_COMMAND_READ_BIT           D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT
//
// Those can be deduced on buffer creation:
//
// IndexBuffer         (R buffer)   VK_ACCESS_INDEX_READ_BIT                      D3D12_RESOURCE_STATE_INDEX_BUFFER
// VertexBuffer        (R buffer)   VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT           D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
// UniformBuffer       (R buffer)   VK_ACCESS_UNIFORM_READ_BIT                    D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
//
// Read                (R Texture)  VK_ACCESS_SHADER_READ_BIT
// Write               (W Texture)  VK_ACCESS_SHADER_WRITE_BIT
// RenderTargetRead    (R Texture)  VK_ACCESS_INPUT_ATTACHMENT_READ_BIT
//
// D3D12 distinguishes difference between Copy/Blit operations and Resolve ones, 
// while Vulkan treats all of them as Transfer operations. On the other hand,
// Vulkan distinguishes difference between copy operations inside VRAM and transfers
// between RAM and VRAM, while Direct3D12 treats them as the same.
//
// TransferSource      (R)          VK_ACCESS_HOST_READ_BIT                       D3D12_RESOURCE_STATE_COPY_SOURCE
// TransferDestination (W)          VK_ACCESS_HOST_WRITE_BIT                      D3D12_RESOURCE_STATE_COPY_DEST
//
// CopySource          (R)          VK_ACCESS_TRANSFER_READ_BIT                   D3D12_RESOURCE_STATE_COPY_SOURCE
// CopyDestination     (W)          VK_ACCESS_TRANSFER_WRITE_BIT                  D3D12_RESOURCE_STATE_COPY_DEST
//                     
// ResolveSource       (R)          VK_ACCESS_TRANSFER_READ_BIT                   D3D12_RESOURCE_STATE_RESOLVE_SOURCE
// ResolveDestination  (W)          VK_ACCESS_TRANSFER_WRITE_BIT                  D3D12_RESOURCE_STATE_RESOLVE_DEST
//
// Should this need to be explicitly state? Or should this pixel/non-pixel flags always declared?
// Texture                          VK_ACCESS_SHADER_READ_BIT                     D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
//                                                                                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
//
// Image               (RW texture) VK_ACCESS_SHADER_READ_BIT                     D3D12_RESOURCE_STATE_UNORDERED_ACCESS
//                                  VK_ACCESS_SHADER_WRITE_BIT
//
// RenderTargetWrite   (W  texture) VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT          D3D12_RESOURCE_STATE_RENDER_TARGET
//
// Those two could be figured out from RenderTargetRead/Write, Format and RenderPass Load/Store operations
// DepthWrite          (W  texture) VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT  D3D12_RESOURCE_STATE_DEPTH_WRITE
// DepthRead           (R  texture) VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT   D3D12_RESOURCE_STATE_DEPTH_READ
//
// Present             (R)          VK_ACCESS_COLOR_ATTACHMENT_READ_BIT           D3D12_RESOURCE_STATE_PRESENT

D3D12_RESOURCE_STATES TranslateBufferAccess(BufferAccess usage, BufferType type)
{
    D3D12_RESOURCE_STATES access = D3D12_RESOURCE_STATE_COMMON;

    bool canBeWritten = false;

    uint32 usageMask = underlyingType(usage);

    if (checkBitmask(usageMask, underlyingType(BufferAccess::Read)))
    {
        // Allows buffer to be read as Vertex Buffer during Draw
        if (type == BufferType::Vertex)
        {
            setBitmask(access, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        }
        else
        // Allows buffer to be read as Index Buffer during Indexed Draw
        if (type == BufferType::Index)
        {
            setBitmask(access, D3D12_RESOURCE_STATE_INDEX_BUFFER);
        }
        else
        // Allows buffer to be read as Uniform Buffer
        if (type == BufferType::Uniform)
        {
            setBitmask(access, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        }
        else
        // Allows buffer to be used as Indirect Command Buffer during Draw and Dispatch Indirect commands
        if (type == BufferType::Indirect)
        {
            setBitmask(access, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
        }

        // Storage - general Buffer read access by Shaders (Common)
    }

    // Allows Writing to Buffers (by Compute Shaders)
    if (checkBitmask(usageMask, underlyingType(BufferAccess::Write)))
    {
        // Is Common enough? There is no D3D12 flag for Buffer Shader Write.
        // setBitmask(access, VK_ACCESS_SHADER_WRITE_BIT);
        canBeWritten = true;
    }

    // Transfer between System Memory and Dedicated Memory
    if (checkBitmask(usageMask, underlyingType(BufferAccess::TransferSource)))
    {
        setBitmask(access, D3D12_RESOURCE_STATE_COPY_SOURCE);
    }

    if (checkBitmask(usageMask, underlyingType(BufferAccess::TransferDestination)))
    {
        setBitmask(access, D3D12_RESOURCE_STATE_COPY_DEST);
    }

    // Copy and Blit operations
    if (checkBitmask(usageMask, underlyingType(BufferAccess::CopySource)))
    {
        setBitmask(access, D3D12_RESOURCE_STATE_COPY_SOURCE);
    }

    if (checkBitmask(usageMask, underlyingType(BufferAccess::CopyDestination)))
    {
        setBitmask(access, D3D12_RESOURCE_STATE_COPY_DEST);
    }

    return access;
}

D3D12_RESOURCE_STATES TranslateTextureAccess(TextureAccess usage, Format format)
{
    D3D12_RESOURCE_STATES access = D3D12_RESOURCE_STATE_COMMON;

    uint32 usageMask = underlyingType(usage);

    // Allows Reading and Sampling Textures by Shaders (if format is Depth-Stencil, allows Depth comparison test read)
    if (checkBitmask(usageMask, underlyingType(TextureAccess::Read)))
    {
        // Generic access by all Shader Stages
        setBitmask(access, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        setBitmask(access, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // TODO: Possible optimization by specifying that texture will be 
        //       accessed only by Fragment Shader, or only by Geometry
        //       Stages (Vertex, Control, Evaluation, Geometry Shaders).

        if ( isDepth(format) ||
             isStencil(format) ||
             isDepthStencil(format) )
        {
            setBitmask(access, D3D12_RESOURCE_STATE_DEPTH_READ);
        }
    }

    // Allows Writing to Textures (by Compute Shaders)
    if (checkBitmask(usageMask, underlyingType(TextureAccess::Write)))
    {
        // Generic access by all Shader Stages
        setBitmask(access, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        setBitmask(access, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // TODO: Possible optimization by specifying that texture will be 
        //       accessed only by Fragment Shader, or only by Geometry
        //       Stages (Vertex, Control, Evaluation, Geometry Shaders).
    }

    // Texture can be one of Color, Depth, Stencil read sources during rendering operations
    if (checkBitmask(usageMask, underlyingType(TextureAccess::RenderTargetRead)))
    {
        if ( isDepth(format) ||
             isStencil(format) ||
             isDepthStencil(format) )                            // Read via HiZ, Early-Z and Depth Test
        {
            setBitmask(access, D3D12_RESOURCE_STATE_DEPTH_READ);
        }
    }

    // Texture can be one of Color, Depth, Stencil destination for Rendering operations
    if (checkBitmask(usageMask, underlyingType(TextureAccess::RenderTargetWrite)))
    {
        if ( isDepth(format) ||
             isStencil(format) ||
             isDepthStencil(format) )                            // Written via Depth Write, Depth Clear
        {
            setBitmask(access, D3D12_RESOURCE_STATE_DEPTH_WRITE); // Overrides Read-Only Depth-Stencil mode (if PSO enables Depth writes)
        }
        else
        {
            setBitmask(access, D3D12_RESOURCE_STATE_RENDER_TARGET);
        }
    }

    // Any kind of transfer or present operation overrides previously selected layout

    // Resolving MSAA surfaces
    if (checkBitmask(usageMask, underlyingType(TextureAccess::ResolveSource)))
    {
        setBitmask(access, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
    }

    if (checkBitmask(usageMask, underlyingType(TextureAccess::ResolveDestination)))
    {
        setBitmask(access, D3D12_RESOURCE_STATE_RESOLVE_DEST);
    }

    // Transfer between System Memory and Dedicated Memory
    if (checkBitmask(usageMask, underlyingType(TextureAccess::TransferSource)))
    {
        setBitmask(access, D3D12_RESOURCE_STATE_COPY_SOURCE);
    }

    if (checkBitmask(usageMask, underlyingType(TextureAccess::TransferDestination)))
    {
        setBitmask(access, D3D12_RESOURCE_STATE_COPY_DEST);
    }

    // Copy and Blit operations
    if (checkBitmask(usageMask, underlyingType(TextureAccess::CopySource)))
    {
        setBitmask(access, D3D12_RESOURCE_STATE_COPY_SOURCE);
    }

    if (checkBitmask(usageMask, underlyingType(TextureAccess::CopyDestination)))
    {
        setBitmask(access, D3D12_RESOURCE_STATE_COPY_DEST);
    }

    // Present
    if (checkBitmask(usageMask, underlyingType(TextureAccess::Present)))
    {
        access = D3D12_RESOURCE_STATE_PRESENT;
    }

    return access;
}

void CommandBufferD3D12::barrier(const Buffer& _buffer,
                                 const BufferAccess initAccess)
{
    const BufferD3D12& buffer = reinterpret_cast<const BufferD3D12&>(_buffer);

    // TODO: Ensure this Command Buffer is Graphic one !
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    MemoryUsage heapUsage = buffer.heap._usage;

    // If buffer was created on Upload or Immediate Heap,
    // it's initial state is General, and it needs to 
    // stay in that access mode.
    if (heapUsage == MemoryUsage::Upload ||
        heapUsage == MemoryUsage::Immediate)
    {
        return;
    }

    D3D12_RESOURCE_STATES dstState =TranslateBufferAccess(initAccess, buffer.apiType);

    // If app tries to init Buffer as TransferDestination,
    // and it already is in this state due to D3D12 resource
    // creation requirements, barrier can be skipped.
    if ( (heapUsage == MemoryUsage::Download ||
          heapUsage == MemoryUsage::Linear) &&
         dstState == D3D12_RESOURCE_STATE_COPY_DEST )
    {
        return;
    }

    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                 // D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY - marks place in command buffer where transition begins
                 // D3D12_RESOURCE_BARRIER_FLAG_END_ONLY   - marks place in command buffer where transition ends  
    barrier.Transition.pResource   = buffer.handle;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter  = dstState;

    ValidateComNoRet( command->ResourceBarrier(1, &barrier) )
}

void CommandBufferD3D12::barrier(const Buffer& _buffer,
                                 const uint64 offset,
                                 const uint64 size,
                                 const BufferAccess currentAccess,
                                 const BufferAccess newAccess)
{
    const BufferD3D12& buffer = reinterpret_cast<const BufferD3D12&>(_buffer);

    MemoryUsage heapUsage = buffer.heap._usage;

    // If buffer was created on Upload or Immediate Heap,
    // it's initial state is General, and it needs to 
    // stay in that access mode.
    if (heapUsage == MemoryUsage::Upload ||
        heapUsage == MemoryUsage::Immediate)
    {
        return;
    }

    // TODO: Ensure this Command Buffer is Graphic one !
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                 // D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY - marks place in command buffer where transition begins
                 // D3D12_RESOURCE_BARRIER_FLAG_END_ONLY   - marks place in command buffer where transition ends  
    barrier.Transition.pResource   = buffer.handle;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = TranslateBufferAccess(currentAccess, buffer.apiType);
    barrier.Transition.StateAfter  = TranslateBufferAccess(newAccess, buffer.apiType);
   
    ValidateComNoRet( command->ResourceBarrier(1, &barrier) )
}

// Vulkan tries to hide barrier latency by specifying after completion of which pipeline stage it should start, and before which it should end.
// At the same time D3D12 is trying to do the same, by allowing split-barriers, where app manuall places begining and ending barrrier events on CommandBuffer.
// (see: http://www.gamedev.net/topic/676655-how-could-we-benefit-from-using-split-barriers/ )

void CommandBufferD3D12::barrier(const Texture& _texture,
                                 const TextureAccess initAccess) 
{
    const TextureD3D12& texture = reinterpret_cast<const TextureD3D12&>(_texture);

    // TODO: Ensure this Command Buffer is Graphic one !
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                 // D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY - marks place in command buffer where transition begins
                 // D3D12_RESOURCE_BARRIER_FLAG_END_ONLY   - marks place in command buffer where transition ends  
    barrier.Transition.pResource   = texture.handle;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter  = TranslateTextureAccess(initAccess, texture.state.format);

    ValidateComNoRet( command->ResourceBarrier(1, &barrier) )
}

void CommandBufferD3D12::barrier(const Texture& _texture,
                                 const TextureAccess currentAccess,
                                 const TextureAccess newAccess) 
{
    const TextureD3D12& texture = reinterpret_cast<const TextureD3D12&>(_texture);

    // TODO: Ensure this Command Buffer is Graphic one !
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                 // D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY - marks place in command buffer where transition begins
                 // D3D12_RESOURCE_BARRIER_FLAG_END_ONLY   - marks place in command buffer where transition ends  
    barrier.Transition.pResource   = texture.handle;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = TranslateTextureAccess(currentAccess, texture.state.format);
    barrier.Transition.StateAfter  = TranslateTextureAccess(newAccess, texture.state.format);
   
    ValidateComNoRet( command->ResourceBarrier(1, &barrier) )
}

void CommandBufferD3D12::barrier(const Texture& _texture,
                                 const uint32v2 mipmaps,
                                 const uint32v2 layers,
                                 const TextureAccess currentAccess,
                                 const TextureAccess newAccess) 
{
    const TextureD3D12& texture = reinterpret_cast<const TextureD3D12&>(_texture);

    // TODO: Ensure this Command Buffer is Graphic one !
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    // Each surface needs to be described separately if whole resource is not transitioning
    uint32 barriersCount = mipmaps.count * layers.count;

    if (barriersCount > 1)
    {
        D3D12_RESOURCE_BARRIER* barrier = new D3D12_RESOURCE_BARRIER[barriersCount];
        for(uint32 layer=0; layer<layers.count; ++layer)
        {
            for(uint32 mipmap=0; mipmap<mipmaps.count; ++mipmap)
            {
                uint32 index = layer * mipmaps.count + mipmap;
      
                UINT subresource = D3D12CalcSubresource(mipmaps.base + mipmap,
                                                        layers.base  + layer,
                                                        0u,                     // Plane Slice - DepthStencil formats have two slices
                                                        texture.state.mipmaps,
                                                        texture.state.layers);  // D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES for whole texture
      
                barrier[index].Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier[index].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                                    // D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY - marks place in command buffer where transition begins
                                    // D3D12_RESOURCE_BARRIER_FLAG_END_ONLY   - marks place in command buffer where transition ends
                barrier[index].Transition.pResource   = texture.handle;
                barrier[index].Transition.Subresource = subresource;
                barrier[index].Transition.StateBefore = TranslateTextureAccess(currentAccess, texture.state.format);
                barrier[index].Transition.StateAfter  = TranslateTextureAccess(newAccess, texture.state.format);
            }
        }

        ValidateComNoRet( command->ResourceBarrier(barriersCount, barrier) )

        delete [] barrier;
    }
    else
    {
        UINT subresource = D3D12CalcSubresource(mipmaps.base,
                                                layers.base,
                                                0u,                     // Plane Slice - DepthStencil formats have two slices
                                                texture.state.mipmaps,
                                                texture.state.layers);  // D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES for whole texture
      
        D3D12_RESOURCE_BARRIER barrier;
        barrier.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                     // D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY - marks place in command buffer where transition begins
                     // D3D12_RESOURCE_BARRIER_FLAG_END_ONLY   - marks place in command buffer where transition ends  
        barrier.Transition.pResource   = texture.handle;
        barrier.Transition.Subresource = subresource;
        barrier.Transition.StateBefore = TranslateTextureAccess(currentAccess, texture.state.format);
        barrier.Transition.StateAfter  = TranslateTextureAccess(newAccess, texture.state.format);
      
        ValidateComNoRet( command->ResourceBarrier(1, &barrier) )
    }
}

// Ensures that operations on "before" resource are finished before operations of "after" resource starts.
// (as both resources are sharing the same memory, so it prevens concurrent work and data corruption).
//
// barrier.Type                     = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
// barrier.Aliasing.pResourceBefore = ; // TODO: ID3D12Resource *
// barrier.Aliasing.pResourceAfter  = ; // TODO: ID3D12Resource *

// UAV (r/w) accessess barrier, previous operations need to finish before next ones
// (prevents concurrent execution, and data corruption)
//
// barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
// barrier.UAV.pResource = texture->handle;


// SEMAPHORES
//////////////////////////////////////////////////////////////////////////


SemaphoreD3D12::SemaphoreD3D12(Direct3D12Device* _gpu) :
    gpu(_gpu),
    fence(nullptr),
    waitForValue(0)
{
}

SemaphoreD3D12::~SemaphoreD3D12()
{
    // Fences are owned by device queues, and released on device destruction
    fence = nullptr;
}

std::shared_ptr<Semaphore> Direct3D12Device::createSemaphore(void)
{
    return std::make_shared<SemaphoreD3D12>(this);
}

} // en::gpu
} // en

#endif
