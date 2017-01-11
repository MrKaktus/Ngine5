REM This script should be run in ./project/ subdirectory
REM each time source files are added or removed.

REM Remove everything except of this script 
del ".\vs2017" /Q /S
mkdir vs2017
copy postBuild.bat vs2017\
cd vs2017

REM Generate Visual Studio project 
cmake -G "Visual Studio 15 2017 Win64" ./../../
