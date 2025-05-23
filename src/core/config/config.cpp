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

#include "assert.h"

#include "core/config/context.h" 
#include "core/storage.h" 
#include "core/utilities/parser.h"
#include "utilities/strings.h"
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

bool Context::create(int argc, const char **argv)
{
    using namespace en::storage;

#ifdef EN_PLATFORM_ANDROID
    enLog << "Starting module: Config.\n";
#endif

    // Finish default initialization
    if (!EN_CFG_CONFIGURATION_ENABLED)
    {
        return false;
    }

    // Overload Startup Settings from config file
    std::string filepath = EN_CFG_CONFIGURATION_PATH;
    std::string filename = EN_CFG_CONFIGURATION_FILE;

    // Open config file 
    File* file = Storage->open(filepath + filename);
    if (file)
    {
        // Config file should have max 1MB size
        if (file->size() > 1*MB)
        {
            delete file;
            return false;
        }

        // Allocate temporary buffer for file content
        uint32 size = static_cast<uint32>(file->size());
        uint8* buffer = new uint8[size];
        if (!buffer)
        {
            delete file;
            return false;
        }

        // Read file to buffer and close file
        if (!file->read(buffer))
        {
            delete file;
            return false;  
        } 
        delete file;
      
        // Create parser to quickly process text from file
        Parser text(buffer, size);

        // TODO: Parsing config file, extracting names, types and values of variables to map's.
    }

    // Terminal parameters override config file ones
    assert( argc >= 0 );
    for(uint32 i=1; i<static_cast<uint32>(argc); ++i)
    {
        // Parameters should be in format:
        //
        // Optional prefix   : -, --
        // Body              : alpha.string.dots.allowed
        // Optional separator: =
        // Optional value    : string, number
        //

        // Skip optional prefix      
        uint32 startKeyOffset = 0;
        while(argv[i][startKeyOffset] == '-')
        {
            startKeyOffset++;
        }

        // Determine key length (terminated by separator or 0)
        uint32 endKeyOffset = startKeyOffset;
        while(argv[i][endKeyOffset] != '=' &&
              argv[i][endKeyOffset] != 0)
        {
            endKeyOffset++;
        }
        endKeyOffset--;

        // Extract key name if there is one
        if (endKeyOffset >= startKeyOffset)
        {
            std::string keyName(&argv[i][startKeyOffset], endKeyOffset - startKeyOffset + 1);

            // Check if there is optional separator, and following value
            if (argv[i][endKeyOffset + 1] == '=' &&
                argv[i][endKeyOffset + 2] != 0)
            {
                uint32 startValueOffset = endKeyOffset + 2;
                uint32 endValueOffset   = startValueOffset; 
                while(argv[i][endValueOffset] != 0)
                {
                    endValueOffset++;
                }
                endValueOffset--;

                // Extract value string
                const char* valueStart = &argv[i][startValueOffset];
                uint32 valueLength = endValueOffset - startValueOffset + 1;
                std::string value(valueStart, valueLength);

                // Determine value type
                if (isFloat(valueStart, valueLength))
                {
                    doubles.insert( std::pair<std::string, double>(keyName, stringTo<double>(value)) );
                }
                else
                if (isInteger(valueStart, valueLength))
                {
                    ints.insert( std::pair<std::string, sint64>(keyName, stringTo<sint64>(value)) );
                }
                else
                {
                    strings.insert( std::pair<std::string, std::string>(keyName, value) );
                }
            }
            else
            {
                // This is single key without value
                keys.push_back(keyName);
            }
        }
    }

    return true;
}

void Context::destroy(void)
{
    keys.clear();
    bools.clear();
    ints.clear();
    doubles.clear();
    strings.clear();
    versions.clear();
}

bool Interface::get(const std::string& name)
{
    return get(name.c_str());
}
 
bool Interface::get(const std::string name)
{
    return get(name.c_str());
}

