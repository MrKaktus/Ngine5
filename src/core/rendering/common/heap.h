/*

 Ngine v5.0
 
 Module      : Common Heap.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_COMMON_HEAP
#define ENG_CORE_RENDERING_COMMON_HEAP

#include "core/rendering/heap.h"

namespace en
{
   namespace gpu
   {
   class CommonHeap : public Heap
      {
      public:
      uint32 _size;

      CommonHeap(const uint32 size);

      virtual uint32 size(void) const;

      virtual ~CommonHeap() {};                           // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif