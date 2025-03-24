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
    uint64     size;
    BufferType apiType;
    uint64     mappedOffset;
    uint64     mappedSize;
      
    CommonBuffer(const BufferType type, uint64 length);
    virtual uint64 length(void) const;
    virtual BufferType type(void) const;
    
    virtual ~CommonBuffer();
};

class CommonBufferView : public BufferView
{
    public:
    Format _format;
    uint64 _offset;
    uint64 _length;
    
    CommonBufferView(const Format format, const uint64 offset, const uint64 length);
    virtual uint64 offset(void) const;
    virtual uint64 length(void) const;
    virtual Format format(void) const;
    
    virtual ~CommonBufferView() {};
};

} // en::gpu
} // en
#endif
