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
   enum class MemoryUsage : uint32
      {
      Static   = 0,   // Resources will be allocated once and used only by GPU (Static textures, Render Targets)
      Streamed    ,   // Resources will be updated every few frames (Geometry Cache's, Sparse Textures)
      Immediate   ,   // Data that ideally should be updated in the same frame (Uniform's, Push Constants)
      Temporary       // Special memory type for Intermediate Render Targets.
      };

   class Heap : public SafeObject<Heap>
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
                                 const uint32 size) = 0;
      
      // Create unformatted generic buffer of given type and size.
      // This is specialized method, that allows passing pointer
      // to data, to directly initialize buffer content.
      // It is allowed on mobile devices conforming to UMA architecture.
      // On Discreete GPU's with NUMA architecture, only Transient buffers
      // can be created with it.
      virtual Ptr<Buffer> create(const BufferType type,
                                 const uint32 size,
                                 const void* data) = 0;
   
      virtual Ptr<Texture> create(const TextureState state) = 0;

      virtual ~Heap() {};                           // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif

