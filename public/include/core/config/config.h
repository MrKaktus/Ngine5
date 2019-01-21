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

namespace en
{
   namespace config
   { 
   struct Interface
      {
      // If variable was set in configuration files, that match the requested
      // name, value corresponding to it will be written to pointed destination.
      bool get(const std::string& name);
      bool get(const std::string  name);
      bool get(const char*        name);

      bool get(const std::string& name, bool* destination);
      bool get(const std::string  name, bool* destination);
      bool get(const char*        name, bool* destination);

      bool get(const std::string& name, sint64* destination);
      bool get(const std::string  name, sint64* destination);
      bool get(const char*        name, sint64* destination);

      bool get(const std::string& name, double* destination);
      bool get(const std::string  name, double* destination);
      bool get(const char*        name, double* destination);

      bool get(const std::string& name, std::string& destination);
      bool get(const std::string  name, std::string& destination);
      bool get(const char*        name, std::string& destination);

      bool get(const std::string& name, Nversion& destination);
      bool get(const std::string  name, Nversion& destination);
      bool get(const char*        name, Nversion& destination);
      };
   }

extern config::Interface Config;

   // Helper function:
   //
   // uint32 workers;
   // en::setConfigValue<uint32>("en.parallel.workers", workers, defaultWorkersCount, MaxWorkers);
   // 
   template<typename T>
   void setConfigValue(const char* name, T& value, const T initialValue, const T maxValue)
   {
   value = initialValue;
   
   // Initial value can be overriden from config file
   sint64 tempValue = value;
   en::Config.get(name, &tempValue);
   value = static_cast<T>(tempValue);
   
   // Ensure set value doesn't exceed expected maximum
   if (value > maxValue)
      value = maxValue;
   }
}

#endif
