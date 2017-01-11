Param(
   [string]$projectDirectory
)

$projectName="ENGINE_PROJECT_NAME"

# Should be 'vs2015' etc.
#
Set-Location -Path $projectDirectory


# Patching: ENGINE_PROJECT_NAME.vcxproj
########################################################

# TODO: Remove INSTALL and BUILD_ALL projects from solution, and all CMake files


# Patching: ENGINE_PROJECT_NAME.vcxproj
########################################################
#using xml
#$xml = [xml](Get-Content .\ENGINE_PROJECT_NAME.vcxproj)
#$xml.SelectNodes("//command") | % { 
#    $_."#text" = $_."#text".Replace("C:\Prog\Laun.jar", "C:\Prog32\folder\test.jar") 
#    }

#$xml.Save("C:\Users\graimer\Desktop\test.xml")

#(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace '', '' | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'




$beforeBuildDebug="ENGINE_PROJECT_NAME\\project\\vs2015\\Debug\\</OutDir>"
$afterBuildDebug="ENGINE_PROJECT_NAME\bin\win64\Debug\</OutDir>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeBuildDebug", "$afterBuildDebug" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeBuildRelease="ENGINE_PROJECT_NAME\\project\\vs2015\\Release\\</OutDir>"
$afterBuildRelease="ENGINE_PROJECT_NAME\bin\win64\Release\</OutDir>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeBuildRelease", "$afterBuildRelease" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeIntermediateDebug="ENGINE_PROJECT_NAME.dir\\Debug\\</IntDir>"
$afterIntermediateDebug="`$(SolutionDir)\..\..\build\win64\Debug\</IntDir>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeIntermediateDebug", "$afterIntermediateDebug" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeIntermediateRelease="ENGINE_PROJECT_NAME.dir\\Release\\</IntDir>"
$afterIntermediateRelease="`$(SolutionDir)\..\..\build\win64\Release\</IntDir>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeIntermediateRelease", "$afterIntermediateRelease" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeDatabaseDebug="project/vs2015/Debug/ENGINE_PROJECT_NAME.pdb</ProgramDataBaseFile>"
$afterDatabaseDebug="bin/win64/Debug/ENGINE_PROJECT_NAME.pdb</ProgramDataBaseFile>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeDatabaseDebug", "$afterDatabaseDebug" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeDatabaseRelease="project/vs2015/Release/ENGINE_PROJECT_NAME.pdb</ProgramDataBaseFile>"
$afterDatabaseRelease="bin/win64/Release/ENGINE_PROJECT_NAME.pdb</ProgramDataBaseFile>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeDatabaseRelease", "$afterDatabaseRelease" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeLibDebug="project/vs2015/Debug/Verify2.lib</ImportLibrary>"
$afterLibDebug="build/win64/Debug/Verify2.lib</ImportLibrary>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeLibDebug", "$afterLibDebug" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'

$beforeLibRelease="project/vs2015/Release/Verify2.lib</ImportLibrary>"
$afterLibRelease="build/win64/Release/Verify2.lib</ImportLibrary>"
(Get-Content '.\ENGINE_PROJECT_NAME.vcxproj') -replace "$beforeLibRelease", "$afterLibRelease" | Set-Content '.\ENGINE_PROJECT_NAME.vcxproj'
