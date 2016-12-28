#!/bin/bash

# Generate Cmake file that will be used to build project
function createMakefile
{
touch CMakeLists.txt
cat <<EOF > CMakeLists.txt
cmake_minimum_required(VERSION 2.8)

# Automatic generator of IDE project for macOS
# TODO: Add iOS, tvOS targets.
#       Add Visual Studio project generation.

PROJECT($1)

FILE(GLOB_RECURSE $1_Source
     "./src/*.cpp"
     "./src/*.mm")

add_executable($1 \${$1_Source})

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_SUPPRESS_REGENERATION true)           # We don't want ZERO_CHECK target to be generated

# MacOS
if (CMAKE_SYSTEM_NAME STREQUAL Darwin)

   include_directories(../../Engine/Ngine5/bin/macOS/$<CONFIG>/usr/local/include/Ngine/)

   SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
   SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a" ".dylib" ".framework")

   # https://cmake.org/Wiki/CMake/Tutorials/Exporting_and_Importing_Targets
   #
   add_library(Ngine STATIC IMPORTED)
   set_property(TARGET Ngine PROPERTY IMPORTED_LOCATION_RELEASE /Volumes/FAT32_SHARE/Engine/Ngine5/bin/macOS/Release/libNgine.a)
   set_property(TARGET Ngine PROPERTY IMPORTED_LOCATION_DEBUG   /Volumes/FAT32_SHARE/Engine/Ngine5/bin/macOS/Debug/libNgine.a)

   find_package(ZLIB REQUIRED)
   if (NOT ZLIB_FOUND)
      message(FATAL_ERROR "Could not find Zlib")
   endif()

   find_package(OpenAL REQUIRED)
   if (NOT OPENAL_FOUND)
      message(FATAL_ERROR "Could not find OpenAL")
   endif()

   find_library(LIBRARY_FBX_DEBUG
                NAMES fbxsdk
                PATHS "/Applications/Autodesk/FBX\ SDK/2015.1/lib/clang/debug/")
   if (NOT LIBRARY_FBX_DEBUG)
      message(FATAL_ERROR "Could not find Debug FBX SDK library")
   endif()

   find_library(LIBRARY_FBX_RELEASE
                NAMES fbxsdk
                PATHS "/Applications/Autodesk/FBX\ SDK/2015.1/lib/clang/release/")
   if (NOT LIBRARY_FBX_RELEASE)
      message(FATAL_ERROR "Could not find Release FBX SDK library")
   endif()

   # Alternative is to add them directly:
   # add_library(Fbx STATIC IMPORTED)
   # set_property(TARGET Fbx PROPERTY IMPORTED_LOCATION_DEBUG   /Applications/Autodesk/FBX\ SDK/2015.1/lib/clang/debug/libfbxsdk.a)
   # set_property(TARGET Fbx PROPERTY IMPORTED_LOCATION_RELEASE /Applications/Autodesk/FBX\ SDK/2015.1/lib/clang/release/libfbxsdk.a)

   find_library(FRAMEWORK_COCOA Cocoa)
   if (NOT FRAMEWORK_COCOA)
      message(FATAL_ERROR "Could not find Cocoa.Framework")
   endif()

   find_library(FRAMEWORK_METAL Metal)
   if (NOT FRAMEWORK_METAL)
      message(FATAL_ERROR "Could not find Metal.Framework")
   endif()

   find_library(FRAMEWORK_QUARTZ_CORE QuartzCore)
   if (NOT FRAMEWORK_QUARTZ_CORE)
      message(FATAL_ERROR "Could not find QuartzCore.Framework")
   endif()

   # OPENAL_INCLUDE_DIR


   message("Linking:")
   message("FBX Debug  : \${LIBRARY_FBX_DEBUG}") 
   message("FBX Release: \${LIBRARY_FBX_RELEASE}") 
   message("Zlib       : \${ZLIB_LIBRARIES}")
   message("OpenAL     : \${OPENAL_LIBRARY}")
   message("Metal      : \${FRAMEWORK_METAL}")
   message("Cocoa      : \${FRAMEWORK_COCOA}")
   message("QuartzCore : \${FRAMEWORK_QUARTZ_CORE}")

   target_link_libraries($1 Ngine 
                         debug \${LIBRARY_FBX_DEBUG} 
                         optimized \${LIBRARY_FBX_RELEASE} 
                         \${ZLIB_LIBRARIES} 
                         \${OPENAL_LIBRARY} 
                         \${FRAMEWORK_METAL} 
                         \${FRAMEWORK_COCOA}
                         \${FRAMEWORK_QUARTZ_CORE})

   set_target_properties($1 PROPERTIES
      COMPILE_FLAGS                    "-fno-objc-arc"
      RUNTIME_OUTPUT_DIRECTORY_DEBUG   ./../../bin/macOS/Debug 
      RUNTIME_OUTPUT_DIRECTORY_RELEASE ./../../bin/macOS/Release)

   # To Directly specify Xcode attributes use:
   # XCODE_ATTRIBUTE_<xcode attribute>
   # http://stackoverflow.com/questions/6910901/how-do-i-print-a-list-of-build-settings-in-xcode-project

   set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC       "NO")
   set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
   set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY           "libc++")
