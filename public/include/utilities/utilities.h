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
   #define setBit(number, bit)    (number |= 1 << bit)
   #define clearBit(number, bit)  (number &= ~(1 << bit))
   #define changeBit(number, bit) (number ^= 1 << bit)
   #define checkBit(number, bit)  (number & (1 << bit))
   #define getBit(number, bit)    (number & (1 << bit))
   
   // Checks if given value is in range
   template<typename T>
   bool inRange(T value, T min, T max)
   {
   return (value >= min && value <= max);
   }

   template<typename T>
   T min(T a, T b)
   {
      return a < b ? a : b;
   }
   
   template<typename T>
   T max(T a, T b)
   {
      return a > b ? a : b;
   }
   
   // Checks if given point is inside of rectangle
   // x,y - position
   // z,w - width, height
   bool inRectangle(float4 rect, float2 position);
   
   // Checks if given number is a power of two
   bool powerOfTwo(uint8  in);
   bool powerOfTwo(uint16 in);
   bool powerOfTwo(uint32 in);
   bool powerOfTwo(uint64 in);
   
   // Returns next power of two for given numer
   uint16 nextPowerOfTwo(uint8  in);
   uint32 nextPowerOfTwo(uint16 in);
   uint64 nextPowerOfTwo(uint32 in);
   uint64 nextPowerOfTwo(uint64 in);
   
   // Returns which power of two given number is
   bool whichPowerOfTwo(uint32 in, uint32& power);
   bool whichPowerOfTwo(uint64 in, uint32& power);
   
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
   template <typename E>
   constexpr typename std::underlying_type<E>::type underlyingType(E e)
   {
      return static_cast<typename std::underlying_type<E>::type>(e);
   }
}

#endif