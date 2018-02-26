/*

 Ngine v5.0
 
 Module      : Allocator.
 Requirements: none
 Description : Provides interface for blocks allocation algorithm.

*/

#ifndef ENG_CORE_ALGORITHMS_ALLOCATOR
#define ENG_CORE_ALGORITHMS_ALLOCATOR

#include "core/types.h"

namespace en
{
   // Interface for allocation algorithm
   class Allocator
      {
      public:
      // Returns offset for new allocation, or false
      virtual bool allocate(const uint64 requestedSize,
                            const uint64 requestedAlignment,
                                  uint64& offset) = 0;

      // Deallocates given memory region
      virtual bool deallocate(const uint64 offset,
                              const uint64 size) = 0;  

      virtual ~Allocator() {};                         // Polymorphic deletes require a virtual base destructor
      };
}

#endif