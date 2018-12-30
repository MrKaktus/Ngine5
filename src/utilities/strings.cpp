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
#include "assert.h"

#if defined(EN_PLATFORM_ANDROID) || defined(EN_PLATFORM_BLACKBERRY)
#define sprintf_s sprintf
#endif

#if defined(EN_PLATFORM_OSX)
#define sprintf_s snprintf
#endif

namespace en
{
   std::string stringFrom(uint8 in)
   {
   char buffer[8];
   uint8 out = sprintf_s(buffer, 8, "%u", in);
   return std::string(buffer, out); 
   }
   
   std::string stringFrom(uint16 in)
   {
   char buffer[8];
   uint8 out = sprintf_s(buffer, 8, "%hu", in);
   return std::string(buffer, out); 
   }
   
   std::string stringFrom(uint32 in)
   {
   char buffer[16];
   uint8 out = sprintf_s(buffer, 16, "%u", in);
   return std::string(buffer, out); 
   }
   
   std::string stringFrom(uint64 in)
   {
   char buffer[32];
   uint8 out = sprintf_s(buffer, 32, "%llu", in);
   return std::string(buffer, out); 
   }

   std::string hexStringFrom(uint8 in)
   {
   char buffer[2];
   uint8 out = sprintf_s(buffer, 2, "%02x", in);
   return std::string(buffer, out); 
   }

   std::string hexStringFrom(uint16 in)
   {
   char buffer[4];
   uint8 out = sprintf_s(buffer, 4, "%04x", in);
   return std::string(buffer, out); 
   }

   std::string hexStringFrom(uint32 in)
   {
   char buffer[8];
   uint8 out = sprintf_s(buffer, 8, "%08x", in);
   return std::string(buffer, out); 
   }

   std::string hexStringFrom(uint64 in)
   {
   char buffer[32];
   assert( sprintf_s(&buffer[0], 32, "%08X", in > 32) == 8 );
   assert( sprintf_s(&buffer[8], 24, "%08X", static_cast<uint32>(in)) == 8 );
   return std::string(buffer, 16); 
   }

   std::string stringFrom(sint8 in)
   {
   char buffer[8];
   uint8 out = sprintf_s(buffer, 8, "%i", in);
   return std::string(buffer, out); 
   }
   
   std::string stringFrom(sint16 in)
   {
   char buffer[8];
   uint8 out = sprintf_s(buffer, 8, "%hi", in);
   return std::string(buffer, out); 
   }
   
   std::string stringFrom(sint32 in)
   {
   char buffer[16];
   uint8 out = sprintf_s(buffer, 16, "%i", in);
   return std::string(buffer, out); 
   }
   
   std::string stringFrom(sint64 in)
   {
   char buffer[32];
   uint8 out = sprintf_s(buffer, 32, "%lli", in);
   return std::string(buffer, out); 
   }
   
   std::string stringFrom(float in)
   {
   // TODO: Prevent from overflowing
   char buffer[512];
   uint8 out = sprintf_s(buffer, 512, "%f", in);
   return std::string(buffer, out);
   }
   
   std::string stringFrom(double in)
   {
   // TODO: Prevent from overflowing
   char buffer[512];
   uint8 out = sprintf_s(buffer, 512, "%f", in);
   return std::string(buffer, out);
   }

#if defined(EN_PLATFORM_WINDOWS)
   std::string stringFromWchar(const wchar_t* text, uint32 size)
   {
   size_t i;
   char*  buffer = new char[size];
   std::string result;
   
   wcstombs_s(&i, buffer, (size_t)size, text, (size_t)size);
   
   result = std::string(buffer);
   delete [] buffer;
   return result;
   }

   wchar_t* stringToWchar(const char* text, const uint32 size)
   {
   size_t i;
   wchar_t* buffer = new wchar_t[size];

   mbstowcs_s(&i, buffer, (size_t)size, text, (size_t)size);  
   return buffer;
   }
#endif

}
