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

bool Nparser::read(string& word, bool& eol)
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

bool Nparser::readLine(string& line)
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