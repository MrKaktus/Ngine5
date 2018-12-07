/*

 Ngine v5.0
 
 Module           : Work-Stealing Deque
 Multi-Threaded   : Safe
 Type             : Deque (Double-ended circular queue)
 Producers/Consum : Single-Producer / Multiple-Consumers (SPMC)
 Data structure   : Array-based (growing Virtual Memory, no relocation)
 Intrusiveness    : Intrusive (array based, so no extra allocations)
 Maximum size     : Unbounded (to defined VM limit)
 Overflow behavior: Fails on overflow (debug assert)
 Garbage collector: Not Required
 Priorities       : No support
 Ordering         : FIFO (for producer), no ordering (for consumers)
 Producer FPG     : Wait-freedom
 Consumer FPG     : Lock-freedom
 Expected usage   : A queue usually contains substantial amount of elements
 Failure behavior : Non-blocking (error message on empty/full**)
 Description      : Implementation of Chase-Lev "Dynamic Circular Work-Stealing Deque".
                    http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.170.1097&rep=rep1&type=pdf
                
                    Implementation is based on below:
                    "Correct and Efficient Work-Stealing for Weak Memory Models"
                    https://www.di.ens.fr/~zappa/readings/ppopp13.pdf

    ** TODO: Implement proper error message on full 

    FPG - Forward Progress Guarantee:
    http://www.1024cores.net/home/lock-free-algorithms/introduction
*/

#ifndef ENG_MEMORY_WORK_STEALING_DEQUE
#define ENG_MEMORY_WORK_STEALING_DEQUE

#include "core/defines.h"
#include "core/types.h"
#include "core/memory/pageAllocator.h"
#include <atomic>

/* Transcript of original C11 code (for reference):

#include <atomic>

typedef struct 
{
   std::atomic_size_t size;
   std::atomic_int    buffer[];
} Array;

typedef struct
{
   std::atomic_size_t top, bottom;
   std::atomic<Array*> array;       // original was: Atomic(Array *) array;
} Deque;

constexpr int ABORT = 0xFFFFFFFE; // Definition missing in original code snippet
constexpr int EMPTY = 0xFFFFFFFF; // Definition missing in original code snippet

int take(Deque *q)
{
   size_t b = std::atomic_load_explicit(&q->bottom, std::memory_order_relaxed) - 1;
   Array *a = std::atomic_load_explicit(&q->array, std::memory_order_relaxed);
   std::atomic_store_explicit(&q->bottom, b, std::memory_order_relaxed);
   std::atomic_thread_fence(std::memory_order_seq_cst);
   size_t t = std::atomic_load_explicit(&q->top, std::memory_order_relaxed);
   int x;
   if (t <= b)
      {
      // Non-empty queue.
      x = std::atomic_load_explicit(&a->buffer[b % a->size], std::memory_order_relaxed);
      if (t == b)
         {
         // Single last element in queue.
         if (!std::atomic_compare_exchange_strong_explicit(&q->top, &t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed))
            {
            // Failed race.
            x = EMPTY;
            }
         std::atomic_store_explicit(&q->bottom, b + 1, std::memory_order_relaxed);
         }
      }
   else // Empty queue.
      {
      x = EMPTY;
      std::atomic_store_explicit(&q->bottom, b + 1, std::memory_order_relaxed);
      }
   return x;
}

void push(Deque *q, int x)
{
   size_t b = std::atomic_load_explicit(&q->bottom, std::memory_order_relaxed);
   size_t t = std::atomic_load_explicit(&q->top, std::memory_order_acquire);
   Array *a = std::atomic_load_explicit(&q->array, std::memory_order_relaxed);
   if (b - t > a->size - 1)
      {
      // Full queue.
      resize(q); // <-- not implemented 
      a = std::atomic_load_explicit(&q->array, std::memory_order_relaxed);
      }
   std::atomic_store_explicit(&a->buffer[b % a->size], x, std::memory_order_relaxed);
   std::atomic_thread_fence(std::memory_order_release);
   std::atomic_store_explicit(&q->bottom, b + 1, std::memory_order_relaxed);
}

int steal(Deque *q)
{
   size_t t = std::atomic_load_explicit(&q->top, std::memory_order_acquire);
   std::atomic_thread_fence(std::memory_order_seq_cst);
   size_t b = std::atomic_load_explicit(&q->bottom, std::memory_order_acquire);
   int x = EMPTY;
   if (t < b)      // <-- shouldn't it be <= like in take() case? Currently it won't steal if there is only one element on queue
      {
      // Non-empty queue.
      Array *a = std::atomic_load_explicit(&q->array, std::memory_order_consume);
      x = std::atomic_load_explicit(&a->buffer[t % a->size], std::memory_order_relaxed);
      if (!std::atomic_compare_exchange_strong_explicit(&q->top, &t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed))
         {
         // Failed race.
         return ABORT;
         }
      }
   return x;
}

//*/

