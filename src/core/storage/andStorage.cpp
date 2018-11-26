/*

 Ngine v5.0
 
 Module      : Android File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.

*/

#include "core/storage/andStorage.h"

#if defined(EN_PLATFORM_ANDROID)

#include <fstream>

// for native asset manager
#include <sys/types.h>
#include <android/configuration.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/storage_manager.h>
#include <android/obb.h>

#include "core/log/log.h"

namespace en
{
   namespace storage
   {
   AndFile::AndFile(AAsset* _handle) :
      handle(_handle),
      CommonFile()
   {
   assert( handle );
   fileSize = AAsset_getLength(handle);   
   }
   
   AndFile::~AndFile()
   {
   assert( handle );
   AAsset_close(handle);
   handle = nullptr;
   }
      
   bool AndFile::read(const uint64 offset, const uint64 _size, void* buffer, uint64* readBytes)
   {
   assert( handle );
   assert( offset + _size <= fileSize );
   
   AAsset_seek(handle, offset, SEEK_SET);
   uint64 numBytesRead = AAsset_read(handle, buffer, _size);
   
   if (readBytes != nullptr)
      *readBytes = numBytesRead;

   if (numBytesRead != _size)
      return false;

   return true;
   }
  
   // You should not write into your app bundle, and instead
   // use one of the specific folders to store files.
   //
   // TODO: Which folders are those on Android ?

   bool AndFile::write(const uint64 _size, void* buffer)
   {
   // TODO: Finish
   return false;
   }
   
   bool AndFile::write(const uint64 offset, const uint64 _size, void* buffer)
   {
   // TODO: Finish
   return false;
   }
    
    
    
    
      
#define MaxPathLength 16384




   AndInterface::AndInterface() :
      manager(nullptr),
      CommonInterface()
   {
   Log << "Starting module: Storage.\n";
   
   AAssetManager* assetManager; // TODO: Acquire it!

   manager = assetManager;
   }
   
   bool AndInterface::exist(const std::string& filename)
   {
   // TODO: Finish!
   return false;
   }
   
   std::shared_ptr<File> AndInterface::open(const std::string& filename, const FileAccess mode)
   {
   std::shared_ptr<AndFile> result = nullptr;
      
   // TODO: Open for writing
   assert( mode == Read );

   AAsset* asset = AAssetManager_open(manager, filename.c_str(), AASSET_MODE_STREAMING);
   if (asset)
      result = std::make_shared<AndFile>(asset);
   else
      Log << std::string("Couldn't open file " + filename + "!"); 

   return result;
   }


   
   bool AndInterface::exist(const std::string& filename);
   {
   // Creates input stream
   AAsset* handle = AAssetManager_open(manager, filename.c_str(), AASSET_MODE_STREAMING); 
   if (!handle) 
      return false;

   AAsset_close(handle);
   return true;
   }
   
   AndInterface::~AndInterface()
   {
   }

   }
}
#endif
