/*
 
 Ngine v5.0
 
 Module      : File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.
 
*/

#ifndef ENG_STORAGE
#define ENG_STORAGE

#include "core/defines.h"
#include "core/types.h"

#include <string>
using namespace std;

#ifdef EN_PLATFORM_ANDROID
// for native asset manager
#include <sys/types.h>
#include <android/configuration.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/storage_manager.h>
#include <android/obb.h>
#endif

namespace en
{
   namespace storage
   {
   enum FileAccess
      {
      Read      = 0,
      Write        ,
      ReadWrite
      };

   class Nfile
         {
         private:
#if defined(EN_PLATFORM_ANDROID)
         AAsset*  handle;    // File handle
         uint32   filesize;  // File size      TODO: Migrate to 64bit
#endif
#if defined(EN_PLATFORM_BLACKBERRY)
         FILE*    handle;    // File handle
         uint64   filesize;  // File size
#endif
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
         fstream* handle;    // File stream handle
         uint64   filesize;  // File size
#endif

         public:
#if defined(EN_PLATFORM_ANDROID)
         Nfile(AAsset* handle);
#endif
#if defined(EN_PLATFORM_BLACKBERRY)
         Nfile(FILE* handle);
#endif
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
         Nfile(fstream* handle);
#endif
        ~Nfile();

#if defined(EN_PLATFORM_ANDROID)
         uint32 size(void);                     // Returns file size   
#endif
#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
         uint64 size(void);                     // Returns file size   
#endif                      
         bool   read(void* buffer);             // Reads whole file to specified buffer
#if defined(EN_PLATFORM_ANDROID)
         bool   read(const uint32 offset, 
                     const uint32 size, 
                     void* buffer);             // Reads part of file
#endif
#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
         bool   read(const uint64 offset, 
                     const uint32 size, 
                     void* buffer);             // Reads part of file
#endif
         uint32 read(const uint64 offset,
                     const uint32 maxSize,
                     string& word);             // Read zero terminated word, but not exceed maxSize chars. Return word length.
         uint32 readWord(const uint64 offset, 
                         const uint32 maxSize, 
                         string& word);         // Reads string until whitespace is found
         uint32 readLine(const uint64 offset, 
                         const uint32 maxSize, 
                         string& line);         // Reads string to the end of line 

         bool   write(const uint32 size, 
                      void* buffer);            // Writes block of data to file
         bool   write(const uint64 offset, 
                      const uint32 size, 
                      void* buffer);            // Writes to file at specified location
         };

   struct Interface
          {
          bool open(const string& filename, Nfile** file, FileAccess mode = Read);  // Opens file
          uint32 read(const string& filename, string& dst);                         // Read file to string
          bool exist(const string& filename); // Check if file exist
          };
   }

extern storage::Interface Storage;
}

#endif