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

#include "core/defines.h"
#include "core/types.h"

#include "core/rendering/blend.h"
#include "core/rendering/buffer.h"
#include "core/rendering/depthStencil.h"
#include "core/rendering/multisampling.h"
#include "core/rendering/pipeline.h"
#include "core/rendering/raster.h"
#include "core/rendering/renderPass.h"
#include "core/rendering/synchronization.h"
#include "core/rendering/texture.h"
#include "core/rendering/viewport.h"

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
   
// Try to limit amount of Command Buffers to 15-30 per frame, and submit
// them in batches, to limit submissions to ~5 per queue per frame (engine
// currently is not supporting CommandBuffer batch submissions).
class CommandBuffer : public std::enable_shared_from_this<CommandBuffer>
{
    public:
    
    
    // Command Buffeer lifecycle:
    
    
    virtual void start(
        const Semaphore* waitForSemaphore = nullptr) = 0;

    virtual void commit(
        Semaphore* signalSemaphore = nullptr) = 0;
    
    /// Incurrs full CPU-GPU synchronization.
    virtual void waitUntilCompleted(void) = 0;
    
    
    // Data transfers:
    
    
    virtual void copy(
        const Buffer& source,
        const Buffer& destination) = 0;
       
    virtual void copy(
        const Buffer& source,
        const Buffer& destination,
        const uint64 size,
        const uint64 srcOffset = 0u,
        const uint64 dstOffset = 0u) = 0;

    /// Before copying data from staging buffer to destination texture
    /// application should use below GpuDevice method to obtain required
    /// alignment and padding layout for data in that staging buffer:
    ///
    /// ImageMemoryAlignment textureMemoryAlignment(const TextureState& state,
    ///                                             const uint32 mipmap,
    ///                                             const uint32 layer);
    ///
    virtual void copy(
        const Buffer&  source,
        const uint64   srcOffset,
        const uint32   srcRowPitch,
        const Texture& texture,
        const uint32   mipmap,
        const uint32   layer) = 0;

    /// Copies data from staging buffer to destination texture, replacing only
    /// specified rectangular area. Layout specifies how data is structured in
    /// source buffer. Layout size needs to match size of replaced region in
    /// bytes, and layout rowSize needs to match size of single line, or single
    /// row of compressed blocks. Layout alignment is ignored as it's driven
    /// by srcOffset. In multi-plane surfaces, plane to copy needs to be specified.
    virtual void copyRegion2D(
        const Buffer& source,
        const uint64 srcOffset,
        const uint32 srcRowPitch,
        const Texture& texture,
        const uint32 mipmap,
        const uint32 layer, // Array layer, layer+face, face
        const uint32v2 origin,
        const uint32v2 region,
        const uint8 plane = 0) = 0;
       

    // Resource transitions:
    
    
    /// For each newly created buffer resource, initial barrier need to be
    /// called to transfer it to valid access state.
    virtual void barrier(
        const Buffer& buffer,
        const BufferAccess initAccess) = 0;

    /// Optimize the way buffer is being accessed during different operation
    /// types, by different units in GPU.
    virtual void barrier(
        const Buffer& buffer,
        const uint64 offset,
        const uint64 size,
        const BufferAccess currentAccess,
        const BufferAccess newAccess) = 0;

    /// For each newly created texture resource, initial barrier need to be
    /// called to transfer it to valid access state.
    virtual void barrier(
        const Texture& texture,
        const TextureAccess initAccess) = 0;

    /// Change the way texture is internally stored in memory, to optimize
    /// access to it. Do that by describing the way it was used in the past,
    /// and the way it will be used now.
    virtual void barrier(
        const Texture& texture,
        const TextureAccess currentAccess,
        const TextureAccess newAccess) = 0;

    /// Change the way texture is internally stored in memory, to optimize
    /// access to it. Do that by describing the way it was used in the past,
    /// and the way it will be used now. Specifies only subset of mipmaps and
    /// layers to transition (or single mipmap).
    virtual void barrier(
        const Texture& texture,
        const uint32v2 mipmaps,
        const uint32v2 layers,
        const TextureAccess currentAccess,
        const TextureAccess newAccess) = 0;


    // Render pass:
      
      
    virtual void startRenderPass(
        const RenderPass& pass,
        const Framebuffer& framebuffer) = 0;

    virtual void endRenderPass(void) = 0;


