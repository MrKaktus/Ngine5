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

#include "assert.h"

#include "core/rendering/common/buffer.h"
#include "core/rendering/common/inputLayout.h"
#include "core/rendering/common/heap.h"

namespace en
{
   namespace gpu
   {
   CommonHeap::CommonHeap(const MemoryUsage usage, const uint32 size) :
      _usage(usage),
      _size(size)
   {
   }

   uint32 CommonHeap::size(void) const
   {
      return _size;
   }

//   bool allocate(uint64 size, uint64 alignment, uint64* offset)
//   {
//   // TODO: Alocating algorithm!
//   // TODO: Requires minimum Heap size.
//   // TODO: Generalize this allocator to be of an Engine Type. 
//   //       This will allow it to be used also for CPU memory sub-allocations,
//   //       or to hook-up user specific memory allocators.
//   // TODO: Add "makeAliasable" call to the resources API.
//   // TODO: Ensure resources "dealloc" on Heap's allocator on destruction.
//   // TODO: General and Specified memory allocators are needed.
//   return false;
//   }
   
   shared_ptr<Buffer> CommonHeap::createBuffer(const uint32 elements, const Formatting& formatting, const uint32 step)
   {
   assert( elements );
   assert( formatting.column[0] != Attribute::None );

   uint32 elementSize = formatting.elementSize();
   uint32 size        = elements * elementSize;
   shared_ptr<Buffer> buffer = createBuffer(BufferType::Vertex, size);
   if (buffer)
      {
      CommonBuffer* common = reinterpret_cast<CommonBuffer*>(buffer.get());
      
      common->formatting = formatting;
      common->elements   = elements;
      common->step       = step;
      }

   return buffer;
   }
      
   shared_ptr<Buffer> CommonHeap::createBuffer(const uint32 elements, const Attribute format)
   {
   assert( elements );
   assert( format == Attribute::u8  ||
           format == Attribute::u16 ||
           format == Attribute::u32 );
     
   uint32 elementSize = AttributeSize[underlyingType(format)];
   uint32 size        = elements * elementSize;
   shared_ptr<Buffer> buffer = createBuffer(BufferType::Index, size);
   if (buffer)
      {
      CommonBuffer* common = reinterpret_cast<CommonBuffer*>(buffer.get());
      
      common->formatting.column[0] = format;
      common->elements = elements;
      }
      
   return buffer;
   }

   // Should be implemented by API class
   shared_ptr<Buffer> CommonHeap::createBuffer(const BufferType type, const uint32 size)
   {
   assert(0);
   return shared_ptr<Buffer>(nullptr);
   }

   }
}
