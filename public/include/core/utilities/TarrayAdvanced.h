/*

 Ngine v5.0
 
 Module      : Advanced array template.
 Requirements: none
 Description : Simple thread-safe array template. It stores 
               objects in array of fixed size. Constructors 
               and destructors of objects are called directly 
               on allocated memory.


*/

// TODO: This should be named: "MemoryPool"

#ifndef ENG_CORE_UTILITIES_TARRAY_ADVANCED
#define ENG_CORE_UTILITIES_TARRAY_ADVANCED

#include "core/defines.h"
#include "threading/mutex.h"
#include "utilities/utilities.h"

#include "assert.h"
#include <bitset>
using namespace std;

template <typename obj, uint32 maxsize>
class cachealign TarrayAdvanced
      {
      private:
      bitset<maxsize> used; // Flags indicating if specified field is used
      void*   buffer; // Pointer to array of objects
      uint32* list;   // List of free fields in array
      uint32  count;  // Count of free elements

      uint32  size;   // Total array size
      Mutex   lock;   // Mutex for thread safety

      TarrayAdvanced(const TarrayAdvanced&);
      TarrayAdvanced& operator=(const TarrayAdvanced&);

      public:
      TarrayAdvanced();
     ~TarrayAdvanced();

      bool create(uint32 size);
      obj* allocate(void);       // Creates object and adds it into array.
      bool free(obj* const ptr); // Deletes object and removes it from array.
      };

template <typename obj, uint32 maxsize>
TarrayAdvanced<obj, maxsize>::TarrayAdvanced() :
   buffer(NULL),
   list(NULL),
   count(0),
   size(0)
{
}

template <typename obj, uint32 maxsize>
TarrayAdvanced<obj, maxsize>::~TarrayAdvanced()
{
obj* element = static_cast<obj*>(buffer);

// Call explicitly destructors on all stored objects
for(uint32 i=0; i<size; ++i)
   if (used[i])
	   element[i].~obj();

// Free memory
delete [] static_cast<uint8*>(buffer);
delete [] list;
}

template <typename obj, uint32 maxsize>
bool TarrayAdvanced<obj, maxsize>::create(uint32 _size)
{
// Check if not already created
if (buffer)
   return false;
if (list)
   return false;

// Allocate memory for objects array
buffer = operator new[] (_size * sizeof(obj));
if (!buffer)
   return false;

// Allocate memory for free objects list
list = new uint32[_size];
if (!list)
   {
   delete [] static_cast<obj*>(buffer);
   return false;
   }

// Mark all fields as free
count = _size;
size = _size;
used.reset();
for(uint32 i=0; i<size; ++i)
   list[i] = i;

return true;
}

template <typename obj, uint32 maxsize>
obj* TarrayAdvanced<obj, maxsize>::allocate(void)
{
assert(buffer);
assert(list);

// Be sure to be thread safe
lock.lock();

// Check if not full already
if (count == 0)
   {
   lock.unlock();
   return NULL;
   }

// Reserve one entry
uint16 index = list[count-1];
--count;

// Call constructor of object on pre-allocated memory
obj* element = static_cast<obj*>(buffer);
new (element + index) obj();
used[index] = true;

// Exiting critical section
lock.unlock();
return (element + index);
}

template <typename obj, uint32 maxsize>
bool TarrayAdvanced<obj, maxsize>::free(obj* const ptr)
{
assert(buffer);
assert(list);
assert(ptr);

// Calculate object index in array
if ( reinterpret_cast<uint64>(ptr) < reinterpret_cast<uint64>(buffer) )
   return false;
uint32 index = static_cast<uint32>( reinterpret_cast<uint64>(ptr) - reinterpret_cast<uint64>(buffer) ) / sizeof(obj);
if (index >= size)
   return false;

// Be sure to be thread safe
lock.lock();

// Check if really alocated
if (used[index] == false)
   {
   lock.unlock();
   return false;
   }

// Destroy object and mark its field as free for reusing
obj* element = (obj*)buffer;
(element + index)->obj::~obj();
used[index] = false;
list[count] = index;
count++;
lock.unlock();
return true;
}

#endif
