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
#include "core/rendering/d3d12/dx12Validate.h"
#include "core/rendering/d3d12/dx12Device.h"
#include "core/rendering/d3d12/dx12Buffer.h"
#include "core/rendering/d3d12/dx12Texture.h"
#include "core/rendering/d3d12/dx12Pipeline.h"
#include "utilities/strings.h"
#include "parallel/scheduler.h"

namespace en
{
namespace gpu
{

CommandBufferD3D12::CommandBufferD3D12(
        Direct3D12Device* _gpu, 
        ID3D12CommandQueue* _queue, 
        uint32              _queueIndex,
        ID3D12CommandList* _handle) :
    gpu(_gpu),
    queue(_queue),
    queueIndex(_queueIndex),
    handle(_handle),
    fence(nullptr),
    waitForValue(0),
    commitValue(0),
    renderPass(nullptr),
    framebuffer(nullptr),
    started(false),
    encoding(false),
    commited(false),
    buffersCount(0),
    CommandBuffer()
{
    for(uint32 i=0; i<MaxInputLayoutAttributesCount; ++i)
    {
        bufferStride[i] = 0;
    }
}

CommandBufferD3D12::~CommandBufferD3D12()
{
    // RenderPass needs to be finished before Command Buffer is destroyed
    assert( !encoding );

    // Finish encoded tasks
    if (!commited)
    {
        commit();
    }

    // Release Command Buffer
    assert( handle );
    ValidateCom( handle->Release() )
    handle = nullptr;
    queue  = nullptr;
    gpu    = nullptr;
}


// RENDER PASS
//////////////////////////////////////////////////////////////////////////


void CommandBufferD3D12::start(const Semaphore* waitForSemaphore)
{
    assert( !started );

    // Remember fence and it's value, for which this CommandBuffer execution should wait for.
    if (waitForSemaphore)
    {
        const SemaphoreD3D12* semaphore = reinterpret_cast<const SemaphoreD3D12*>(waitForSemaphore);
        fence        = semaphore->fence;
        waitForValue = semaphore->waitForValue;
    }

    started = true;
}

void CommandBufferD3D12::startRenderPass(const RenderPass& pass, const Framebuffer& _framebuffer)
{
    assert( started );
    assert( !encoding );
 
    renderPass  = reinterpret_cast<const RenderPassD3D12*>(&pass);
    framebuffer = reinterpret_cast<const FramebufferD3D12*>(&_framebuffer);

    // Descriptor for empty Color Attachment output
    D3D12_RENDER_TARGET_VIEW_DESC nullColorDesc;
    nullColorDesc.Format               = DXGI_FORMAT_B8G8R8A8_UNORM;
    nullColorDesc.ViewDimension        = D3D12_RTV_DIMENSION_TEXTURE2D;
    nullColorDesc.Texture2D.MipSlice   = 0u;
    nullColorDesc.Texture2D.PlaneSlice = 0u;

    // Descriptor for empty Depth-Sterncil Attachment output
    D3D12_DEPTH_STENCIL_VIEW_DESC nullDepthDesc;
    nullDepthDesc.Format               = DXGI_FORMAT_D32_FLOAT;
    nullDepthDesc.ViewDimension        = D3D12_DSV_DIMENSION_TEXTURE2D;
    nullDepthDesc.Flags                = D3D12_DSV_FLAG_NONE;
    nullDepthDesc.Texture2D.MipSlice   = 0u;

    // TODO: Should View dimmension be inherited from other RTV?

    // Create new Views that bind resources to Descriptors on the fly
    uint32 lastUsedIndex = 0;
    uint32 index = 0;
    for(uint32 i=0; i<8; ++i)
    {
        if (checkBit(renderPass->usedAttachments, i))
        {
            lastUsedIndex = i;
            ValidateNoRet( gpu, CreateRenderTargetView(framebuffer->colorHandle[i]->texture.handle,
                                                      &framebuffer->colorDesc[i],
                                                       gpu->handleRTV[i]) )
            ++index;
        }
        else
        {
            // Clear binding. To ensure proper shader behavior (reading 0's, writes are discarded)
            ValidateNoRet( gpu, CreateRenderTargetView(nullptr,
                                                      &nullColorDesc,
                                                       gpu->handleRTV[i]) )
        }
    }

    // Create Depth-Stencil View
    if (renderPass->depthStencil)
    {
        ValidateNoRet( gpu, CreateDepthStencilView(framebuffer->depthHandle->texture.handle,
                                                  &framebuffer->depthDesc,
                                                   gpu->handleDSV) )
    }
    else
    {
        ValidateNoRet( gpu, CreateDepthStencilView(nullptr,
                                                  &nullDepthDesc,
                                                   gpu->handleDSV) )
    }

    // RTV and DSV handles could be sourced from Framebuffer (if preallocated).

    // TODO: Ensure this Command Buffer is Graphic one !
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    // Clear Color Attachments
    // Engine currently doesn't support clearing sub-areas of render targets
    // (there are some uses for it like cinematic mode, but not all API's support it).
    for(uint32 i=0; i<=lastUsedIndex; ++i)
    {
        if (checkBit(renderPass->usedAttachments, i))
        {
            if (renderPass->colorState[i].loadOp == LoadOperation::Clear)
            {
                ValidateComNoRet( command->ClearRenderTargetView(gpu->handleRTV[i],
                                                                 reinterpret_cast<const FLOAT*>(&renderPass->colorState[i].clearValue),
                                                                 0, nullptr) )
            }
        }
    }

    // Clear Depth-Stencil Attachment
    if (renderPass->depthStencil)
    {
        if (renderPass->depthState.clearFlags > 0)
        {
            ValidateComNoRet( command->ClearDepthStencilView(gpu->handleDSV,
                                                             renderPass->depthState.clearFlags,
                                                             renderPass->depthState.clearDepth,
                                                             renderPass->depthState.clearStencil,
                                                             0, nullptr) )
        }
    }

    // Always fill up all consecutive descriptors, from first one to
    // the last one that is used, thus TRUE can be passed in third parameter.
    ValidateComNoRet( command->OMSetRenderTargets((lastUsedIndex + 1), gpu->handleRTV, TRUE, &gpu->handleDSV) )

    encoding = true;
}

void CommandBufferD3D12::endRenderPass(void)
{
    assert( started );
    assert( encoding );

    // Direct3D12 has no notion of Render Passes. Thus if there are resolve
    // targets assigned, their resolve needs to be handled here.
    if (renderPass->resolve)
    {
        // TODO: Ensure this Command Buffer is Graphic one !
        ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

        // Optional Color Attachment's resolve
        for(uint32 i=0; i<8; ++i)
        {
            if (checkBit(renderPass->usedAttachments, i))
            {
                if (renderPass->colorState[i].storeOp == StoreOperation::Resolve ||
                    renderPass->colorState[i].storeOp == StoreOperation::StoreAndResolve)
                {
                    assert( framebuffer->resolveHandle[i] );

                    // MSAA source and resolve destination View's need to have the same texel format
                    assert( framebuffer->colorHandle[i]->desc.Format == framebuffer->resolveHandle[i]->desc.Format );

                    // MSAA source and resolve destination View's need to have the same amount of mipmaps and layers
                    assert( framebuffer->colorHandle[i]->mipmaps.count == framebuffer->resolveHandle[i]->mipmaps.count );
                    assert( framebuffer->colorHandle[i]->layers.count  == framebuffer->resolveHandle[i]->layers.count  );

                    // Amount of mipmaps and layers to resolve
                    UINT mipmaps = framebuffer->colorHandle[i]->mipmaps.count;
                    UINT layers  = framebuffer->colorHandle[i]->layers.count;

                    DXGI_FORMAT format = framebuffer->colorHandle[i]->desc.Format;

                    // Resolve source (MSAA)
                    UINT srcSubresource = D3D12CalcSubresource(framebuffer->colorHandle[i]->mipmaps.base,
                                                               framebuffer->colorHandle[i]->layers.base,
                                                               0u,                      // Plane Slice - DepthStencil formats have two slices
                                                               mipmaps,
                                                               layers); 

                    // Resolve destination
                    UINT dstSubresource = D3D12CalcSubresource(framebuffer->resolveHandle[i]->mipmaps.base,
                                                               framebuffer->resolveHandle[i]->layers.base,
                                                               0u,                      // Plane Slice - DepthStencil formats have two slices
                                                               mipmaps,
                                                               layers); 

                    // Resolves original resources (on top of which Views for rendering were created)
                    ValidateComNoRet( command->ResolveSubresource(framebuffer->resolveHandle[i]->texture.handle,
                                                                  dstSubresource,
                                                                  framebuffer->colorHandle[i]->texture.handle,
                                                                  srcSubresource,
                                                                  format) )
                }
            }
        }

        // D3D12 doesn't support Depth, nor Stencil resolves
    }

    // Release references
    renderPass  = nullptr;
    framebuffer = nullptr;

    encoding = false;
}


// SETTING INPUT ASSEMBLER VERTEX BUFFERS
//////////////////////////////////////////////////////////////////////////


void CommandBufferD3D12::setVertexBuffers(const uint32 firstSlot,
                                          const uint32 count,
                                          const std::shared_ptr<Buffer>(&buffers)[],
                                          const uint64* offsets) const
{
    assert( started );
    assert( count );
    assert( (firstSlot + count) <= gpu->support.maxInputLayoutBuffersCount );

    // TODO: Ensure this Command Buffer is Graphic one !
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    // Extract Direct3D12 buffer handles
    D3D12_VERTEX_BUFFER_VIEW* views = new D3D12_VERTEX_BUFFER_VIEW[count]; // TODO: Optimize by allocating on the stack maxBuffersCount sized fixed array.
    for(uint32 i=0; i<count; ++i)
    {
        assert( buffers[i] );
        BufferD3D12* bufferD3D12 = reinterpret_cast<BufferD3D12*>(buffers[i].get());
        ID3D12Resource* resource = bufferD3D12->handle;
      
        assert( bufferStride[firstSlot + i] == bufferD3D12->formatting.elementSize() );

        // Populate Buffer View
        uint32 currentOffset = offsets ? offsets[i] : 0;
        views[i].BufferLocation = resource->GetGPUVirtualAddress() + currentOffset; // Final location is Base Buffer Adress + current Offset
        views[i].SizeInBytes    = buffers[i]->length() - currentOffset;             // Cannot bind more data than there is in buffer
        views[i].StrideInBytes  = bufferStride[firstSlot + i];
    }

    ValidateComNoRet( command->IASetVertexBuffers(firstSlot, count, views) )

    delete [] views;
}

void CommandBufferD3D12::setInputBuffer(const uint32  firstSlot,
                                        const uint32  slots,
                                        const Buffer& _buffer,
                                        const uint64* offsets) const
{
    assert( started );
    assert( slots );
    assert( (firstSlot + slots) <= gpu->support.maxInputLayoutBuffersCount );

    const BufferD3D12& buffer = reinterpret_cast<const BufferD3D12&>(_buffer);
    ID3D12Resource* resource = buffer.handle;

    // TODO: Ensure this Command Buffer is Graphic one !
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    // Extract Direct3D12 buffer handles
    D3D12_VERTEX_BUFFER_VIEW* views = new D3D12_VERTEX_BUFFER_VIEW[slots]; // TODO: Optimize by allocating on the stack maxBuffersCount sized fixed array.
    for(uint32 i=0; i<slots; ++i)
    {
        assert( bufferStride[firstSlot + i] == buffer.formatting.elementSize() ); // TODO: remove when removing formatting from buffers

        // Populate Buffer View
        uint32 currentOffset = offsets ? offsets[i] : 0;
        views[i].BufferLocation = resource->GetGPUVirtualAddress() + currentOffset; // Final location is Base Buffer Adress + current Offset
        views[i].SizeInBytes    = buffer.length() - currentOffset;                  // Cannot bind more data than there is in buffer
        views[i].StrideInBytes  = bufferStride[firstSlot + i];
    }

    ValidateComNoRet( command->IASetVertexBuffers(firstSlot, slots, views) )

    delete [] views;
}

void CommandBufferD3D12::setVertexBuffer(const uint32 slot,
                                         const Buffer& _buffer,
                                         const uint64 offset) const
{
    assert( started );
    assert( slot < gpu->support.maxInputLayoutBuffersCount );

    const BufferD3D12& buffer = reinterpret_cast<const BufferD3D12&>(_buffer);
    ID3D12Resource* resource = buffer.handle;

    assert( bufferStride[slot] == buffer.formatting.elementSize() );
      
    // Populate Buffer View
    D3D12_VERTEX_BUFFER_VIEW view;
    view.BufferLocation = resource->GetGPUVirtualAddress() + offset; // Final location is Base Buffer Adress + current Offset
    view.SizeInBytes    = buffer.length() - offset;                  // Cannot bind more data than there is in buffer
    view.StrideInBytes  = bufferStride[slot];

    // TODO: Ensure this Command Buffer is Graphic one !
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    ValidateComNoRet( command->IASetVertexBuffers(slot, 1, &view) )
}

void CommandBufferD3D12::setIndexBuffer(const Buffer& _buffer,
                                        const Attribute type,
                                        const uint32 offset)
{
    assert( started );
    assert( encoding );
    assert( type == Attribute::u16 ||
            type == Attribute::u32 );

    // Elements are assembled into Primitives.
    const BufferD3D12& buffer = reinterpret_cast<const BufferD3D12&>(_buffer);
    ID3D12Resource* resource = buffer.handle;

    assert( buffer.apiType == BufferType::Index );

    D3D12_INDEX_BUFFER_VIEW desc;
    desc.BufferLocation = resource->GetGPUVirtualAddress() + offset; // Final offset In Index Buffer is calculated at draw call.
    desc.SizeInBytes    = buffer.length() - offset;                  // Cannot bind more data than there is in buffer
    desc.Format         = DXGI_FORMAT_R16_UINT;
    if (type == Attribute::u32)
    {
        desc.Format     = DXGI_FORMAT_R32_UINT;
    }

    // TODO: Ensure this Command Buffer is Graphic one !
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    ValidateComNoRet( command->IASetIndexBuffer(&desc) );
}


// TRANSFER COMMANDS
//////////////////////////////////////////////////////////////////////////


void CommandBufferD3D12::copy(const Buffer& source,
                              const Buffer& destination)
{
    assert( started );
    assert( source.length() <= destination.length() );

    copy(source, destination, source.length());
}

void CommandBufferD3D12::copy(const Buffer& source,
                              const Buffer& destination,
                              const uint64 size,
                              const uint64 srcOffset,
                              const uint64 dstOffset)
{
    assert( started );
    assert( source.type() == BufferType::Transfer );
    assert( (srcOffset + size) <= source.length() );
    assert( (dstOffset + size) <= destination.length() );
   
    const BufferD3D12& src = reinterpret_cast<const BufferD3D12&>(source);
    const BufferD3D12& dst = reinterpret_cast<const BufferD3D12&>(destination);

    // TODO: Check if graphics or compute!  
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    ValidateComNoRet( command->CopyBufferRegion(dst.handle,
                                                dstOffset,
                                                src.handle,
                                                srcOffset,
                                                size) )
}
         
void CommandBufferD3D12::copy(const Buffer&  transfer,
                              const uint64   srcOffset,
                              const uint32   srcRowPitch,
                              const Texture& texture,
                              const uint32   mipmap,
                              const uint32   layer)
{
    assert( started );
    assert( transfer.type() == BufferType::Transfer );
    assert( texture.mipmaps() > mipmap );
    assert( texture.layers() > layer );
   
    const BufferD3D12&  source      = reinterpret_cast<const BufferD3D12&>(transfer);
    const TextureD3D12& destination = reinterpret_cast<const TextureD3D12&>(texture);

    assert( source.size >= srcOffset + destination.size(mipmap) );

    // TODO: Check if graphics or compute!  
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    // Get information about buffer layout for upload
    D3D12_RESOURCE_DESC desc = destination.handle->GetDesc();

    // Based on amount of mip-maps and layers, calculates
    // index of subresource to modify.
    UINT subresource = D3D12CalcSubresource(mipmap,
                                            layer,
                                            0u,
                                            destination.state.mipmaps,
                                            destination.state.layers);
   
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
    UINT64                             requiredUploadBufferSize = 0;

    ValidateNoRet( gpu, GetCopyableFootprints(&desc,
                                              subresource, // First subresource to modify
                                              1,           // Subresources count
                                              0,
                                              &layout,
                                              nullptr,
                                              nullptr,
                                              &requiredUploadBufferSize) )

    // Verify that source buffer has enough storage to read from specified offset.
    // (application should properly fill it will all requred paddings).
    assert( srcOffset + requiredUploadBufferSize <= source.size );
      
    // Verify that row pitch expected by driver matches one used by application
    assert( layout.Footprint.RowPitch == srcRowPitch );
      
    // Take into notice offset in source buffer
    layout.Offset             = srcOffset;
      
    // Copy from Buffer with given alignments
    D3D12_TEXTURE_COPY_LOCATION srcLocation;
    srcLocation.pResource       = source.handle;
    srcLocation.Type            = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLocation.PlacedFootprint = layout;

    // Copy to given subresource of destination texture
    D3D12_TEXTURE_COPY_LOCATION dstLocation;
    dstLocation.pResource        = destination.handle;
    dstLocation.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.SubresourceIndex = subresource;

    ValidateComNoRet( command->CopyTextureRegion(&dstLocation,
                                                 0, 0, 0,      // Upper-Left corner of destination region
                                                 &srcLocation,
                                                 nullptr) )    // Size of source region/volume to copy (in Texture-Texture copy)
}

void CommandBufferD3D12::copyRegion2D(
    const Buffer&  _source,
    const uint64   srcOffset,
    const uint32   srcRowPitch,
    const Texture& texture,
    const uint32   mipmap,
    const uint32   layer,
    const uint32v2 origin,
    const uint32v2 region,
    const uint8    plane)
{
    const BufferD3D12&  source      = reinterpret_cast<const BufferD3D12&>(_source);
    const TextureD3D12& destination = reinterpret_cast<const TextureD3D12&>(texture);

    assert( started );
    assert( source.type() == BufferType::Transfer );
    assert( mipmap < destination.state.mipmaps );
    assert( layer < destination.state.layers );
    assert( origin.x + region.width  <= destination.width(mipmap) );
    assert( origin.y + region.height <= destination.height(mipmap) );
  //assert( source.size >= srcOffset + srcAlignment.surfaceSize(region.width, region.height) );
    assert( srcRowPitch % D3D12_TEXTURE_DATA_PITCH_ALIGNMENT == 0 );

    // TODO: Check if graphics or compute!
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    // Based on amount of mip-maps and layers, calculat index of subresource to modify.
    UINT subresource = D3D12CalcSubresource(mipmap,
                                            layer,
                                            plane,
                                            destination.state.mipmaps,
                                            destination.state.layers);
   
    D3D12_SUBRESOURCE_FOOTPRINT regionLayout;
    regionLayout.Format   = TranslateTextureFormat[underlyingType(destination.state.format)];
    regionLayout.Width    = region.width;
    regionLayout.Height   = region.height;
    regionLayout.Depth    = 1;
    regionLayout.RowPitch = srcRowPitch;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT bufferLayout;
    bufferLayout.Offset    = srcOffset;
    bufferLayout.Footprint = regionLayout;

    // Copy from Buffer with given alignments
    D3D12_TEXTURE_COPY_LOCATION srcLocation;
    srcLocation.pResource       = source.handle;
    srcLocation.Type            = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLocation.PlacedFootprint = bufferLayout;

    // Copy to given subresource of destination texture
    D3D12_TEXTURE_COPY_LOCATION dstLocation;
    dstLocation.pResource        = destination.handle;
    dstLocation.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.SubresourceIndex = subresource;

    // Copy to below region
    D3D12_BOX copyRegion;
    copyRegion.left   = 0;
    copyRegion.top    = 0;
    copyRegion.front  = 0;
    copyRegion.right  = region.width;
    copyRegion.bottom = region.height;
    copyRegion.back   = 1;

    ValidateComNoRet( command->CopyTextureRegion(&dstLocation,
                                                 origin.x, origin.y, 0,
                                                 &srcLocation,
                                                 &copyRegion) )
}


// PIPELINE COMMANDS
//////////////////////////////////////////////////////////////////////////


void CommandBufferD3D12::setPipeline(const Pipeline& _pipeline)
{
    assert( started );

    const PipelineD3D12& pipeline = reinterpret_cast<const PipelineD3D12&>(_pipeline);

    // TODO: Check if graphics or compute!  
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

    ValidateComNoRet( command->SetGraphicsRootSignature(pipeline.layout->handle) )
        
    // TODO: Cache Root Signature and prevent it's rebinding if it's the same.

    ValidateComNoRet( command->SetPipelineState(pipeline.handle) )

    // Dynamic States:
    // https://msdn.microsoft.com/en-us/library/windows/desktop/dn899196(v=vs.85).aspx

    // Stream Out is currently unsupported:
    // - ValidateComNoRet( command->SOSetTargets() )

    ValidateComNoRet( command->RSSetViewports(pipeline.viewportsCount, &pipeline.viewport[0]) )
    ValidateComNoRet( command->RSSetScissorRects(pipeline.viewportsCount, &pipeline.scissor[0]) )

    ValidateComNoRet( command->OMSetBlendFactor(pipeline.blendColor) )

    ValidateComNoRet( command->OMSetStencilRef(pipeline.stencilRef) )

    // Metal needs primitive topology type to be specified at
    // PSO creation time, when layered rendering is enabled, and
    // can specify primitive adjacency and ordering at Draw time. 
    //
    // D3D12 always need to specify primitive topology type at 
    // PSO creation, but still can specify primitive adjacency 
    // and ordering at Draw time. 
    //
    // Vulkan on the other end is most restrictive, it cannot 
    // dynamically change drawn primitive type, as it needs 
    // primitive topology, adjacency and ordering info at PSO 
    // creation time.
    //
    // Thus Vulkan behavior needs to be followed.
    ValidateComNoRet( command->IASetPrimitiveTopology(pipeline.topology) )

    // TODO: Check at runtime if given states really change, to prevent their constant rebinding with each PSO.
    // TODO: Decide, which of the states below, can be promoted to be dynamic and set explicitly on CommandBuffer
    //       (by modifying Vulkan backend PSO to have those states as dynamic, and loosing possibility for best
    //       optimization).

    // Input Assembler buffer strides
    memcpy(&bufferStride[0], &pipeline.bufferStride[0], pipeline.buffersCount * sizeof(uint32));
    buffersCount = pipeline.buffersCount;
}


// DRAW COMMANDS
//////////////////////////////////////////////////////////////////////////


void CommandBufferD3D12::draw(
    const uint32 elements,
    const uint32 instances,
    const uint32 firstVertex,
    const uint32 firstInstance) const
{
    assert( started );
    assert( encoding );
    assert( elements );

    // TODO: Check if graphics or compute!  
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);
   
