/*

 Ngine v5.0
 
 Module      : OSX Strings conversions.
 Requirements: none
 Description : Supports conversions of different
               data types from and to string type.
               Allows programmer to easily append
               numerical values into string form.

*/

#include "core/defines.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include "utilities/osxStrings.h"

namespace en
{
   NSString* stringTo_NSString(const string& in)
   {
   return [NSString stringWithUTF8String:in.c_str()];
   }

   double stringTo_f64(const string& in)
   {
   return [[NSString stringWithUTF8String:in.c_str()] doubleValue];
   }
   
   float stringTo_f32(const string& in)
   {
   return [[NSString stringWithUTF8String:in.c_str()] floatValue];
   }

   sint32 stringTo_s32(const string& in)
   {
   return [[NSString stringWithUTF8String:in.c_str()] intValue];
   }

   uint32 stringTo_u32(const string& in)
   {
   return static_cast<uint32>([[NSString stringWithUTF8String:in.c_str()] longLongValue]);
   }
}
#endif
