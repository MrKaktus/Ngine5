/*

 Ngine v5.0
 
 Module      : Utilities
 Requirements: types
 Description : Holds declarations of different macros
               and helper functions that simplifies
               overall programming process and are not
               related with any specific module.
   
*/

#ifndef ENG_UTILITIES
#define ENG_UTILITIES

#include <string>                 // For C++11 strongly typed enum cast
#include <type_traits>

#include "core/defines.h"
#include "core/types/basic.h"
#include "core/types/float2.h"
#include "core/types/float4.h"

namespace en
{
#ifndef M_PI
   #define  M_PI 3.141592653589793238462643383279
#endif

   // Bit manipulation 
   #define setBit(number, bit)    number |= (1ULL << bit)
   #define clearBit(number, bit)  number &= ~(1ULL << bit)
   #define changeBit(number, bit) number ^= (1ULL << bit)

   // Bitmask manipulation (some defines are longer than just using bit operators, but they are declared for readability of code, and prevention of typos)
   #define setBitmask(number, bitmask)    number |= bitmask
   #define clearBitmask(number, bitmask)  number &= ~bitmask
   #define copyBitmask(destination, source, bitmask)  destination = (destination & ~bitmask) | (source & bitmask)

   // Bit query
   #define checkBit(number, bit)  (number & (1 << bit))
   #define checkBitmask(number, bitMask)  ((number & bitMask) == bitMask)
   #define getBit(number, bit)    (number & (1 << bit))
   #define bitChanged(previous, current, bit) (checkBit(current, bit) != checkBit(previous, bit))
   #define bitsChanged(previous, current, bitMask) ((current & bitMask) != (previous & bitMask))

   // Integer division rounded up
   #define intDivUp(number, divisor) ((number + (divisor - 1)) / divisor)

   uint32 bitsCount(uint32 number);

   // Returns position of first set MSB, or false if in is 0
   bool highestBit(uint32 in, uint32& number);

   // Checks if given value is in range
   template<typename T>
   bool inRange(T value, T min, T max)
   {
   return (value >= min && value <= max);
   }

#ifndef min
   template<typename T>
   T min(T a, T b)
   {
      return a < b ? a : b;
   }
#endif

#ifndef max
   template<typename T>
   T max(T a, T b)
   {
      return a >= b ? a : b;
   }
#endif
 
   // Checks if given point is inside of rectangle
   // x,y - position
   // z,w - width, height
   bool inRectangle(float4 rect, float2 position);
   
   // Checks if given number is a power of two (returns true for 0)
   // See more at: http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
   #define powerOfTwo(in) !(in & (in-1))

   // Returns next power of two for given numer (in case of 0, returns 0)
   uint16 nextPowerOfTwo(uint8  in);
   uint32 nextPowerOfTwo(uint16 in);
   uint64 nextPowerOfTwo(uint32 in);
   uint64 nextPowerOfTwo(uint64 in);
   
   // Returns which power of two given number is
   bool whichPowerOfTwo(uint32 in, uint32& power);
   bool whichPowerOfTwo(uint64 in, uint32& power);
   
   // Aligns given adress to multiple of alignment (it needs to be power of two or zero)
   uint32 align(uint32 adress, uint32 alignment);
   uint64 align(uint64 adress, uint32 alignment);

   // Rounds up number to multiple of given alignment
   uint32 roundUp(const uint32 num, const uint32 alignment);
   uint64 roundUp(const uint64 num, const uint64 alignment);
   
   // Changes endiannes of variable
   uint16 endiannes(uint16 in);
   uint32 endiannes(uint32 in);
   uint64 endiannes(uint64 in);
   
   // Radians and Degrees conversions
   float  radians(float s);
   double radians(double s);
   float  degrees(float s);
   double degrees(double s);
   
   // Casts C++11 strongly typed enum value to it's underlying type
   // Note: Visual Studio 2013 supports 'constexpr' only with 'November CTP'.
   //       Visual Studio 2015 supports it by default.
   template <typename E>
   constexpr typename std::underlying_type<E>::type underlyingType(E e)
   {
      return static_cast<typename std::underlying_type<E>::type>(e);
   }
}

#endif
