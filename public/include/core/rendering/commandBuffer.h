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

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"

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
   
   class CommandBuffer : public SafeObject<CommandBuffer>
      {
      public:
      virtual void start(const Ptr<Semaphore> waitForSemaphore = nullptr) = 0;

      virtual void startRenderPass(const Ptr<RenderPass> pass, 
                                   const Ptr<Framebuffer> framebuffer) = 0;

      virtual void setDescriptors(const Ptr<PipelineLayout> layout, 
                                  const Ptr<DescriptorSet> set,
                                  const uint32 index = 0u) = 0;

      virtual void setDescriptors(const Ptr<PipelineLayout> layout, 
                                  const uint32 count,
                                  const Ptr<DescriptorSet>* sets,
                                  const uint32 firstIndex = 0u) = 0;

      virtual void setPipeline(const Ptr<Pipeline> pipeline) = 0;
      
      // Assigns Vertex Buffers to specified input attachment slots.
      // Optionally starting offsets in those buffers can be specified.
      virtual void setVertexBuffers(const uint32 count, 
                                    const uint32 firstSlot, 
                                    const Ptr<Buffer>* buffers, 
                                    const uint64* offsets = nullptr) const = 0;

      // Helper method simplifying setting of single Vertex Buffer.
      virtual void setVertexBuffer(const uint32 slot, 
                                   const Ptr<Buffer> buffer, 
                                   const uint64 offset = 0u) const = 0;

      virtual void copy(Ptr<Buffer> source,
                        Ptr<Buffer> destination) = 0;
         
      virtual void copy(Ptr<Buffer> source,
                        Ptr<Buffer> destination,
                        uint64 size,
                        uint64 srcOffset = 0u,
                        uint64 dstOffset = 0u) = 0;

      virtual void copy(Ptr<Buffer> source,
                        const uint64 srcOffset,
                        Ptr<Texture> texture,
                        const uint32 mipmap,
                        const uint32 layer) = 0;

      virtual void copy(Ptr<Buffer> source,
                        Ptr<Texture> texture,
                        const uint32 mipmap,
                        const uint32 layer) = 0;
         
      virtual void draw(const DrawableType primitiveType,
                        const uint32       elements      = 1,      // Elements to process (they are assembled into Primitives)
                        const Ptr<Buffer>  indexBuffer   = Ptr<Buffer>(nullptr), // Optional Index buffer
                        const uint32       instances     = 1,      // Instances to draw
                        const uint32       firstElement  = 0,      // First element to process (or index in Index buffer if specified)
                        const sint32       firstVertex   = 0,      // VertexID from which numeration should start (can be negative)
                        const uint32       firstInstance = 0) = 0; // InstanceID from which numeration should start
         
      virtual void draw(const DrawableType primitiveType,
                        const Ptr<Buffer>  indirectBuffer,         // Buffer from which Draw parameters are sourced
                        const uint32       firstEntry   = 0,       // First entry to process in Indirect buffer
                        const Ptr<Buffer>  indexBuffer  = Ptr<Buffer>(nullptr), // Optional Index buffer
                        const uint32       firstElement = 0) = 0;  // First index to process in Index buffer (if specified)

      virtual void endRenderPass(void) = 0;

      // Optimize the way buffer is beeing accessed during 
      // different operation types, by different units in GPU.
      virtual void barrier(const Ptr<Buffer> buffer, 
                           const uint64 offset,
                           const uint64 size,
                           const BufferAccess currentAccess,
                           const BufferAccess newAccess) = 0;

      // Optimize the way texture is internally stored in memory,
      // by describing the way it was used in the past, and the 
      // way it will be used now.
      virtual void barrier(const Ptr<Texture>  _texture, 
                           const TextureAccess currentAccess,
                           const TextureAccess newAccess) = 0;

      // Optimize the way texture is internally stored in memory,
      // by describing the way it was used in the past, and the 
      // way it will be used now. Specifies only subset of mipmaps
      // and layers to transition (or single mipmap).
      virtual void barrier(const Ptr<Texture>  texture, 
                           const uint32v2      mipmaps, 
                           const uint32v2      layers,
                           const TextureAccess currentAccess,
                           const TextureAccess newAccess) = 0;

      virtual void commit(const Ptr<Semaphore> signalSemaphore = nullptr) = 0;
      virtual void waitUntilCompleted(void) = 0;
      
      virtual ~CommandBuffer() {};                        // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif
