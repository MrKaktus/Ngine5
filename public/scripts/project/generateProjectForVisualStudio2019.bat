REM This script should be run in ./project/ subdirectory
REM each time source files are added or removed.

REM Remove everything except of this script 
del vs2019 /s /q
rmdir vs2019 /s /q
mkdir vs2019
copy postBuild.bat vs2019\
cd vs2019

REM Generate Visual Studio project 
cmake -G "Visual Studio 16 2019" -A x64 ./../../

REM Patch Visual Studio project, by removing unwanted CMake stuff
del CMakeFiles /s /q
rmdir CMakeFiles /s /q
del cmake_install.cmake /q
del CMakeCache.txt /q
del ALL_BUILD.vcxproj /s /q
del ALL_BUILD.vcxproj.filters /s /q
del INSTALL.vcxproj /s /q
del INSTALL.vcxproj.filters /s /q

cd ..
powershell -executionpolicy bypass "& '.\patchProject.ps1' 'vs2019' "


