/*

 Ngine v5.0
 
 Module      : Page Allocator.
 Requirements: none
 Description : Allocates memory and reserver adress space range for future 
               growth of allocation on page granularity.

*/

#include "assert.h"

#include "core/utilities/pageAllocator.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <mach/vm_map.h>
#endif

#if defined(EN_PLATFORM_WINDOWS)
// Only really needs WinBase.h for Virtual Memory
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace en
{
   void* allocate(const uint64 size, const uint64 maximumSize)
   {
   // Size and maximum size needs to be explicitly multiple of 4KB
   assert( size % 4096 == 0 );
   assert( maximumSize % 4096 == 0 );

   void* temp = nullptr;
   
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

   // First reserve max size, to which given allocation can grow
   mach_vm_address_t address = 0;
   kern_return_t result = mach_vm_allocate(mach_task_self(),
                                           &address,
                                           (mach_vm_size_t)maximumSize,
                                           VM_FLAGS_ANYWHERE);
      
   // Once adress space for max possible size is reserved, allocate initial size
   if (result == KERN_SUCCESS)
      {
      temp = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
      
      result = mach_vm_behavior_set(mach_task_self(),
                                    address,
                                    (mach_vm_size_t)size,
                                    VM_BEHAVIOR_WILLNEED);
         
      // Mark rest of reserved adress space as not needed. See also:
      // https://stackoverflow.com/questions/21809072/virtual-memory-on-osx-ios-versus-windows-commit-reserve-behaviour
      kern_return_t result2;
      result2 = mach_vm_behavior_set(mach_task_self(),
                                     address + size,
                                     (mach_vm_size_t)(maximumSize - size),
                                     VM_BEHAVIOR_DONTNEED);
         
    
      if (result != KERN_SUCCESS ||
          result2 != KERN_SUCCESS)
         {
         // Couldn't alllocate physical pages, reverting reservation and faulting
         mach_vm_deallocate(mach_task_self(),
                            address,
                            (mach_vm_size_t)maximumSize);
         temp = nullptr;
         }
      }

#elif defined(EN_PLATFORM_WINDOWS)
   // First reserve max size, to which given allocation can grow
   temp = VirtualAlloc(nullptr,      // Reserve anywhere in adress space
                       maximumSize,  // Max size to which this allocation can grow
                       MEM_RESERVE,  // Reserve adress space without allocating memory
                       0);           // No page properties during reserve

   // Once adress space for max possible size is reserved, allocate initial size
   if (temp)
      {
      if (!VirtualAlloc(temp,            // Allocate pages at start of reserved space
                        size,            // Initial size to allocate
                        MEM_COMMIT,      // Pre-allocate memory (will truly allocate and clear on first access)
                        PAGE_READWRITE)) // No page properties during reserve
         {
         // Couldn't alllocate physical pages, reverting reservation and faulting
         VirtualFree(temp, 0, MEM_RELEASE);
         temp = nullptr;
         }
      }
#else
   static_assert(0, "Virtual Memory allocation not implemented on this platform!");
#endif

   return temp;
   }

   bool reallocate(void* address, const uint64 currentSize, const uint64 newSize)
   {
   // Current and new size needs to be explicitly multiple of 4KB
   assert( currentSize % 4096 == 0 );
   assert( newSize % 4096 == 0 );

   void*  subAddress = static_cast<void*>(static_cast<uint8*>(address) + currentSize);
   uint64 growSize   = newSize - currentSize;
   
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
   kern_return_t result = mach_vm_behavior_set(mach_task_self(),
                                               static_cast<mach_vm_address_t>(reinterpret_cast<uintptr_t>(subAddress)),
                                               (mach_vm_size_t)growSize,
                                               VM_BEHAVIOR_WILLNEED);
   if (result != KERN_SUCCESS)
      return false;
   
#elif defined(EN_PLATFORM_WINDOWS)
   if (!VirtualAlloc(subAddress,      // Allocate pages at end of already allocated section
                     growSize,        // Size to grow allocation by
                     MEM_COMMIT,      // Pre-allocate memory (will truly allocate and clear on first access)
                     PAGE_READWRITE)) // No page properties during reserve
      return false;
#else
   static_assert(0, "Virtual Memory allocation not implemented on this platform!");
#endif

   return true;
   }

   void deallocate(void* address, const uint64 maximumSize)
   {
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
   kern_return_t error = mach_vm_deallocate(mach_task_self(),
                                            static_cast<mach_vm_address_t>(reinterpret_cast<uintptr_t>(address)),
                                            (mach_vm_size_t)maximumSize);
   if (error != KERN_SUCCESS)
   {
      mach_error("Virtual Memory destruction failed!", error);
   }

#elif defined(EN_PLATFORM_WINDOWS)
   VirtualFree(address, 0, MEM_RELEASE);
#else
   static_assert(0, "Virtual Memory allocation not implemented on this platform!");
#endif
   }
}
