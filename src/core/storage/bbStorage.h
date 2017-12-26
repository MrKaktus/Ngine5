/*

 Ngine v5.0
 
 Module      : BlackBerryOS 10 File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.

*/

#ifndef ENG_CORE_STORAGE_BLACKBERRY
#define ENG_CORE_STORAGE_BLACKBERRY

#include "core/storage/storage.h"

#if defined(EN_PLATFORM_BLACKBERRY)

#include <stdio.h>

namespace en
{
   namespace storage
   {
   class BBFile : public CommonFile
      {
      public:
      FILE*  handle;

      virtual bool   read(const uint64 offset,
                          const uint64 size,
                          void* buffer,
                          uint64* readBytes = nullptr); // Reads part of file

      virtual bool   write(const uint64 size,
                           void* buffer);            // Writes block of data to file
      virtual bool   write(const uint64 offset,
                           const uint64 size,
                           void* buffer);            // Writes to file at specified location

      BBFile(FILE* handle);
      virtual ~BBFile();
      };
      
   class BBInterface : public CommonInterface
      {
      public:
      virtual bool exist(const string& filename); // Check if file exist
      virtual shared_ptr<File> open(const string& filename,
                             const FileAccess mode = Read);  // Opens file

      BBInterface();
      virtual ~BBInterface();
      };
   }
}
#endif

#endif
