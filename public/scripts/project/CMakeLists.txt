cmake_minimum_required(VERSION 2.8)

# Automatic generator of IDE projects for macOS and Windows
# TODO: Add iOS, tvOS targets.

# Visual Studio: Solution
PROJECT(ENGINE_PROJECT_NAME)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# We don't want ZERO_CHECK target to be generated
set(CMAKE_SUPPRESS_REGENERATION true)           

# MacOS
if (CMAKE_SYSTEM_NAME STREQUAL Darwin)

   FILE(GLOB_RECURSE $SourceFiles
      "./src/*.cpp"
      "./src/*.mm")

   add_executable(ENGINE_PROJECT_NAME ${SourceFiles})

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
   message("FBX Debug  : ${LIBRARY_FBX_DEBUG}") 
   message("FBX Release: ${LIBRARY_FBX_RELEASE}") 
   message("Zlib       : ${ZLIB_LIBRARIES}")
   message("OpenAL     : ${OPENAL_LIBRARY}")
   message("Metal      : ${FRAMEWORK_METAL}")
   message("Cocoa      : ${FRAMEWORK_COCOA}")
   message("QuartzCore : ${FRAMEWORK_QUARTZ_CORE}")

   target_link_libraries(ENGINE_PROJECT_NAME Ngine 
                         debug ${LIBRARY_FBX_DEBUG} 
                         optimized ${LIBRARY_FBX_RELEASE} 
                         ${ZLIB_LIBRARIES} 
                         ${OPENAL_LIBRARY} 
                         ${FRAMEWORK_METAL} 
                         ${FRAMEWORK_COCOA}
                         ${FRAMEWORK_QUARTZ_CORE})

   set_target_properties(ENGINE_PROJECT_NAME PROPERTIES
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
   add_executable(ENGINE_PROJECT_NAME EXCLUDE_FROM_ALL ${SourceFiles})

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

   link_directories(${LINK_DIRECTORIES} \$\(WindowsSDK_LibraryPath_x64\))

   # Visual Studio: Properties->Linker->Input->Additional Dependencies
   #
   target_link_libraries(ENGINE_PROJECT_NAME 
                         dinput8
                         debug ${LIBRARY_ENGINE_X64_DEBUG} 
                         optimized ${LIBRARY_ENGINE_X64_RELEASE})


   #  General:
   #  - Output Directory       : $(SolutionDir)\..\..\bin\$(Platform)\$(Configuration)\
   #  - Intermediate Directory : $(SolutionDir)\..\..\build\$(Platform)\$(Configuration)\
   #  - Build Log File         : .\..\..\bin\$(Platform)\$(Configuration)\$(MSBuildProjectName).log
   #  Debugging:
   #  - Working Dirctory       : $(ProjectDir)..\..\

   # TODO ABOVE !!!

   # Need to run INSTALL.vcproj to execute below:
   install(TARGETS ENGINE_PROJECT_NAME RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)

   # RuntimeLibrary
   # 0 (MultiThreaded) == /MT
   # 1 (MultiThreadedDebug) == /MTd
   # 2 (MultiThreadedDLL) == /MD
   # 3 (MultiThreadedDebugDLL) == /MDd
   #
   set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   /MDd")
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")

   # Configuration Properties:
   #   C/C++:
   #     General:
   #     - Debug Information Format       : Program Database (/Zi)
   #
   set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   /Zi")

   # Configuration Properties:
   #   C/C++:
   #     Optimization:
   #     - Optimization                   : Full Optimization (/Ox)
   #
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")

   add_custom_command(TARGET ENGINE_PROJECT_NAME
                      POST_BUILD
                      COMMAND postBuild.bat)
endif()