    // Vulkan cannot dynamically change drawn primitive type,
    // thus it is set during Pipeline State Object binding.

    ValidateComNoRet( command->DrawInstanced(elements,           // Number of vertices to draw
                                             max(1U, instances), // Number of instances to draw
                                             firstVertex,        // StartVertexLocation - Index of first vertex to draw
                                             firstInstance) )    // StartInstanceLocation - A value added to each index before reading per-instance data from a vertex buffer.
}

void CommandBufferD3D12::drawIndexed(
    const uint32  elements,
    const uint32  instances,
    const uint32  firstIndex,
    const sint32  firstVertex,
    const uint32  firstInstance) const
{
    assert( started );
    assert( encoding );
    assert( elements );

    // TODO: Check if graphics or compute!  
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);
   
    // Vulkan cannot dynamically change drawn primitive type,
    // thus it is set during Pipeline State Object binding.

    ValidateComNoRet( command->DrawIndexedInstanced(elements,           // Number of indices to use for draw
                                                    max(1U, instances), // Number of instances to draw
                                                    firstIndex,         // Index of first index to start (multiplied by elementSize will give starting offset in IBO). There can be several buffers with separate indexes groups in one GPU Buffer.
                                                    firstVertex,        // BaseVertexLocation - A value added to each index before reading a vertex from the vertex buffer.
                                                    firstInstance) )    // StartInstanceLocation - A value added to each index before reading per-instance data from a vertex buffer.
}

