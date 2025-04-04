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
#include <vector>

namespace en
{
namespace config
{

struct Context
{
    // Parsed variables of different types
    std::vector<std::string>           keys;
    std::map<std::string, bool>        bools;
    std::map<std::string, sint64>      ints;
    std::map<std::string, double>      doubles;
    std::map<std::string, std::string> strings;
    std::map<std::string, Nversion>    versions;
    
    Context();
   ~Context();

    bool create(int argc, const char **argv);
    void destroy(void);
};

} // en::config

extern config::Context ConfigContext;

} // en

#endif