bool Interface::get(const char* name)
{
    // TODO: Optimize by replacing vector with other type of container for more optimal search
    for(uint32 i=0; i<ConfigContext.keys.size(); ++i)
    {
        if (ConfigContext.keys[i] == name)
        {
            return true;
        }
    }

    return false;
}

bool Interface::get(const std::string& name, bool* destination)
{
    return get(name.c_str(), destination);
}

bool Interface::get(const std::string name, bool* destination)
{
    return get(name.c_str(), destination);
}

bool Interface::get(const char* name, bool* destination)
{
    // WA: std::map::find() crashes when map is empty!
    if (ConfigContext.bools.empty())
    {
        return false;
    }

    std::map<std::string, bool>::iterator it = ConfigContext.bools.find(name);
    if (it == ConfigContext.bools.end())
    {
        return false;
    }

    *destination = it->second;
    return true;
}

bool Interface::get(const std::string& name, sint64* destination)
{
    return get(name.c_str(), destination);
}

bool Interface::get(const std::string name, sint64* destination)
{
    return get(name.c_str(), destination);
}

bool Interface::get(const char* name, sint64* destination)
{
    // WA: std::map::find() crashes when map is empty!
    if (ConfigContext.ints.empty())
    {
        return false;
    }

    std::map<std::string, sint64>::iterator it = ConfigContext.ints.find(name);
    if (it == ConfigContext.ints.end())
    {
        return false;
    }

    *destination = it->second;
    return true;
}

bool Interface::get(const std::string& name, double* destination)
{
    return get(name.c_str(), destination);
}
 
bool Interface::get(const std::string name, double* destination)
{
    return get(name.c_str(), destination);
}
    
bool Interface::get(const char* name, double* destination)
{
    // WA: std::map::find() crashes when map is empty!
    if (ConfigContext.doubles.empty())
    {
        return false;
    }

    std::map<std::string, double>::iterator it = ConfigContext.doubles.find(name);
    if (it == ConfigContext.doubles.end())
    {
        return false;
    }

    *destination = it->second;
    return true;
}

bool Interface::get(const std::string& name, std::string& destination)
{
    return get(name.c_str(), destination);
}

bool Interface::get(const std::string name, std::string& destination)
{
    return get(name.c_str(), destination);
}

bool Interface::get(const char* name, std::string& destination)
{
    // WA: std::map::find() crashes when map is empty!
    if (ConfigContext.strings.empty())
    {
        return false;
    }

    std::map<std::string, std::string>::iterator it = ConfigContext.strings.find(name);
    if (it == ConfigContext.strings.end())
    {
        return false;
    }

    destination = it->second;
    return true;
}

bool Interface::get(const std::string& name, Nversion& destination)
{
    // WA: std::map::find() crashes when map is empty!
    if (ConfigContext.versions.empty())
    {
        return false;
    }

    std::map<std::string, Nversion>::iterator it = ConfigContext.versions.find(name);
    if (it == ConfigContext.versions.end())
    {
        return false;
    }

    destination = it->second;
    return true;
}

bool Interface::get(const std::string name, Nversion& destination)
{
    // WA: std::map::find() crashes when map is empty!
    if (ConfigContext.versions.empty())
    {
        return false;
    }

    std::map<std::string, Nversion>::iterator it = ConfigContext.versions.find(name);
    if (it == ConfigContext.versions.end())
    {
        return false;
    }

    destination = it->second;
    return true;
}

bool Interface::get(const char* name, Nversion& destination)
{
    // WA: std::map::find() crashes when map is empty!
    if (ConfigContext.versions.empty())
    {
        return false;
    }

    std::map<std::string, Nversion>::iterator it = ConfigContext.versions.find(name);
    if (it == ConfigContext.versions.end())
    {
        return false;
    }

    destination = it->second;
    return true;
}

} // en::config

config::Context   ConfigContext;
config::Interface Config;

} // en
