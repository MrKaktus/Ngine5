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

#include "core/rendering/metal/mtlBuffer.h"
#include "core/rendering/metal/mtlHeap.h"
#include "core/rendering/metal/mtlDevice.h"

namespace en
{
   namespace gpu
   { 
#if defined(EN_PLATFORM_IOS)
   HeapMTL::HeapMTL(id<MTLHeap> handle, const uint32 _size) :
      handle(handle),
      CommonHeap(_size)
   {
   }
      
   HeapMTL::~HeapMTL()
   {
   [handle release];
   }
#endif
#if defined(EN_PLATFORM_OSX)
   HeapMTL::HeapMTL(id<MTLDevice> device, const uint32 _size) :
      handle(device),
      CommonHeap(_size)
   {
   }
      
   HeapMTL::~HeapMTL()
   {
   handle = nullptr;
   }
#endif
      
   // Create unformatted generic buffer of given type and size.
   // This method can still be used to create Vertex or Index buffers,
   // but it's adviced to use ones with explicit formatting.
   Ptr<Buffer> HeapMTL::create(const BufferType type, const uint32 size)
   {
   return ptr_dynamic_cast<Buffer, BufferMTL>(new BufferMTL(handle, type, size));
   }
   
   // Create unformatted generic buffer of given type and size.
   // This is specialized method, that allows passing pointer
   // to data, to directly initialize buffer content.
   // It is allowed on mobile devices conforming to UMA architecture.
   // On Discreete GPU's with NUMA architecture, only Transient buffers
   // can be created and populated with it.
   Ptr<Buffer> HeapMTL::create(const BufferType type, const uint32 size, const void* data)
   {
   return ptr_dynamic_cast<Buffer, BufferMTL>(new BufferMTL(handle, type, size, data));
   }
 
   Ptr<Texture> HeapMTL::create(const TextureState state)
   {
   return ptr_dynamic_cast<Texture, TextureMTL>(new TextureMTL(handle, state));
   }
   
   Ptr<Heap> MetalDevice::create(uint32 size)
   {
   Ptr<Heap> heap = nullptr;

#if defined(EN_PLATFORM_IOS)
   uint32 roundedSize = roundUp(size, 4096);
   
   MTLHeapDescriptor desc;
   desc.size         = roundedSize;
   desc.storageMode  = MTLStorageModePrivate;
   desc.cpuCacheMode = MTLCPUCacheModeDefaultCache;
   
   id<MTLHeap> handle = nil;
   handle = [device newHeapWithDescriptor:desc];
   if (handle)
      {
      heap = ptr_dynamic_cast<Heap, HeapMTL>(Ptr<HeapMTL>(new HeapMTL(handle, roundedSize)));
      }
#else
   // On macOS we emulate Heaps by passing calls directly to Device
   heap = ptr_dynamic_cast<Heap, HeapMTL>(Ptr<HeapMTL>(new HeapMTL(device, roundUp(size, 4096u))));
#endif
   
   return heap;
   }
      
   }
}
#endif