endif()


# Windows
if (CMAKE_SYSTEM_NAME STREQUAL Windows)

   SET(CMAKE_FIND_LIBRARY_PREFIXES "")
   SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")

endif()
EOF
}

# Create script that autogenerates Xcode project
function createXcodeGenerator
{
touch generateProject.command
chmod +X generateProject.command
cat <<EOF > generateProject.command
#!/bin/bash

# This script should be run in ./project/macOS/ subdirectory
# each time source files are added or removed.

# Remove everything except of this script 
rm -rfv macOS
mkdir macOS
cd macOS

# Generate Xcode project 
cmake -G Xcode ./../../
EOF
}

# Create initial main.cpp with main entrypoint
function createSourceFiles
{
touch main.cpp
cat <<EOF > main.cpp
#include "Ngine.h"

int main(int argc, const char* argv[])
{
   // You're ready to go!

   return 0;
}
EOF
}

# Determine project destination 
PROJECTS_PATH="./../../../../Projects/"
if [ $2 ]
then
PROJECTS_PATH=$2
# TODO: Calculate relative path from project custom path to the Engine directory and take it into notice when generating CMake and other scripts !!!
fi

if [ $1 ]
then
    # Create directory for future repository
    if [ ! -d "$PROJECTS_PATH" ]; then
        mkdir $PROJECTS_PATH 
    fi

    cd $PROJECTS_PATH

    # Create directory for new project
    if [ ! -d "$1" ]; then
        mkdir $1/
        mkdir $1/bin/
        mkdir $1/build/
        mkdir $1/project/
        mkdir $1/resources/
        mkdir $1/src/

        cd $1
        createMakefile $1

        cd project/
        createXcodeGenerator
        cd ./../

        cd src/
        createSourceFiles

        cd -

        # Print out success summary
        NAME_LENGTH=${#1} 
        echo "                                                "
        echo " \"$1\" "
        echo -n " ="
        start=1
        end=$NAME_LENGTH+1
        for ((i=$start; i<=$end; i++)); do echo -n =; done
        echo " "
        echo "                                                " 
        echo " Project created successfully.                  "
        echo "                                                "
    else
    echo "                                                "
    echo " Warning!                                       "
    echo " ========                                       "
    echo "                                                " 
    echo " Project directory already exists! Terminating. "
    echo "                                                "
    fi
else
echo "                                                                      "
echo " macOS - Creates new Project                                          "
echo " ===========================                                          "
echo "                                                                      "
echo " This script will automatically generate directory structure for new  "
echo " project using Ngine. It will also create scripts that can be used to "
echo " autogenerate IDE projects for different platforms (VisualStudio      "
echo " Solution on Windows, Xcode project on macOS, etc.)                   "
echo "                                                                      "
echo " Usage:                                                               "
echo " ./newProject.command ProjectName                                     "
echo "    - Will create new project directory called \"ProjectName\" in the "
echo "      directory \"Projects\" that is placed at the same level as Engine"
echo "      main directory.                                                 "
echo "                                                                      "
echo " ./newProject.command ProjectName custom/path/                        "
echo "    - Will create new project directory called \"ProjectName\" in the "
echo "      directory pointed out by \"custom/path/\". Custom directory can "
echo "      be a relative path or global path.                              "
echo "                                                                      "
fi