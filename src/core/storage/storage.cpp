/*
 
 Ngine v4.0
 
 Module      : File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.
 
*/

#include "core/storage/context.h" 
#include "core/utilities/parser.h"
#include "utilities/strings.h"

#include "assert.h"

#ifdef EN_PLATFORM_ANDROID
#include "core/log/log.h"
#endif
#ifdef EN_PLATFORM_BLACKBERRY
#include <stdio.h>
#endif

namespace en
{
   namespace storage
   {

#ifdef EN_PLATFORM_ANDROID
   Nfile::Nfile(AAsset* handle) :
       handle(handle),
       filesize(0)
   {
   if (handle)
      filesize = AAsset_getLength(handle);
   else
      Log << "Error creating file in constructor - invalid handle!!";
   }
#endif
#ifdef EN_PLATFORM_BLACKBERRY
   Nfile::Nfile(FILE* handle) :
       handle(handle),
       filesize(0)
   {
   if (handle)
      {
      fseek(handle, 0, SEEK_END);
      filesize = (uint32)ftell(handle);
      fseek(handle, 0, SEEK_SET);
      }
   }
#endif
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
   Nfile::Nfile(fstream* handle) :
       handle(handle),
       filesize(0)
   {
   if (handle)
      {
      handle->seekg(0, ios::end);
      filesize = handle->tellg();
      handle->seekg(0, ios::beg);
      }
   }
#endif

   Nfile::~Nfile()
   {
#ifdef EN_PLATFORM_ANDROID
   if (handle)
      {
      AAsset_close(handle);
      handle = NULL;
      }
#endif
#ifdef EN_PLATFORM_BLACKBERRY
   if (handle)
      {
      fclose(handle);
      handle = NULL;
      }   
#endif
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
   if (handle)
      {
      handle->close();
      delete handle;
      }
#endif
   }

#ifdef EN_PLATFORM_ANDROID
   uint32 Nfile::size(void)
   {
   return filesize;
   }
#endif
#ifdef EN_PLATFORM_BLACKBERRY
   uint64 Nfile::size(void)
   {
   return filesize;
   }
#endif
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
   uint64 Nfile::size(void)
   {
   return filesize;
   }
#endif


#ifdef EN_PLATFORM_ANDROID
   bool Nfile::read(void* buffer)
   {
   if (!handle)
      return false;
   AAsset_seek(handle, 0, SEEK_SET);
   uint32 numBytesRead = AAsset_read(handle, buffer, filesize); 
   if (numBytesRead != filesize)
      {
      Log << "Error when reading file, read " << numBytesRead << " from " << filesize << " bytes!";
      return false;
      }
   return true;
   }
#endif
#ifdef EN_PLATFORM_BLACKBERRY
   bool Nfile::read(void* buffer)
   {
   if (handle == NULL)
      return false;
   fseek(handle, 0, SEEK_SET);
   uint32 read = fread((char*)buffer, 1, filesize, handle);
   if (read != filesize)
      return false;
   return true;
   }
#endif
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
   bool Nfile::read(void* buffer)
   {
   if (!handle)
      return false;
   handle->clear();
   handle->seekg(0,ios::beg);
   handle->read((char*)buffer,filesize);
   if (handle->bad() || handle->eof())
      return false;
   return true;
   }
#endif

#ifdef EN_PLATFORM_ANDROID
   bool Nfile::read(const uint32 offset, const uint32 size, void* buffer)
   {
   if (!handle)
      return false;
   AAsset_seek(handle, offset, SEEK_SET);
   uint32 numBytesRead = AAsset_read(handle, buffer, size); 
   if (numBytesRead != size)
      {
      Log << "Error when reading file, read " << numBytesRead << " from " << size << " bytes!";
      return false;
      }
   return true;
   }
#endif
#ifdef EN_PLATFORM_BLACKBERRY
   bool Nfile::read(const uint64 offset, const uint32 size, void* buffer)
   {
   if (handle == NULL)
      return false;
   fseeko64(handle, offset, SEEK_SET);
   if (fread((char*)buffer, size, 1, handle) != size)
      return false;
   return true;
   }
#endif
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
   bool Nfile::read(const uint64 offset, const uint32 size, void* buffer)
   {
   if (!handle)
      return false;
   handle->clear();
   handle->seekg(offset,ios::beg);
   handle->read((char*)buffer, size);
   if (handle->bad() || handle->eof())
      return false;
   return true;
   }
#endif

