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

#include "core/rendering/buffer.h"
#include "core/rendering/texture.h"
#include "core/utilities/TintrusivePointer.h"

namespace en
{
   namespace gpu
   {
   // MemoryUsage:
   //
   // Static    - Fastest possible GPU read and write. There is no CPU access to this kind of memory,
   //             thus all resources need to be populated using Staging buffers. This is the only type
   //             of memory that allows textures creation. Buffers can be created as well.
   // Streamed  - Fastest possible CPU read and write. Used to stream data to Static memory. Can be mapped.
   // Immediate - Fastest possible CPU write, with immediate upload to GPU memory if possible. Dedicated
   //             for fast changing key data like Push Constants and UBO's. Can be mapped as well.
   // Temporary - Fastest possible allocation, GPU read and write. Backup memory for temporary Render-Targets.
   //
   enum class MemoryUsage : uint32
      {
      Static   = 0,   // Resources will be allocated once and used only by GPU (Static Textures, Render Targets)
      Streamed    ,   // Resources will be updated every few frames (Geometry Cache's, Staging Buffers)
      Immediate   ,   // Data that ideally should be updated in the same frame (Uniform's, Push Constants)
      Temporary       // Use as a backup memory for intermediate Render Targets.
      };

   class GpuDevice;

   class Heap : public SafeObject<Heap>
      {
      public:
      virtual uint32 size(void) const = 0;

      // Return parent device
      virtual Ptr<GpuDevice> device(void) const = 0;

      // Create formatted Vertex buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> createBuffer(const uint32 elements,
                                       const Formatting& formatting,
                                       const uint32 step = 0u) = 0;
        
      // Create formatted Index buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> createBuffer(const uint32 elements,
                                       const Attribute format) = 0;

      // Create unformatted generic buffer of given type and size.
      // This method can still be used to create Vertex or Index buffers,
      // but it's adviced to use ones with explicit formatting.
      virtual Ptr<Buffer> createBuffer(const BufferType type,
                                       const uint32 size) = 0;

      // Creates textures on Heaps with MemoryUsage::Static.
      virtual Ptr<Texture> createTexture(const TextureState state) = 0;

      virtual ~Heap() {};                           // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif

