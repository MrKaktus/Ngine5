/*

 Ngine v5.0
 
 Module      : Engine configuration context.
 Requirements: none
 Description : Holds declarations of Startup settings 
               that need to be known during engine 
               startup for setting up all subsystems. 
               It also maintains Run-Time settings that 
               need to be known during application execution.

*/

#include "core/config/context.h" 
#include "core/storage.h" 
#include "core/utilities/parser.h"

#include "core/log/log.h"

namespace en
{
   namespace config
   {

   Context::Context()
   {
   }

   Context::~Context()
   {
   }

   bool Context::create(void)
   {
   using namespace en::storage;

#ifdef EN_PLATFORM_ANDROID
   Log << "Starting module: Config.\n";
#endif

   // Finish default initialization
   if (!EN_CFG_CONFIGURATION_ENABLED)
      return false;

   // Overload Startup Settings from config file
   string filepath = EN_CFG_CONFIGURATION_PATH;
   string filename = EN_CFG_CONFIGURATION_FILE;

   // Open config file 
   Ptr<File> file = Storage->open(filepath + filename);
   if (!file)
      return false;

   // Config file should have max 1MB size
   if (file->size() > 1024*1024)
      return false;

   // Allocate temporary buffer for file content
   uint32 size = static_cast<uint32>(file->size());
   uint8* buffer = new uint8[size];
   if (!buffer)
      return false;
   
   // Read file to buffer and close file
   if (!file->read(buffer))
      return false;   
   file = nullptr;
   
   // Create parser to quickly process text from file
   Nparser text(buffer, size);


   // TODO: Parsing config file, extracting names, types and values of variables to map's.


   return true;
   }

   void Context::destroy(void)
   {
   bools.clear();
   uint8s.clear();
   uint16s.clear();
   sint16s.clear();
   uint32s.clear();
   strings.clear();
   versions.clear();
   }

   bool Interface::get(const string& name, bool* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.bools.empty())
      return false;

   map<string, bool>::iterator it = ConfigContext.bools.find(name);
   if (it == ConfigContext.bools.end())
      return false;

   *destination = it->second;
   return true;
   }

   bool Interface::get(const string name, bool* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.bools.empty())
      return false;

   map<string, bool>::iterator it = ConfigContext.bools.find(name);
   if (it == ConfigContext.bools.end())
      return false;

   *destination = it->second;
   return true;
   }

   bool Interface::get(const char* name, bool* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.bools.empty())
      return false;

   map<string, bool>::iterator it = ConfigContext.bools.find(name);
   if (it == ConfigContext.bools.end())
      return false;

   *destination = it->second;
   return true;
   }

   bool Interface::get(const string& name, uint8* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.uint8s.empty())
      return false;

   map<string, uint8>::iterator it = ConfigContext.uint8s.find(name);
   if (it == ConfigContext.uint8s.end())
      return false;

   *destination = it->second;
   return true;
   }

   bool Interface::get(const string name, uint8* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.uint8s.empty())
      return false;

   map<string, uint8>::iterator it = ConfigContext.uint8s.find(name);
   if (it == ConfigContext.uint8s.end())
      return false;

   *destination = it->second;
   return true;
   }

   bool Interface::get(const char* name, uint8* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.uint8s.empty())
      return false;

   map<string, uint8>::iterator it = ConfigContext.uint8s.find(name);
   if (it == ConfigContext.uint8s.end())
      return false;

   *destination = it->second;
   return true;
   }

   bool Interface::get(const string& name, uint16* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.uint16s.empty())
      return false;

   map<string, uint16>::iterator it = ConfigContext.uint16s.find(name);
   if (it == ConfigContext.uint16s.end())
      return false;

   *destination = it->second;
   return true;
   }

   bool Interface::get(const string name, uint16* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.uint16s.empty())
      return false;

   map<string, uint16>::iterator it = ConfigContext.uint16s.find(name);
   if (it == ConfigContext.uint16s.end())
      return false;

   *destination = it->second;
   return true;
   }

   bool Interface::get(const char* name, uint16* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.uint16s.empty())
      return false;

   map<string, uint16>::iterator it = ConfigContext.uint16s.find(name);
   if (it == ConfigContext.uint16s.end())
      return false;

   *destination = it->second;
   return true;
   }

   bool Interface::get(const string& name, sint16* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.sint16s.empty())
      return false;

   map<string, sint16>::iterator it = ConfigContext.sint16s.find(name);
   if (it == ConfigContext.sint16s.end())
      return false;

   *destination = it->second;
   return true;
   }

   bool Interface::get(const string name, sint16* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.sint16s.empty())
      return false;

   map<string, sint16>::iterator it = ConfigContext.sint16s.find(name);
   if (it == ConfigContext.sint16s.end())
      return false;

   *destination = it->second;
   return true;
   }

   bool Interface::get(const char* name, sint16* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.sint16s.empty())
      return false;

   map<string, sint16>::iterator it = ConfigContext.sint16s.find(name);
   if (it == ConfigContext.sint16s.end())
      return false;

   *destination = it->second;
   return true;
   }
   
   bool Interface::get(const string& name, uint32* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.uint32s.empty())
      return false;

   map<string, uint32>::iterator it = ConfigContext.uint32s.find(name);
   if (it == ConfigContext.uint32s.end())
      return false;

   *destination = it->second;
   return true;
   }
 
   bool Interface::get(const string name, uint32* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.uint32s.empty())
      return false;

   map<string, uint32>::iterator it = ConfigContext.uint32s.find(name);
   if (it == ConfigContext.uint32s.end())
      return false;

   *destination = it->second;
   return true;
   }
    
   bool Interface::get(const char* name, uint32* destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.uint32s.empty())
      return false;

   map<string, uint32>::iterator it = ConfigContext.uint32s.find(name);
   if (it == ConfigContext.uint32s.end())
      return false;

   *destination = it->second;
   return true;
   }
     
   bool Interface::get(const string& name, string& destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.strings.empty())
      return false;

   map<string, string>::iterator it = ConfigContext.strings.find(name);
   if (it == ConfigContext.strings.end())
      return false;

   destination = it->second;
   return true;
   }

   bool Interface::get(const string name, string& destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.strings.empty())
      return false;

   map<string, string>::iterator it = ConfigContext.strings.find(name);
   if (it == ConfigContext.strings.end())
      return false;

   destination = it->second;
   return true;
   }

   bool Interface::get(const char* name, string& destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.strings.empty())
      return false;

   map<string, string>::iterator it = ConfigContext.strings.find(name);
   if (it == ConfigContext.strings.end())
      return false;

   destination = it->second;
   return true;
   }

   bool Interface::get(const string& name, Nversion& destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.versions.empty())
      return false;

   map<string, Nversion>::iterator it = ConfigContext.versions.find(name);
   if (it == ConfigContext.versions.end())
      return false;

   destination = it->second;
   return true;
   }

   bool Interface::get(const string name, Nversion& destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.versions.empty())
      return false;

   map<string, Nversion>::iterator it = ConfigContext.versions.find(name);
   if (it == ConfigContext.versions.end())
      return false;

   destination = it->second;
   return true;
   }

   bool Interface::get(const char* name, Nversion& destination)
   {
   // WA: std::map::find() crashes when map is empty!
   if (ConfigContext.versions.empty())
      return false;

   map<string, Nversion>::iterator it = ConfigContext.versions.find(name);
   if (it == ConfigContext.versions.end())
      return false;

   destination = it->second;
   return true;
   }

   }

config::Context   ConfigContext;
config::Interface Config;
}
