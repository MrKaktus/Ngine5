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

#include "core/defines.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/mtlHeap.h"
#include "core/rendering/metal/mtlDevice.h"

namespace en
{
   namespace gpu
   { 
#if defined(EN_PLATFORM_IOS)
   HeapMTL::HeapMTL(id<MTLHeap> handle) :
      handle(handle)
      Heap()
   {
   }
      
   HeapMTL::~HeapMTL()
   {
   [handle release];
   }
#endif
#if defined(EN_PLATFORM_OSX)
   // TODO: How to emulate Heaps on OSX ?
   HeapMTL::HeapMTL() :
      Heap()
   {
   }
      
   HeapMTL::~HeapMTL()
   {
   }
#endif
      
   Ptr<Heap> MetalDevice::create(uint32 size)
   {
   Ptr<Heap> heap = nullptr;

#if defined(EN_PLATFORM_IOS)
   MTLHeapDescriptor desc;
   desc.size         = size;
   desc.storageMode  = MTLStorageModePrivate;
   desc.cpuCacheMode = MTLCPUCacheModeDefaultCache;
   
   id<MTLHeap> handle = nil;
   handle = [device newHeapWithDescriptor:desc];
   if (handle)
      {
      heap = ptr_dynamic_cast<Heap, HeapMTL>(Ptr<HeapMTL>(new HeapMTL(handle)));
      }
#endif
   
   return heap;
   }
      
   }
}
#endif
