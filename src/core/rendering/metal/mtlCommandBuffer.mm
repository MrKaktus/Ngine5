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

   void CommandBufferMTL::start(const Ptr<Semaphore> waitForSemaphore)
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
   
 
   void CommandBufferMTL::startRenderPass(const Ptr<RenderPass> pass, const Ptr<Framebuffer> _framebuffer)
   {
   assert( renderEncoder == nil );
  
   RenderPassMTL*  renderPass  = raw_reinterpret_cast<RenderPassMTL>(&pass);
   FramebufferMTL* framebuffer = raw_reinterpret_cast<FramebufferMTL>(&_framebuffer);
   
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
            minLayersCount = framebuffer->color[i].arrayLength;
         }
      else // TODO: Uncomment once 2DMSArray is supported
      if (0 /* framebuffer->color[i].textureType == MTLTextureType2DMultisampleArray */)
         {
         layeredRendering = true;
         if (framebuffer->color[i].arrayLength < minLayersCount)
            minLayersCount = framebuffer->color[i].arrayLength;
            
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
         minLayersCount = framebuffer->depthStencil[0].arrayLength;
      }
   if (framebuffer->depthStencil[1])
      if (framebuffer->depthStencil[1].textureType == MTLTextureType2DArray)
         {
         layeredRendering = true;
         if (framebuffer->depthStencil[1].arrayLength < minLayersCount)
            minLayersCount = framebuffer->depthStencil[1].arrayLength;
         }
