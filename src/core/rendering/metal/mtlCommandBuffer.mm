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

#include "core/log/log.h"
#include "core/rendering/metal/mtlDevice.h"


#include "core/rendering/metal/mtlCommandBuffer.h"

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

   CommandBufferMTL::CommandBufferMTL(const id<MTLDevice> _device) :
      device(_device),
      handle(nil),
      renderEncoder(nil),
      commited(false)
   {
   }

   void CommandBufferMTL::bind(const Ptr<RasterState> raster)
   {
   
   }
   
 //void CommandBufferMTL::bind(const Ptr<ViewportScissorState> viewportScissor);
 
   void CommandBufferMTL::bind(const Ptr<DepthStencilState> depthStencil)
   {
   
   }
   
   void CommandBufferMTL::bind(const Ptr<BlendState> blend)
   {
   
   }


   // RENDER PASS
   //////////////////////////////////////////////////////////////////////////
   
 
   bool CommandBufferMTL::startRenderPass(const Ptr<RenderPass> pass)
   {
   if (renderEncoder != nil)
      return false;
    
   const Ptr<RenderPassMTL> temp = ptr_dynamic_cast<RenderPassMTL, RenderPass>(pass);
   
   renderEncoder = [handle renderCommandEncoderWithDescriptor:temp->desc];
   return true;
   }
   
   bool CommandBufferMTL::endRenderPass(void)
   {
   if (renderEncoder == nil)
      return false;
   [renderEncoder endEncoding];
   [renderEncoder release];
   renderEncoder = nullptr;
   return true;
   }
   
   
   // DRAW COMMANDS
   //////////////////////////////////////////////////////////////////////////


   // Types of primitives to draw
   const MTLPrimitiveType TranslateDrawableType[DrawableTypesCount]
      {
      MTLPrimitiveTypePoint                , // Points
      MTLPrimitiveTypeLine                 , // Lines
      MTLPrimitiveTypeLineStrip            , // LineLoops       (unsupported)
      MTLPrimitiveTypeLineStrip            , // LineStripes
      MTLPrimitiveTypeTriangle             , // Triangles
      MTLPrimitiveTypeTriangleStrip        , // TriangleFans    (unsupported)
      MTLPrimitiveTypeTriangleStrip        , // TriangleStripes
      MTLPrimitiveTypeTriangleStrip        , // Patches         (unsupported)
      };

   void CommandBufferMTL::draw(const DrawableType primitiveType,
                               const uint32       elements,
                               const Ptr<Buffer>  indexBuffer,
                               const uint32       instances,
                               const uint32       firstElement,
                               const sint32       firstVertex,
                               const uint32       firstInstance)
   {
   MTLPrimitiveType primitive = TranslateDrawableType[primitiveType];
   
   // Elements are assembled into Primitives.
   if (indexBuffer)
      {
      Ptr<BufferMTL> index = ptr_dynamic_cast<BufferMTL, Buffer>(indexBuffer);
      
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
   void CommandBufferMTL::draw(const DrawableType primitiveType,
                               const Ptr<Buffer>  indirectBuffer,
                               const uint32       firstEntry,
                               const Ptr<Buffer>  indexBuffer,
                               const uint32       firstElement)
   {
   assert( indirectBuffer );
   
   MTLPrimitiveType primitive = TranslateDrawableType[primitiveType];
   Ptr<BufferMTL> indirect = ptr_dynamic_cast<BufferMTL, Buffer>(indirectBuffer);
   assert( indirect->apiType == BufferType::Indirect );
   
   if (indexBuffer)
      {
      Ptr<BufferMTL> index = ptr_dynamic_cast<BufferMTL, Buffer>(indexBuffer);
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



















   
   
   void CommandBufferMTL::setVertexBuffer(Ptr<Buffer> buffer, uint32 slot)
   {
   assert( buffer );
   Ptr<BufferMTL> ptr = ptr_dynamic_cast<BufferMTL, Buffer>(buffer);
   [renderEncoder setVertexBuffer:ptr->handle offset:0 atIndex:(NSUInteger)slot];
   
// - (void)setVertexBuffer:(nullable id <MTLBuffer>)buffer offset:(NSUInteger)offset atIndex:(NSUInteger)index;
// - (void)setVertexBufferOffset:(NSUInteger)offset atIndex:(NSUInteger)index NS_AVAILABLE(10_11, 8_3);
// - (void)setVertexBuffers:(const id <MTLBuffer> __nullable [__nullable])buffers offsets:(const NSUInteger [__nullable])offsets withRange:(NSRange)range;
   }
   
/* Init Resoure */

   // Populate Private buffer before use (first time only)
   bool CommandBufferMTL::populate(Ptr<Buffer> transfer, Ptr<Buffer> buffer)
   {
   assert( transfer );
   assert( transfer->type() == BufferType::Transfer );
   assert( buffer );

   Ptr<BufferMTL> source      = ptr_dynamic_cast<BufferMTL, Buffer>(transfer);
   Ptr<BufferMTL> destination = ptr_dynamic_cast<BufferMTL, Buffer>(buffer);

   assert( source->size >= destination->size );

   @autoreleasepool
   {
      // Blit to Private buffer
      id <MTLBlitCommandEncoder> blit = [handle blitCommandEncoder];
      [blit copyFromBuffer:source->handle
              sourceOffset:0
                  toBuffer:destination->handle
         destinationOffset:0
                      size:destination->size];

      [blit endEncoding];
      blit = nil;
   } // autoreleasepool
   
   return true;
   }

   // Populate Private texture before use (first time only)
   bool CommandBufferMTL::populate(Ptr<Buffer> transfer, Ptr<Texture> texture, uint32 mipmap, uint32 layer)
   {
   assert( transfer );
   assert( transfer->type() == BufferType::Transfer );
   assert( texture );
   assert( texture->mipmaps() > mipmap );
   assert( texture->layers() > layer );
   
   Ptr<BufferMTL>  source      = ptr_dynamic_cast<BufferMTL, Buffer>(transfer);
   Ptr<TextureMTL> destination = ptr_dynamic_cast<TextureMTL, Texture>(texture);

   assert( source->size >= destination->size(mipmap) );

   @autoreleasepool
   {
      // Blit to Private buffer
      id <MTLBlitCommandEncoder> blit = [handle blitCommandEncoder];
      [blit copyFromBuffer:source->handle
              sourceOffset:0
         sourceBytesPerRow:destination->width(mipmap)
       sourceBytesPerImage:destination->size(mipmap)
                sourceSize:MTLSizeMake(destination->width(mipmap), destination->height(mipmap), 1)
                 toTexture:destination->handle
          destinationSlice:layer
          destinationLevel:mipmap
         destinationOrigin:MTLOriginMake(0, 0, 0)
                   options:MTLBlitOptionNone ];
      
      [blit endEncoding];
      blit = nil;
   } // autoreleasepool
   
   return true;
   }

/* Blitting */

   // TODO


   
   void CommandBufferMTL::commit(void)
   {
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
   
   // Release buffer
   [handle release];
   }

   Ptr<CommandBuffer> MetalDevice::createCommandBuffer(const QueueType type, const uint32 parentQueue)
   {
   // Metal exposes only one logical queue that is universal, and handles load balancing on HW queues on it's own.
   assert( type == QueueType::Universal );
   assert( parentQueue == 0u );
   
   // Buffers and Encoders are single time use  ( in Vulkan CommandBuffers they can be recycled / reused !!! )
   // Multiple buffers can be created simultaneously for one queue
   // Buffers are executed in order in queue
   Ptr<CommandBufferMTL> buffer = new CommandBufferMTL(device);
   
   buffer->handle = [queue commandBuffer];
   
   return ptr_dynamic_cast<CommandBuffer, CommandBufferMTL>(buffer);
   }
   
   }
}
