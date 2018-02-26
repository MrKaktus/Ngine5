/*

 Ngine v5.0
 
 Module      : Pool Allocator.
 Requirements: none
 Description : Pool allocator of objects of the same type. It grows on request, 
               but backing allocation memory adress will change, so all object 
               in pool should be adressed using their index, and not the direct
               pointer. Stored object size cannot be smaller than 8 bytes.
               It is not thread safe. Constructors and destructors of objects 
               need to be called explicitly on allocated memory.

*/

#ifndef ENG_CORE_UTILITIES_POOL_ALLOCATOR
#define ENG_CORE_UTILITIES_POOL_ALLOCATOR

#include "assert.h"

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/alignment.h"

#include "core/utilities/NonCopyable.h"
#include "core/utilities/pageAllocator.h"
#include "utilities/utilities.h"

// Pool stops doubling its size after reaching below size (4MB)
#define PoolDoublingBarrier 4*1024*1024

namespace en
{
   template<typename T>
   class PoolAllocator : private NonCopyable
      {
      private:
      // Ensure that entry is big enough to store pointer
      static_assert(sizeof(T) >= sizeof(void*), "PoolAllocator element size smaller than pointer size!");

      T* memory; // Backing memory
      T* head;   // Points at first free entry (free entries store pointers to 
                 // each other, creating single linked list of all free entries).

      uint32 entrySize;  // Size of single entry, rounded up to multiple of alignment size
      uint64 size;       // Current memory size in bytes (rounded up to multiple of 4KB)
      uint64 maxSize;    // Max memory allocation in bytes (rounded up to multiple of 4KB)
      uint64 doubleSizeUntil;  // Doubling allocation size barrier in bytes

      bool reallocate(void);   // Fails if maximum allowed capacity was reached

      public:
      // Alignment specifies each element starting address alignment, and needs 
      // to be power of two. If element size differs from multiple of alignment 
      // size, each element address is padded to that alignment. When pool is 
      // growing, it's capacity is doubling until reaching defined size. After 
      // that it always grows by that size, until reaching maxCapacity.
      PoolAllocator(const uint32 capacity,
                    const uint32 maxCapacity, 
                    const uint32 alignment = cacheline, 
                    const uint32 doubleSizeUntil = PoolDoublingBarrier);

      // Deallocator is not calling destructors of allocated objects
     ~PoolAllocator();

      // Pass if pool should grow during allocation, when full
      T* allocate(void);

      // Returns pointer to Nth entry
      T* entry(const uint32 index);

      // Returns entry index
      bool index(const T& entry, uint32& index);

      void deallocate(T& resource);
      };

   template<typename T>
   PoolAllocator<T>::PoolAllocator(
      const uint32 capacity, 
      const uint32 maxCapacity,
      const uint32 alignment, 
      const uint32 _doubleSizeUntil) :
         memory(nullptr),
         head(nullptr),
         entrySize(roundUp(static_cast<uint64>(sizeof(T)), static_cast<uint64>(alignment))),
         size(roundUp(entrySize*capacity, 4096)),
         maxSize(roundUp(entrySize*maxCapacity, 4096)),
         doubleSizeUntil(_doubleSizeUntil)
   {   
   assert( powerOfTwo(alignment) );

   memory = reinterpret_cast<T*>(en::allocate(size, maxSize));
   if (memory)
      {
      // Due to allocating multiple of 4KB blocks, there may be some extra entries
      uint32 actualCapacity = size / entrySize;

      // At the beginning all entries are free and thus point to the next one
      head = memory;
      for(uint32 i=0; i<(actualCapacity - 1); ++i)
         *(T**)((uint8*)(memory) + i * entrySize) = (T*)((uint8*)(memory) + (i + 1) * entrySize);

      // Last free element is not pointing at anything
      *(T**)((uint8*)(memory) + (actualCapacity - 1) * entrySize) = nullptr;
      }
   }

   template<typename T>
   PoolAllocator<T>::~PoolAllocator()
   {
   en::deallocate((void*)memory);
   }

   template<typename T>
   bool PoolAllocator<T>::reallocate(void)
   {
   assert( head == nullptr );
   if ( size >= maxSize )
      return false;

   uint64 newSize = size < doubleSizeUntil ? size * 2 : roundUp(size + doubleSizeUntil, 4096);
   if (newSize > maxSize)
      newSize = maxSize;

   uint32 actualCapacity = static_cast<uint32>(size / entrySize);
   uint32 newCapacity = static_cast<uint32>(newSize / entrySize);

   if (en::reallocate((void*)memory, size, newSize))
      {
      head = (T*)((uint8*)memory + actualCapacity * entrySize);
      for(uint32 i=actualCapacity; i<(newCapacity - actualCapacity - 1); ++i)
         *(T**)((uint8*)(memory) + i * entrySize) = (T*)((uint8*)(memory) + (i + 1) * entrySize);
      
      // Last free element is not pointing at anything
      *(T**)((uint8*)memory + (newCapacity - 1) * entrySize) = nullptr;

      size = newSize;
      return true;
      }

   return false;
   }

   template<typename T>
   T* PoolAllocator<T>::allocate(void)
   {
   // If pool is full and reallocation is allowed try to grow
   if (memory && (head == nullptr))
      if (!reallocate())
         return nullptr;

   T* entry = head;

   // Head points to next free block (or nullptr if this one was the last one)
   head = *(T**)(entry);

   return entry;
   }

   template<typename T>
   T* PoolAllocator<T>::entry(const uint32 index)
   {
   if (index >= (size / entrySize))
      return nullptr;

   return (T*)((uint8*)(memory) + index * entrySize);
   }

   template<typename T>
   bool PoolAllocator<T>::index(const T& entry, uint32& index)
   {
   if (!memory)
      return false;

   uint64 offset = ((uint8*)(&entry) - (uint8*)(memory));
   if (offset >= maxSize)
      return false;

   index = offset / entrySize;
   return true;
   }

   template<typename T>
   void PoolAllocator<T>::deallocate(T& resource)
   {
   *(T**)(&resource) = head;
   head = &resource;
   }
}

#endif