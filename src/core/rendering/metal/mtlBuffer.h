/*

 Ngine v5.0
 
 Module      : Metal Buffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_METAL_BUFFER
#define ENG_CORE_RENDERING_METAL_BUFFER

#include "core/defines.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/metal.h"
#include "core/rendering/common/buffer.h"

namespace en
{
   namespace gpu
   {
   class BufferMTL : public BufferCommon
      {
      public:
      id<MTLBuffer> handle;

      BufferMTL(const id<MTLDevice> device, const BufferType type, const uint32 size);
      BufferMTL(const id<MTLDevice> device, const BufferType type, const uint32 size, const void* data);
      
      // Returns pointer to buffers memory. This function can be only called on Transfer buffers.
      virtual void* content(void) const;
      
      //virtual void* map(const DataAccess access = ReadWrite);
      //virtual bool unmap(void);
      
      virtual ~BufferMTL();
      };
      
   class BufferViewMTL : public BufferView
      {
      public:
      Ptr<Buffer> parent;  // Buffer from which this View is created
      
      BufferViewMTL();
      virtual ~BufferViewMTL();
      };

   }
}
#endif

#endif
