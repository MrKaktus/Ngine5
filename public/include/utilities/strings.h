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

// Extracts variable from string
template <class type>
type stringTo(const string& in)
{
#ifndef ENG_IPHONE
 istringstream parser(in);
 type out;
 parser >> out;
 return out;
#else
 // Works only with float types !!
 NSString* nsStringObj = [NSString stringWithUTF8String: in.c_str()];
 return [nsStringObj floatValue];
#endif
}

// Converts variable value to string representation
string stringFrom(uint8  in);
string stringFrom(uint16 in);
string stringFrom(uint32 in);
string stringFrom(uint64 in);
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

#endif