namespace en
{
   enum class DequeResult : uint8
      {
      Success  = 0,
      Empty       ,
      Abort       ,
      };

/*
   template<typename T>
   typedef struct 
   {
      std::atomic_size_t size;
      std::atomic<T>*    buffer;  // Array of elements of type T (ensures that elements of any size can be managed in multi-threaded non-blocking manner by managing pointers to them)
   } Array;
*/
   template<typename T>
   class WorkStealingDeque
      {
      private:
    //std::atomic<Array<T*>*> array; // Pointer to array (so that whole array can be atomically replaced after growing)
      std::atomic<sint64> top;    // Head of the queue
      std::atomic<sint64> bottom; // Tail of the queue
      std::atomic<T>*     buffer; // Backing memory (array of elements of type T)
      std::atomic<uint64> size;   // Current memory size in bytes (rounded up to multiple of 4KB)
      uint64 maxSize;             // Max memory allocation in bytes (rounded up to multiple of 4KB)
      uint64 doubleSizeUntil;     // Doubling allocation size barrier in bytes

      void resize(void);          // Asserts if maximum allowed capacity was reached
   
      public:
      // First element starting address is always aligned to page size (4KB).
      // When deque is growing, it's size is doubling until reaching doubleSizeUntil 
      // size in bytes. After that, it always grows by that size, until reaching 
      // maxCapacity.
      WorkStealingDeque(const uint32 capacity,                               // In entries
                        const uint32 maxCapacity,                            // In entries
                        const uint32 doubleSizeUntil = PoolDoublingBarrier); // In bytes
     ~WorkStealingDeque();

      DequeResult take(T& value);   // Called only by owner thread
      void        push(T value);    // Called only by owner thread
      DequeResult steal(T& value);  // Can be called by any thread
      };
 
   static_assert(sizeof(WorkStealingDeque<void*>) == 48, "en::WorkStealingDeque<void*> size mismatch!");

   template<typename T>
   WorkStealingDeque<T>::WorkStealingDeque(
      const uint32 capacity, 
      const uint32 maxCapacity,
      const uint32 _doubleSizeUntil) :
         top(0),
         bottom(0),
         buffer(nullptr),
         size(roundUp(sizeof(T) * capacity, 4096)),
         maxSize(roundUp(sizeof(T) * maxCapacity, 4096)),
         doubleSizeUntil(_doubleSizeUntil)
   {
   // Managed elements cannot exceed pointer size (max 8 bytes), to ensure that
   // they will be loaded and stored in atomic manner within single operation
   assert( sizeof(T) <= sizeof(void*) );

   //assert( powerOfTwo(sizeof(T)) );
 
   buffer = reinterpret_cast<std::atomic<T>*>(virtualAllocate(size, maxSize));
   }

   template<typename T>
   WorkStealingDeque<T>::~WorkStealingDeque()
   {
   virtualDeallocate(buffer, maxSize);
   }

   template<typename T>
   void WorkStealingDeque<T>::resize(void)
   {
   assert( size < maxSize );

   uint64 newSize = size < doubleSizeUntil ? size * 2 : roundUp(size + doubleSizeUntil, 4096);
   if (newSize > maxSize)
      newSize = maxSize;

   // Note [1]:
   //
   // Because circular deque is backed by virtual alocation, it's growing doesn't
   // affect data location in memory (preserving all pointers), nor deque logical 
   // structure (when deque is full, bottom index points at it's end, and after 
   // growing, it's pointing at next available element in memory).
   // This allows optimization where atomic loads of array adress can be skipped.
   bool resize = virtualReallocate((void*)buffer, size, newSize);
   assert( resize );

   // Ensure that other threads during steal will see changed size 
   std::atomic_store_explicit(&size, newSize, std::memory_order_relaxed); // Added due to [1]
   }

