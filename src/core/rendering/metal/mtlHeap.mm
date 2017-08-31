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

#include "core/rendering/metal/mtlHeap.h"

#if defined(EN_MODULE_RENDERER_METAL)

#include "core/rendering/metal/mtlBuffer.h"
#include "core/rendering/metal/mtlDevice.h"

namespace en
{
   namespace gpu
   {
   HeapMTL::HeapMTL(MetalDevice* _gpu, id<MTLHeap> handle, const MemoryUsage _usage, const uint32 _size) :
      gpu(_gpu),
      handle(handle),
      CommonHeap(_usage, _size)
   {
   }
      
   HeapMTL::~HeapMTL()
   {
   // Auto-release pool to ensure that Metal ARC will flush garbage collector
   @autoreleasepool
      {
      [handle release];
      }
   }
   
   // Return parent device
   Ptr<GpuDevice> HeapMTL::device(void) const
   {
   return Ptr<GpuDevice>(gpu);
   }
      
   // Create unformatted generic buffer of given type and size.
   // This method can still be used to create Vertex or Index buffers,
   // but it's adviced to use ones with explicit formatting.
   Ptr<Buffer> HeapMTL::createBuffer(const BufferType type, const uint32 size)
   {
   Ptr<BufferMTL> ptr = new BufferMTL(Ptr<HeapMTL>(this), type, size);
   return ptr_reinterpret_cast<Buffer>(&ptr);
   }
 
   Ptr<Texture> HeapMTL::createTexture(const TextureState state)
   {
   // Do not create textures on Heaps designated for Streaming.
   // (Engine currently is not supporting Linear Textures).
   assert( _usage == MemoryUsage::Tiled );
   
   Ptr<TextureMTL> ptr = new TextureMTL(handle, state);
   if (ptr)
      ptr->heap = Ptr<HeapMTL>(this);
   
   return ptr_reinterpret_cast<Texture>(&ptr);
   }
   
   Ptr<Heap> MetalDevice::createHeap(const MemoryUsage usage, const uint32 size)
   {
   Ptr<HeapMTL> heap = nullptr;

   uint32 roundedSize = roundUp(size, 4096u);
   
   MTLHeapDescriptor* desc = [MTLHeapDescriptor alloc];
   desc.size         = roundedSize;
#if defined(EN_PLATFORM_IOS)
   desc.storageMode  = MTLStorageModeShared;
#else
   desc.storageMode  = MTLStorageModePrivate;
#endif
   desc.cpuCacheMode = MTLCPUCacheModeDefaultCache;
   
   id<MTLHeap> handle = nil;
   handle = [device newHeapWithDescriptor:desc];
   if (handle)
      heap = new HeapMTL(this, handle, usage, roundedSize);
      
#ifndef APPLE_ARC
   // Auto-release pool to ensure that Metal ARC will flush garbage collector
   @autoreleasepool
      {
      [desc release];
      }
#endif

   return ptr_reinterpret_cast<Heap>(&heap);
   }
      
   }
}
#endif
