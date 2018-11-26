/*

 Ngine v4.0
 
 Module      : Text parser.
 Requirements: none
 Description : Set of helper functions for text parsing.
               It contains basic functions required to
               parse ASCII based file format or script. 

*/

#include "core/config/config.h"
#include "core/utilities/parser.h"

#include <cassert>

Nparser::Nparser(uint8* buffer, uint64 size) :
    buffer(buffer),
    offset(0),
    size(size)
{
 assert(buffer);
 assert(size);
}

Nparser::~Nparser()
{
 delete [] buffer;
}

bool Nparser::position(uint64 newOffset)
{
 // Check if offset is in range
 if (newOffset >= size)
    return false;

 this->offset = newOffset;
 return true;
}

bool Nparser::read(std::string& word, bool& eol)
{
 // Check if offset is in range
 if (offset >= size)
    return false;

 // Clean passed parameters
 word.erase();
 eol = false;

 // Try to extract first word from given location
 uint8 byte = 0;
 bool found = false;
 for(;;)
    {
    byte = buffer[offset++];

    if (isCharacter(byte))
       {
       word.push_back(byte);
       found = true;
       }
    else
    if (isEol(byte))
       {
       eol = true;
       break;
       }
    else
    if (found)
       break;
    if (offset >= size)
       break;
    }

 return found;
}

bool Nparser::readLine(std::string& line)
{
// Check if offset is in range
if (offset >= size)
   return false;

// Clean passed parameters
line.erase();

// Try to extract first line from given location
uint8 byte = 0;
bool found = false;
for(;;)
   {
   byte = buffer[offset++];
   if (isEol(byte))
      break;

   if (isCharacter(byte))
      found = true;

   line.push_back(byte);
   if (offset >= size)
      break;
   }

return found;
}

bool Nparser::skipToNextLine(void)
{
 // Check if offset is in range
 if (offset >= size)
    return false;

 bool found = false;
 uint8 byte = 0;
 for(;;)
    {
    byte = buffer[offset];

    if (!found)
       {
       if (isEol(byte))
          found = true;
       }
    else
       if (!isEol(byte))
          break;

    offset++;
    }

 return true;
}

bool Nparser::end(void)
{
 return (offset >= size);
}

bool isCypher(uint8 input)
{
return ((input > 47) && (input < 58));
}

bool isCharacter(uint8 input)
{
return ((input > 32) && (input < 127));
}

bool isWhitespace(uint8 input)
{
if ( input == ' '  || 
     input == '\t' || 
     input == '\r' || 
     input == '\n' || 
     input == '\v' || 
     input == '\f' ) 
   return true;
return false;
}

bool isEol(uint8 input)
{
if ( input == '\r' || 
     input == '\n' || 
     input == '\v' || 
     input == '\f' ) 
   return true;
return false;
}

   bool isInteger(const char* text, const uint32 length)
   {
   // Integer notation:
   // 
   // [-][cccc]
   //
   // - negative sign is optional
   // - integer part is required
   //
   // Minimum case: c
   assert( length > 0 );

   uint32 offset = 0;

   // Verify optional negative sign
   if (text[0] == '-')
      ++offset;

   if (offset == length)
      return false;

   // Verify integer part
   for(; offset<length; ++offset)
      if (!isCypher(text[offset]))
         break;

   if (offset == length)
      return true;

   return false;
   }

   bool isFloat(const char* text, const uint32 length)
   {
   // Floating point notation:
   // 
   // [-][cccc].cccc[e/E][cccc][f/F]
   //
   // - negative sign is optional
   // - integer part or fractional part is optional
   // - dot is required
   // - e notation is optional
   // - f/d postfix is optional
   //
   // Minimum case: .x
   assert( length >= 2 );

   uint32 offset = 0;

   // Verify optional negative sign
   if (text[0] == '-')
      ++offset;

   // Verify optional integer part
   bool integerPart = false;
   if (isCypher(text[offset]))
      {
      integerPart = true;
      for(++offset; offset<length; ++offset)
         {
         if (isCypher(text[offset]))
            continue;  
         if (text[offset] == '.')
            break;
         else
            return false;
         }
      }
   if (offset == length)
      return false;

   // Skip required dot
   if (text[offset] == '.')
      ++offset;

   // If there is no integer part, fractional part is required
   if (!integerPart &&
       !isCypher(text[offset]))
      return false;

   // Skip fractional part
   for(; offset<length; ++offset)
      if (!isCypher(text[offset]))
         break;

   if (offset == length)
      return true;

   // Verify optional e/E notation
   if (text[offset] == 'e' ||
       text[offset] == 'E')
      for(++offset; offset<length; ++offset)
         if (!isCypher(text[offset]))
            break;

   if (offset == length)
      return true;

   if (text[offset] == 'f' ||
       text[offset] == 'F' )
      if (offset == length - 1)
         return true;

   return false;
   }