#if defined(EN_PLATFORM_IOS)
   // TODO: Uncomment once 2DMSArray is supported
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


   void CommandBufferMTL::setVertexBuffers(const uint32 count, const uint32 firstSlot, const Ptr<Buffer>* buffers, const uint64* offsets) const
   {
   assert( count );
   // assert( (firstSlot + count) <= gpu->properties.limits.maxVertexInputBindings ); TODO: Populate GPU support field depending on iOS and macOS limitations and check it here

   // Extrack Metal buffer handles
   id<MTLBuffer>* handles = new id<MTLBuffer>[count];   // TODO: Optimize by allocating on the stack maxBuffersCount sized fixed array.
   for(uint32 i=0; i<count; ++i)
      {
      assert( buffers[i] );
      handles[i] = ptr_dynamic_cast<BufferMTL, Buffer>(buffers[i])->handle;
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

   void CommandBufferMTL::setVertexBuffer(const uint32 slot, const Ptr<Buffer> buffer, const uint64 offset) const
   {
   [renderEncoder setVertexBuffer:ptr_dynamic_cast<BufferMTL, Buffer>(buffer)->handle 
                           offset:offset 
						  atIndex:slot];
   }


   // TRANSFER COMMANDS
   //////////////////////////////////////////////////////////////////////////


   // Copies content of source buffer to destination buffer
   void CommandBufferMTL::copy(Ptr<Buffer> source, Ptr<Buffer> destination)
   {
   assert( source );
   assert( destination );
   assert( source->length() <= destination->length() );

   copy(source, destination, source->length(), 0u, 0u);
   }
   
   void CommandBufferMTL::copy(Ptr<Buffer> source,
      Ptr<Buffer> destination,
      uint64 size,
      uint64 srcOffset,
      uint64 dstOffset)
   {
   assert( source );
   assert( source->type() == BufferType::Transfer );
   assert( destination );
   assert( (srcOffset + size) <= source->length() );
   assert( (dstOffset + size) <= destination->length() );
   
   BufferMTL* src = raw_reinterpret_cast<BufferMTL>(&source);
   BufferMTL* dst = raw_reinterpret_cast<BufferMTL>(&destination);

   @autoreleasepool
      {
      // Blit to Private buffer
      id <MTLBlitCommandEncoder> blit = [handle blitCommandEncoder];
      [blit copyFromBuffer:src->handle
              sourceOffset:src->offset + srcOffset
                  toBuffer:dst->handle
         destinationOffset:dst->offset + dstOffset
                      size:size];

      [blit endEncoding];
      //blit = nil;
      } // autoreleasepool
   
   }

   void CommandBufferMTL::copy(Ptr<Buffer> transfer, const uint64 srcOffset, Ptr<Texture> texture, const uint32 mipmap, const uint32 layer)
   {
   assert( transfer );
   assert( transfer->type() == BufferType::Transfer );
   assert( texture );
   assert( texture->mipmaps() > mipmap );
   assert( texture->layers() > layer );
   
   BufferMTL*  source      = raw_reinterpret_cast<BufferMTL>(&transfer);
   TextureMTL* destination = raw_reinterpret_cast<TextureMTL>(&texture);

   assert( source->size >= srcOffset + destination->size(mipmap) );

   LinearAlignment layout = gpu->textureLinearAlignment(texture, mipmap, layer);

      // Blit to Private buffer
      id <MTLBlitCommandEncoder> blit = [handle blitCommandEncoder];
      [blit copyFromBuffer:source->handle
              sourceOffset:source->offset + srcOffset
         sourceBytesPerRow:layout.rowSize
       sourceBytesPerImage:layout.size
                sourceSize:MTLSizeMake(destination->width(mipmap), destination->height(mipmap), 1)
                 toTexture:destination->handle
          destinationSlice:layer
          destinationLevel:mipmap
         destinationOrigin:MTLOriginMake(0, 0, 0)
                   options:MTLBlitOptionNone ];
      
      [blit endEncoding];
      
      deallocateObjectiveC(blit);
   }

   // Copies content of buffer, to given mipmap and layer of destination texture
   void CommandBufferMTL::copy(Ptr<Buffer> transfer, Ptr<Texture> texture, const uint32 mipmap, const uint32 layer)
   {
   copy(transfer, 0u, texture, mipmap, layer);
   }


   // DRAW COMMANDS
   //////////////////////////////////////////////////////////////////////////


   void CommandBufferMTL::draw(const uint32       elements,
                               const Ptr<Buffer>  indexBuffer,
                               const uint32       instances,
                               const uint32       firstElement,
                               const sint32       firstVertex,
                               const uint32       firstInstance)
   {
   assert( elements );
 
   // Elements are assembled into Primitives.
   if (indexBuffer)
      {
      BufferMTL* index = raw_reinterpret_cast<BufferMTL>(&indexBuffer);
      
      assert( index->apiType == BufferType::Index );
      
      uint32 elementSize = 2;
      MTLIndexType indexType = MTLIndexTypeUInt16;
      if (index->formatting.column[0] == Attribute::u32)
         {
         elementSize = 4;
         indexType = MTLIndexTypeUInt32;
         }

      // Why baseVertex can be negative?

      // IOS 9.0+, OSX 10.11+
      [renderEncoder drawIndexedPrimitives:primitive
                                indexCount:elements
                                 indexType:indexType
                               indexBuffer:index->handle
                         indexBufferOffset:(firstElement * elementSize)
                             instanceCount:max(1U, instances)
                                baseVertex:(NSInteger)firstVertex
                              baseInstance:firstInstance];
         
//      [renderEncoder drawIndexedPrimitives:primitive
//                                indexCount:elements
//                                 indexType:indexType
//                               indexBuffer:index->handle
//                         indexBufferOffset:(firstElement * elementSize)
//                             instanceCount:min(1U, instances)];
      }
   else
      {
      // IOS 9.0+, OSX 10.11+
      [renderEncoder drawPrimitives:primitive
                        vertexStart:firstElement
                        vertexCount:elements
                      instanceCount:max(1U, instances)
                       baseInstance:firstInstance];

//      [renderEncoder drawPrimitives:primitive
//                        vertexStart:firstElement
//                        vertexCount:elements
//                      instanceCount:min(1U, instances)];
      }
   }

   // IOS 9.0+, OSX 10.11+
   void CommandBufferMTL::draw(const Ptr<Buffer>  indirectBuffer,
                               const uint32       firstEntry,
                               const Ptr<Buffer>  indexBuffer,
                               const uint32       firstElement)
   {
   assert( indirectBuffer );
   
   BufferMTL* indirect = raw_reinterpret_cast<BufferMTL>(&indirectBuffer);
   assert( indirect->apiType == BufferType::Indirect );
   
   if (indexBuffer)
      {
      BufferMTL* index = raw_reinterpret_cast<BufferMTL>(&indexBuffer);
      assert( index->apiType == BufferType::Index );
      
      uint32 elementSize = 2;
      MTLIndexType indexType = MTLIndexTypeUInt16;
      if (index->formatting.column[0] == Attribute::u32)
         {
         elementSize = 4;
         indexType = MTLIndexTypeUInt32;
         }

      // IndirectIndexedDrawArgument can be directly cast to MTLDrawIndexedPrimitivesIndirectArguments
      [renderEncoder drawIndexedPrimitives:primitive
                                 indexType:indexType
                               indexBuffer:index->handle
                         indexBufferOffset:(firstElement * elementSize)
                            indirectBuffer:indirect->handle
                      indirectBufferOffset:(firstEntry * sizeof(IndirectIndexedDrawArgument))];
      }
   else
      {
      // IndirectDrawArgument can be directly cast to MTLDrawPrimitivesIndirectArguments
      [renderEncoder drawPrimitives:primitive
                     indirectBuffer:indirect->handle
               indirectBufferOffset:(firstEntry * sizeof(IndirectDrawArgument))];
      }
   }



















   




   
   void CommandBufferMTL::commit(const Ptr<Semaphore> signalSemaphore)
   {
   // TODO: Do we need to synchronize CommandBuffer execution with Swap-Chain ?
   
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

   Ptr<CommandBuffer> MetalDevice::createCommandBuffer(const QueueType type, const uint32 parentQueue)
   {
   // Metal exposes only one logical queue that is universal, and handles load balancing on HW queues on it's own.
   assert( type == QueueType::Universal );
   assert( parentQueue == 0u );
   
   // Buffers and Encoders are single time use  ( in Vulkan CommandBuffers can be recycled / reused !!! )
   // Multiple buffers can be created simultaneously for one queue
   // Buffers are executed in order in queue
   Ptr<CommandBufferMTL> buffer = new CommandBufferMTL(this);

   // Acquired Command Buffer is autoreleased.
   buffer->handle = [queue commandBufferWithUnretainedReferences];

   return ptr_reinterpret_cast<CommandBuffer>(&buffer);
   }
   
   }
}
#endif
