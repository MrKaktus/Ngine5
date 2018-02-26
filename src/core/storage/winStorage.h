/*

 Ngine v5.0
 
 Module      : Windows File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.

*/

#ifndef ENG_CORE_STORAGE_WINDOWS
#define ENG_CORE_STORAGE_WINDOWS

#include "core/storage/storage.h"

// Disable this to use WinAPI instead
#define UseFStreamOverWinAPI 1

#if defined(EN_PLATFORM_WINDOWS)
namespace en
{
   namespace storage
   {
   class WinFile : public CommonFile
      {
      public:
#if UseFStreamOverWinAPI
      fstream* handle;
#else
      HANDLE handle;
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

#if UseFStreamOverWinAPI
      WinFile(fstream* handle);
#else
      WinFile(HANDLE handle);
#endif
      virtual ~WinFile();
      };
      
   class WinInterface : public CommonInterface
      {
      public:
      virtual bool exist(const string& filename); // Check if file exist
      virtual shared_ptr<File> open(const string& filename,
                             const FileAccess mode = Read);  // Opens file

      WinInterface();
      virtual ~WinInterface();
      };
   }
}
#endif

#endif
