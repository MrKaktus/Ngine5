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

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "core/rendering/metal/mtlBuffer.h"
#include "core/rendering/metal/mtlDevice.h"

namespace en
{
   namespace gpu
   {
  
   BufferMTL::BufferMTL(const id memory, const BufferType type, const uint32 size) :
      handle(nil),
      CommonBuffer(type, size)
   {
   MTLResourceOptions options = (MTLCPUCacheModeDefaultCache << MTLResourceCPUCacheModeShift); // MTLCPUCacheModeWriteCombined
   
   // Based on buffer type, it's located in CPU RAM or GPU VRAM on NUMA architectures.
#if defined(EN_PLATFORM_IOS)
   options |= (MTLStorageModeShared << MTLResourceStorageModeShift);
#else
   if (type == BufferType::Transfer)
      options |= (MTLStorageModeShared << MTLResourceStorageModeShift);
   else
      options |= (MTLStorageModePrivate << MTLResourceStorageModeShift);
#endif

#if defined(EN_PLATFORM_IOS)
   handle = [(id<MTLHeap>)memory newBufferWithLength:(NSUInteger)size
                                             options:options];
#else
   handle = [(id<MTLDevice>)memory newBufferWithLength:(NSUInteger)size
                                               options:options];
#endif
   }

   BufferMTL::BufferMTL(const id memory, const BufferType type, const uint32 size, const void* data) :
      handle(nil),
      CommonBuffer(type, size)
   {
   assert( data );
#if defined(EN_PLATFORM_OSX)
   assert( type == BufferType::Transfer );
#endif

   MTLResourceOptions options = (MTLCPUCacheModeDefaultCache << MTLResourceCPUCacheModeShift) | // MTLCPUCacheModeWriteCombined
                                (MTLStorageModeShared << MTLResourceStorageModeShift);

   // Creates a MTLBuffer object by copying data from an existing storage allocation into a new allocation.
#if defined(EN_PLATFORM_IOS)
   handle = [(id<MTLHeap>)memory newBufferWithLength:(NSUInteger)size
                                             options:options];
   // TODO: Copy data to Heap allocation !
#else
   handle = [(id<MTLDevice>)memory newBufferWithBytes:data
                                               length:(NSUInteger)size
                                              options:options];
#endif

   // Currently unsupported:
   //
   // Creates a MTLBuffer object that reuses an existing storage allocation and does not allocate any new storage.
   // Memory region needs to be allocated with: vm_allocate or mmap (malloc is not allowed).
   //
   // handle = [device newBufferWithBytesNoCopy:ptr
   //                                    length:(NSUInteger)size
   //                                   options:options
   //                               deallocator:nil];
   }

   BufferMTL::~BufferMTL()
   {
   handle = nil;
   }

   void* BufferMTL::content(void) const
   {
   assert( apiType == BufferType::Transfer );
   
   return [handle contents];
   }


   BufferViewMTL::BufferViewMTL()
   {
   }
   
   BufferViewMTL::~BufferViewMTL()
   {
   }
   
   }
}
#endif
