/*

 Ngine v5.0
 
 Module      : Strings conversions.
 Requirements: none
 Description : Supports conversions of different
               data types from and to string type.
               Allows programmer to easily append
               numerical values into string form.

*/

#ifndef ENG_UTILITIES_STRINGS
#define ENG_UTILITIES_STRINGS

#include "core/defines.h"
#include "core/types.h"

#include <sstream>

namespace en
{
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

   // To prevent Objective-C contamination, we need to
   // encapsulate template implementation into separate mm file.
//   extern double stringTo_f64(const std::string& in);
//   extern float  stringTo_f32(const std::string& in);
//   extern sint32 stringTo_s32(const std::string& in);
//   extern uint32 stringTo_u32(const std::string& in);
   
template <class type>
type stringTo(const std::string& in)
{
    std::istringstream parser(in);
    type out;
    parser >> out;
    return out;
}

//template<>
//double stringTo<double>(const std::string& in)
//{
//    return stringTo_f64(in);
//}
//
//template<>
//float stringTo<float>(const std::string& in)
//{
//    return stringTo_f32(in);
//}
//
//template<>
//sint32 stringTo<sint32>(const std::string& in)
//{
//    return stringTo_s32(in);
//}
//
//template<>
//uint32 stringTo<uint32>(const std::string& in)
//{
//    return stringTo_u32(in);
//}

#else

// Extracts variable from string
template <class type>
type stringTo(const std::string& in)
{
    std::istringstream parser(in);
    type out;
    parser >> out;
    return out;
}

#endif

// Converts variable value to string representation
std::string stringFrom(uint8  in);
std::string stringFrom(uint16 in);
std::string stringFrom(uint32 in);
std::string stringFrom(uint64 in);
std::string hexStringFrom(uint8 in);
std::string hexStringFrom(uint16 in);
std::string hexStringFrom(uint32 in);
std::string hexStringFrom(uint64 in);
std::string stringFrom(sint8  in);
std::string stringFrom(sint16 in);
std::string stringFrom(sint32 in);
std::string stringFrom(sint64 in);
std::string stringFrom(float  in);
std::string stringFrom(double in);

//template <class type>
//std::string stringFrom(type in)
//{
//#ifndef ENG_IPHONE
//    std::ostringstream ss;
//    ss << in;
//    return  ss.str();
//#else
//    // Works only with integer types !!
//    NSString* nsStringObj = [NSString stringWithFormat:@"%d",in];
//    return std::string([nsStringObj UTF8String]);
//#endif
//}

#ifdef EN_PLATFORM_WINDOWS
std::string stringFromWchar(const wchar_t* text, uint32 size);
wchar_t*    stringToWchar(const char* text, const uint32 size);
#endif

} // en

#endif