   template<typename T>
   DequeResult WorkStealingDeque<T>::take(T& value)
   {
      // Note [2]:
      //
      // Original Chase-Lev algorithm uses signed integers for Top and Bottom
      // markers. Algorithm from which this implementation is derived exposes
      // pseudocode where Top and Bottom are of type size_t. size_t type is
      // guaranteed to be unsigned. 
      // As a result, this algorithm doesn't work, as below subtraction changes
      // B to be equal to MAX_INT instead of -1, which breaks below conditions 
      // and as a result, is never increased. Thus in this implementation Top and
      // Bottom are of type sint64.
      sint64 b = std::atomic_load_explicit(&bottom, std::memory_order_relaxed) - 1;

    //Array *a = std::atomic_load_explicit(&array, std::memory_order_relaxed);  // Optimized out due to [1]
      std::atomic_store_explicit(&bottom, b, std::memory_order_relaxed);
      std::atomic_thread_fence(std::memory_order_seq_cst);
      sint64 t = std::atomic_load_explicit(&top, std::memory_order_relaxed);
   
      DequeResult result = DequeResult::Success;
      T temp;
      if (t <= b)
         {
         // Non-empty queue
         uint32 capacity = static_cast<uint32>(size / sizeof(T));
         temp = std::atomic_load_explicit(&buffer[b % capacity], std::memory_order_relaxed);
         if (t == b)
            {
            // Single last element in queue
            if (!std::atomic_compare_exchange_strong_explicit(&top, &t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed))
               result = DequeResult::Empty; // Failed race
   
            std::atomic_store_explicit(&bottom, b + 1, std::memory_order_relaxed);
            }
         }
      else // Empty queue
         {
         result = DequeResult::Empty;
         std::atomic_store_explicit(&bottom, b + 1, std::memory_order_relaxed);
         }
   
      if (result == DequeResult::Success)
         value = temp;
   
      return result;
   }
   
   template<typename T>
   void WorkStealingDeque<T>::push(T value)
   {
      sint64 b = std::atomic_load_explicit(&bottom, std::memory_order_relaxed);
      sint64 t = std::atomic_load_explicit(&top, std::memory_order_acquire);
    //Array *a = std::atomic_load_explicit(&array, std::memory_order_relaxed);      // Optimized out due to [1]
      uint32 capacity = static_cast<uint32>(size / sizeof(T));
      if (b - t > capacity - 1)
         {
         // Full queue
         resize();
       //a = std::atomic_load_explicit(&array, std::memory_order_relaxed);          // Optimized out due to [1]
         capacity = static_cast<uint32>(size / sizeof(T));
         }
      std::atomic_store_explicit(&buffer[b % capacity], value, std::memory_order_relaxed);
      std::atomic_thread_fence(std::memory_order_release);
      std::atomic_store_explicit(&bottom, b + 1, std::memory_order_relaxed);
   }
   
   template<typename T>
   DequeResult WorkStealingDeque<T>::steal(T& value)
   {
      sint64 t = std::atomic_load_explicit(&top, std::memory_order_acquire);
      std::atomic_thread_fence(std::memory_order_seq_cst);
      sint64 b = std::atomic_load_explicit(&bottom, std::memory_order_acquire);
   
      DequeResult result = DequeResult::Empty;
      T temp;
      if (t < b)
         {
         // Non-empty queue
       //Array *a = std::atomic_load_explicit(&array, std::memory_order_consume);             // Optimized out due to [1]
         uint64 currentSize = std::atomic_load_explicit(&size, std::memory_order_consume);    // Added due to [1]
         uint32 capacity = static_cast<uint32>(currentSize / sizeof(T));
         temp = std::atomic_load_explicit(&buffer[t % capacity], std::memory_order_relaxed);
         result = DequeResult::Success;
         if (!std::atomic_compare_exchange_strong_explicit(&top, &t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed))
            result = DequeResult::Abort; // Failed race
         }
   
      if (result == DequeResult::Success)
         value = temp;
   
      return result;
   }
}
#endif
