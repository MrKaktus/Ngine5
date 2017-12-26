/*

 Ngine v5.0
 
 Module      : BlackBerryOS 10 File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.

*/

#include "core/storage/bbStorage.h"

#if defined(EN_PLATFORM_BLACKBERRY)

#include <fstream>
using namespace std;

namespace en
{
   namespace storage
   {
   BBFile::BBFile(FILE* _handle) :
      handle(_handle),
      CommonFile()
   {
   assert( handle );
   fseek(handle, 0u, SEEK_END);
   fileSize = ftell(handle);
   fseek(handle, 0u, SEEK_SET);
   }
   
   BBFile::~BBFile()
   {
   assert( handle );
   fclose(handle);
   handle = nullptr;
   }
      
   bool BBFile::read(const uint64 offset, const uint64 _size, void* buffer, uint64* readBytes)
   {
   assert( handle );
   assert( offset + _size <= fileSize );
   
   fseeko64(handle, offset, SEEK_SET);
   uint64 numBytesRead = fread((char*)buffer, _size, 1, handle);
   
   if (readBytes != nullptr)
      *readBytes = numBytesRead;

   if (numBytesRead != _size)
      return false;

   return true;
   }

   // You should not write into your app bundle, and instead
   // use one of the specific folders to store files.
   //
   // TODO: Which folders are those on BlackBerryOS 10?

   bool BBFile::write(const uint64 _size, void* buffer)
   {
   // TODO: Finish
   return false;
   }
   
   bool BBFile::write(const uint64 offset, const uint64 _size, void* buffer)
   {
   // TODO: Finish
   return false;
   }
    
   BBInterface::BBInterface() :
      CommonInterface()
   {
   // TODO: Finish!
   }
   
   BBInterface::~BBInterface()
   {
   }
   
   bool BBInterface::exist(const string& filename)
   {
   FILE* file = fopen(string("app/native/" + filename).c_str(), "r");
   if (file == nullptr)
      return false;

   fclose(file);
   return true;
   }
   
   shared_ptr<File> BBInterface::open(const string& filename, const FileAccess mode)
   {
   shared_ptr<BBFile> result = nullptr;

   FILE* handle;

   if (mode == Read)
      handle = fopen(string("app/native/" + filename).c_str(), "r");
   else
   if (mode == Write)
      handle = fopen(string("app/native/" + filename).c_str(), "w");
   else
      handle = fopen(string("app/native/" + filename).c_str(), "rw");

   if (handle)
      result = make_shared<BBFile>(handle);
  
   return result;
   }
   
   }
}
#endif
