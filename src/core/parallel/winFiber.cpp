/*

 Ngine v5.0
 
 Module      : Threads support.
 Requirements: none
 Description : Allows easy creation and management of threads.

*/

#include "core/parallel/winFiber.h"

#if defined(EN_PLATFORM_WINDOWS)

#include "assert.h"
using namespace std;

namespace en
{
   winFiber::winFiber(const uint32 stackSize, const uint32 _maximumStackSize) :
      maximumStackSize(_maximumStackSize)
   {
   handle = CreateFiberEx(stackSize,
                          maximumStackSize,
DWORD dwFlags,
                          (LPFIBER_START_ROUTINE)functionExecutingTask,
                          this);
   }


/*
#include <Windows.h>

LPVOID handle = CreateFiber(
SIZE_T dwStackSize,
LPFIBER_START_ROUTINE lpStartAdress,
LPVOID lpParameter);

LPVOID handle = CreateFiberEx(
SIZE_T dwStackCommitSize,
SIZE_T dwStackReserveSize,
DWORD dwFlags,
LPFIBER_START_ROUTINE lpStartAdress,
LPVOID lpParameter);
*/

}
#endif
