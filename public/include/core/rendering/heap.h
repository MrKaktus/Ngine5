/*

 Ngine v5.0
 
 Module      : Heap.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_HEAP
#define ENG_CORE_RENDERING_HEAP

#include <memory>
using namespace std;

#include "core/rendering/buffer.h"
#include "core/rendering/texture.h"

namespace en
{
   namespace gpu
   {
   // MemoryUsage:
   //
   // Linear     - Fastest possible GPU read and write. There is no CPU access
   //              to this memory, thus all resources need to be populated using
   //              Staging buffers from Upload heap. Linear memory usage is
   //              dedicated for Buffers, allocated once and used only by GPU.
   // Tiled      - Similar to Linear memory usage, but this one is dedicated to
   //              Textures allocation and allows them to be backed by Tiled and
   //              Compressed memory (Static Textures etc.).
   // Renderable - Similar to Linear memory usage, but dedicated to allocation
   //              of Render Targets. It's optimal for linear access, and allows
   //              Render Targets specific compressions.Depth and Stencil render
   //              targets should also be allocated on it. Can be used as backup
   //              memory for temporary Render-Targets. May allow resources
   //              aliasing.
   // Upload     - Fastest possible CPU write. Used to stream data to dedicated
   //              memory. Can be mapped. Ideal for resources updated every few
   //              frames (Geometry Cache's, Staging Buffers).
   // Download   - Fastest possible CPU read. Used to stream data from dedicated
   //              memory. Can be mapped. Ideal for resources updated with
   //              results of GPU operations (Screenshots).
   // Immediate  - Fastest possible CPU write, with immediate upload to GPU
   //              memory if possible. Dedicated for fast changing key data like
   //              Push Constants and UBO's. Can be mapped as well. This type of
   //              memory shouldn't be used to read data back from GPU as it
   //              will have performance impact.
   //
   enum class MemoryUsage : uint32
      {
      Linear      = 0,
      Tiled          ,
      Renderable     ,
      Upload         ,
      Download       ,
      Immediate
      };

   class GpuDevice;

   class Heap : public enable_shared_from_this<Heap>
      {
      public:
      virtual uint32 size(void) const = 0;

      // Return parent device
      virtual shared_ptr<GpuDevice> device(void) const = 0;

      // Create formatted buffer for storing array of structures (AOS). In most
      // cases it will be used as Vertex buffer that matches given InputLayout,
      // and can be set on CommandBuffer as input to InputAssembler stage.
      // Each element of array is a tightly packed structure containing up to
      // MaxInputLayoutAttributesCount of variables. Each such variable can be
      // treated as a column and each element as a row in data array. Each
      // column has it's specified format, and can be a scalar or vector
      // containing up to 4 channels. Elements creating array can be used to
      // store for e.g. Vertices, Control Points, or other data. 
      virtual shared_ptr<Buffer> createBuffer(
         const uint32 elements,
         const Formatting& formatting,
         const uint32 step = 0u) = 0;
        
      // Create formatted Index buffer that can be bound to InputLayout.
      virtual shared_ptr<Buffer> createBuffer(
         const uint32 elements,
         const Attribute format) = 0;

      // Create unformatted generic buffer of given type and size. This method
      // can still be used to create Vertex or Index buffers, but it's adviced
      // to use ones with explicit formatting.
      virtual shared_ptr<Buffer> createBuffer(
         const BufferType type,
         const uint32 size) = 0;

      // Creates textures on Heaps with MemoryUsage::Tiled or Renderable.
      virtual shared_ptr<Texture> createTexture(
         const TextureState state) = 0;

      // Resources sub-allocated from heap, keep internal reference to it, so 
      // heap won't be released until last resource created from it wont be 
      // released as well.
      virtual ~Heap() {};
      };
   }
}
#endif

