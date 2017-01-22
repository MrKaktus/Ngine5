/*

 Ngine v5.0
 
 Module      : Windows File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.

*/

#include "core/storage/winStorage.h"

#if defined(EN_PLATFORM_WINDOWS)

#include <iostream>
#include <fstream>
using namespace std;

namespace en
{
   namespace storage
   {
   WinFile::WinFile(fstream* _handle) :
      handle(_handle),
      CommonFile()
   {
   assert( handle );
   handle->seekg(0u, ios::end);
   fileSize = handle->tellg();
   handle->seekg(0u, ios::beg);
   }
   
   WinFile::~WinFile()
   {
   assert( handle );
   handle->close();
   delete handle;
   }
   
   bool WinFile::read(const uint64 offset, const uint64 _size, void* buffer, uint64* readBytes)
   {
   assert( handle );
   assert( offset + _size <= fileSize );
   
   handle->clear();
   handle->seekg(offset, ios::beg);
   handle->read((char*)buffer, _size);
   uint64 read = handle->gcount();
   if (readBytes != nullptr)
      *readBytes = read;

   if (read != _size)
      return false;
   
   if (handle->bad() || handle->eof())
      return false;
      
   return true;
   }

   // You should not write into your app bundle, and instead
   // use one of the specific folders to store files.
   //
   // TODO: Which folders are those on BlackBerryOS 10?

   bool WinFile::write(const uint64 _size, void* buffer)
   {
   assert( handle );

   handle->clear();
   handle->seekg(0, ios::beg);
   handle->write((char*)buffer, _size);
   if (handle->bad() || handle->eof())
      return false;
   return true;
   }
   
   bool WinFile::write(const uint64 offset, const uint64 _size, void* buffer)
   {
   assert( handle );

   handle->clear();
   handle->seekg(offset, ios::beg);
   handle->write((char*)buffer, _size);
   if (handle->bad() || handle->eof())
      return false;
   return true;
   }
    
    
    
 
   WinInterface::WinInterface() :
      CommonInterface()
   {

   }

   WinInterface::~WinInterface()
   {
   }
   
   bool WinInterface::exist(const string& filename)
   {
   ifstream* rfile;
          
   // Creates input stream
   rfile = new ifstream(filename.c_str(), ios::in | ios::binary | ios::ate);
   if (rfile->fail())
      return false;

   rfile->close();
   return true;
   }
   
   Ptr<File> WinInterface::open(const string& filename, const FileAccess mode)
   {
   Ptr<WinFile> result = nullptr;
   
   fstream* handle = nullptr;

   if (mode == Read)
      handle = new fstream(filename.c_str(), ios::in  | ios::binary);
   else if (mode == Write)
      handle = new fstream(filename.c_str(), ios::out | ios::binary); 
   else
      handle = new fstream(filename.c_str(), ios::in  | ios::out | ios::binary);

   if (handle->good())
      result = new WinFile(handle);
   else
      delete handle;

   return raw_reinterpret_cast<File>(&result);
   }

   }
}
#endif
