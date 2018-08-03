/*

 Ngine v5.0
 
 Module      : Metal Command Buffer.
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
//#include "core/rendering/metal/mtlTexture.h"
#include "core/rendering/metal/mtlBlend.h"
#include "core/rendering/metal/mtlRenderPass.h"
//#include "core/rendering/metal/mtlRaster.h"
//#include "core/rendering/metal/mtlViewport.h"
//#include "core/rendering/metal/mtlShader.h"
//
//#include "utilities/osxStrings.h"
//
//#if defined(EN_PLATFORM_IOS)
//// For checking IOS version
//#import <Foundation/Foundation.h>
//#include <Availability.h>
//#endif

//#include "platform/osx/AppDelegate.h"    // For registering Window Delegate


namespace en
{
   namespace gpu
   {

   CommandBufferMTL::CommandBufferMTL(MetalDevice* _gpu) :
      gpu(_gpu),
      handle(nil),
      renderEncoder(nil),
      commited(false)
   {
   }

   void CommandBufferMTL::start(const Semaphore* waitForSemaphore)
   {
   // In Vulkan app needs to explicitly state when it starts to
   // encode commands to Command Buffer. In Metal we do that when
   // we create command encoders used to encode commands to 
   // Command Buffer.
   //
   // Additionally in Vulkan, CommandBuffer needs to wait when using
   // Swap-Chain surface, until presentation engine finishes reading
   // from it. In Metal, surface is not returned until read is complete
   // (active waiting lock).
   }

   // RENDER PASS
   //////////////////////////////////////////////////////////////////////////
   
 
   void CommandBufferMTL::startRenderPass(const shared_ptr<RenderPass> pass, const shared_ptr<Framebuffer> _framebuffer)
   {
   assert( renderEncoder == nil );
  
   RenderPassMTL*  renderPass  = reinterpret_cast<RenderPassMTL*>(pass.get());
   FramebufferMTL* framebuffer = reinterpret_cast<FramebufferMTL*>(_framebuffer.get());
   
   // Patch Texture handles
   bool layeredRendering = false;
   uint32 minLayersCount = 2048;
   for(uint32 i=0; i<gpu->support.maxColorAttachments; ++i)
      {
      renderPass->desc.colorAttachments[i].texture        = framebuffer->color[i];
      renderPass->desc.colorAttachments[i].resolveTexture = framebuffer->resolve[i];
      
      // Determine amount of layers shared by all attachments
      if (framebuffer->color[i].textureType == MTLTextureType2DArray)
         {
         layeredRendering = true;
         if (framebuffer->color[i].arrayLength < minLayersCount)
            minLayersCount = static_cast<uint32>(framebuffer->color[i].arrayLength);
         }
      else // 2DMSArray is supported in macOS 10.14+
      if (framebuffer->color[i].textureType == MTLTextureType2DMultisampleArray)
         {
         layeredRendering = true;
         if (framebuffer->color[i].arrayLength < minLayersCount)
            minLayersCount = static_cast<uint32>(framebuffer->color[i].arrayLength);
            
         assert( framebuffer->resolve[i].textureType == MTLTextureType2DArray );
         assert( framebuffer->resolve[i].arrayLength >= framebuffer->color[i].arrayLength );
         }
      }
   renderPass->desc.depthAttachment.texture   = framebuffer->depthStencil[0];
   renderPass->desc.stencilAttachment.texture = framebuffer->depthStencil[1];
#if defined(EN_PLATFORM_IOS)
   renderPass->desc.depthAttachment.resolveTexture = framebuffer->depthStencil[2];
#endif

   // Determine amount of layers shared by all attachments (include depth and stencil ones)
   if (framebuffer->depthStencil[0].textureType == MTLTextureType2DArray)
      {
      layeredRendering = true;
      if (framebuffer->depthStencil[0].arrayLength < minLayersCount)
         minLayersCount = static_cast<uint32>(framebuffer->depthStencil[0].arrayLength);
      }
   if (framebuffer->depthStencil[1])
      if (framebuffer->depthStencil[1].textureType == MTLTextureType2DArray)
         {
         layeredRendering = true;
         if (framebuffer->depthStencil[1].arrayLength < minLayersCount)
            minLayersCount = static_cast<uint32>(framebuffer->depthStencil[1].arrayLength);
         }
#if defined(EN_PLATFORM_IOS)
   // TODO: Uncomment once 2DMSArray is supported on iOS
   if (0 /* framebuffer->depthStencil[0].textureType == MTLTextureType2DMultisampleArray */)
      {
      layeredRendering = true;
      if (framebuffer->depthStencil[0].arrayLength < minLayersCount)
         minLayersCount = framebuffer->depthStencil[0].arrayLength;
         
      assert( framebuffer->depthStencil[2].textureType == MTLTextureType2DArray );
      assert( framebuffer->depthStencil[2].arrayLength >= framebuffer->color[i].arrayLength );
      }
