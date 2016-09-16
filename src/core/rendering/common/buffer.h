/*

 Ngine v5.0
 
 Module      : Common Buffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_COMMON_BUFFER
#define ENG_CORE_RENDERING_COMMON_BUFFER

#include "core/rendering/buffer.h"

namespace en
{
   namespace gpu
   {
   class CommonBuffer : public Buffer
      {
      public:
      Formatting formatting;
      uint32     elements;
      uint32     step;
      uint32     size;
      BufferType apiType;
      
      CommonBuffer(const BufferType type, uint32 length);
      virtual uint32 length(void) const;
      virtual BufferType type(void) const;
      
      // Returns pointer to buffers memory. This function can be only called on Transfer buffers.
      virtual void* content(void) const;
      
      //virtual void* map(const DataAccess access = ReadWrite);
      //virtual bool unmap(void);
      
      virtual ~CommonBuffer();
      };
   }
}
#endif
