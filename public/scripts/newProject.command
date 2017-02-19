#!/bin/bash

function createMakefile
{
#
# CMake File
#
# $projectName - evaluated project name variable
# `$ - Windows escape characters
# \$ - macOS escape characters
#
cp ./../../Engine/public/scripts/project/CMakeLists.txt ./
sed -i 's/ENGINE_PROJECT_NAME/$projectName/g' CMakeLists.txt
}

function createGitIgnore
{
cp ./../../Engine/public/scripts/project/.gitignore ./
sed -i 's/ENGINE_PROJECT_NAME/$projectName/g' .gitignore
}

function createXcodeGenerator
{
cp ./../../../Engine/public/scripts/project/generateProjectForXcode.command ./
}

function createVS2015Generator
{
cp ./../../../Engine/public/scripts/project/generateProjectForVisualStudio2015.bat ./
cp ./../../../Engine/public/scripts/project/postBuild.bat ./
sed -i 's/ENGINE_PROJECT_NAME/$projectName/g' postBuild.bat
}

function createVS2017Generator
{
cp ./../../../Engine/public/scripts/project/generateProjectForVisualStudio2017.bat ./
cp ./../../../Engine/public/scripts/project/postBuild.bat ./
sed -i 's/ENGINE_PROJECT_NAME/$projectName/g' postBuild.bat
}

function createSourceFiles
{
cp ./../../../Engine/public/scripts/project/main.cpp ./
}

# Verify that project name was specified
if [ -z "$1" ]; then
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
    cd -
    exit
fi

# Determine project destination 
PROJECTS_PATH="./../../../../Projects/"
if [ $2 ]
then
PROJECTS_PATH=$2
# TODO: Calculate relative path from project custom path to the Engine directory and take it into notice when generating CMake and other scripts !!!
fi

# Create directory for future repository
if [ ! -d "$PROJECTS_PATH" ]; then
    mkdir $PROJECTS_PATH 
fi

cd $PROJECTS_PATH

# Create directory for new project
if [ -d "$1" ]; then
    echo "                                                "
    echo " Warning!                                       "
    echo " ========                                       "
    echo "                                                " 
    echo " Project directory already exists! Terminating. "
    echo "                                                "
    cd -
    exit
fi

# Start proper project setup
projectName=$1
mkdir $projectName
mkdir $projectName/bin
mkdir $projectName/build
mkdir $projectName/project
mkdir $projectName/resources
mkdir $projectName/resources/fonts
mkdir $projectName/resources/models
mkdir $projectName/resources/shaders
mkdir $projectName/resources/textures
mkdir $projectName/src

cd $projectName
createMakefile
createGitIgnore

cd project
createXcodeGenerator
createVS2015Generator
createVS2017Generator
cp ./../../../Engine/public/scripts/project/patchProject.ps1 ./
sed -i 's/ENGINE_PROJECT_NAME/$projectName/g' patchProject.ps1
cd ./../

cd src
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
