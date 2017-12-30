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
      // If variable was set in configuration files, that match the requested
      // name, value corresponding to it will be written to pointed destination.
      bool get(const string& name);
      bool get(const string  name);
      bool get(const char*   name);

      bool get(const string& name, bool* destination);
      bool get(const string  name, bool* destination);
      bool get(const char*   name, bool* destination);

      bool get(const string& name, sint64* destination);
      bool get(const string  name, sint64* destination);
      bool get(const char*   name, sint64* destination);

      bool get(const string& name, double* destination);
      bool get(const string  name, double* destination);
      bool get(const char*   name, double* destination);

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
