Param(
   [string]$projectDirectory
)

$projectName="ENGINE_PROJECT_NAME"

# Should be 'vs2015' etc.
#
Set-Location -Path $projectDirectory


# Patching: ENGINE_PROJECT_NAME.sln
########################################################


# Find ALL_BUILD and INSTALL projects and remove them from solution.
#
# Algorithm steps:
# - find line with "ALL_BUILD"
# - extract last GUID from this line
# - remove this line and next three
# - remove all lines with this GUID
#
# - find line with "INSTALL"
# - extract last GUID from this line
# - remove this line and next four
# - remove all lines with this GUID

# REMOVE PROJECT ALL_BUILD
#
$projectLineNumber = ( Get-Content '.\ENGINE_PROJECT_NAME.sln' | Select-String -pattern "ALL_BUILD" -simplematch | Select-Object -ExpandProperty LineNumber )
$fileLines = (Get-Content '.\ENGINE_PROJECT_NAME.sln' | Measure-Object | Select-Object -ExpandProperty Count ) 
$lastLineNumberBefore = $projectLineNumber-2
$firstLineNumberAfter = $projectLineNumber+3

$guids = @()
$lineNumberGUID = $projectLineNumber-1
$guids += ( (Get-Content '.\ENGINE_PROJECT_NAME.sln')[$lineNumberGUID..$lineNumberGUID] | Select-String -pattern '{[-0-9A-F]+?}' -AllMatches | Select-Object -ExpandProperty Matches | Select-Object -ExpandProperty Value )
$guid_all_build = $guids[1]

$array = @()
$array += (Get-Content '.\ENGINE_PROJECT_NAME.sln')[0..$lastLineNumberBefore] 
$array += (Get-Content '.\ENGINE_PROJECT_NAME.sln')[$firstLineNumberAfter..$fileLines]
$array | Set-Content '.\ENGINE_PROJECT_NAME.sln'

# REMOVE PROJECT INSTALL
#
$projectLineNumber = ( Get-Content '.\ENGINE_PROJECT_NAME.sln' | Select-String -pattern "INSTALL" -simplematch | Select-Object -ExpandProperty LineNumber )
$fileLines = (Get-Content '.\ENGINE_PROJECT_NAME.sln' | Measure-Object | Select-Object -ExpandProperty Count ) 
$lastLineNumberBefore = $projectLineNumber-2
$firstLineNumberAfter = $projectLineNumber+4

$guids_install = @()
$lineNumberGUID = $projectLineNumber-1
$guids_install += ( (Get-Content '.\ENGINE_PROJECT_NAME.sln')[$lineNumberGUID..$lineNumberGUID] | Select-String -pattern '{[-0-9A-F]+?}' -AllMatches | Select-Object -ExpandProperty Matches | Select-Object -ExpandProperty Value )
$guid_install = $guids_install[1]

$array = @()
$array += (Get-Content '.\ENGINE_PROJECT_NAME.sln')[0..$lastLineNumberBefore] 
$array += (Get-Content '.\ENGINE_PROJECT_NAME.sln')[$firstLineNumberAfter..$fileLines]
$array | Set-Content '.\ENGINE_PROJECT_NAME.sln'

# REMOVE PROJECT REFERENCES
#
(Get-Content '.\ENGINE_PROJECT_NAME.sln') | Select-String -pattern "$guid_all_build" -notmatch | Set-Content '.\ENGINE_PROJECT_NAME.sln'
(Get-Content '.\ENGINE_PROJECT_NAME.sln') | Select-String -pattern "$guid_install" -notmatch | Set-Content '.\ENGINE_PROJECT_NAME.sln'


# Patching: ENGINE_PROJECT_NAME.vcxproj
########################################################


