/*

 Ngine v5.0
 
 Module      : FIFO Queue.
 Requirements: none
 Description : First In First Out queue.

*/

#ifndef ENG_UTILITIES_QUEUE
#define ENG_UTILITIES_QUEUE

#include "assert.h"

#include "core/defines.h"
#include "core/types.h"
#include "core/memory/alignment.h"
#include "core/memory/pageAllocator.h"
#include "core/utilities/NonCopyable.h"

namespace en
{

template<typename T>
class CircularQueue : private NonCopyable
{
    private:
    uint8* memory; // Backing memory
    uint8* head;   // Points at first element to take from queue
    uint8* tail;   // Points at next slot tu push element

    uint32 entrySize : 31; // Size of single entry, rounded up to multiple of alignment size
    uint32 full      : 1;  // Flag indicating that queue is full
    uint32 size;           // Current memory size in bytes (rounded up to multiple of 4KB)


    public:
    // Alignment specifies each element starting address alignment, and needs
    // to be power of two. If element size differs from multiple of alignment
    // size, each element address is padded to that alignment.
    CircularQueue(const uint32 capacity,
                  const uint32 alignment = cacheline);

    // Deallocator is not calling destructors of allocated objects
   ~CircularQueue();

    // Push element on a queue, returns false if queue is full
    bool push(const T element);

    // Takes element from queue, and saves it in desired location,
    // returns false is queue is empty.
    bool pop(T* element);
};

// Type doesn't make a difference here
// CompileTimeOffsetReporting( CircularQueue<uint8>, head );
// CompileTimeSizeReporting( CircularQueue<uint8> );
static_assert(sizeof(CircularQueue<uint8>) == 32, "en::CircularQueue size mismatch!");

// TODO: Simplify by replacing head and tail from pointers to uint64 indexes wrapping around % size

template<typename T>
CircularQueue<T>::CircularQueue(
        const uint32 capacity,
        const uint32 alignment) :
    memory(nullptr),
    head(nullptr),
    tail(nullptr),
    entrySize(static_cast<uint32>(roundUp(static_cast<uint64>(sizeof(T)), static_cast<uint64>(alignment)))),
    full(false),
    size(roundUp(entrySize * capacity, 4096))
{
    assert( powerOfTwo(alignment) );

    memory = reinterpret_cast<uint8*>(virtualAllocate(size, size));
    head   = memory;
    tail   = memory;

    if (!memory)
    {
        size = 0;
    }
}

template<typename T>
CircularQueue<T>::~CircularQueue()
{
    virtualDeallocate((void*)memory, size);
}

template<typename T>
bool CircularQueue<T>::push(const T entry)
{
    if (full)
    {
        return false;
    }
      
    *((T*)tail) = entry;
   
    // Move tail pointer to next entry
    // (wrap around at the beginning of allocation if reached it's end)
    tail = tail + entrySize;
    if (tail == memory + size)
    {
        tail = memory;
    }

    // If tail reached head, buffer is full
    if (tail == head)
    {
        full = true;
    }

    return true;
}
   
template<typename T>
bool CircularQueue<T>::pop(T* entry)
{
    if (head == tail && !full)
    {
        return false;
    }

    *entry = *((T*)head);
   
    // Move head pointer to next entry
    // (wrap around at the beginning of allocation if reached it's end)
    head = head + entrySize;
    if (head == memory + size)
    {
        head = memory;
    }

    full = false;
    return true;
}

} // en

#endif
