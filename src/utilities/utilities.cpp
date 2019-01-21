/*

 Ngine v5.0
 
 Module      : Utilities
 Requirements: types
 Description : Holds declarations of different macros
               and helper functions that simplifies
               overall programming process and are not
               related with any specific module.
   
*/

#include "assert.h"
#include <math.h>
#include "utilities/utilities.h"

namespace en
{
   // Checks if given point is inside of rectangle
   // x,y - position
   // z,w - width, height
   bool inRectangle(float4 rect, float2 position)
   {
   if ((position.x > rect.x) &&
       (position.y > rect.y) &&
       (position.x < rect.x + rect.z) &&
       (position.y < rect.y + rect.w))
      return true;
   return false;
   }
   
   uint32 bitsCount(uint32 number)
   {
   number = number - ((number >> 1) & 0x55555555);
   number = (number & 0x33333333) + ((number >> 2) & 0x33333333);
   return (((number + (number >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
   }

   // Returns position of first set MSB, or false if in is 0
   bool highestBit(const uint32 in, uint32& number)
   {
   if (in == 0)
      return false;

   number = in;
   number |= (number >>  1);
   number |= (number >>  2);
   number |= (number >>  4);
   number |= (number >>  8);
   number |= (number >> 16);
   number = number ^ (number >> 1);
   return true;
   }
   // Alternative:
   // number = 1 << (fls(in) - 1)

   //uint32 index = 0;
   //while(in != 0)
   //   {
   //   if (in == 1)
   //      break;
   //   in = in >> 1;
   //   index++;
   //   }

   //number = index;
   //return true;
   //}
 
   // Returns next power of two for given numer (in case of 0, returns 0).
   // If number is already power of two, the same value is returned.
   uint16 nextPowerOfTwo(uint8 in)
   {
   uint16 temp = in;
   temp--;
   temp |= temp >> 1;
   temp |= temp >> 2;
   temp |= temp >> 4;
   temp++;
   return temp;
   }
   
   uint32 nextPowerOfTwo(uint16 in)
   {
   uint32 temp = in;
   temp--;
   temp |= temp >> 1;
   temp |= temp >> 2;
   temp |= temp >> 4;
   temp |= temp >> 8;
   temp++;
   return temp;
   }
   
   uint64 nextPowerOfTwo(uint32 in)
   {
   uint64 temp = in;
   temp--;
   temp |= temp >> 1;
   temp |= temp >> 2;
   temp |= temp >> 4;
   temp |= temp >> 8;
   temp |= temp >> 16;
   temp++;
   return temp;
   }
   
   uint64 nextPowerOfTwo(uint64 in)
   {
   uint64 temp = in;
   temp--;
   temp |= temp >> 1;
   temp |= temp >> 2;
   temp |= temp >> 4;
   temp |= temp >> 8;
   temp |= temp >> 16;
   temp |= temp >> 32;
   temp++;
   return temp;
   }
   
   bool whichPowerOfTwo(uint32 in, uint32& power)
   {
   if (!in) 
      return false;
   if (!powerOfTwo(in))
      return false;
   
   power = 0;
   while(!(in % 2))
      {
      in = in >> 1;
      ++power;
      }
   return true;
   }
   
   bool whichPowerOfTwo(uint64 in, uint32& power)
   {
   if (!in) 
      return false;
   if (!powerOfTwo(in))
      return false;
   
   power = 0;
   while(!(in % 2))
      {
      in = in >> 1;
      ++power;
      }
   return true;
   }

   uint32 align(uint32 adress, uint32 alignment)
   {
   // Alignment need to be power of two
   assert( !(alignment & (alignment - 1)) );

   if (alignment == 0u)
      return adress;
   
   return static_cast<uint32>( (adress + (uint64(alignment) - 1ull)) & ~(uint64(alignment) - 1ull) );
   }

   uint64 align(uint64 adress, uint32 alignment)
   {
   // Alignment need to be power of two
   assert( !(alignment & (alignment - 1)) );

   if (alignment == 0u)
      return adress;
   
   return ( (adress + (uint64(alignment) - 1ull)) & ~(uint64(alignment) - 1ull) );
   }
   
   uint32 roundUp(uint32 num, uint32 alignment)
   {
   if (alignment == 0u)
      return num;
   
   uint32 remainder = num % alignment;
   if (remainder == 0u)
      return num;
   
   return num + alignment - remainder;
   }

   uint64 roundUp(uint64 num, uint64 alignment)
   {
   if (alignment == 0u)
      return num;
   
   uint64 remainder = num % alignment;
   if (remainder == 0u)
      return num;
   
   return num + alignment - remainder;
   }
   
   // Changes endiannes of variable
   uint16 endiannes(uint16 in)
   {
   return (in >> 8) | (in << 8);
   }
   
   uint32 endiannes(uint32 in)
   {
   return (in >> 24) | (in << 24) |
          ((in & 0x00FF0000) >> 8) | ((in & 0x0000FF00) << 8);
   }
   
   uint64 endiannes(uint64 in)
   {
   return (in >> 56) | (in << 56) |
          ((in & 0x00FF000000000000) >> 40) | ((in & 0x000000000000FF00) << 40) |
          ((in & 0x0000FF0000000000) >> 24) | ((in & 0x0000000000FF0000) << 24) |
          ((in & 0x000000FF00000000) >> 8)  | ((in & 0x00000000FF000000) << 8);
   }
   
   // Change of degrees to radians
   float radians(float s)
   {
   return static_cast<float>((s*M_PI)/180.0);
   }
   
   double radians(double s)
   {
   return (s*M_PI)/180.0;
   }
   
   // Change radians to degrees
   float degrees(float s)
   {
   return static_cast<float>((s*180.0)/M_PI);
   }
   
   double degrees(double s)
   {
   return (s*180.0)/M_PI;
   }

}
