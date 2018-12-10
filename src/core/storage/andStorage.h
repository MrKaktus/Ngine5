/*

 Ngine v5.0
 
 Module      : Android File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.

*/

#ifndef ENG_CORE_STORAGE_ANDROID
#define ENG_CORE_STORAGE_ANDROID

#include "core/storage/storage.h"

#if defined(EN_PLATFORM_ANDROID)
namespace en
{
   namespace storage
   {
   class AndFile : public CommonFile
      {
      public:
      AAsset* handle;
      
      virtual bool   read(const uint64 offset,
                          const uint64 size,
                          void* buffer,
                          uint64* readBytes = nullptr); // Reads part of file
         
      virtual bool   write(const uint64 size,
                           void* buffer);            // Writes block of data to file
      virtual bool   write(const uint64 offset,
                           const uint64 size,
                           void* buffer);            // Writes to file at specified location

      AndFile(AAsset* handle);
      virtual ~AndFile();
      };
      
   class AndInterface : public CommonStorage
      {
      public:
      AAssetManager* manager;

      virtual bool exist(const std::string& filename); // Check if file exist
      virtual std::shared_ptr<File> open(const std::string& filename,
                             const FileAccess mode = Read);  // Opens file

      AndInterface();
      virtual ~AndInterface();
      };
   }
}
#endif

#endif
