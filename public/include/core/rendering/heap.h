/*
 
 Ngine v5.0
 
 Module      : Heap.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.
 
 */


#ifndef ENG_CORE_RENDERING_HEAP
#define ENG_CORE_RENDERING_HEAP

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"

namespace en
{
   namespace gpu
   {
   class Heap : public SafeObject<Heap>
      {
      public:
      virtual ~Heap() {};                                  // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif
