/*

 Ngine v5.0
 
 Module      : Strings conversions.
 Requirements: none
 Description : Supports conversions of different
               data types from and to string type.
               Allows programmer to easily append
               numerical values into string form.

*/

#ifndef ENG_UTILITIES_TSTRINGS
#define ENG_UTILITIES_TSTRINGS

#include "core/defines.h"
#include "core/types.h"

#include <sstream>
using namespace std;

namespace en
{
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

   // To prevent Objective-C contamination, we need to
   // encapsulate template implementation into separate mm file.
//   extern double stringTo_f64(const string& in);
//   extern float  stringTo_f32(const string& in);
//   extern sint32 stringTo_s32(const string& in);
//   extern uint32 stringTo_u32(const string& in);
   
   template <class type>
   type stringTo(const string& in)
   {
   istringstream parser(in);
   type out;
   parser >> out;
   return out;
   }

//   template<>
//   double stringTo<double>(const string& in)
//   {
//   return stringTo_f64(in);
//   }
//   
//   template<>
//   float stringTo<float>(const string& in)
//   {
//   return stringTo_f32(in);
//   }
//
//   template<>
//   sint32 stringTo<sint32>(const string& in)
//   {
//   return stringTo_s32(in);
//   }
//
//   template<>
//   uint32 stringTo<uint32>(const string& in)
//   {
//   return stringTo_u32(in);
//   }

#else
   // Extracts variable from string
   template <class type>
   type stringTo(const string& in)
   {
   istringstream parser(in);
   type out;
   parser >> out;
   return out;
   }
#endif


   // Converts variable value to string representation
   string stringFrom(uint8  in);
   string stringFrom(uint16 in);
   string stringFrom(uint32 in);
   string stringFrom(uint64 in);
   string hexStringFrom(uint8 in);
   string hexStringFrom(uint16 in);
   string hexStringFrom(uint32 in);
   string hexStringFrom(uint64 in);
   string stringFrom(sint8  in);
   string stringFrom(sint16 in);
   string stringFrom(sint32 in);
   string stringFrom(sint64 in);
   string stringFrom(float  in);
   string stringFrom(double in);
   
   //template <class type>
   //string stringFrom(type in)
   //{
   //#ifndef ENG_IPHONE
   // ostringstream ss;
   // ss << in;
   // return  ss.str();
   //#else
   // // Works only with integer types !!
   // NSString* nsStringObj = [NSString stringWithFormat:@"%d",in];
   // return string([nsStringObj UTF8String]);
   //#endif
   //}
   
   #ifdef EN_PLATFORM_WINDOWS
   string stringFromWchar(const wchar_t* text, uint32 size);
   #endif
}

#endif
