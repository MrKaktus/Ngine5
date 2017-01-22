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

#include "core/utilities/TintrusivePointer.h"

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

   class File : public SafeObject<File>
      {
      public:
      virtual uint64 size(void) = 0;                 // File size in bytes
      virtual bool   read(void* buffer) = 0;         // Reads whole file to specified buffer
      virtual bool   read(const uint64 offset,
                          const uint64 size,
                          void* buffer,
                          uint64* readBytes = nullptr) = 0; // Reads part of file
         
      // TODO: Temp, file should be read in blocks to memory, and then parsed!
      virtual uint32 read(const uint64 offset,
                          const uint32 maxSize,
                          string& word) = 0;         // Read zero terminated word, but not exceed maxSize chars. Return word length.
      virtual uint32 readWord(const uint64 offset,
                              const uint32 maxSize,
                              string& word) = 0;
      virtual uint32 readLine(const uint64 offset,
                              const uint32 maxSize,
                              string& line) = 0;
         
      virtual bool   write(const uint64 size,
                           void* buffer) = 0;        // Writes block of data to file
      virtual bool   write(const uint64 offset,
                           const uint64 size,
                           void* buffer) = 0;        // Writes to file at specified location

      virtual ~File() {};                            // Polymorphic deletes require a virtual base destructor
      };

   class Interface : public SafeObject<Interface>
      {
      public:
      static bool create(void);                       // Creates instance of this class (OS specific) and assigns it to "Storage".

      virtual bool exist(const string& filename) = 0; // Check if file exist
      virtual Ptr<File> open(const string& filename,
                             const FileAccess mode = Read) = 0;  // Opens file
         
      virtual uint64 read(const string& filename,
                          string& dst) = 0;          // Convenience method for reading whole file as string

      virtual ~Interface() {};                       // Polymorphic deletes require a virtual base destructor
      };
   }

extern Ptr<storage::Interface> Storage;
}

#endif