#endif

   if (layeredRendering)
      renderPass->desc.renderTargetArrayLength = minLayersCount;
   
   renderEncoder = [handle renderCommandEncoderWithDescriptor:renderPass->desc];
   }
   
   void CommandBufferMTL::endRenderPass(void)
   {
   assert( renderEncoder != nil );
   
   [renderEncoder endEncoding];
   
   deallocateObjectiveC(renderEncoder);
   }
   

   // SETTING INPUT ASSEMBLER VERTEX BUFFERS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferMTL::setVertexBuffers(const uint32 firstSlot,
                                           const uint32 count,
                                           const shared_ptr<Buffer>(&buffers)[],
                                           const uint64* offsets) const
   {
   assert( count );
   // assert( (firstSlot + count) <= gpu->properties.limits.maxVertexInputBindings ); TODO: Populate GPU support field depending on iOS and macOS limitations and check it here

   // Extract Metal buffer handles
   id<MTLBuffer>* handles = new id<MTLBuffer>[count];   // TODO: Optimize by allocating on the stack maxBuffersCount sized fixed array.
   for(uint32 i=0; i<count; ++i)
      {
      handles[i] = reinterpret_cast<BufferMTL*>(buffers[i].get())->handle;
      }

   // Generate default zero offsets array if none is passed
   uint64* finalOffsets = (uint64*)offsets;
   if (!offsets)
      {
      finalOffsets = new uint64[count];
      memset(finalOffsets, 0, sizeof(uint64)*count);
      }

   [renderEncoder setVertexBuffers:handles 
                           offsets:(const NSUInteger*)finalOffsets   // Warning: Will fail if compiled as 32bit
                         withRange:NSMakeRange(firstSlot, count)];

   delete [] handles;
   if (!offsets)
      delete [] finalOffsets;
   }

   void CommandBufferMTL::setInputBuffer(
      const uint32  firstSlot,
      const uint32  slots,
      const Buffer& _buffer,
      const uint64* offsets) const
   {
   assert( slots );
   assert( (firstSlot + slots) <= gpu->support.maxInputLayoutBuffersCount );

   const BufferMTL& buffer = reinterpret_cast<const BufferMTL&>(_buffer);
   
   // TODO: Cache currently bound buffer, and only update offsets?
   
   // All slots get bind the same Metal buffer handle
   id<MTLBuffer>* handles = new id<MTLBuffer>[slots];   // TODO: Optimize by allocating on the stack maxBuffersCount sized fixed array.
   for(uint32 i=0; i<slots; ++i)
      handles[i] = buffer.handle;

   // Generate default zero offsets array if none is passed
   uint64* finalOffsets = (uint64*)offsets;
   if (!offsets)
      {
      finalOffsets = new uint64[slots];
      memset(finalOffsets, 0, sizeof(uint64) * slots);
      }

   [renderEncoder setVertexBuffers:handles
                           offsets:(const NSUInteger*)finalOffsets   // Warning: Will fail if compiled as 32bit
                         withRange:NSMakeRange(firstSlot, slots)];

   delete [] handles;
   if (!offsets)
      delete [] finalOffsets;
   }

   void CommandBufferMTL::setIndexBuffer(
      const Buffer& buffer,
      const Attribute type,
      const uint32 offset)
   {
   indexBuffer = reinterpret_cast<const BufferMTL*>(&buffer);
   
   assert( indexBuffer->apiType == BufferType::Index );
   
   elementSize = 2;
   indexType = MTLIndexTypeUInt16;
   if (indexBuffer->formatting.column[0] == Attribute::u32)
      {
      elementSize = 4;
      indexType = MTLIndexTypeUInt32;
      }
   }
      
   void CommandBufferMTL::setVertexBuffer(const uint32 slot,
                                          const Buffer& buffer,
                                          const uint64 offset) const
   {
   [renderEncoder setVertexBuffer:reinterpret_cast<const BufferMTL&>(buffer).handle
                           offset:offset 
                          atIndex:slot];
   }


   // TRANSFER COMMANDS
   //////////////////////////////////////////////////////////////////////////


   // Copies content of source buffer to destination buffer
   void CommandBufferMTL::copy(const Buffer& source, const Buffer& destination)
   {
   assert( source.length() <= destination.length() );

   copy(source, destination, source.length(), 0u, 0u);
   }
   
   void CommandBufferMTL::copy(
      const Buffer& source,
      const Buffer& destination,
      const uint64 size,
      const uint64 srcOffset,
      const uint64 dstOffset)
   {
   assert( source.type() == BufferType::Transfer );
   assert( (srcOffset + size) <= source.length() );
   assert( (dstOffset + size) <= destination.length() );
   
   const BufferMTL& src = reinterpret_cast<const BufferMTL&>(source);
   const BufferMTL& dst = reinterpret_cast<const BufferMTL&>(destination);

   @autoreleasepool
      {
      // Blit to Private buffer
      id <MTLBlitCommandEncoder> blit = [handle blitCommandEncoder];
      [blit copyFromBuffer:src.handle
              sourceOffset:src.offset + srcOffset
                  toBuffer:dst.handle
         destinationOffset:dst.offset + dstOffset
                      size:size];

      [blit endEncoding];
      //blit = nil;
      } // autoreleasepool
   }

   void CommandBufferMTL::copy(
       const Buffer&  transfer,
       const uint64   srcOffset,
       const uint32   srcRowPitch,
       const Texture& texture,
       const uint32   mipmap,
       const uint32   layer)
   {
   assert( transfer.type() == BufferType::Transfer );
   assert( texture.mipmaps() > mipmap );
   assert( texture.layers() > layer );
   
   const BufferMTL&  source      = reinterpret_cast<const BufferMTL&>(transfer);
   const TextureMTL& destination = reinterpret_cast<const TextureMTL&>(texture);

   assert( source.size >= srcOffset + destination.size(mipmap) );

   ImageMemoryAlignment layout = gpu->textureMemoryAlignment(destination.state, mipmap, layer);
   assert( source.size >= srcOffset + layout.surfaceSize(destination.state.mipWidth(mipmap), destination.state.mipHeight(mipmap)) );
      
      
      // Blit to Private texture
      id <MTLBlitCommandEncoder> blit = [handle blitCommandEncoder];
      [blit copyFromBuffer:source.handle
              sourceOffset:source.offset + srcOffset
         sourceBytesPerRow:srcRowPitch
       sourceBytesPerImage:0 // 0 should work as it copies single surface
                sourceSize:MTLSizeMake(destination.width(mipmap), destination.height(mipmap), 1)
                 toTexture:destination.handle
          destinationSlice:layer
          destinationLevel:mipmap
         destinationOrigin:MTLOriginMake(0, 0, 0)
                   options:MTLBlitOptionNone ];
      
      [blit endEncoding];
      
      deallocateObjectiveC(blit);
   }

   void CommandBufferMTL::copyRegion2D(
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
   const BufferMTL&  source      = reinterpret_cast<const BufferMTL&>(_source);
   const TextureMTL& destination = reinterpret_cast<const TextureMTL&>(texture);

   assert( source.type() == BufferType::Transfer );
   assert( mipmap < destination.state.mipmaps );
   assert( layer < destination.state.layers );
   assert( origin.x + region.width  <= destination.width(mipmap) );
   assert( origin.y + region.height <= destination.height(mipmap) );
 //assert( source.size >= srcOffset + layout.size );

   // Specify planes to blit
   MTLBlitOption blitOption = MTLBlitOptionNone;
   if (isDepthStencil(destination.state.format))
      {
      blitOption = MTLBlitOptionDepthFromDepthStencil;
      if (plane == 1)
         blitOption = MTLBlitOptionStencilFromDepthStencil;
      }

      // Blit to Private texture
      id <MTLBlitCommandEncoder> blit = [handle blitCommandEncoder];
      [blit copyFromBuffer:source.handle
              sourceOffset:source.offset + srcOffset
         sourceBytesPerRow:srcRowPitch
       sourceBytesPerImage:0 // 0 should work as it copies single surface
                sourceSize:MTLSizeMake(region.width, region.height, 1)
                 toTexture:destination.handle
          destinationSlice:layer
          destinationLevel:mipmap
         destinationOrigin:MTLOriginMake(origin.x, origin.y, 0)
                   options:blitOption];
      
      [blit endEncoding];
      
      deallocateObjectiveC(blit);
   }

   // DRAW COMMANDS
   //////////////////////////////////////////////////////////////////////////

   void CommandBufferMTL::draw(
      const uint32  elements,
      const uint32  instances,
      const uint32  firstVertex,
      const uint32  firstInstance) const
   {
   assert( elements );
   
   // IOS 9.0+, OSX 10.11+
   [renderEncoder drawPrimitives:primitive
                     vertexStart:firstVertex
                     vertexCount:elements
                   instanceCount:max(1U, instances)
                    baseInstance:firstInstance];

// [renderEncoder drawPrimitives:primitive
//                   vertexStart:firstVertex
//                   vertexCount:elements
//                 instanceCount:min(1U, instances)];
   }

   void CommandBufferMTL::drawIndexed(
      const uint32  elements,
      const uint32  instances,
      const uint32  firstIndex,
      const sint32  firstVertex,
      const uint32  firstInstance) const
   {
   assert( elements );
   assert( indexBuffer );
   
   // Why baseVertex can be negative?

   // IOS 9.0+, OSX 10.11+
   [renderEncoder drawIndexedPrimitives:primitive
                             indexCount:elements
                              indexType:indexType
                            indexBuffer:indexBuffer->handle
                      indexBufferOffset:(firstIndex * elementSize)
                          instanceCount:max(1U, instances)
                             baseVertex:(NSInteger)firstVertex
                           baseInstance:firstInstance];
      
// [renderEncoder drawIndexedPrimitives:primitive
//                           indexCount:elements
//                            indexType:indexType
//                          indexBuffer:index.handle
//                    indexBufferOffset:(firstIndex * elementSize)
//                        instanceCount:min(1U, instances)];
   }
   
   // IOS 9.0+, OSX 10.11+
   void CommandBufferMTL::drawIndirect(
      const Buffer& indirectBuffer,
      const uint32  firstEntry) const
   {
   const BufferMTL& indirect = reinterpret_cast<const BufferMTL&>(indirectBuffer);
   assert( indirect.apiType == BufferType::Indirect );
   
   // IndirectDrawArgument can be directly cast to MTLDrawPrimitivesIndirectArguments
   [renderEncoder drawPrimitives:primitive
                  indirectBuffer:indirect.handle
            indirectBufferOffset:(firstEntry * sizeof(IndirectDrawArgument))];
   }
   
   // IOS 9.0+, OSX 10.11+
   void CommandBufferMTL::drawIndirectIndexed(
      const Buffer& indirectBuffer,
      const uint32  firstEntry,
      const uint32  firstIndex) const
   {
   const BufferMTL& indirect = reinterpret_cast<const BufferMTL&>(indirectBuffer);
   assert( indirect.apiType == BufferType::Indirect );
   
   assert( indexBuffer );

   // IndirectIndexedDrawArgument can be directly cast to MTLDrawIndexedPrimitivesIndirectArguments
   [renderEncoder drawIndexedPrimitives:primitive
                              indexType:indexType
                            indexBuffer:indexBuffer->handle
                      indexBufferOffset:(firstIndex * elementSize)
                         indirectBuffer:indirect.handle
                   indirectBufferOffset:(firstEntry * sizeof(IndirectIndexedDrawArgument))];
   }


   void CommandBufferMTL::commit(Semaphore* signalSemaphore)
   {
   // TODO: How to translate CommandBuffer execution synchronization to
   //       Swap-Chain presentment from this explicit API to macOS
   //       one based on waitUntilScheduled()?

   assert( !commited );
   [handle commit];
   commited = true;
   }
    
   void CommandBufferMTL::waitUntilCompleted(void)
   {
   [handle waitUntilCompleted];
   }
   
   CommandBufferMTL::~CommandBufferMTL()
   {
   // Finish encoded tasks
   if (!commited)
      [handle commit];
      
   // Don't wait for completion
   
   deallocateObjectiveC(handle);
   }

   shared_ptr<CommandBuffer> MetalDevice::createCommandBuffer(const QueueType type, const uint32 parentQueue)
   {
   // Metal exposes only one logical queue that is universal, and handles load balancing on HW queues on it's own.
   assert( type == QueueType::Universal );
   assert( parentQueue == 0u );
   
   // Buffers and Encoders are single time use  ( in Vulkan CommandBuffers can be recycled / reused !!! )
   // Multiple buffers can be created simultaneously for one queue
   // Buffers are executed in order in queue
   shared_ptr<CommandBufferMTL> buffer = make_shared<CommandBufferMTL>(this); // consider shared_from_this()

   // Acquired Command Buffer is autoreleased.
   buffer->handle = [queue commandBufferWithUnretainedReferences];

   return buffer;
   }
   
   }
}
#endif
