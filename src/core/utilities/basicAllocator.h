/*

 Ngine v5.0
 
 Module      : Basic Allocator.
 Requirements: none
 Description : Provides logic for blocks allocation 
               in given address space.

*/

#ifndef ENG_CORE_UTILITIES_BASIC_ALLOCATOR
#define ENG_CORE_UTILITIES_BASIC_ALLOCATOR

#include <memory>
using namespace std;

#include "core/algorithm/allocator.h"

namespace en
{
   struct MemoryChunk
      {
      uint64       offset;
      uint64       size;
      MemoryChunk* prev;
      MemoryChunk* next;

      MemoryChunk();
      };

   // Simple allocator algorithm (used for example by GPU Heaps)
   class BasicAllocator : public Allocator
      {
      public:
      uint64       size;
      uint64       available;
      MemoryChunk* freeHead;
      
      BasicAllocator(uint64 size);

      virtual bool allocate(const uint64 requestedSize,
                            const uint64 requestedAlignment,
                                  uint64& offset);
      virtual bool deallocate(const uint64 offset,
                              const uint64 size);

      // Helper methods
      void insertBefore(MemoryChunk* ptr, uint64 offset, uint64 size);
      void insertAfter(MemoryChunk* ptr, uint64 offset, uint64 size);
      void remove(MemoryChunk* ptr);

      virtual ~BasicAllocator();
      };
}
#endif
