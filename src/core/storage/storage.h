/*

 Ngine v5.0
 
 Module      : Common File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.

*/

#ifndef ENG_CORE_STORAGE_COMMON
#define ENG_CORE_STORAGE_COMMON

#include "core/storage.h"

namespace en
{
   namespace storage
   {
   class CommonFile : public File
      {
      public:
      uint64 fileSize;
      
      virtual uint64 size(void);                     // File size in bytes
      virtual bool   read(volatile void* buffer);             // Reads whole file to specified buffer

      // Schould be implemented by specialization class
      virtual bool   read(const uint64 offset,
                          const uint64 size,
                          volatile void* buffer,
                          uint64* readBytes = nullptr); // Reads part of file
         
      // TODO: Temp, file should be read in blocks to memory, and then parsed!
      virtual uint32 read(const uint64 offset,
                          const uint32 maxSize,
                          std::string& word);         // Read zero terminated word, but not exceed maxSize chars. Return word length.
      virtual uint32 readWord(const uint64 offset,
                              const uint32 maxSize,
                              std::string& word);
      virtual uint32 readLine(const uint64 offset,
                              const uint32 maxSize,
                              std::string& line);

      CommonFile();
      virtual ~CommonFile() {};
      };
      
   class CommonInterface : public Interface
      {
      public:
      std::string processPath;     // Global path to this process
      
      virtual uint64 read(const std::string& filename, std::string& dst);

      CommonInterface();
      virtual ~CommonInterface();
      };
   }
}
#endif
