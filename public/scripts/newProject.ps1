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

function createMakefile
{
#
# CMake File
#
# $projectName - evaluated project name variable
# `$ - Windows escape characters
# \$ - macOS escape characters
#
(Get-Content '.\..\..\Engine\public\scripts\project\CMakeLists.txt') -replace 'ENGINE_PROJECT_NAME', "$projectName" | Set-Content 'CMakeLists.txt'
}

function createGitIgnore
{
(Get-Content '.\..\..\Engine\public\scripts\project\.gitignore') -replace 'ENGINE_PROJECT_NAME', "$projectName" | Set-Content '.gitignore'
}

function createXcodeGenerator
{
Copy-Item '.\..\..\..\Engine\public\scripts\project\generateProjectForXcode.command' -destination .
}

function createVS2015Generator
{
Copy-Item '.\..\..\..\Engine\public\scripts\project\generateProjectForVisualStudio2015.bat' -destination .
}

function createVS2017Generator
{
Copy-Item '.\..\..\..\Engine\public\scripts\project\generateProjectForVisualStudio2017.bat' -destination .
}

function createSharedScripts
{
(Get-Content '.\..\..\..\Engine\public\scripts\project\postBuild.bat') -replace 'ENGINE_PROJECT_NAME', "$projectName" | Set-Content 'postBuild.bat'
(Get-Content '.\..\..\..\Engine\public\scripts\project\patchProject.ps1') -replace 'ENGINE_PROJECT_NAME', "$projectName" | Set-Content 'patchProject.ps1'
}

function createSourceFiles
{
Copy-Item '.\..\..\..\Engine\public\scripts\project\main.cpp' -destination .
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
New-Item -ItemType directory -Path $projectName\resources\fonts | Out-Null
New-Item -ItemType directory -Path $projectName\resources\models | Out-Null
New-Item -ItemType directory -Path $projectName\resources\shaders | Out-Null
New-Item -ItemType directory -Path $projectName\resources\textures | Out-Null
New-Item -ItemType directory -Path $projectName\src | Out-Null

Set-Location -Path $projectName
createMakefile
createGitIgnore

Set-Location -Path project
createXcodeGenerator
createVS2015Generator
createVS2017Generator
createSharedScripts
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
