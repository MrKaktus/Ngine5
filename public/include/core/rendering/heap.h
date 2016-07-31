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

namespace en
{
   namespace gpu
   {
   enum class MemoryType : uint32
      {
      Dedicated = 0,   // Alocated on dedicated memory (GPU VRAM on discreete GPU's)
      Shared       ,   // Alocated on shared memory (CPU RAM)
      };

   enum class MemoryUsage : uint32
      {
      Static   = 0,   // Resources will be allocated once and used only by GPU (Static textures, Render Targets)
      Streamed    ,   // Resources will be updated every few frames (Geometry Cache's, Sparse Textures)
      Immediate   ,   // Data that ideally should be updated in the same frame (Uniform's, Push Constants)
      Temporary       // Special memory type for Intermediate Render Targets.
      };

   class Heap 
      {
      public:
      virtual uint32 size(void) const = 0;

      // Create formatted Vertex buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> create(const uint32 elements,
                                 const Formatting& formatting,
                                 const uint32 step = 0,
                                 const void* data = nullptr) = 0;
        
      // Create formatted Index buffer that can be bound to InputAssembler.
      virtual Ptr<Buffer> create(const uint32 elements,
                                 const Attribute format,
                                 const void* data = nullptr) = 0;

      // Create unformatted generic buffer of given type and size.
      // This method can still be used to create Vertex or Index buffers,
      // but it's adviced to use ones with explicit formatting.
      virtual Ptr<Buffer> create(const BufferType type,
                                 const uint32 size,
                                 const void* data = nullptr) = 0;
      
      virtual Ptr<Texture> create(const TextureState state) = 0;

      virtual ~Heap() {};                           // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif