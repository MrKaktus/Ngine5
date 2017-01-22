/*

 Ngine v5.0
 
 Module      : iOS/OSX File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.

*/

#ifndef ENG_CORE_STORAGE_OSX
#define ENG_CORE_STORAGE_OSX

#include "core/storage/storage.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
namespace en
{
   namespace storage
   {
   class OSXFile : public CommonFile
      {
      public:
      fstream* handle;    // File stream handle
      
      virtual bool   read(const uint64 offset,
                          const uint64 size,
                          void* buffer,
                          uint64* readBytes = nullptr); // Reads part of file
         
      virtual bool   write(const uint64 size,
                           void* buffer);            // Writes block of data to file
      virtual bool   write(const uint64 offset,
                           const uint64 size,
                           void* buffer);            // Writes to file at specified location

      OSXFile(fstream* handle);
      virtual ~OSXFile();
      };
      
   class OSXInterface : public CommonInterface
      {
      public:
      virtual bool exist(const string& filename); // Check if file exist
      virtual Ptr<File> open(const string& filename,
                             const FileAccess mode = Read);  // Opens file

      OSXInterface();
      virtual ~OSXInterface();
      };
   }
}
#endif

#endif
