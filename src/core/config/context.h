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

#ifndef ENG_CORE_CONFIG_CONTEXT
#define ENG_CORE_CONFIG_CONTEXT

#include "core/configuration.h"
#include "core/storage.h"
#include "core/config/config.h"
#include "utilities/Nversion.h"

#include <map>
using namespace std;

namespace en
{
   namespace config
   {
   struct Context
          {
          // Parsed variables of different types
          map<string, bool>     bools;
          map<string, uint8>    uint8s;
          map<string, uint16>   uint16s;
          map<string, sint16>   sint16s;
          map<string, uint32>   uint32s;
          map<string, string>   strings;
          map<string, Nversion> versions;

          Context();
         ~Context();

          bool create(void);
          void destroy(void);
          };
   }

extern config::Context ConfigContext;
}

#endif