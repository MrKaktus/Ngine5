/*

 Ngine v5.0
 
 Module      : OSX Strings conversions.
 Requirements: none
 Description : Supports conversions of different
               data types from and to string type.
               Allows programmer to easily append
               numerical values into string form.

*/

#ifndef ENG_UTILITIES_OSX_STRINGS
#define ENG_UTILITIES_OSX_STRINGS

#include "core/defines.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#import <Foundation/NSString.h>

#include "utilities/strings.h"

namespace en
{
   extern NSString* stringTo_NSString(const std::string& in);

// Looks like OSX/gcc sucks at Templates

//   template<>
//   NSString* stringTo<NSString*>(const std::string& in)
//   {
//   return [NSString stringWithUTF8String:in.c_str()];
//   }
}
#endif

#endif
