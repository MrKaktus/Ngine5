/*

 Ngine v5.0
 
 Module      : Command Buffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_COMMAND_BUFFER
#define ENG_CORE_RENDERING_COMMAND_BUFFER

#include <memory>
using namespace std;

#include "core/defines.h"
#include "core/types.h"

#include "core/rendering/buffer.h"
#include "core/rendering/texture.h"
#include "core/rendering/blend.h"
#include "core/rendering/raster.h"
#include "core/rendering/multisampling.h"
#include "core/rendering/viewport.h"
#include "core/rendering/depthStencil.h"
#include "core/rendering/pipeline.h"
#include "core/rendering/renderPass.h"
#include "core/rendering/pipeline.h"
#include "core/rendering/synchronization.h"

namespace en
{
   namespace gpu
   {   
   typedef struct
      {
      uint32 elements;
      uint32 instances;
      uint32 firstElement;
      uint32 firstInstance;
      } IndirectDrawArgument;

   typedef struct
      {
      uint32 elements;        // Elements to process (indexes in Index buffer)
      uint32 instances;       // Instances to process
      uint32 firstElement;    // Element to start from (index in Index buffer)
      sint32 firstVertex;     // Starting VertexID (can be negative)
      uint32 firstInstance;   // Starting InstanceID
      } IndirectIndexedDrawArgument;
   
   class CommandBuffer : public enable_shared_from_this<CommandBuffer>
      {
      public:
      virtual void start(const shared_ptr<Semaphore> waitForSemaphore = nullptr) = 0;

      virtual void startRenderPass(const shared_ptr<RenderPass> pass, 
                                   const shared_ptr<Framebuffer> framebuffer) = 0;

      virtual void setDescriptors(const PipelineLayout& layout,
                                  const DescriptorSet& set,
                                  const uint32 index = 0u) = 0;

      virtual void setDescriptors(const PipelineLayout& layout, 
                                  const uint32 count,
                                  const shared_ptr<DescriptorSet>(&sets)[],
                                  const uint32 firstIndex = 0u) = 0;

      virtual void setPipeline(const Pipeline& pipeline) = 0;
      
      // Assigns Vertex Buffers to specified input attachment slots.
      // Optionally starting offsets in those buffers can be specified.
      virtual void setVertexBuffers(const uint32 count, 
                                    const uint32 firstSlot, 
                                    const shared_ptr<Buffer>(&buffers)[],
                                    const uint64* offsets = nullptr) const = 0;

      // Helper method simplifying setting of single Vertex Buffer.
      virtual void setVertexBuffer(const uint32 slot, 
                                   const Buffer& buffer, 
                                   const uint64 offset = 0u) const = 0;

      virtual void copy(const Buffer& source,
                        const Buffer& destination) = 0;
         
      virtual void copy(const Buffer& source,
                        const Buffer& destination,
                        const uint64 size,
                        const uint64 srcOffset = 0u,
                        const uint64 dstOffset = 0u) = 0;

      // Before copying data from staging buffer to destination texture
      // application should use below GpuDevice method to obtain alignment
      // and padding layout of data in that staging buffer:
      //
      // LinearAlignment textureLinearAlignment(const Texture& texture, 
      //                                        const uint32 mipmap, 
      //                                        const uint32 layer);
      //
      virtual void copy(const Buffer& source,
                        const uint64 srcOffset,
                        const Texture& texture,
                        const uint32 mipmap,
                        const uint32 layer) = 0;

      virtual void copy(const Buffer& source,
                        const Texture& texture,
                        const uint32 mipmap,
                        const uint32 layer) = 0;
         
                                                               // Primitive Type is specified in bound Pipeline State Object
      virtual void draw(const uint32  elements,                // Elements to process (they are assembled into Primitives)
                        const Buffer* indexBuffer   = nullptr, // Optional Index buffer
                        const uint32  instances     = 1,       // Instances to draw
                        const uint32  firstElement  = 0,       // First element to process (or index in Index buffer if specified)
                        const sint32  firstVertex   = 0,       // First Per-Vertex entry in VBO from which numeration should start (can be negative)
                        const uint32  firstInstance = 0) = 0;  // First Per-Instance entry in VBO from which numeration should start
         
                                                              // Primitive Type is specified in bound Pipeline State Object
      virtual void draw(const Buffer& indirectBuffer,         // Buffer from which Draw parameters are sourced
                        const uint32  firstEntry   = 0,       // First entry to process in Indirect buffer
                        const Buffer* indexBuffer  = nullptr, // Optional Index buffer
                        const uint32  firstElement = 0) = 0;  // First index to process in Index buffer (if specified)

      virtual void endRenderPass(void) = 0;

      // For each newly created buffer resource, initial
      // barrier need to be called to transfer it to valid
      // access state.
      virtual void barrier(const Buffer& buffer,
                           const BufferAccess initAccess) = 0;

      // Optimize the way buffer is beeing accessed during 
      // different operation types, by different units in GPU.
      virtual void barrier(const Buffer& buffer,
                           const uint64 offset,
                           const uint64 size,
                           const BufferAccess currentAccess,
                           const BufferAccess newAccess) = 0;

      // For each newly created texture resource, initial
      // barrier need to be called to transfer it to valid
      // access state.
      virtual void barrier(const Texture& texture,
                           const TextureAccess initAccess) = 0;

      // Optimize the way texture is internally stored in memory,
      // by describing the way it was used in the past, and the 
      // way it will be used now.
      virtual void barrier(const Texture& texture,
                           const TextureAccess currentAccess,
                           const TextureAccess newAccess) = 0;

      // Optimize the way texture is internally stored in memory,
      // by describing the way it was used in the past, and the 
      // way it will be used now. Specifies only subset of mipmaps
      // and layers to transition (or single mipmap).
      virtual void barrier(const Texture& texture,
                           const uint32v2 mipmaps,
                           const uint32v2 layers,
                           const TextureAccess currentAccess,
                           const TextureAccess newAccess) = 0;

      virtual void commit(const shared_ptr<Semaphore> signalSemaphore = nullptr) = 0;
      
      // Incurrs full CPU-GPU synchronization.
      virtual void waitUntilCompleted(void) = 0;
      
      virtual ~CommandBuffer() {};                        // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif
