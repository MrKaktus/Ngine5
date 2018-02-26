/*

 Ngine v5.0
 
 Module      : Array template.
 Requirements: none
 Description : Thread-safe, cache aligned, continous, resizable 
               array for N objects. There is no default construction
               or destruction of objects stored in array! You need
               to use placement new on allocated memory. As array 
               can be resized by user, it can move in memory. 
               Therefore all pointers to stored elements need to 
               be invalidated after such operation.

*/

#ifndef ENG_CORE_UTILITIES_ARRAY
#define ENG_CORE_UTILITIES_ARRAY

#include "core/defines.h"
#include "core/utilities/memory.h"
#include "threading/mutex.h"

namespace en
{
   // Array
   template <typename T>
   class array
      {
      public:
      T*     buffer;
      uint32 size;
      
      array(uint32 capacity = 0);
     ~array();
      bool resize(uint32 capacity);
      T& operator[](size_t index);
      };
      
   template <typename T>
   array<T>::array(uint32 capacity) :
      buffer(nullptr),
      size(0)
   {
   if (capacity)
      {
      buffer = allocate<T>(capacity);
      size   = capacity;
      assert(buffer);
      }
   }

   template <typename T>
   array<T>::~array()
   {
   deallocate<T>(buffer);
   //delete [] buffer;
   }

   template <typename T>
   bool array<T>::resize(uint32 capacity)
   {
   if (capacity < size)
      return true;
   
   // Lock array until reallocation is done
   Mutex mutex;
   mutex.lock();
   
   T* newBuffer = allocate<T>(capacity);
   if (!newBuffer)
      return false;
   
   memcpy(newBuffer, buffer, size*sizeof(T));

   // Objects were copied, so their destructors don't need to be called on 
   // release of old backing memory.
   deallocate<T>(buffer); 
   buffer = newBuffer;
   size   = capacity;
   return true;
   }

   template <typename T>
   T& array<T>::operator[](size_t index) 
   {
   return buffer[index];
   }
}

#endif