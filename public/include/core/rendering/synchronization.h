/*

 Ngine v5.0
 
 Module      : Synchronization.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.
               
               Cross-process / cross-API surface.

*/

#ifndef ENG_CORE_RENDERING_SYNCHRONIZATION
#define ENG_CORE_RENDERING_SYNCHRONIZATION

#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"

namespace en
{
   namespace gpu
   {
   // Bitmask
   enum class BufferAccess : uint32
      {
      Read                = 0x0001,  // Allows Buffers to be read (as Vertex, Index, Uniform, Indirect or general buffer type)
      Write               = 0x0002,  // Allows Buffers to be written (by Compute Shaders)
      SystemSource        = 0x0080,  // System memory backed resource, used as source of data transfer to dedicated memory.
      SystemDestination   = 0x0100,  // System memory backed resource, used as destination of data transfer from dedicated memory.
      CopySource          = 0x0200,  // Resource backed by dedicated memory. Used as source for Copy or Blit operation.
      CopyDestination     = 0x0400,  // Resource backed by dedicated memory. Used as destination of Copy or Blit operation.
      };

   inline BufferAccess operator| (BufferAccess a, BufferAccess b)
   {
   return static_cast<BufferAccess>(underlyingType(a) | underlyingType(b));
   }

   // Bitmask
   enum class TextureAccess : uint32
      {
      Read                = 0x0001,  // Allows Reading and Sampling Textures by Shaders (if format is Depth-Stencil, allows Depth comparison test read)
      Write               = 0x0002,  // Allows Writing to Textures (by Compute Shaders)
      Atomic              = 0x0004,  // Allows Atomic operations on Texture
      RenderTargetRead    = 0x0008,  // Texture can be source of reads during rendering operations:
                                     // Color         - read via Blening / LogicOp or Subpass load
                                     // Depth-Stencil - read via HiZ, Early-Z and Depth Test
      RenderTargetWrite   = 0x0010,  // Texture can be destination for rendering operations:
                                     // Color         - written via Subpass save or Blening / LogicOp
                                     // Depth-Stencil - written via Depth Write
      ResolveSource       = 0x0020,  // Can be source of MSAA resolve operation.
      ResolveDestination  = 0x0040,  // Can be destination of MSAA resolve operation.
      SystemSource        = 0x0080,  // System memory backed resource, used as source of data transfer to dedicated memory.
      SystemDestination   = 0x0100,  // System memory backed resource, used as destination of data transfer from dedicated memory.
      CopySource          = 0x0200,  // Resource backed by dedicated memory. Used as source for Copy or Blit operation.
      CopyDestination     = 0x0400,  // Resource backed by dedicated memory. Used as destination of Copy or Blit operation.
      Present             = 0x0800   // Resource can be present on window / display. Present cannot be used with any other option.
      };

   inline TextureAccess operator| (TextureAccess a, TextureAccess b)
   {
   return static_cast<TextureAccess>(underlyingType(a) | underlyingType(b));
   }

   // Synchronization primitive. Used to sync CommandBuffers between Queues and each other.
   class Semaphore : public SafeObject<Semaphore>
      {
      public:
      virtual ~Semaphore() {};
      };
   }
}

#endif