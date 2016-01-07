/*

 Ngine v5.0
 
 Module      : Strings conversions.
 Requirements: none
 Description : Supports conversions of different
               data types from and to string type.
               Allows programmer to easily append
               numerical values into string form.

*/

#include "utilities/strings.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_BLACKBERRY)
#define sprintf_s sprintf
#endif

#if defined(EN_PLATFORM_OSX)
#define sprintf_s snprintf
#endif

namespace en
{
   string stringFrom(uint8 in)
   {
   char buffer[8];
   uint8 out = sprintf_s(buffer, 8, "%u", in);
   return string(buffer,out); 
   }
   
   string stringFrom(uint16 in)
   {
   char buffer[8];
   uint8 out = sprintf_s(buffer, 8, "%hu", in);
   return string(buffer,out); 
   }
   
   string stringFrom(uint32 in)
   {
   char buffer[16];
   uint8 out = sprintf_s(buffer, 16, "%u", in);
   return string(buffer,out); 
   }
   
   string stringFrom(uint64 in)
   {
   char buffer[32];
   uint8 out = sprintf_s(buffer, 32, "%llu", in);
   return string(buffer,out); 
   }

   string stringFrom(sint8 in)
   {
   char buffer[8];
   uint8 out = sprintf_s(buffer, 8, "%i", in);
   return string(buffer,out); 
   }
   
   string stringFrom(sint16 in)
   {
   char buffer[8];
   uint8 out = sprintf_s(buffer, 8, "%hi", in);
   return string(buffer,out); 
   }
   
   string stringFrom(sint32 in)
   {
   char buffer[16];
   uint8 out = sprintf_s(buffer, 16, "%i", in);
   return string(buffer,out); 
   }
   
   string stringFrom(sint64 in)
   {
   char buffer[32];
   uint8 out = sprintf_s(buffer, 32, "%lli", in);
   return string(buffer,out); 
   }
   
   string stringFrom(float in)
   {
   // TODO: Prevent from overflowing
   char buffer[512];
   uint8 out = sprintf_s(buffer, 512, "%f", in);
   return string(buffer, out);
   }
   
   string stringFrom(double in)
   {
   // TODO: Prevent from overflowing
   char buffer[512];
   uint8 out = sprintf_s(buffer, 512, "%f", in);
   return string(buffer, out);
   }

#ifdef EN_PLATFORM_WINDOWS
   string stringFromWchar(const wchar_t* text, uint32 size)
   {
   size_t i;
   char*  buffer = new char[size];
   string result;
   
   wcstombs_s(&i, buffer, (size_t)size, text, (size_t)size);
   
   result = string(buffer);
   delete [] buffer;
   return result;
   }
#endif

}
