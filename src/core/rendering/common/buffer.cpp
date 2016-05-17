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
#include "core/rendering/common/inputAssembler.h"

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
      "u64",
      "s64",
      "f64",
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
      "v2u64",
      "v2s64",
      "v2f64",
      "v3u8_norm",
      "v3u8_srgb",
      "v3s8_norm",
      "v3u8",
      "v3s8",
      "v3u16_norm",
      "v3s16_norm",
      "v3u16",
      "v3s16",
      "v3f16",
      "v3u32",
      "v3s32",
      "v3f32",
      "v3u64",
      "v3s64",
      "v3f64",
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
      "v4u64",
      "v4s64",
      "v4f64",
      "v3f11_11_10",
      "v4u10_10_10_2_norm",
      "v4s10_10_10_2_norm",
      "v4u10_10_10_2",
      "v4s10_10_10_2",
      "v4u10_10_10_2_norm_rev",
      "v4s10_10_10_2_norm_rev",
      "v4u10_10_10_2_rev",
      "v4s10_10_10_2_rev"
      };
   #endif
      
   BufferCommon::BufferCommon(const BufferType type) :
      formatting(),
      elements(0),
      step(0),
      apiType(type)
   {
   }

   BufferCommon::~BufferCommon()
   {
   }
   
   BufferType BufferCommon::type(void) const
   {
   return apiType;
   }
   
   void* BufferCommon::map(const DataAccess access)
   {
   // Should be implemented by given API
   assert(0);
   }
   
   bool BufferCommon::unmap(void)
   {
   // Should be implemented by given API
   assert(0);
   }
   
   Ptr<Buffer> CommonDevice::create(const uint32 elements, const Formatting& formatting, const uint32 step, const void* data)
   {
   assert( elements );
   assert( formatting.column[0] != Attribute::None );

   uint32 elementSize = formatting.elementSize();
   uint32 size        = elements * elementSize;
   Ptr<Buffer> buffer = create(BufferType::Vertex, size, data);
   if (buffer)
      {
      Ptr<BufferCommon> common = ptr_dynamic_cast<BufferCommon, Buffer>(buffer);
      
      common->formatting = formatting;
      common->elements   = elements;
      common->step       = step;
      }

   return buffer;
   }
      
   Ptr<Buffer> CommonDevice::create(const uint32 elements, const Attribute format, const void* data)
   {
   assert( elements );
   assert( format == Attribute::u8  ||
           format == Attribute::u16 ||
           format == Attribute::u32 );
     
   uint32 elementSize = TranslateAttributeSize[underlyingType(format)];
   uint32 size        = elements * elementSize;
   Ptr<Buffer> buffer = create(BufferType::Index, size, data);
   if (buffer)
      {
      Ptr<BufferCommon> common = ptr_dynamic_cast<BufferCommon, Buffer>(buffer);
      
      common->formatting.column[0] = format;
      common->elements = elements;
      }
      
   return buffer;
   }
   
   // Should be implemented by API class
   Ptr<Buffer> CommonDevice::create(const BufferType type, const uint32 size, const void* data)
   {
   assert(0);
   return Ptr<Buffer>(nullptr);
   }
   
   }
}
