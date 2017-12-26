/*

 Ngine v5.0
 
 Module      : iOS/OSX File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.

*/

#include "core/storage/osxStorage.h"
#include "core/storage/osxStorageFile.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

#include <fstream>
using namespace std;

#include "utilities/osxStrings.h"

namespace en
{
   namespace storage
   {
#ifdef APPLE_WAY
   OSXFile::OSXFile(NSData* _handle) :
      handle(_handle),
      CommonFile()
   {
   assert( handle );
   fileSize = [handle length];
   }

   OSXFile::~OSXFile()
   {
   assert( handle );
   deallocateObjectiveC(handle);
   }

   bool OSXFile::read(const uint64 offset, const uint64 _size, void* buffer, uint64* readBytes)
   {
   assert( handle );
   assert( offset + _size <= fileSize );
   
   [handle getBytes:buffer range:NSMakeRange(offset, _size)];

   return true;
   }
  
   // You should not write into your app bundle, and instead use one of the following folders to store files, which can be obtained using NSSearchPathForDirectoriesInDomains:
   //
   // - Document folder (NSDocumentDirectory).
   // - Application Support folder (NSApplicationSupportDirectory).
   // - Caches folder (NSCachesDirectory).

   bool OSXFile::write(const uint64 _size, void* buffer)
   {
   assert( handle );

   return true;
   }
   
   bool OSXFile::write(const uint64 offset, const uint64 _size, void* buffer)
   {
   assert( handle );

   return true;
   }
   
#else

   OSXFile::OSXFile(fstream* _handle) :
      handle(_handle),
      CommonFile()
   {
   assert( handle );
   handle->seekg(0u, ios::end);
   fileSize = handle->tellg();
   handle->seekg(0u, ios::beg);
   }
   
   OSXFile::~OSXFile()
   {
   assert( handle );
   handle->close();
   delete handle;
   }
      
   bool OSXFile::read(const uint64 offset, const uint64 _size, void* buffer, uint64* readBytes)
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
  
   // You should not write into your app bundle, and instead use one of the following folders to store files, which can be obtained using NSSearchPathForDirectoriesInDomains:
   //
   // - Document folder (NSDocumentDirectory).
   // - Application Support folder (NSApplicationSupportDirectory).
   // - Caches folder (NSCachesDirectory).

   bool OSXFile::write(const uint64 _size, void* buffer)
   {
   assert( handle );

   handle->clear();
   handle->seekg(0, ios::beg);
   handle->write((char*)buffer, _size);
   if (handle->bad() || handle->eof())
      return false;
   return true;
   }
   
   bool OSXFile::write(const uint64 offset, const uint64 _size, void* buffer)
   {
   assert( handle );

   handle->clear();
   handle->seekg(offset, ios::beg);
   handle->write((char*)buffer, _size);
   if (handle->bad() || handle->eof())
      return false;
   return true;
   }
#endif
    
    
    
#include <mach-o/dyld.h>  // _NSGetExecutablePath
#include <stdlib.h>       // realpath

#define MaxPathLength 16384

   NSString* fileInBundleByName(const string& filename)
   {
   string name;
   string ext;
   
   // Divide filename to name and extension
   sint64 pos = filename.rfind(".");
   if (pos == string::npos)
      name = filename;
   else
      {
      name = filename.substr(0, pos);
      ext  = filename.substr(pos + 1, filename.length() - pos - 1);
      }
  
   // Find final path to resource in bundle
   NSString* NSname = [NSString stringWithUTF8String: name.c_str()];
   NSString* NSext  = [NSString stringWithUTF8String: ext.c_str()];
   return [[NSBundle mainBundle] pathForResource:NSname ofType:NSext];
 
   //NSURL* url = [[NSBundle mainBundle] URLForResource:stringTo_NSString(filename) withExtension:@"obj"];
   }


#if 0
   // Divide filename to name and extension
   sint32 pos = filename.rfind(".");
   if (pos == string::npos)
      {
      dst = string("");
      return 0;
      }
  
   string name = filename.substr(0, pos);
   string ext  = filename.substr(pos + 1, filename.length() - pos - 1);
  
   // Read file from sandbox to memory
   NSString* NSname = [NSString stringWithUTF8String: name.c_str()];
   NSString* NSext  = [NSString stringWithUTF8String: ext.c_str()];
   NSString* path   = [[NSBundle mainBundle] pathForResource:NSname ofType:NSext];
   NSData*   data   = [NSData dataWithContentsOfFile:path];
  
   size = [data length];
  
   // Checking if there is anything to read
   if (size == 0)
      {
      dst = string("");
   	return 0;
      }
   
   // Creating destination buffer and reading file
   buffer = new uint8[size+1];
   [data getBytes:buffer];
#endif



   OSXInterface::OSXInterface() :
      CommonInterface()
   {
   // Query executable path length
   uint32_t pathSize = 0;
   _NSGetExecutablePath(nullptr, &pathSize);
   assert( pathSize < MaxPathLength );
   
   // Obtain symbolic path of process executable
   char* symbolicPath = new char[pathSize];
   assert( _NSGetExecutablePath(symbolicPath, &pathSize) == 0 );
   
   // Convert to real path
   char* path = new char[pathSize];
   assert( realpath(symbolicPath, path) != NULL );
   delete [] symbolicPath;

   processPath = string(path);
   delete [] path;

   // TODO: Finish
   }

   OSXInterface::~OSXInterface()
   {
   }
   
   bool OSXInterface::exist(const string& filename)
   {
   // TODO: Finish!
   return false;
   }
   
   shared_ptr<File> OSXInterface::open(const string& filename, const FileAccess mode)
   {
   shared_ptr<OSXFile> result = nullptr;
   
   NSString* path = fileInBundleByName(filename);

   if (path)
      {
      // Maps file content to virtual adress space
      NSError* error = nil;
      NSData* data = [NSData dataWithContentsOfFile:path
                                            options:NSMappedRead
                                              error:&error];

      // Alternative approach #1:
      //
      // Gets actual handle to file
      // NSFileHandle *handle = [NSFileHandle fileHandleForReadingAtPath:path];
      // NSData *fileData = [handle readDataOfLength:];
      // [handle closeFile];
      //
      // Alternative approach #2:
      //
      // Read file from sandbox to memory
      //NSData* data = [NSData dataWithContentsOfFile:path];
   
      if ([data length] > 0)
         result = make_shared<OSXFile>(data);
      }
      
   return result;
   }

   }
}
#endif
