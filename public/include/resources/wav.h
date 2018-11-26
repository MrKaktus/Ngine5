/*

 Ngine v5.0
 
 Module      : WAV files support
 Requirements: none
 Description : Supports reading sounds from *.wav files.

*/

#ifndef ENG_RESOURCES_WAV
#define ENG_RESOURCES_WAV

#include "core/defines.h"
#include "core/types.h"

namespace en
{
   namespace wav
   {
   std::shared_ptr<audio::Sample> load(const std::string& filename);
   }
}

#endif
