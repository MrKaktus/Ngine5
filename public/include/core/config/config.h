/*

 Ngine v5.0
 
 Module      : Engine configuration.
 Requirements: none
 Description : Holds declarations of Startup settings 
               that need to be known during engine 
               startup for setting up all subsystems. 
               It also maintains Run-Time settings that 
               need to be known during application execution.

*/

#ifndef ENG_CORE_CONFIG
#define ENG_CORE_CONFIG

#include "core/defines.h"
#include "core/types.h"
#include "utilities/Nversion.h"

#include <string>
using namespace std;

namespace en
{
   namespace config
   { 
   struct Interface
          {
          // If there was variable set in configuration 
          // files that match the requested name, value
          // corresponding to it will be written to pointed
          // destination. 
          bool get(const string& name, bool* destination); 
          bool get(const string  name, bool* destination); 
          bool get(const char*   name, bool* destination);
          bool get(const string& name, uint8* destination);
          bool get(const string  name, uint8* destination);  
          bool get(const char*   name, uint8* destination);
          bool get(const string& name, uint16* destination);
          bool get(const string  name, uint16* destination);
          bool get(const char*   name, uint16* destination);
          bool get(const string& name, sint16* destination);
          bool get(const string  name, sint16* destination);
          bool get(const char*   name, sint16* destination);
          bool get(const string& name, uint32* destination); 
          bool get(const string  name, uint32* destination); 
          bool get(const char*   name, uint32* destination); 
          bool get(const string& name, string& destination);
          bool get(const string  name, string& destination);
          bool get(const char*   name, string& destination);
          bool get(const string& name, Nversion& destination);
          bool get(const string  name, Nversion& destination);
          bool get(const char*   name, Nversion& destination);
          };
   }

extern config::Interface Config;
}

#endif