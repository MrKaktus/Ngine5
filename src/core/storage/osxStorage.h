/*

 Ngine v5.0
 
 Module      : iOS/OSX File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.

*/

#ifndef ENG_CORE_STORAGE_OSX
#define ENG_CORE_STORAGE_OSX

#include "core/storage/storage.h"

#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)

// Use Appe NS calls
#define APPLE_WAY

//#ifdef APPLE_WAY
//#undef aligned
//#include <Foundation/Foundation.h>  // NSBundle
//#endif

namespace en
{
   namespace storage
   {      
   class OSXInterface : public CommonInterface
      {
      public:
      virtual bool exist(const string& filename); // Check if file exist
      virtual Ptr<File> open(const string& filename,
                             const FileAccess mode = Read);  // Opens file

      OSXInterface();
      virtual ~OSXInterface();
      };
   }
}
#endif

#endif
