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
#include "utilities/strings.h"

namespace en
{
   namespace gpu
   {
   CommandBufferD3D12::CommandBufferD3D12(Direct3D12Device* _gpu, 
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
      currentIndexBuffer(nullptr),
      CommandBuffer()
   {
   }

   CommandBufferD3D12::~CommandBufferD3D12()
   {
   // RendeRPass needs to be finished before Command Buffer is destroyed
   assert( !encoding );

   // Finish encoded tasks
   if (!commited)
      commit();

   // Release Command Buffer
   assert( handle );
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

   // Remember fence and it's value, for which this CommandBuffer execution should wait for.
   if (waitForSemaphore)
      {
      SemaphoreD3D12* semaphore = raw_reinterpret_cast<SemaphoreD3D12>(&waitForSemaphore);
      fence        = semaphore->fence;
      waitForValue = semaphore->waitForValue;
      }

   started = true;
   }

   void CommandBufferD3D12::startRenderPass(const Ptr<RenderPass> pass, const Ptr<Framebuffer> _framebuffer)
   {
   assert( started );
   assert( !encoding );
    
   assert( pass );
   assert( _framebuffer );
   
   renderPass  = ptr_reinterpret_cast<RenderPassD3D12>(&pass);
   framebuffer = ptr_reinterpret_cast<FramebufferD3D12>(&_framebuffer);

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
                                                  &nullColorDesc,
                                                   gpu->handleRTV[i]) )
         }
   
