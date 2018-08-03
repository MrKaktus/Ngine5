/*

 Ngine v5.0
 
 Module      : iOS/OSX File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.

*/

#ifndef ENG_CORE_STORAGE_FILE_OSX
#define ENG_CORE_STORAGE_FILE_OSX

#include "core/storage/storage.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#ifdef APPLE_WAY
#undef aligned
#include <Foundation/Foundation.h>  // NSBundle
#endif

namespace en
{
   namespace storage
   {
   class OSXFile : public CommonFile
      {
      public:
#ifdef APPLE_WAY
      NSData* handle;
#else
      fstream* handle;    // File stream handle
#endif

      virtual bool   read(const uint64 offset,
                          const uint64 size,
                          volatile void* buffer,
                          uint64* readBytes = nullptr); // Reads part of file
         
      virtual bool   write(const uint64 size,
                           void* buffer);            // Writes block of data to file
      virtual bool   write(const uint64 offset,
                           const uint64 size,
                           void* buffer);            // Writes to file at specified location

#ifdef APPLE_WAY
      OSXFile(NSData* handle);
#else
      OSXFile(fstream* handle);
#endif
      virtual ~OSXFile();
      };
   }
}
#endif

#endif
