/*

 Ngine v5.0
 
 Module      : Threading primitive.
 Requirements: none
 Description : Provides optimal atomic primitive for sharing across threads.

*/

#ifndef ENG_PARALLEL_SHARED_ATOMIC
#define ENG_PARALLEL_SHARED_ATOMIC

#include "assert.h"

#include "core/defines.h"
#include "core/types.h"
#include "core/memory/alignment.h"

#include <atomic>

namespace en
{

// It is aligned to cacheline size (64 bytes) to prevent false hazard case,
// where two threads access the same cache line, but write to different data
// locations inside of it (so they don't access the same data in parallel, 
// just the same cache line). This would cause expensive cache synchronization 
// (MESI) which would serialize CPU's. This alignment ensures that such 
// synchronization occurs only when value itself is accessed by different
// threads (it might be mutex, semaphore, etc.).
class cachealign SharedAtomic
{
    protected:
    std::atomic<uint32> value;

    public:
    SharedAtomic() { assert((uint64)(&value) % cacheline == 0); value.store(0U, std::memory_order_release); };
   ~SharedAtomic() {};
};

// CompileTimeSizeReporting( SharedAtomic );
static_assert(sizeof(SharedAtomic) == cacheline, "en::SharedAtomic size mismatch!");

} // en

#endif