void CommandBufferD3D12::drawIndirect(
    const Buffer& indirectBuffer,
    const uint32  firstEntry) const
{
    assert( started );
    assert( encoding );

    const BufferD3D12& indirect = reinterpret_cast<const BufferD3D12&>(indirectBuffer);
    assert( indirect.apiType == BufferType::Indirect );
   
    // TODO: Check if graphics or compute!
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);
   
    UINT MaxCommandCount      = indirect.length() / sizeof(IndirectDrawArgument);
    UINT ArgumentBufferOffset = firstEntry * sizeof(IndirectDrawArgument);

    ValidateComNoRet( command->ExecuteIndirect(indirect.signature,
                                               MaxCommandCount - firstEntry,
                                               indirect.handle,
                                               ArgumentBufferOffset,
                                               nullptr,
                                               0) )
}

void CommandBufferD3D12::drawIndirectIndexed(
    const Buffer& indirectBuffer,
    const uint32  firstEntry,
    const uint32  firstIndex) const
{
    assert( started );
    assert( encoding );

    const BufferD3D12& indirect = reinterpret_cast<const BufferD3D12&>(indirectBuffer);
    assert( indirect.apiType == BufferType::Indirect );
   
    // TODO: Check if graphics or compute!  
    ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);
   
    // D3D12 Indirect Drawing
    // https://msdn.microsoft.com/en-us/library/windows/desktop/dn903925(v=vs.85).aspx
    //
    // Engine supports only arrays of one of two indirect arguments:
    //
    // - D3D12_DRAW_ARGUMENTS (exposed as IndirectDrawArgument)
    // - D3D12_DRAW_INDEXED_ARGUMENTS (exposed as IndirectIndexedDrawArgument)
    //
    // Unsupported D3D12 indirect arguments:
    // 
    // - D3D12_DISPATCH_ARGUMENTS
    // - D3D12_VERTEX_BUFFER_VIEW
    // - D3D12_INDEX_BUFFER_VIEW
    // - D3D12_GPU_VIRTUAL_ADDRESS (a typedef'd synonym of UINT64).
    // - D3D12_CONSTANT_BUFFER_VIEW_DESC
    //
    // D3D12 allows mixing of different Indirect Arguments in the GPU generated buffer.

    UINT MaxCommandCount      = indirect.length() / sizeof(IndirectIndexedDrawArgument);
    UINT ArgumentBufferOffset = firstEntry * sizeof(IndirectIndexedDrawArgument);

    ValidateComNoRet( command->ExecuteIndirect(indirect.signatureIndexed,
                                               MaxCommandCount - firstEntry,
                                               indirect.handle,
                                               ArgumentBufferOffset,
                                               nullptr,
                                               0) )
}


