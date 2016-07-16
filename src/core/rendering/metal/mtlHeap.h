/*
 
 Ngine v5.0
 
 Module      : Metal Heap.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.
 
 */

#ifndef ENG_CORE_RENDERING_METAL_HEAP
#define ENG_CORE_RENDERING_METAL_HEAP

#include "core/defines.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/metal.h"
#include "core/rendering/heap.h"

namespace en
{
   namespace gpu
   {
   class HeapMTL : public Heap
      {
      public:
#if defined(EN_PLATFORM_IOS)
      id <MTLHeap> handle;
      
      HeapMTL(id<MTLHeap> handle);
#endif
#if defined(EN_PLATFORM_OSX)
      HeapMTL();      
#endif
      virtual ~HeapMTL();
      };
   }
}
#endif

#endif
