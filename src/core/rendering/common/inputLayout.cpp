/*

 Ngine v5.0
 
 Module      : Input Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/common/device.h"
#include "core/rendering/common/buffer.h"
#include "core/rendering/common/inputLayout.h"

namespace en
{
   namespace gpu
   {
   // Default Constructor, all Attributes default to None
   Formatting::Formatting()
   {
   }
   
   // Create data formatting layout by passing at least one data column format
   Formatting::Formatting(
      const Attribute col0,
      const Attribute col1,
      const Attribute col2,
      const Attribute col3,
      const Attribute col4,
      const Attribute col5,
      const Attribute col6,
      const Attribute col7,                                                                
      const Attribute col8,
      const Attribute col9,
      const Attribute col10,
      const Attribute col11,
      const Attribute col12,
      const Attribute col13,
      const Attribute col14,
      const Attribute col15)
   {
   column[0]  = col0;
   column[1]  = col1;
   column[2]  = col2;
   column[3]  = col3;
   column[4]  = col4;
   column[5]  = col5;
   column[6]  = col6;
   column[7]  = col7;
   column[8]  = col8;
   column[9]  = col9;
   column[10] = col10;
   column[11] = col11;
   column[12] = col12;
   column[13] = col13;
   column[14] = col14;
   column[15] = col15;
   
   // This shouldn't be neccessary, all attributes should default to None 
   if (MaxInputLayoutAttributesCount > 16)
      for(uint32 i=16; i<MaxInputLayoutAttributesCount; ++i)
         column[i] = Attribute::None;
   }

   uint32 Formatting::elementSize(void) const
   {
   uint32 size = 0;
   for(uint32 i=0; i<MaxInputLayoutAttributesCount; ++i)
      {
      if (column[i] == Attribute::None)
         break;
         
      size += AttributeSize[underlyingType(column[i])];
      }
      
   return size;
   }

   Formatting::~Formatting()
   {
   }

   Ptr<InputLayout> CommonDevice::createInputLayout(const DrawableType primitiveType, const uint32 controlPoints)
   {
   return ((GpuDevice*)this)->createInputLayout(primitiveType, controlPoints, 0u, 0u, nullptr, nullptr);
   }

   Ptr<InputLayout> CommonDevice::createInputLayout(const DrawableType primitiveType, const uint32 controlPoints, const Ptr<Buffer> buffer)
   {
   assert( buffer );
   
   Ptr<CommonBuffer> common = ptr_dynamic_cast<CommonBuffer, Buffer>(buffer);
   
   // Compute amount of used attributes
   uint32 usedAttributes = 0;
   for(; usedAttributes<support.maxInputLayoutAttributesCount; ++usedAttributes)
      if (common->formatting.column[usedAttributes] == Attribute::None)
         break;

   // Compute amount of used buffers
   uint32 usedBuffers = 1;

   // Create temporary buffers
   uint32 offsetInElement = 0;
   AttributeDesc* attributes = new AttributeDesc[usedAttributes];
   for(uint32 i=0; i<usedAttributes; ++i)
      {
      Attribute attribute = common->formatting.column[i];
      attributes[i].format = attribute;
      attributes[i].buffer = 0;
      attributes[i].offset = offsetInElement;
      offsetInElement += AttributeSize[underlyingType(attribute)];
      }

   BufferDesc* buffers = new BufferDesc[usedBuffers];
   for(uint32 i=0; i<usedBuffers; ++i)
      {
      // Ptr<CommonBuffer> common = ptr_dynamic_cast<CommonBuffer, Buffer>(buffer[i]);
      buffers[i].elementSize = common->formatting.elementSize();
      buffers[i].stepRate    = common->step;
      }

   Ptr<InputLayout> inputLayout = ((GpuDevice*)this)->createInputLayout(primitiveType, controlPoints, usedAttributes, usedBuffers, attributes, buffers);

   // Free temporary buffers
   delete [] attributes;
   delete [] buffers;

   return inputLayout;
   }

   }
}
