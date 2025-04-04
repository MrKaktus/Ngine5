/*

 Ngine v5.0
 
 Module      : ZIP package support
 Requirements: none
 Description : Supports reading compressed and
               uncompressed files from *.zip 
               archive.

*/

#ifndef ENG_RESOURCES_ZIP
#define ENG_RESOURCES_ZIP

#include "core/alignment.h"
#include "core/types.h"

namespace en
{
namespace zip
{

// File Entry Header
alignTo(1)
struct Header
{
    uint32 signature;   // Signature of File Entry Header -> 04034b50
    uint16 version;     // Minimum zip version required to extract file
    uint16 flags;       // General purpose flags
    uint16 compression; // Compression type
    uint16 time;        // File last modification time
    uint16 date;        // File last modification date
    uint32 crc32;       // Checksum
    uint32 sizePacked;  // Size after compression
    uint32 size;        // Orginal uncompressed size
    uint16 nameLength;  // Length of file name
    uint32 extraLength; // Length of extra field
};
alignToDefault

} // en::zip
} // en

#endif