$beforeBuildDebug="ENGINE_PROJECT_NAME\\project\\vs2015\\Debug\\</OutDir>"
$afterBuildDebug="ENGINE_PROJECT_NAME\bin\win64\Debug\</OutDir>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeBuildDebug", "$afterBuildDebug" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeBuildRelease="ENGINE_PROJECT_NAME\\project\\vs2015\\Release\\</OutDir>"
$afterBuildRelease="ENGINE_PROJECT_NAME\bin\win64\Release\</OutDir>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeBuildRelease", "$afterBuildRelease" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeIntermediateDebug="ENGINE_PROJECT_NAME.dir\\Debug\\</IntDir>"
$afterIntermediateDebug="`$(SolutionDir)..\..\build\win64\Debug\</IntDir>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeIntermediateDebug", "$afterIntermediateDebug" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeIntermediateRelease="ENGINE_PROJECT_NAME.dir\\Release\\</IntDir>"
$afterIntermediateRelease="`$(SolutionDir)..\..\build\win64\Release\</IntDir>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeIntermediateRelease", "$afterIntermediateRelease" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeDatabaseDebug="project/vs2015/Debug/ENGINE_PROJECT_NAME.pdb</ProgramDataBaseFile>"
$afterDatabaseDebug="bin/win64/Debug/ENGINE_PROJECT_NAME.pdb</ProgramDataBaseFile>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeDatabaseDebug", "$afterDatabaseDebug" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeDatabaseRelease="project/vs2015/Release/ENGINE_PROJECT_NAME.pdb</ProgramDataBaseFile>"
$afterDatabaseRelease="bin/win64/Release/ENGINE_PROJECT_NAME.pdb</ProgramDataBaseFile>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeDatabaseRelease", "$afterDatabaseRelease" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeImportDebug="project/vs2015/Debug/ENGINE_PROJECT_NAME.lib</ImportLibrary>"
$afterImportDebug="build/win64/Debug/ENGINE_PROJECT_NAME.lib</ImportLibrary>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeImportDebug", "$afterImportDebug" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeImportRelease="project/vs2015/Release/ENGINE_PROJECT_NAME.lib</ImportLibrary>"
$afterImportRelease="build/win64/Release/ENGINE_PROJECT_NAME.lib</ImportLibrary>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeImportRelease", "$afterImportRelease" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeCMakeDebug="CMAKE_INTDIR=`"Debug`";"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeCMakeDebug", "" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeCMakeDebugSec="CMAKE_INTDIR=\\`"Debug\\`";"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeCMakeDebugSec", "" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeCMakeRelease="CMAKE_INTDIR=`"Release`";"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeCMakeRelease", "" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeCMakeReleaseSec="CMAKE_INTDIR=\\`"Release\\`";"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeCMakeReleaseSec", "" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'


# Generating: ENGINE_PROJECT_NAME.vcxproj.user
########################################################

$user_settings=@"
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="14.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup Condition="'`$(Configuration)|`$(Platform)'=='Debug|Win32'">
    <LocalDebuggerWorkingDirectory>`$(ProjectDir)..\..\</LocalDebuggerWorkingDirectory>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
  </PropertyGroup>
  <PropertyGroup Condition="'`$(Configuration)|`$(Platform)'=='Release|Win32'">
    <LocalDebuggerWorkingDirectory>`$(ProjectDir)..\..\</LocalDebuggerWorkingDirectory>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
  </PropertyGroup>
  <PropertyGroup Condition="'`$(Configuration)|`$(Platform)'=='Debug|x64'">
    <LocalDebuggerWorkingDirectory>`$(ProjectDir)..\..\</LocalDebuggerWorkingDirectory>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
  </PropertyGroup>
  <PropertyGroup Condition="'`$(Configuration)|`$(Platform)'=='Release|x64'">
    <LocalDebuggerWorkingDirectory>`$(ProjectDir)..\..\</LocalDebuggerWorkingDirectory>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
  </PropertyGroup>
</Project>
"@

$user_settings | Set-Content 'ENGINE_PROJECT_NAME.vcxproj.user'