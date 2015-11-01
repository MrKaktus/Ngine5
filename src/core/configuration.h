/*

 Ngine v5.0
 
 Module      : Compile-Time configuration.
 Requirements: none
 Description : Holds declarations of Compile-Time
               settings that need to be known during 
               code compilation. 

*/

#ifndef ENG_CORE_CONFIGURATION
#define ENG_CORE_CONFIGURATION

#include "core/defines.h"

// Engine always tries to load startup settings from file.
#define EN_CFG_CONFIGURATION_ENABLED true

// Defines source path and filename of engine config file.
#ifdef EN_DEBUG
 #define EN_CFG_CONFIGURATION_PATH "resources/"
 #define EN_CFG_CONFIGURATION_FILE "engine.config"
#else
 #define EN_CFG_CONFIGURATION_PATH ""
 #define EN_CFG_CONFIGURATION_FILE "engine.config"
#endif

// Define profiler and debuger levels
#ifdef EN_DEBUG
   #define EN_PROFILER_VALIDATE_GRAPHICS_API 
 //#define EN_PROFILER_TRACE_GRAPHICS_API         // Comment out to turn on this feature
   #define EN_PROFILE
   #define EN_VALIDATE_GRAPHIC_CAPS_AT_RUNTIME
#endif

// Engine compile time settings
#define EN_MAX_ARRAY_SIZE    65536

#endif