   uint32 Nfile::read(const uint64 offset, const uint32 maxSize, string& word)
   {
   word.clear();
   uint64 fileOffset = offset;
   uint32 counter=0;
   uint8 letter;
   for(; counter<maxSize; ++counter)
      {
      if (!read(fileOffset, 1, &letter))
         return counter;

      if (!letter)
         return counter;

      fileOffset++;
      word.push_back(letter);
      }

   return counter;
   }

   uint32 Nfile::readWord(const uint64 offset, const uint32 maxSize, string& word)
   {
   word.clear();
   uint64 fileOffset = offset;
   uint32 counter=0;
   uint8 letter;
   for(; counter<maxSize; ++counter)
      {
      if (!read(fileOffset, 1, &letter))
         return counter;

      if (isWhitespace(letter))
         return counter;

      fileOffset++;
      word.push_back(letter);
      }

   return counter;
   }

   uint32 Nfile::readLine(const uint64 offset, const uint32 maxSize, string& line)
   {
   line.clear();
   uint64 fileOffset = offset;
   uint32 counter=0;
   uint8 letter;
   for(; counter<maxSize; ++counter)
      {
      if (!read(fileOffset, 1, &letter))
         return counter;

      if (isEol(letter))
         return counter;

      fileOffset++;
      line.push_back(letter);
      }

   return counter;
   }

   bool Nfile::write(const uint32 size, void* buffer)
   {
   if (!handle)
      return false;
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
   handle->clear();
   handle->seekg(0,ios::beg);
   handle->write((char*)buffer,size);
   if (handle->bad() || handle->eof())
      return false;
#endif
   return true;
   }

   bool Nfile::write(const uint64 offset, const uint32 size, void* buffer)
   {
   if (!handle)
      return false;
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
   handle->clear();
   handle->seekg(offset,ios::beg);
   handle->write((char*)buffer,size);
   if (handle->bad() || handle->eof())
      return false;
#endif
   return true;
   }


   Context::Context()
   {
   }

   Context::~Context()
   {
   }

#ifdef EN_PLATFORM_ANDROID
   bool Context::create(AAssetManager* assetManager)
   {
   Log << "Starting module: Storage.\n";

   manager = assetManager;
   return true;
   }
#endif
#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
   bool Context::create(void)
   {
   return true;
   }
#endif

   void Context::destroy(void)
   {
   }

   fstream* Context::openForOverwriting(const string& filename)
   {
   fstream* wfile;

   // Creates input stream
   wfile = new fstream(filename.c_str(), ios::out | ios::ate);
   if (wfile->fail())
      return NULL;

   return wfile;
   }

   void Context::close(fstream* file)
   {
   if (file != NULL)
      {
      file->close();
      delete file;
      }
   }

   bool Interface::open(const string& filename, Nfile** file, FileAccess mode)
   {
#ifdef EN_PLATFORM_ANDROID
   assert( mode == Read );

   AAsset* asset = AAssetManager_open(StorageContext.manager, filename.c_str(), AASSET_MODE_STREAMING); 
   if (!asset) 
      { 
      Log << string("Couldn't open file " + filename + "!"); 
      return false;
      } 

   *file = new Nfile(asset);
   return true;
#endif
#ifdef EN_PLATFORM_BLACKBERRY
   FILE* handle;

   if (mode == Read)
      handle = fopen(string("app/native/" + filename).c_str(), "r");
   else if (mode == Write)
      handle = fopen(string("app/native/" + filename).c_str(), "w");
   else
      handle = fopen(string("app/native/" + filename).c_str(), "rw");

   if (handle == NULL)
      return false;
  
   *file = new Nfile(handle);
   return true;
#endif
#ifdef EN_PLATFORM_WINDOWS
   fstream* handle = nullptr;

   if (mode == Read)
      handle = new fstream(filename.c_str(), ios::in  | ios::binary);
   else if (mode == Write)
      handle = new fstream(filename.c_str(), ios::out | ios::binary); 
   else
      handle = new fstream(filename.c_str(), ios::in  | ios::out | ios::binary);

   if (!handle->good())
      {
      delete handle;
      return false;
      }
  
   *file = new Nfile(handle);
   return true;
#endif

   return false;
   }

