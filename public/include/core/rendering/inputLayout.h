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

#ifndef ENG_CORE_RENDERING_INPUT_LAYOUT
#define ENG_CORE_RENDERING_INPUT_LAYOUT

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"
#include "core/rendering/buffer.h"

namespace en
{
   namespace gpu
   {
   struct AttributeDesc
      {
      Attribute format;   // Format in which input data are represented
      uint32    buffer;   // Index of bound buffer that will be used as source for this attribute.
      uint32    offset;   // Offset in bytes to first element of given attribute in source buffer.
      };                  // This offset needs to be aligned on some architectures.
      
   struct BufferDesc
      {
      uint32 elementSize; // Size of one element in buffer (all attributes with their padding)
      uint32 stepRate;    // Attribute update rate, 0 (default) means after each Vertex,
      };                  // (1+) means after how many Instances it should be updated

   // Handle for Input Assembler binding specification
   class InputLayout : public SafeObject<InputLayout>
      {
      public:
      virtual ~InputLayout() {};              // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif
