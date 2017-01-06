Param(
   [string]$projectName,
   [string]$projectsPath
)

$infoString=@"

 Windows - Creates new Project
 =============================

 This script will automatically generate directory structure for new
 project using Ngine. It will also create scripts that can be used to
 autogenerate IDE projects for different platforms (VisualStudio
 Solution on Windows, Xcode project on macOS, etc.^)

 Usage:
 ./newProject.cmd ProjectName
    - Will create new project directory called "ProjectName" in the
      directory "Projects" that is placed at the same level as Engine
      main directory.

 ./newProject.cmd ProjectName custom\path\
    - Will create new project directory called "ProjectName" in the
      directory pointed out by "custom\path\". Custom directory can
      be a relative path or global path.
"@

$warningString=@"

 Warning!
 ========

 Project directory already exists! Terminating.

"@

#
# CMake File
#
# $projectName - evaluated project name variable
# `$ - Windows escape characters
# \$ - macOS escape characters
#
$makefile=@"
cmake_minimum_required(VERSION 2.8)

# Automatic generator of IDE project for macOS
# TODO: Add iOS, tvOS targets.
#       Add Visual Studio project generation.

# In Visual Studio its Solution name
PROJECT($projectName)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_SUPPRESS_REGENERATION true)           # We don't want ZERO_CHECK target to be generated

# MacOS
if (CMAKE_SYSTEM_NAME STREQUAL Darwin)

   FILE(GLOB_RECURSE `$SourceFiles
      "./src/*.cpp"
      "./src/*.mm")

   add_executable($projectName `${SourceFiles})

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
   message("FBX Debug  : `${LIBRARY_FBX_DEBUG}") 
   message("FBX Release: `${LIBRARY_FBX_RELEASE}") 
   message("Zlib       : `${ZLIB_LIBRARIES}")
   message("OpenAL     : `${OPENAL_LIBRARY}")
   message("Metal      : `${FRAMEWORK_METAL}")
   message("Cocoa      : `${FRAMEWORK_COCOA}")
   message("QuartzCore : `${FRAMEWORK_QUARTZ_CORE}")

   target_link_libraries($projectName Ngine 
                         debug `${LIBRARY_FBX_DEBUG} 
                         optimized `${LIBRARY_FBX_RELEASE} 
                         `${ZLIB_LIBRARIES} 
                         `${OPENAL_LIBRARY} 
                         `${FRAMEWORK_METAL} 
                         `${FRAMEWORK_COCOA}
                         `${FRAMEWORK_QUARTZ_CORE})

   set_target_properties($projectName PROPERTIES
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

   FILE(GLOB_RECURSE SourceFiles
      "./src/*.c"
      "./src/*.cpp"
      "./src/*.h"
      "./src/*.hpp"
      "./src/*.inl")

   # Visual Studio: projectName.vcxproj > projectName.exe
   add_executable($projectName `${SourceFiles})

   # Visual Studio: Properties->C/C++->General->Additional Include Directories
   # TODO: This should be shared path with MacOS
   include_directories(../../Engine/public/include/)




   SET(CMAKE_FIND_LIBRARY_PREFIXES "")
   SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")

   find_library(LIBRARY_ENGINE_X64_DEBUG
                NAMES Ngine5dbg
                PATHS "../../Engine/bin/win64/Debug/")
   if (NOT LIBRARY_ENGINE_X64_DEBUG)
      message(FATAL_ERROR "Could not find Engine x64 Debug library")
   endif()

   find_library(LIBRARY_ENGINE_X64_RELEASE
                NAMES Ngine5
                PATHS "../../Engine/bin/win64/Release/")
   if (NOT LIBRARY_ENGINE_X64_RELEASE)
      message(FATAL_ERROR "Could not find Engine x64 Release library")
   endif()

   # Direct Input 8 library should be resolved at build time in Visual Studio
   # add_library(dinput8 STATIC IMPORTED)

   link_directories(`${LINK_DIRECTORIES} \`$\(WindowsSDK_LibraryPath_x64\))

   # Visual Studio: Properties->Linker->Input->Additional Dependencies
   #
   target_link_libraries($projectName 
                         dinput8
                         debug `${LIBRARY_ENGINE_X64_DEBUG} 
                         optimized `${LIBRARY_ENGINE_X64_RELEASE})


   #  General:
   #  - Output Directory       : `$(SolutionDir)\..\..\bin\`$(Platform)\`$(Configuration)\
   #  - Intermediate Directory : `$(SolutionDir)\..\..\build\`$(Platform)\`$(Configuration)\
   #  - Build Log File         : .\..\..\bin\`$(Platform)\`$(Configuration)\`$(MSBuildProjectName).log
   #  Debugging:
   #  - Working Dirctory       : `$(ProjectDir)..\..\

   # TODO ABOVE !!!

   # Need to run INSTALL.vcproj to execute below:
   install(TARGETS $projectName RUNTIME DESTINATION `${PROJECT_BINARY_DIR}/bin)

   # RuntimeLibrary
   # 0 (MultiThreaded) == /MT
   # 1 (MultiThreadedDebug) == /MTd
   # 2 (MultiThreadedDLL) == /MD
   # 3 (MultiThreadedDebugDLL) == /MDd
   #
   set(CMAKE_CXX_FLAGS_DEBUG   "`${CMAKE_CXX_FLAGS_DEBUG}   /MDd")
   set(CMAKE_CXX_FLAGS_RELEASE "`${CMAKE_CXX_FLAGS_RELEASE} /MD")

   # Configuration Properties:
   #   C/C++:
   #     General:
   #     - Debug Information Format       : Program Database (/Zi)
   #
   set(CMAKE_CXX_FLAGS_DEBUG   "`${CMAKE_CXX_FLAGS_DEBUG}   /Zi")

   # Configuration Properties:
   #   C/C++:
   #     Optimization:
   #     - Optimization                   : Full Optimization (/Ox)
   #
   set(CMAKE_CXX_FLAGS_RELEASE "`${CMAKE_CXX_FLAGS_RELEASE} /Ox")

endif()
"@

$generateProjectForXcode=@"
#!/bin/bash

# This script should be run in ./project/ subdirectory
# each time source files are added or removed.

# Remove everything except of this script 
rm -rfv macOS
mkdir macOS
cd macOS

# Generate Xcode project 
cmake -G Xcode ./../../
"@

$generateProjectForVisualStudio2015=@"
REM This script should be run in ./project/ subdirectory
REM each time source files are added or removed.

REM Remove everything except of this script 
del vs2015 /s /q
rmdir vs2015 /s /q
mkdir vs2015
cd vs2015

REM Generate Visual Studio project 
cmake -G "Visual Studio 14 2015 Win64" ./../../

REM Clear CMake trash
REM del CMakeFiles /s /q
REM rmdir CMakeFiles /s /q
REM del cmake_install.cmake /q
REM del CMakeCache.txt /q
"@




$generateProjectForVisualStudio2017=@"
REM This script should be run in ./project/ subdirectory
REM each time source files are added or removed.

REM Remove everything except of this script 
del ".\vs2017" /Q /S
mkdir vs2017
cd vs2017

REM Generate Visual Studio project 
cmake -G "Visual Studio 15 2017 Win64" ./../../
"@

$mainSource=@"
#include "Ngine.h"

int main(int argc, const char* argv[])
{
   // You're ready to go!

   return 0;
}
"@

$gitIgnoreBasic=@"
bin/
build/
project/vs2015/$projectName.v14.suo
project/vs2015/$projectName.sdf
project/vs2015/$projectName.VC.db
project/vs2015/$projectName.VC.opendb
project/vs2015/$projectName.opensdf
project/vs2015/$projectName.vcxproj.filters
project/vs2017/$projectName.v15.suo
project/vs2017/$projectName.sdf
project/vs2017/$projectName.VC.db
project/vs2017/$projectName.VC.opendb
project/vs2017/$projectName.opensdf
project/vs2017/$projectName.vcxproj.filters
._*

# Because you should always regenerate project files, those one are discarded as well
project/vs2015/$projectName.sln
project/vs2015/$projectName.vcxproj
project/vs2015/$projectName.vcxproj.user
project/vs2017/$projectName.sln
project/vs2017/$projectName.vcxproj
project/vs2017/$projectName.vcxproj.user
project/macOS/
"@

function createMakefile
{
$makefile | Set-Content 'CMakeLists.txt'
# $text | Add-Content 'CMakeLists.txt'
}

function createGitIgnore
{
$gitIgnoreBasic | Set-Content '.gitignore'
}

function createXcodeGenerator
{
$generateProjectForXcode | Set-Content 'generateProjectForXcode.command'
}

function createVS2015Generator
{
$generateProjectForVisualStudio2015 | Set-Content 'generateProjectForVisualStudio2015.bat'
}

function createVS2017Generator
{
$generateProjectForVisualStudio2017 | Set-Content 'generateProjectForVisualStudio2017.bat'
}

function createSourceFiles
{
$mainSource | Set-Content 'main.cpp'
}

Push-Location

# Verify that project name was specified
If (!$projectName)
{
   Write-Host "$infoString"
   Pop-Location
   exit
}

# Determine project destination 
If (!$projectsPath)
{
   $projectsPath=".\..\..\..\..\Projects\"
}
else
{
   # TODO: Calculate relative path from project custom path to the Engine directory and take it into notice when generating CMake and other scripts !!!
}

# Create directory for future repository
If (!(Test-Path -Path $projectsPath))
{
	New-Item -ItemType directory -Path $projectsPath | Out-Null
}
   
Set-Location -Path $projectsPath

# Create directory for new project
If (Test-Path -Path $projectName)
{
   Write-Host "$warningString"
   Pop-Location
   exit
}

# Start proper project setup
New-Item -ItemType directory -Path $projectName | Out-Null
New-Item -ItemType directory -Path $projectName\bin | Out-Null
New-Item -ItemType directory -Path $projectName\build | Out-Null
New-Item -ItemType directory -Path $projectName\project | Out-Null
New-Item -ItemType directory -Path $projectName\resources | Out-Null
New-Item -ItemType directory -Path $projectName\src | Out-Null

Set-Location -Path $projectName
createMakefile
createGitIgnore

Set-Location -Path project
createXcodeGenerator
createVS2015Generator
createVS2017Generator
Set-Location -Path ..

Set-Location -Path src
createSourceFiles

Pop-Location

# Print out success summary
Write-Host " "
Write-Host " $projectName"
Write-Host -NoNewline " "
for($i=1; $i -le $projectName.length; $i++) { Write-Host -NoNewline "=" }
Write-Host " "
Write-Host " Project created successfully."
Write-Host " "
