/*

 Ngine v5.0
 
 Module      : Buffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/defines.h"

#include "core/rendering/common/device.h"
#include "core/rendering/common/buffer.h"
#include "core/rendering/common/inputLayout.h"

namespace en
{
namespace gpu
{

#ifdef EN_DEBUG
const char* AttributeName[underlyingType(Attribute::Count)] =
{
    "None",
    "u8_norm",
    "s8_norm",
    "u8",
    "s8",
    "u16_norm",
    "s16_norm",
    "u16",
    "s16",
    "f16",
    "u32",
    "s32",
    "f32",
    "v2u8_norm",
    "v2s8_norm",
    "v2u8",
    "v2s8",
    "v2u16_norm",
    "v2s16_norm",
    "v2u16",
    "v2s16",
    "v2f16",
    "v2u32",
    "v2s32",
    "v2f32",
    "v3u32",
    "v3s32",
    "v3f32",
    "v4u8_norm",
    "v4s8_norm",
    "v4u8",
    "v4s8",
    "v4u16_norm",
    "v4s16_norm",
    "v4u16",
    "v4s16",
    "v4f16",
    "v4u32",
    "v4s32",
    "v4f32",
    "v4u10_10_10_2_norm"
};
#endif
   
CommonBuffer::CommonBuffer(const BufferType type, uint64 length) :
    formatting(),
    elements(0u),
    step(0u),
    size(length),
    apiType(type),
    mappedOffset(0u),
    mappedSize(0u)
{
}

CommonBuffer::~CommonBuffer()
{
}

uint64 CommonBuffer::length(void) const
{
    return size;
}

BufferType CommonBuffer::type(void) const
{
    return apiType;
}

CommonBufferView::CommonBufferView(const Format format, const uint64 offset, const uint64 length) :
    _format(format),
    _offset(offset),
    _length(length)
{
}

uint64 CommonBufferView::offset(void) const
{
    return _offset;
}

uint64 CommonBufferView::length(void) const
{
    return _length;
}

Format CommonBufferView::format(void) const
{
    return _format;
}

} // en::gpu
} // en