    // Binding resources:


    virtual void setPipeline(
        const Pipeline& pipeline) = 0;
    
    /// Descriptors need to be set after pipeline, so that
    /// current expected layout (based on pipeline) is known.
    virtual void setDescriptors(
        const PipelineLayout& layout,
        const DescriptorSet& set,
        const uint32 index = 0u) = 0;

    virtual void setDescriptors(
        const PipelineLayout& layout,
        const uint32 count,
        const DescriptorSet*(&sets)[],
        const uint32 firstIndex = 0u) = 0;

    /// Input Assembler:
    /// There is no performance loss from leaving input buffers bound from
    /// previous draw calls, if current Pipeline is not using those buffers
    /// on input. Input Assembler won't fetch data from them, so there is
    /// no need to unbind them.
   
    /// Assigns Vertex Buffers to specified input attachment slots.
    /// Optionally starting offsets in those buffers can be specified.
    virtual void setVertexBuffers(
        const uint32 firstSlot,
        const uint32 count,
        const std::shared_ptr<Buffer>(&buffers)[],
        const uint64* offsets = nullptr) const = 0;

    /// Assign single Vertex Buffer to range of Input Assembler slots.
    /// For each slot specify optional starting offset.
    virtual void setInputBuffer(
        const uint32  firstSlot,
        const uint32  slots,
        const Buffer& buffer,
        const uint64* offsets = nullptr) const = 0;

  ///// Assign single Vertex Buffer to set of Input Assembler slots.
  ///// Selected slots are marked with set bit in slotsMask bitmask.
  ///// For each slot specify optional starting offset (if offsets 
  ///// array is provided, it needs to store count of offsets, equal
  ///// to count of set bits in the bitmask).
  //virtual void setInputBuffer(
  //    const Buffer& buffer,
  //    const uint32  slotsMask,
  //    const uint64* offsets = nullptr) const = 0;

    /// Helper method simplifying setting of single Vertex Buffer.
    virtual void setVertexBuffer(
        const uint32 slot,
        const Buffer& buffer,
        const uint64 offset = 0u) const = 0;

    /// Index Buffer remains bound to Command Buffer after this call.
    virtual void setIndexBuffer(
        const Buffer& buffer,
        const Attribute type,
        const uint32 offset = 0u) = 0;


    // GPU work dispatch:
       
       
    /// Primitive Type is specified in bound Pipeline State Object.
    /// elements      - Elements to process (they are assembled into Primitives)
    /// instances     - Instances to draw
    /// firstVertex   - First vertex in Vertex Buffer from which processing 
    ///                 should start
    /// firstInstance - First instance in Instance buffer to process
    virtual void draw(
        const uint32  elements,
        const uint32  instances     = 1,
        const uint32  firstVertex   = 0,
        const uint32  firstInstance = 0) const = 0;

    /// Primitive Type is specified in bound Pipeline State Object.
    /// elements      - Elements to process (they are assembled into Primitives)
    /// instances     - Instances to draw
    /// firstIndex    - First index in Index buffer to process (if specified)
    /// firstVertex   - First vertex in Vertex Buffer from which processing 
    ///                 should start (added to index from index buffer, can be 
    ///                 negative).
    /// firstInstance - First instance in Instance buffer to process
    virtual void drawIndexed(
        const uint32  elements,
        const uint32  instances     = 1,
        const uint32  firstIndex    = 0,
        const sint32  firstVertex   = 0,
        const uint32  firstInstance = 0) const = 0;
       
    /// Primitive Type is specified in bound Pipeline State Object.
    /// indirectBuffer - Buffer from which Draw parameters are sourced
    /// firstEntry     - First entry to process in Indirect buffer
    virtual void drawIndirect(
        const Buffer& indirectBuffer,
        const uint32  firstEntry    = 0) const = 0;

    /// Primitive Type is specified in bound Pipeline State Object.
    /// indirectBuffer - Buffer from which Draw parameters are sourced
    /// firstEntry     - First entry to process in Indirect buffer
    /// firstIndex     - First index to process in Index buffer
    virtual void drawIndirectIndexed(
        const Buffer& indirectBuffer,
        const uint32  firstEntry    = 0,
        const uint32  firstIndex    = 0) const = 0;
       
    virtual ~CommandBuffer() {};
};

} // en::gpu
} // en

#endif