   // Create Depth-Stencil View
   if (renderPass->depthStencil)
      {
      ProfileNoRet( gpu, CreateDepthStencilView(framebuffer->depthHandle->texture->handle,
                                               &framebuffer->depthDesc,
                                                gpu->handleDSV) )
      }
   else
      {
      ProfileNoRet( gpu, CreateDepthStencilView(nullptr,
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
         if (checkBit(renderPass->usedAttachments, i))
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
               ProfileComNoRet( command->ResolveSubresource(framebuffer->resolveHandle[i]->texture->handle,
                                                            dstSubresource,
                                                            framebuffer->colorHandle[i]->texture->handle,
                                                            srcSubresource,
                                                            format) )
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


   void CommandBufferD3D12::setVertexBuffers(const uint32 count, const uint32 firstSlot, const Ptr<Buffer>* buffers, const uint64* offsets) const
   {
   assert( started );
   assert( count );
   assert( buffers );
   assert( (firstSlot + count) <= gpu->support.maxInputLayoutBuffersCount );

   // TODO: Ensure this Command Buffer is Graphic one !
   ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

   // Extract Direct3D12 buffer handles
   D3D12_VERTEX_BUFFER_VIEW* views = new D3D12_VERTEX_BUFFER_VIEW[count]; // TODO: Optimize by allocating on the stack maxBuffersCount sized fixed array.
   for(uint32 i=0; i<count; ++i)
      {
      assert( buffers[i] );
      BufferD3D12* bufferD3D12 = raw_reinterpret_cast<BufferD3D12>(&buffers[i]);
      ID3D12Resource* resource = bufferD3D12->handle;
      
      // Populate Buffer View
      views[i].BufferLocation = resource->GetGPUVirtualAddress() + offsets[i];  // Final location is Base Buffer Adress + current Offset
      views[i].SizeInBytes    = buffers[i]->length();
      views[i].StrideInBytes  = bufferD3D12->formatting.elementSize();
      }

   ProfileComNoRet( command->IASetVertexBuffers(firstSlot, count, views) )

   delete [] views;
   }

   void CommandBufferD3D12::setVertexBuffer(const uint32 slot, const Ptr<Buffer> buffer, const uint64 offset) const
   {
   assert( started );
   assert( buffer );
   assert( slot < gpu->support.maxInputLayoutBuffersCount );

   BufferD3D12* bufferD3D12 = raw_reinterpret_cast<BufferD3D12>(&buffer);
   ID3D12Resource* resource = bufferD3D12->handle;
      
   // Populate Buffer View
   D3D12_VERTEX_BUFFER_VIEW view;
   view.BufferLocation = resource->GetGPUVirtualAddress() + offset;  // Final location is Base Buffer Adress + current Offset
   view.SizeInBytes    = buffer->length();
   view.StrideInBytes  = bufferD3D12->formatting.elementSize();

   // TODO: Ensure this Command Buffer is Graphic one !
   ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

   ProfileComNoRet( command->IASetVertexBuffers(slot, 1, &view) )
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

   // TODO: Check if graphics or compute!  
   ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

   // Get information about buffer layout for upload
   D3D12_RESOURCE_DESC desc = destination->handle->GetDesc();

   // Based on amount of mip-maps and layers, calculates
   // index of subresource to modify.
   UINT subresource = D3D12CalcSubresource(mipmap,
                                           layer,
                                           0u,
                                           destination->state.mipmaps,
                                           destination->state.layers);
   
   D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
   UINT64                             requiredUploadBufferSize = 0;

   ProfileNoRet( gpu, GetCopyableFootprints(&desc,
                                            subresource, // First subresource to modify
                                            1,           // Subresources count
                                            0,
                                            &layout,
                                            nullptr,
                                            nullptr,
                                            &requiredUploadBufferSize) )

   // Verify that source buffer has enough storage to read from specified offset.
   // (application should properly fill it will all requred paddings).
   assert( srcOffset + requiredUploadBufferSize <= source->size );

   // Take into notice offset in source buffer
   layout.Offset = srcOffset;

   // Copy from Buffer with given alignments
   D3D12_TEXTURE_COPY_LOCATION srcLocation;
   srcLocation.pResource       = source->handle;
   srcLocation.Type            = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
   srcLocation.PlacedFootprint = layout;

   // Copy to given subresource of destination texture
   D3D12_TEXTURE_COPY_LOCATION dstLocation;
   dstLocation.pResource        = destination->handle;
   dstLocation.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
   dstLocation.SubresourceIndex = subresource;

   ProfileComNoRet( command->CopyTextureRegion(&dstLocation,
                                               0, 0, 0,      // Upper-Left corner of destination region
                                               &srcLocation,
                                               nullptr) )    // Size of source region/volume to copy (in Texture-Texture copy)
   }


  // uint32 rowPitch = destination->width(mipmap) * genericTexelSize(destination->state.format);

  // // TODO: ROW PITCH NEEDS TO ME MULTIPLE OF 256 BYTES ON DISCREETE GPU's
  // assert( rowPitch % D3D12_TEXTURE_DATA_PITCH_ALIGNMENT == 0 );



		//D3D12_SUBRESOURCE_DATA textureData = {};
		//textureData.pData = &texture[0];
		//textureData.RowPitch = TextureWidth * TexturePixelSize;
		//textureData.SlicePitch = textureData.RowPitch * TextureHeight;





   void CommandBufferD3D12::copy(Ptr<Buffer> transfer,
                                 Ptr<Texture> texture,
                                 const uint32 mipmap,
                                 const uint32 layer)
   {
   copy(transfer, 0u, texture, mipmap, layer);
   }


   // PIPELINE COMMANDS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferD3D12::setPipeline(const Ptr<Pipeline> _pipeline)
   {
   assert( started );
   assert( _pipeline );

   PipelineD3D12* pipeline = raw_reinterpret_cast<PipelineD3D12>(&_pipeline);

   // TODO: Check if graphics or compute!  
   ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);

   ProfileComNoRet( command->SetGraphicsRootSignature(pipeline->layout->handle) )
        
   ProfileComNoRet( command->SetPipelineState(pipeline->handle) )

   // Dynamic States:
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dn899196(v=vs.85).aspx

   // Stream Out is currently unsupported:
   // - ProfileComNoRet( command->SOSetTargets() )

   ProfileComNoRet( command->RSSetViewports(pipeline->viewportsCount, &pipeline->viewport[0]) )
   ProfileComNoRet( command->RSSetScissorRects(pipeline->viewportsCount, &pipeline->scissor[0]) )

   ProfileComNoRet( command->OMSetBlendFactor(pipeline->blendColor) )

   ProfileComNoRet( command->OMSetStencilRef(pipeline->stencilRef) )

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
   ProfileComNoRet( command->IASetPrimitiveTopology(pipeline->topology) )

   // TODO: Check at runtime if given states really change, to prevent their constant rebinding with each PSO.
   // TODO: Decide, which of the states below, can be promoted to be dynamic and set explicitly on CommandBuffer
   //       (by modifying Vulkan backend PSO to have those states as dynamic, and loosing possibility for best
   //       optimization).
   }


   // DRAW COMMANDS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferD3D12::draw(const uint32       elements,
                                 const Ptr<Buffer>  indexBuffer,
                                 const uint32       instances,
                                 const uint32       firstElement,
                                 const sint32       firstVertex,
                                 const uint32       firstInstance)
   {
   assert( started );
   assert( encoding );
   assert( elements );

   // TODO: Check if graphics or compute!  
   ID3D12GraphicsCommandList* command = reinterpret_cast<ID3D12GraphicsCommandList*>(handle);
   
   // Vulkan cannot dynamically change drawn primitive type,
   // thus it is set during Pipeline State Object binding.

   // Elements are assembled into Primitives.
   if (indexBuffer)
      {
      BufferD3D12* index = raw_reinterpret_cast<BufferD3D12>(&indexBuffer);
      assert( index->apiType == BufferType::Index );
      
      // Prevent binding the same IBO several times in row
      if (index != currentIndexBuffer)
         {
         D3D12_INDEX_BUFFER_VIEW desc;
         desc.BufferLocation = index->handle->GetGPUVirtualAddress(); // Offset In Index Buffer is calculated at draw call.
         desc.SizeInBytes    = index->length();  
         desc.Format         = DXGI_FORMAT_R16_UINT;
         if (index->formatting.column[0] == Attribute::u32)
            desc.Format      = DXGI_FORMAT_R32_UINT;
         
         // Index Buffer remains bound to Command Buffer after this call,
         // but because there is no other way to do indexed draw than to
         // go through this API, it's safe to leave it bounded.
         ProfileComNoRet( command->IASetIndexBuffer(&desc) )

         currentIndexBuffer = index;
         }

      // TODO: Do I correctly interprete BaseVertexLocation & StartInstanceLocation?
      //       Are they equal to starting VertexID and InstanceID?

      ProfileComNoRet( command->DrawIndexedInstanced(elements,           // Number of indices to use for draw
                                                     max(1U, instances), // Number of instances to draw
                                                     firstElement,       // Index of first index to start (multiplied by elementSize will give starting offset in IBO). There can be several buffers with separate indexes groups in one GPU Buffer.
                                                     firstVertex,        // BaseVertexLocation - A value added to each index before reading a vertex from the vertex buffer.
                                                     firstInstance) )    // StartInstanceLocation - A value added to each index before reading per-instance data from a vertex buffer.
      }
   else
      {
      ProfileComNoRet( command->DrawInstanced(elements,           // Number of vertices to draw
                                              max(1U, instances), // Number of instances to draw
                                              firstVertex,        // Index of first vertex to draw
                                              firstInstance) )    // StartInstanceLocation - A value added to each index before reading per-instance data from a vertex buffer.
      }
   }

   void CommandBufferD3D12::draw(const Ptr<Buffer>  indirectBuffer,
                                 const uint32       firstEntry,
                                 const Ptr<Buffer>  indexBuffer,
                                 const uint32       firstElement)
   {
   assert( started );
   assert( encoding );
   assert( indirectBuffer );
   
   BufferD3D12* indirect = raw_reinterpret_cast<BufferD3D12>(&indirectBuffer);
   assert( indirect->apiType == BufferType::Indirect );
   
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

   UINT MaxCommandCount = 0;
   UINT ArgumentBufferOffset = 0;

   if (indexBuffer)
      {
      BufferD3D12* index = raw_reinterpret_cast<BufferD3D12>(&indexBuffer);
      assert( index->apiType == BufferType::Index );
      
      // Prevent binding the same IBO several times in row
      if (index != currentIndexBuffer)
         {
         D3D12_INDEX_BUFFER_VIEW desc;
         desc.BufferLocation = index->handle->GetGPUVirtualAddress(); // Offset In Index Buffer is calculated at draw call.
         desc.SizeInBytes    = index->length();  
         desc.Format         = DXGI_FORMAT_R16_UINT;
         if (index->formatting.column[0] == Attribute::u32)
            desc.Format      = DXGI_FORMAT_R32_UINT;
         
         // Index Buffer remains bound to Command Buffer after this call,
         // but because there is no other way to do indexed draw than to
         // go through this API, it's safe to leave it bounded.
         ProfileComNoRet( command->IASetIndexBuffer(&desc) )

         currentIndexBuffer = index;
         }

      MaxCommandCount      = indirect->length() / sizeof(IndirectIndexedDrawArgument);
      ArgumentBufferOffset = firstEntry * sizeof(IndirectIndexedDrawArgument);

      ProfileComNoRet( command->ExecuteIndirect(indirect->signatureIndexed,
                                                MaxCommandCount - firstEntry,
                                                indirect->handle,
                                                ArgumentBufferOffset,
                                                nullptr,
                                                0) )
      }
   else
      {
      MaxCommandCount      = indirect->length() / sizeof(IndirectDrawArgument);
      ArgumentBufferOffset = firstEntry * sizeof(IndirectDrawArgument);

      ProfileComNoRet( command->ExecuteIndirect(indirect->signature,
                                                MaxCommandCount - firstEntry,
                                                indirect->handle,
                                                ArgumentBufferOffset,
                                                nullptr,
                                                0) )
      }
   }


   // FINISHING
   //////////////////////////////////////////////////////////////////////////


   // Create signaling Event
   // fenceSignalingEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
   // fence->SetEventOnCompletion(acquiredValue, fenceSignalingEvent); 

   void CommandBufferD3D12::commit(const Ptr<Semaphore> signalSemaphore)
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
      SemaphoreD3D12* semaphore = raw_reinterpret_cast<SemaphoreD3D12>(&signalSemaphore);
      semaphore->fence        = gpu->fence[queueIndex];
      semaphore->waitForValue = commitValue;
      }

