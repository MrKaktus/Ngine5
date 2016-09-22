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
