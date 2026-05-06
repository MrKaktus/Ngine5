REM This script should be run in ./project/ subdirectory
REM each time source files are added or removed.

REM Remove everything except of this script 
del vs2026 /s /q
rmdir vs2026 /s /q
mkdir vs2026
copy postBuild.bat vs2026\
cd vs2026

REM Generate Visual Studio project 
cmake -G "Visual Studio 18 2026" ./../../

REM Patch Visual Studio project, by removing unwanted CMake stuff
del CMakeFiles /s /q
rmdir CMakeFiles /s /q
del cmake_install.cmake /q
del CMakeCache.txt /q
del ALL_BUILD.dir /s /q
del ALL_BUILD.vcxproj /s /q
del ALL_BUILD.vcxproj.filters /s /q
del INSTALL.dir /s /q
del INSTALL.vcxproj /s /q
del INSTALL.vcxproj.filters /s /q

cd ..
powershell -executionpolicy bypass "& '.\patchProject.ps1' 'vs2026' "