// FINISHING
//////////////////////////////////////////////////////////////////////////


// Create signaling Event
// fenceSignalingEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
// fence->SetEventOnCompletion(acquiredValue, fenceSignalingEvent); 

void CommandBufferD3D12::commit(Semaphore* signalSemaphore)
{
    assert( started );
    assert( !encoding );
    assert( !commited );
   
    // TODO: Check if graphics or compute!  
    reinterpret_cast<ID3D12GraphicsCommandList*>(handle)->Close();
   
    // Ensures CommandBuffer submission and following encoding of fence signaling 
    // operation are atomically encoded on queue in multi-threaded environment.
    gpu->queueAcquire[queueIndex].lock();
    {
        // If this CommandBuffer should first synchronize with some other event,
        // we encode wait command just before it's execution. 
        if (fence)
        {
            queue->Wait(fence, waitForValue);
            fence = nullptr;
        }

        // Commit this single Command Buffer for execution
        ID3D12CommandList* lists[] = { handle };
        queue->ExecuteCommandLists(1, lists);
      
        // Get unique value, representing current moment after CommandBuffer 
        // encoding, on it's parent queue, for that queue fence, to signal it
        // once this CommandBuffer is completed.
        gpu->fenceValue[queueIndex]++;
        commitValue = gpu->fenceValue[queueIndex];

        // Queue will signal that fence value (GPU will set this value on Fence)
        // once just commited CommandBuffer is executed.
        queue->Signal(gpu->fence[queueIndex], commitValue);
    }
    gpu->queueAcquire[queueIndex].unlock();

    if (signalSemaphore)
    {
        // This Semaphore can be now used, to synchronize any Queue
        // execution with this particular fence, and it's value.
        SemaphoreD3D12* semaphore = reinterpret_cast<SemaphoreD3D12*>(signalSemaphore);
        semaphore->fence        = gpu->fence[queueIndex];
        semaphore->waitForValue = commitValue;
    }

    // Try to clear any CommandBuffers that are no longer executing.
    gpu->clearCommandBuffersQueue();

    // Add this CommandBuffer to device's array of CB's in flight.
    // This will ensure that CommandBuffer won't be destroyed until
    // fence is not signaled.
    gpu->addCommandBufferToQueue(shared_from_this());

    commited = true;
}
   
