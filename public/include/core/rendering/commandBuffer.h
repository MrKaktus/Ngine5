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
      virtual void bind(const Ptr<RasterState> raster) = 0;
      //virtual void bind(const Ptr<ViewportScissorState> viewportScissor) = 0;
      virtual void bind(const Ptr<DepthStencilState> depthStencil) = 0;
      virtual void bind(const Ptr<BlendState> blend) = 0;

      virtual bool startRenderPass(const Ptr<RenderPass> pass) = 0;
      virtual void set(const Ptr<Pipeline> pipeline) = 0;
      
      // TEMP: Until Descriptor Tables abstraction is not done
      virtual void setVertexBuffer(Ptr<Buffer> buffer, uint32 slot) = 0;
      
      virtual bool populate(Ptr<Buffer> transfer, Ptr<Buffer> buffer) = 0;
      virtual bool populate(Ptr<Buffer> transfer, Ptr<Texture> texture, uint32 mipmap, uint32 layer) = 0;
  
      virtual void draw(const DrawableType primitiveType,
                        const uint32       elements      = 1,      // Elements to process (they are assembled into Primitives)
                        const Ptr<Buffer>  indexBuffer   = nullptr, // Optional Index buffer
                        const uint32       instances     = 1,      // Instances to draw
                        const uint32       firstElement  = 0,      // First element to process (or index in Index buffer if specified)
                        const sint32       firstVertex   = 0,      // VertexID from which numeration should start (can be negative)
                        const uint32       firstInstance = 0) = 0; // InstanceID from which numeration should start
         
      virtual void draw(const DrawableType primitiveType,
                        const Ptr<Buffer>  indirectBuffer,         // Buffer from which Draw parameters are sourced
                        const uint32       firstEntry   = 0,       // First entry to process in Indirect buffer
                        const Ptr<Buffer>  indexBuffer  = nullptr, // Optional Index buffer
                        const uint32       firstElement = 0) = 0;  // First index to process in Index buffer (if specified)

      virtual bool endRenderPass(void) = 0;
      virtual void commit(void) = 0;
      virtual void waitUntilCompleted(void) = 0;
      
      virtual ~CommandBuffer() {};                        // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif
