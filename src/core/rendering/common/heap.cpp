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

#include "core/rendering/common/heap.h"

namespace en
{
   namespace gpu
   {
   CommonHeap::CommonHeap(const uint32 size) :
      _size(size)
   {
   }

   uint32 CommonHeap::size(void) const
   {
      return _size;
   }

   }
}
