/*

 Ngine v5.0
 
 Module      : Random numbers generator.
 Requirements: none
 Description : Set of functions that support random
               numbers generation.

*/

#include "core/defines.h"
#include "core/types.h"
#include "utilities/random.h"

#if defined(EN_PLATFORM_ANDROID)
#include "time.h"
#endif

#if defined(EN_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// Generates random number in range [0,max-1]
uint32 random(uint32 max)
{
uint32 result = 0;

#if defined(EN_PLATFORM_ANDROID)
struct timespec now;
clock_gettime(CLOCK_MONOTONIC, &now);
result = ((uint32)now.tv_sec * 1000000000LL + now.tv_nsec) % max;
#endif

#if defined(EN_PLATFORM_WINDOWS)
result = GetTickCount() % max;
#endif

return result;
}