   // Try to clear any CommandBuffers that are no longer executing.
   gpu->clearCommandBuffersQueue();

   // Add this CommandBuffer to device's array of CB's in flight.
   // This will ensure that CommandBuffer won't be destroyed until
   // fence is not signaled.
   gpu->addCommandBufferToQueue(Ptr<CommandBuffer>(this));

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
         Log << "GPU Hang!\n" << endl;
         }
      
      // Destroy signaling Event
      CloseHandle(fenceSignalingEvent);
      }
   }
   
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dn899171(v=vs.85).aspx

 
   // DEVICE
   //////////////////////////////////////////////////////////////////////////


   Ptr<CommandBuffer> Direct3D12Device::createCommandBuffer(const QueueType type, const uint32 parentQueue)
   {
   // Each thread creates it's Command Buffers from separate Command Allocator
   uint32 thread    = Scheduler.core();
   uint32 queueType = underlyingType(type);
   uint32 cacheId   = currentAllocator[thread][queueType];

   assert( queuesCount[queueType] > parentQueue );
   
   Ptr<CommandBufferD3D12> result = nullptr;

   ID3D12CommandList* handle = nullptr;
   
   Profile( this, CreateCommandList(0u,      /* No Multi-GPU support for now */
                                    TranslateQueueType[queueType],
                                    commandAllocator[thread][queueType][cacheId],
                                    nullptr, /* No initial PipelineState for now */
                                    IID_PPV_ARGS(&handle)) )
   if ( SUCCEEDED(lastResult[thread]) )
      {
      result = new CommandBufferD3D12(this, queue[queueType], queueType, handle);

#if defined(EN_DEBUG)
      // Name CommandBuffer for debugging
      string name("CommandBuffer_T: ");
      name += stringFrom(thread);
      name += " Q: ";
      name += stringFrom(queueType);
      name += " C: ";
      name += stringFrom(cacheId);
      handle->SetPrivateData( WKPDID_D3DDebugObjectName, name.length(), name.c_str());
#endif

      // Switch to next CommandAllocator in Cache, if enough CommandBuffers were allocated on this one
      commandBuffersAllocated[thread][queueType]++;
      if (commandBuffersAllocated[thread][queueType] == MaxCommandBuffersAllocated)
         {
         currentAllocator[thread][queueType] = (currentAllocator[thread][queueType] + 1) % AllocatorCacheSize;
         commandBuffersAllocated[thread][queueType]  = 0;
 
         // It is assumed, that all CommandBuffers execution on newly selected 
         // allocator from cache was finished, and allocator was already reset.
         uint32 cacheId = currentAllocator[thread][queueType];
         assert( commandBuffersExecuting[thread][queueType][cacheId] == 0 );
         }
      }

   // ID3D12GraphicsCommandList extends ID3D12CommandList
   // https://msdn.microsoft.com/en-us/library/windows/desktop/dn903537(v=vs.85).aspx
   // and alows all types of operations (draws, dispatches & copies)

   return ptr_reinterpret_cast<CommandBuffer>(&result);
   }
   
   void Direct3D12Device::addCommandBufferToQueue(Ptr<CommandBuffer> command)
   {
   CommandBufferD3D12* ptr = raw_reinterpret_cast<CommandBufferD3D12>(&command);

   uint32 thread    = Scheduler.core();
   uint32 queueType = ptr->queueIndex;
   uint32 cacheId   = currentAllocator[thread][queueType];
   uint32 executing = commandBuffersExecuting[thread][queueType][cacheId];
   
   assert( executing < MaxCommandBuffersExecuting );

   commandBuffers[thread][queueType][cacheId][executing] = command;
   commandBuffersExecuting[thread][queueType][cacheId]++;
   }

   void Direct3D12Device::clearCommandBuffersQueue(void)
   {
   // Iterate over list of Command Buffers submitted for execution by this thread (per each Queue type and allocator in pool).
   uint32 thread = Scheduler.core();
   for(uint32 queueType=0; queueType<underlyingType(QueueType::Count); ++queueType)
      for(uint32 cacheId=0; cacheId<AllocatorCacheSize; ++cacheId)
         {
         uint32 executing = commandBuffersExecuting[thread][queueType][cacheId];
         for(uint32 i=0; i<executing; ++i)
            {
            CommandBufferD3D12* command = raw_reinterpret_cast<CommandBufferD3D12>(&commandBuffers[thread][queueType][cacheId][i]);

            // Assume that CommandBuffer is finished, after next Fence event is passed (so with one event of delay).
            // This way driver has time to clean-up, and is not throwing Debug errors that we reset CommandAllocator
            // which CommandBuffers are still in flight.
            if (fence[command->queueIndex]->GetCompletedValue() > command->commitValue)
            //if (command->isCompleted())
               {
               // Safely release Command Buffer object
               commandBuffers[thread][queueType][cacheId][i] = nullptr;
               if (i < (executing - 1))
                  {
                  commandBuffers[thread][queueType][cacheId][i] = commandBuffers[thread][queueType][cacheId][executing - 1];
                  commandBuffers[thread][queueType][cacheId][executing - 1] = nullptr;
                  }
         
               executing--;
               commandBuffersExecuting[thread][queueType][cacheId]--;

               // If this CommandAllocator queue of CommandBuffers in flight just get 
               // emptied, and it's not actually used CommandAllocator, it may be reset.
               if (cacheId != currentAllocator[thread][queueType])
                  if (commandBuffersExecuting[thread][queueType][cacheId] == 0)
                     commandAllocator[thread][queueType][cacheId]->Reset();
               }
            }
         }
   }

   }
}
#endif
