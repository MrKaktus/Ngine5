/*

 Ngine v4.0
 
 Module      : Multithreading support.
 Visibility  : Any version.
 Requirements: none
 Description : Brings set of all basic threading
               primitives like atomic operations,
               threads local storage, mutexes and
               so on.

*/

#ifndef ENG_THREADING
#define ENG_THREADING

#include "core/defines.h"
#include "core/types.h"

#include "core/threading/atomics.h"
#include "threading/Ntls.h"
//#include "threading/task.h"

// Describes range and granularity of data 
// in some array that will be processed in
// parallel.
class NRange
      {
      public:
      uint32 start;
      uint32 end;
      uint32 granularity;

      forceinline NRange();
      forceinline NRange(const NRange& src);
      forceinline NRange(const uint32 start, const uint32 end);
      forceinline NRange(const uint32 start, const uint32 end, const uint32 granularity);
      };

forceinline NRange::NRange() :
   start(0),
   end(0),
   granularity(1)
{
}

forceinline NRange::NRange(const NRange& src) :
   start(src.start),
   end(src.end),
   granularity(src.granularity)
{
}

forceinline NRange::NRange(const uint32 _start, const uint32 _end) :
   start(_start),
   end(_end),
   granularity(1)
{
}

forceinline NRange::NRange(uint32 _start, uint32 _end, uint32 _granularity) :
   start(_start),
   end(_end),
   granularity(_granularity)
{
}


#endif