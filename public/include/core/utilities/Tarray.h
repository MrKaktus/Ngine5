/*

 Ngine v5.0
 
 Module      : Array template.
 Requirements: none
 Description : Simple thread-safe array template. It stores 
               objects in a pre allocated array of fixed size. 
               Constructors and destructors of objects should 
               be called directly on pre allocated memory by
               code that uses this array. Therefore it is 
               preffered to store simple types in it, like 
               structures for eg.

*/

#ifndef ENG_CORE_UTILITIES_TARRAY
#define ENG_CORE_UTILITIES_TARRAY

#include "core/defines.h"
#include "threading/mutex.h"
#include "utilities/utilities.h"

namespace en
{
   template <typename obj, uint32 size>
   class cachealign Tarray
      {
      private:
      // Single entry in array
      struct cachealign element
             {
             obj    value;    // Stored value
             uint32 next;     // Next free field id
             };

      element m_buffer[size]; // Pointer to array of elements
      uint32  m_head;         // First free entry
      uint32  m_tail;         // Last free entry
      uint32  m_size;         // Size of array
      Nmutex  m_lock;         // Thread safety
      bool    m_full;         // Indicates array is full

      Tarray(const Tarray&);
      Tarray& operator=(const Tarray&);

      public:
      Tarray();
     ~Tarray();

      uint32 add(void);               // Allocates field for element.
      obj*   get(const uint32 id);    // Returns handle to allocated element in array or NULL.
      bool   del(const uint32 id);    // Deletes element and removes it from array.
      };

   template <typename obj, uint32 size>
   Tarray<obj, size>::Tarray() :
      m_head(0),
      m_tail(size-1),
      m_size(size),
      m_full(false)
   {
   // Each field points to next one in the queue
   for(uint32 i=0; i<m_size-1; i++)
      m_buffer[i].next = i+1;
   }

   template <typename obj, uint32 size>
   Tarray<obj, size>::~Tarray()
   {
   }

   template <typename obj, uint32 size>
   uint32 Tarray<obj, size>::add(void)
   {
   // Check if not full already
   if (m_full)
      return 0;

   // Be sure to be thread safe
   m_lock.lock();

   // Take one field from free ones
   uint32 ret = m_head;
   if (m_head != m_tail)
      m_head = m_buffer[m_head].next;
   else
      m_full = true;

   // Mark allocated field as used
   m_buffer[ret].next = 0xFFFFFFFF;
   ret++;

   // Exiting critical section
   m_lock.unlock();
   return ret;
   }

   template <typename obj, uint32 size>
   obj* Tarray<obj, size>::get(uint32 id)
   {
   // Check if id is in range
   if (!inRange<uint32>(id,1,m_size))
      return 0;

   // Check if field is allocated
   if (m_buffer[id-1].next != 0xFFFFFFFF)
      return 0;

   return &(m_buffer[id-1].value);
   }

   template <typename obj, uint32 size>
   bool Tarray<obj, size>::del(const uint32 id)
   {
   // Check if id is in range
   if ( !inRange<uint32>(id,1,m_size) )
      return false;

   // Be sure to be thread safe
   m_lock.lock();

   // Put this field on queue
   if (m_full)
      {
      m_head = id-1;
      m_tail = id-1;
      m_full = false;
      }
   else
      {
      memset(&(m_buffer[id-1].value), 0, sizeof(obj));
      // 1) Use for locality of array accessess in memory
      m_buffer[id-1].next = m_head;
      m_head = id-1;
      // 2) Use for latest possible id reuse
      //m_buffer[m_tail].next = id-1;
      //m_tail = id-1;
      }

   // Exiting critical section
   m_lock.unlock();
   return true;
   }
}
#endif