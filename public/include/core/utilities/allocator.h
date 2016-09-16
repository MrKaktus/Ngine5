/*

 Ngine v5.0
 
 Module      : Custom Allocator.
 Requirements: none
 Description : Simple thread-safe array template. It stores 
               objects in array of fixed size. Constructors 
               and destructors of objects are called directly 
               on allocated memory.

*/

#ifndef ENG_CORE_UTILITIES_ALLOCATOR
#define ENG_CORE_UTILITIES_ALLOCATOR

#include "assert.h"
#include <bitset>
using namespace std;

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/NonCopyable.h"
#include "threading/mutex.h"

namespace en
{
   // Interface for allocation algorith
   class Allocator : public SafeObject<Allocator>
      {
      public:
      virtual bool allocate(const uint64 requestedSize,
                            const uint64 requestedAlignment,
                                  uint64& offset) = 0; // Returns offset for new allocation, or false
      virtual void deallocate(const uint64 offset,
                              const uint64 size) = 0;  // Deallocates given memory region
      virtual ~Allocator() {};                         // Polymorphic deletes require a virtual base destructor
      }

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
      virtual void deallocate(const uint64 offset,
                              const uint64 size);
      virtual ~BasicAllocator();
      };
   
   
   struct MemoryChunk
      {
      uint64       offset;
      uint64       size;
      MemoryChunk* prev;
      MemoryChunk* next;
      };
   
   BasicAllocator::BasicAllocator(uint64 _size) :
      size(_size),
      available(_size),
      freeHead(new MemoryChunk())
   {
   freeHead->offset = 0u;
   freeHead->size   = size;
   freeHead->prev   = nullptr;
   freeHead->next   = nullptr;
   }
   
   void BasicAllocator::insertAfter(MemoryChunk* ptr, uint64 offset, uint64 size)
   {
   if (!ptr)
      return false;
      
   MemoryChunk* insert = new MemoryChunk();
   insert->offset = offset;
   insert->size   = size;
   insert->prev   = ptr;
   insert->next   = ptr->next;
      
   if (ptr->next)
      ptr->next->prev = insert;
   ptr->next = insert;
   }

   void BasicAllocator::remove(MemoryChunk* ptr)
   {
   if (ptr->next)
      ptr->next->prev = ptr->prev;
         
   if (ptr->prev)
      ptr->prev->next = ptr->next;
      
   delete ptr;
   }
   
   bool BasicAllocator::allocate(const uint64 requestedSize, const uint64 requestedAlignment, uint64& offset)
   {
   if (requestedSize > available)
      return false;

   // Iterate over list of free memory regions
   MemoryChunk* ptr = freeHead;
   while(ptr)
      {
      if (ptr->size >= requestedSize)
         {
         uint64 alignedOffset = roundUp(ptr->offset, requestedAlignment);
         uint64 endOffset     = ptr->offset + ptr->size;
         uint64 newEndOffset  = alignedOffset + requestedSize;
         if (newEndOffset <= endOffset)
            {
            // Allocation can be performed, split free block
            uint64 baseOffset = ptr->offset;
            uint64 baseSize   = ptr->size;
            
            // New empty block in front of allocation
            bool reusedEntry = false;
            if (alignedOffset > baseOffset)
               {
               ptr->size = alignedOffset - baseOffset;
               reusedEntry = true;
               }
            
            // New empty block at end of allocation
            if (newEndOffset < endOffset)
               {
               if (reusedEntry)
                  insertAfter(ptr, newEndOffsett, endOffset - newEndOffset);
               else
                  {
                  ptr->offset = newEndOffset;
                  ptr->size   = alignedOffset - baseOffset;
                  reusedEntry = true;
                  }
               }
            
            // If allocation covers whole empty block, remove it from the list
            if (!reusedEntry)
               remove(ptr);
              
            available -= requestedSize;
            return true;
            }
         }
         
      ptr = ptr->next;
      }
   
   return false;
   }
   
   bool BasicAllocator::deallocate(const uint64 offset, const uint64 freedSize)
   {
   uint64 newEndOffset = offset + freedSize;
   
   if (newEndOffset > size)
      return false;
      
   if (freeHead == nullptr)
      {
      freeHead = new MemoryChunk();
      freeHead->offset = offset;
      freeHead->size   = freedSize;
      freeHead->prev   = nullptr;
      freeHead->next   = nullptr;
      
      available += freedSize;
      return true;
      }
      
   // Iterate over list of free memory regions
   // (add new free memory region, grow or merge two)
   MemoryChunk* ptr = freeHead;
   while(ptr)
      {
      uint64 endOffset = ptr->offset + ptr->size;

      if (ptr->offset > offset)
         {
         // Make sure those blocks don't overlapp
         assert( ptr->offset >= newEndOffset );

         // Touching next block.
         if (ptr->offset == newEndOffset)
            {
            ptr->offset = offset;
            ptr->size  += freedSize;
            
            MemoryChunk* prev = ptr->prev;
            if (prev)
               {
               // Touching previous block, and next block.
               if (prev->offset + prev->size == offset)
                  {
                  prev->size += ptr->size;
                  remove(ptr);
                  }
               }
            
            available += freedSize;
            return true;
            }
         else // In front of this block, but not touching it.
            {
            MemoryChunk* prev = ptr->prev;
            if (prev)
               {
               // Touching previous block.
               if (prev->offset + prev->size == offset)
                  {
                  prev->size += freedSize;
                  
                  available += freedSize;
                  return true;
                  }
               }
            
            // Not touching any block.
            insertBefore(ptr, offset, freedSize);
            available += freedSize;
            return true;
            }
         }
      else // Make sure those blocks don't overlapp
         assert( ptr->offset + ptr->size <= offset);
      }
   
   // If there is completly no empty space, list of blocks is empty.
   return false;
   }
   
   BasicAllocator::~BasicAllocator()
   {
   MemoryChunk* ptr = freeHead;
   
   // Free all elements on the linked list
   while(ptr)
      {
      MemoryChunk* node = ptr->next;
      remove(ptr);
      ptr = node;
      }
   }
   
   


   // + Allows Run-Time capacity initialization through create method
   // + Allocated elements are guaranteed to occupy the same place in memory.
   // - Allocated elements need to be dealocated directly in buffer.
   template <typename T>
   class StaticBuffer : private NonCopyable
         {
         private:
         void*   buffer; // Pointer to memory pool
         uint32* list;   // List of free fields in array
         uint32  count;  // Count of free elements
         uint32  size;   // Memory pool size
         Nmutex  lock;   // Data mutex for thread safety
         bitset<maxsize> used; // Flags indicating if specified field is used

         public:
         StaticBuffer();
        ~StaticBuffer();

         bool create(uint32 capacity); // Allocates memory pool
         T*   allocate(void);          // Creates object in memory pool
         bool free(T* const ptr);      // Deletes object and removes it from memory pool
         };

   template <typename T>
   StaticBuffer<T>::StaticBuffer() :
      buffer(nullptr),
      list(nullptr),
      count(0),
      size(0)
   {
   }

   template <typename T>
   StaticBuffer<T>::~StaticBuffer()
   {
   assert(buffer);
   assert(list);

   T* element = static_cast<T*>(buffer);

   // Call explicitly destructors on all stored objects
   for(uint32 i=0; i<size; ++i)
      if (used[i])
	      element[i].~T();

   // Free memory
   delete [] static_cast<uint8*>(buffer);
   delete [] list;
   }

   template <typename T>
   bool StaticBuffer<T>::create(uint32 capacity)
   {
   // Check if not already created
   if (buffer || list)
      return false;

   // Allocate memory pool
   buffer = operator new[] (capacity * sizeof(T));
   if (!buffer)
      return false;

   // Allocate free objects list
   list = new uint32[capacity];
   if (!list)
      {
      delete [] static_cast<uint8*>(buffer);
      return false;
      }

   // Mark all fields as free starting from beginning of memory pool
   count = capacity;
   size = capacity;
   used.reset();
   for(uint32 i=0; i<capacity; ++i)
      list[i] = capacity - i - 1;
   return true;
   }

   template <typename T>
   T* StaticBuffer<T>::allocate(void)
   {
   assert(buffer);
   assert(list);

   // Be sure to be thread safe
   lock.lock();

   // Check if not full already
   if (count == 0)
      {
      lock.unlock();
      return nullptr;
      }

   // Reserve one entry
   uint32 index = list[count-1];
   --count;

   // Call constructor of object on pre-allocated memory
   T* element = static_cast<T*>(buffer);
   new (element + index) T();
   used[index] = true;

   // Exiting critical section
   lock.unlock();
   return (element + index);
   }

   template <typename T>
   bool StaticBuffer<T>::free(T* const ptr)
   {
   assert(buffer);
   assert(list);
   assert(ptr);

   // Calculate object index in array
   uint64 base   = static_cast<uint64>(buffer);
   uint64 offset = static_cast<uint64>(ptr);
   assert(offset >= base);

   uint32 index = (offset - base) / sizeof(T);
   assert(index < size);

   // Be sure to be thread safe
   lock.lock();

   // Check if really alocated
   if (!used[index])
      {
      lock.unlock();
      return false;
      }

   // Destroy object and mark its field as free for reusing
   T* element = (T*)buffer;
   (element + index)->T::~T();
   used[index] = false;
   list[count] = index;
   count++;
   lock.unlock();
   return true;
   }
}

#endif
