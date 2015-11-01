/*

 Ngine v5.0
 
 Module      : Thread-safe queue.
 Visibility  : Any version.
 Requirements: none
 Description : Provides templated implemen-
               tation of Thread-safe queue.  

*/

#ifndef ENG_THREADING_QUEUE
#define ENG_THREADING_QUEUE

#include "core/defines.h"
#include "core/types.h"
#include "threading/mutex.h"

template <typename T>
class cachealign Nqueue
      {
      private:
      T*     m_queue; // Pointer to queue of elements
      uint32 m_count; // Elements in the queue
      Nmutex m_mutex; // Mutex protecting queue operations
      uint32 m_size;  // Size of the queue buffer

      public:
      Nqueue(uint32 size);
     ~Nqueue();

      bool push(T in);      // Pushes element to the top of the queue
      bool pop(T* ptr);     // Pop's element from the top 
      bool popBack(T* ptr); // Pop's element from the bottom 

      forceinline uint32 size(void); // Elements in the queue
      };

template <typename T>
Nqueue<T>::Nqueue(uint32 size) :
           m_count(0),
           m_size(size)
{
 m_queue = new T[size];
}

template <typename T>
Nqueue<T>::~Nqueue()
{
 delete m_queue;
}

template <typename T>
bool Nqueue<T>::push(T in)
{
 // Entering Critical Section
 m_mutex.lock();

 // Put new element on the queue
 bool result = false;
 if (m_count < m_size)
    {
    m_queue[m_count] = in;
    m_count++;
    result = true;
    }

 // Leaving Critical Section
 m_mutex.unlock();
 return result;
}

template <typename T>
bool Nqueue<T>::pop(T* ptr)
{
 // Entering Critical Section
 m_mutex.lock();

 // Take element from the top
 bool result = false;
 if (m_count > 0)
    {
    m_count--;
    *ptr = m_queue[m_count];
    result = true;
    }

 // Leaving Critical Section
 m_mutex.unlock();
 return result;
}

template <typename T>
bool Nqueue<T>::popBack(T* ptr)
{
 // Entering Critical Section
 m_mutex.lock();

 // Take element from the bottom
 bool result = false;
 if (m_count > 0)
    {
    m_count--;
    *ptr = m_queue[0];
    result = true;

    // Shift elements of the queue by one
    for(uint32 i=0; i<m_count; i++)
       m_queue[i] = m_queue[i+1];
    }

 // Leaving Critical Section
 m_mutex.unlock();
 return result;
}


template <typename T>
forceinline uint32 Nqueue<T>::size(void)
{
 return m_count;
}

#endif