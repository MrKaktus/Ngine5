/*

 Ngine v5.0
 
 Module      : Basic Allocator.
 Requirements: none
 Description : Provides logic for blocks allocation 
               in given address space.

*/

#ifndef ENG_CORE_UTILITIES_BASIC_ALLOCATOR
#define ENG_CORE_UTILITIES_BASIC_ALLOCATOR

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"

namespace en
{
   // Interface for allocation algorith
   class Allocator : public SafeObject<Allocator>
      {
      public:
      virtual bool allocate(const uint64 requestedSize,
                            const uint64 requestedAlignment,
                                  uint64& offset) = 0; // Returns offset for new allocation, or false
      virtual bool deallocate(const uint64 offset,
                              const uint64 size) = 0;  // Deallocates given memory region
      virtual ~Allocator() {};                         // Polymorphic deletes require a virtual base destructor
      };

   struct MemoryChunk
      {
      uint64       offset;
      uint64       size;
      MemoryChunk* prev;
      MemoryChunk* next;
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