   uint32 Interface::read(const string& filename, string& dst)
   {
   uint32    size   = 0;
   uint8*    buffer = NULL;

#ifdef EN_PLATFORM_ANDROID
   // Creates input stream
   AAsset* handle = AAssetManager_open(StorageContext.manager, filename.c_str(), AASSET_MODE_STREAMING); 
   if (!handle) 
      { 
      Log << string("Couldn't open file " + filename + "!");
      dst = string("");
      return 0;
      } 

   // Determine file size
   size = AAsset_getLength(handle);

   // Checking if there is anything to read
   if (size == 0)
      {
      Log << string("File " + filename + " is empty!");
      dst = string("");
      return 0;
      }
   
   // Creating destination buffer and reading file
   buffer = new uint8[size+1];
   AAsset_seek(handle, 0, SEEK_SET);
   uint32 numBytesRead = AAsset_read(handle, buffer, size); 
   if (numBytesRead != size)
      {
      Log << string("Error when reading file, read " + stringFrom(numBytesRead) + " from " + stringFrom(size) + " bytes!");
      dst = string("");
      return 0;
      }
   AAsset_close(handle);
   handle = NULL;
#endif  
#ifdef EN_PLATFORM_IPHONE
   // Divide filename to name and extension
   sint32 pos  = filename.rfind(".");
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
#ifdef EN_PLATFORM_BLACKBERRY
   FILE* file = fopen(string("app/native/" + filename).c_str(), "r");
   if (file == NULL)
      {
      dst = string("");
      return 0;
      }

   // Determine file size
   fseek(file, 0, SEEK_END);
   size = (uint32)ftell(file);
   fseek(file, 0, SEEK_SET);
   
   // Checking if there is anything to read
   if (size == 0)
      {
      fclose(file);
      dst = string("");
      return 0;
      }

   // Creating destination buffer and reading file
   buffer = new uint8[size+1];
   fread((char*)buffer, size, 1, file);
   fclose(file);
#endif
#ifdef EN_PLATFORM_WINDOWS
   ifstream* rfile;
          
   // Creates input stream
   rfile = new ifstream(filename.c_str(), ios::in | ios::binary | ios::ate);
   if (rfile->fail())
      {
      dst = string("");
      return 0;
      }
  	
   // Determine file size
   rfile->seekg(0,ios::end);
   size = (uint32)rfile->tellg();
   rfile->seekg(0,ios::beg);
   
   // Checking if there is anything to read
   if (size == 0)
      {
      dst = string("");
      return 0;
      }
   
   // Creating destination buffer and reading file
   buffer = new uint8[size+1];
   rfile->read((char*)buffer, size);
   rfile->close();
   delete rfile;
#endif
   
   // Converts file content to string
   buffer[size] = 0;
   dst = string((const char *)buffer);
   delete [] buffer;
   
   // Return results
   return size+1;
   }


   bool Interface::exist(const string& filename)
   {
#ifdef EN_PLATFORM_ANDROID
   // Creates input stream
   AAsset* handle = AAssetManager_open(StorageContext.manager, filename.c_str(), AASSET_MODE_STREAMING); 
   if (!handle) 
      return false;

   AAsset_close(handle);
   return true;
#endif  
#ifdef EN_PLATFORM_IPHONE
   // TODO
   assert(0);
#endif  
#ifdef EN_PLATFORM_BLACKBERRY
   FILE* file = fopen(string("app/native/" + filename).c_str(), "r");
   if (file == NULL)
      return false;

   fclose(file);
   return true;
#endif
#ifdef EN_PLATFORM_WINDOWS
   ifstream* rfile;
          
   // Creates input stream
   rfile = new ifstream(filename.c_str(), ios::in | ios::binary | ios::ate);
   if (rfile->fail())
      return false;

   rfile->close();
   return true;
#endif
   assert(0);
   return false;
   }

   }

storage::Context   StorageContext;
storage::Interface Storage;
}