bool CommandBufferD3D12::isCompleted(void)
{
    uint64 reachedMoment = gpu->fence[queueIndex]->GetCompletedValue();
    return reachedMoment >= commitValue;
}

void CommandBufferD3D12::waitUntilCompleted(void)
{
    assert( started );
    assert( !encoding );
    assert( commited );

    if (!isCompleted())
    {
        HANDLE fenceSignalingEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        gpu->fence[queueIndex]->SetEventOnCompletion(commitValue, fenceSignalingEvent);

        // Wait maximum 1 second, then assume GPU hang. // TODO: This should be configurable global
        DWORD gpuWatchDog = 1000;

        // Wait until the fence is completed or watch dog timer is out
        DWORD result = WaitForSingleObject(fenceSignalingEvent, gpuWatchDog);

        if (result == WAIT_TIMEOUT)
        {
            enLog << "GPU Hang!\n\n";
        }
      
        // Destroy signaling Event
        CloseHandle(fenceSignalingEvent);
    }
}
   
// https://msdn.microsoft.com/en-us/library/windows/desktop/dn899171(v=vs.85).aspx


// DEVICE
////////////////////////////////////////////////////////////////////////////////


std::shared_ptr<CommandBuffer> Direct3D12Device::createCommandBuffer(const QueueType type, const uint32 parentQueue)
{
    std::shared_ptr<CommandBufferD3D12> result = nullptr;

    // Each worker thread creates it's Command Buffers from separate Command Allocator
    uint32 workerId = Scheduler->currentWorkerId();
    if (workerId == InvalidWorkerId)
    {
        assert( 0 );
        return result;
    }

    uint32 queueType = underlyingType(type);
    uint32 cacheId   = currentAllocator[workerId];

    assert( queuesCount[queueType] > parentQueue );

    ID3D12CommandList* handle = nullptr;
   
    Validate( this, CreateCommandList(0u,      /* No Multi-GPU support for now */
                                      TranslateQueueType[queueType],
                                      commandAllocator[workerId][cacheId],
                                      nullptr, /* No initial PipelineState for now */
                                      IID_PPV_ARGS(&handle)) )
    if ( SUCCEEDED(lastResult[currentThreadId()]) )
    {
        result = std::make_shared<CommandBufferD3D12>(this, queue[queueType], queueType, handle);

#if defined(EN_DEBUG)
        // Name CommandBuffer for debugging
        std::string name("CommandBuffer_T: ");
        name += stringFrom(workerId);
        name += " Q: ";
        name += stringFrom(queueType);
        name += " C: ";
        name += stringFrom(cacheId);
        handle->SetPrivateData( WKPDID_D3DDebugObjectName, name.length(), name.c_str());
#endif

        // Switch to next CommandAllocator in Cache, if enough CommandBuffers were allocated on this one
        commandBuffersAllocated[workerId]++;
        if (commandBuffersAllocated[workerId] == MaxCommandBuffersAllocated)
        {
            currentAllocator[workerId] = (currentAllocator[workerId] + 1) % AllocatorCacheSize;
            commandBuffersAllocated[workerId] = 0;
 
            // It is assumed, that all CommandBuffers execution on newly selected 
            // allocator from cache was finished, and allocator was already reset.
            uint32 cacheId = currentAllocator[workerId];
            assert( commandBuffersExecuting[workerId][cacheId] == 0 );
        }
    }

    // ID3D12GraphicsCommandList extends ID3D12CommandList
    // https://msdn.microsoft.com/en-us/library/windows/desktop/dn903537(v=vs.85).aspx
    // and alows all types of operations (draws, dispatches & copies)

    return result;
}
   
