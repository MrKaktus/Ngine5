/*
 
 Ngine v5.0
 
 Module      : File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.
 
*/

#ifndef ENG_CORE_STORAGE_CONTEXT
#define ENG_CORE_STORAGE_CONTEXT

#include "core/storage.h"

#include <iostream>
#include <fstream>
using namespace std;

#if defined(EN_PLATFORM_ANDROID)
// for native asset manager
#include <sys/types.h>
//#include <android/native_activity.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

namespace en
{
   namespace storage
   {
   struct Context
          {
          Context();
         ~Context();

#if defined(EN_PLATFORM_ANDROID)
          AAssetManager* manager;

          bool create(AAssetManager* assetManager);
#endif
#if defined(EN_PLATFORM_BLACKBERRY) || defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
          bool create(void);
#endif
          void destroy(void);

          // Internal engine methods for logging
          fstream* openForOverwriting(const string& filename);
          void     close(fstream* file);
          };
   }

extern storage::Context StorageContext;
}

#endif