void Direct3D12Device::addCommandBufferToQueue(std::shared_ptr<CommandBuffer> command)
{
    CommandBufferD3D12* ptr = reinterpret_cast<CommandBufferD3D12*>(command.get());

    uint32 workerId = Scheduler->currentWorkerId();
    assert( workerId != InvalidWorkerId );

    uint32 queueType = ptr->queueIndex;
    uint32 cacheId   = currentAllocator[workerId];
    uint32 executing = commandBuffersExecuting[workerId][cacheId];
   
    assert( executing < MaxCommandBuffersExecuting );

    commandBuffers[workerId][cacheId][executing] = command;
    commandBuffersExecuting[workerId][cacheId]++;
}

void Direct3D12Device::clearCommandBuffersQueue(void)
{
    // Iterate over list of Command Buffers submitted for execution by this worker thread (per each allocator in pool).
    uint32 workerId = Scheduler->currentWorkerId();
    assert( workerId != InvalidWorkerId );
   
    for(uint32 cacheId=0; cacheId<AllocatorCacheSize; ++cacheId)
    {
        uint32 executing = commandBuffersExecuting[workerId][cacheId];
        for(uint32 i=0; i<executing; ++i)
        {
            CommandBufferD3D12* command = reinterpret_cast<CommandBufferD3D12*>(commandBuffers[workerId][cacheId][i].get());

            // Assume that CommandBuffer is finished, after next Fence event is passed (so with one event of delay).
            // This way driver has time to clean-up, and is not throwing Debug errors that we reset CommandAllocator
            // which CommandBuffers are still in flight.
            if (fence[command->queueIndex]->GetCompletedValue() > command->commitValue)
          //if (command->isCompleted())
            {
                // Safely release Command Buffer object
                commandBuffers[workerId][cacheId][i] = nullptr;
                if (i < (executing - 1))
                {
                    commandBuffers[workerId][cacheId][i] = commandBuffers[workerId][cacheId][executing - 1];
                    commandBuffers[workerId][cacheId][executing - 1] = nullptr;
                }
      
                executing--;
                commandBuffersExecuting[workerId][cacheId]--;

                // If this CommandAllocator queue of CommandBuffers in flight just get 
                // emptied, and it's not actually used CommandAllocator, it may be reset.
                if (cacheId != currentAllocator[workerId])
                {
                    if (commandBuffersExecuting[workerId][cacheId] == 0)
                    {
                        commandAllocator[workerId][cacheId]->Reset();
                    }
                }
            }
        }
    }
}

} // en::gpu
} // en

#endif
