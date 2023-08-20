
/*

Sample Project Configuration:

[COMMON]

Configuration Properties:
  General:
  - Output Directory       : $(SolutionDir)\..\bin\$(Platform)\$(Configuration)\
  - Intermediate Directory : $(SolutionDir)\..\build\$(Platform)\$(Configuration)\
  - Build Log File         : .\..\bin\$(Platform)\$(Configuration)\$(MSBuildProjectName).log
  Debugging:
  - Working Dirctory       : $(ProjectDir)..\
  C/C++:
    General:
    - Additional Include Directories : "./../include/"; "./../../../EngineSDK/Ngine4/engine/latest/include/"; "./../../../EngineSDK/Ngine4/common/include/"

[DEBUG]

Configuration Properties:
  C/C++:
    General:
    - Debug Information Format       : Program Database (/Zi)
    Code Generation:
    - Runtime Library                : Multi-threaded Debug (/MTd)
  Linker:
    General:
    - Additional Library Directories : "./../../../EngineSDK/Ngine4/engine/latest/lib/"; "./../../../EngineSDK/Ngine4/common/lib/debug/"
    Input:
    - Additional Dependiences        : Ngine4dbg.lib;

[RELEASE]

Configuration Properties:
  C/C++:
    Optimization:
    - Optimization                   : Full Optimization (/Ox)
    Code Generation:
    - Runtime Library                : Multi-threaded (/MT)
  Linker:
    General:
    - Additional Library Directories : "./../../../EngineSDK/Ngine4/engine/latest/lib/"; "./../../../EngineSDK/Ngine4/common/lib/"
    Input:
    - Additional Dependiences        : Ngine4.lib;

*/




# StringPooling: true == /GF  false == /GF-
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GF")

  # TreatWChar_tAsBuiltInType: true == /Zc:wchar_t  false == /Zc:wchar_t-
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t")

  # WarningLevel: /W<level 0 to 4> or /Wall
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")






ECHO OFF
CLS
PUSHD %CD%
setlocal

REM Verify that project name was specified
if "%1"=="" (

   POPD
   goto :EOF
   )

set PROJECT_NAME="%1"

REM Determine project destination 
if "%2" NEQ "" (
   set PROJECTS_PATH=%2
   REM TODO: Calculate relative path from project custom path to the Engine directory and take it into notice when generating CMake and other scripts !!!
   ) else (
   set PROJECTS_PATH=./../../../../Projects/
   )

REM Create directory for future repository
if not exist "%PROJECTS_PATH%" (
   mkdir %PROJECTS_PATH% 
   )
   
cd %PROJECTS_PATH%

REM Create directory for new project
if exist "%1" (
   echo.
   echo  Warning!
   echo  ========
   echo.
   echo  Project directory already exists! Terminating.
   echo.
   POPD
   goto :EOF
   )

REM Start proper project setup
mkdir "%1"
mkdir %1\bin
mkdir %1\build
mkdir %1\project
mkdir %1\resources
mkdir %1\src

cd %1
REM call :createMakefile %PROJECT_NAME%

cd project
REM call :createXcodeGenerator
cd ..

cd src
REM call :createSourceFiles

POPD

REM Print out success summary
call :strLen NAME_LENGTH %PROJECT_NAME% 
echo.
echo  %1
echo|set /p "= a" < NUL
for /l %%x in (1,1,%NAME_LENGTH%) do (
   echo|set /p "=-" < NUL
   )
echo/
echo.
echo  Project created successfully.
echo.
goto :EOF

REM HELPER FUNNCTIONS

:strLen <return_var> <string>
setlocal enabledelayedexpansion
if "%~2"=="" (set ret=0) else set ret=1
set "tmpstr=%~2"
for %%I in (4096 2048 1024 512 256 128 64 32 16 8 4 2 1) do (
    if not "!tmpstr:~%%I,1!"=="" (
        set /a ret += %%I
        set "tmpstr=!tmpstr:~%%I!"
    )
)
endlocal & set "%~1=%ret%"
goto :EOF


REM Create initial main.cpp with main entrypoint
:createSourceFiles
(set MAIN_CPP="#include ^"Ngine.h^"

int main^(int argc, const char* argv[]^)
{
   // You're ready to go!

   return 0;
}
" )
echo %MAIN_CPP%>main.cpp
exit /b
goto :EOF









:strLen
setlocal enabledelayedexpansion
:strLen_Loop
   if not "!%1:~%len%!"=="" set /A len+ = 1 & goto :strLen_Loop
(endlocal & set %2 = %len%)
goto :eof



:strLen  strVar  [rtnVar]
setlocal disableDelayedExpansion
set len=0
if defined %~1 for /f "delims=:" %%N in (
  '"(cmd /v:on /c echo(!%~1!&echo()|findstr /o ^^"'
) do set /a "len=%%N-3"
endlocal & if "%~2" neq "" (set %~2=%len%) else echo %len%
exit /b

:strlen <resultVar> <stringVar>
(   
    setlocal EnableDelayedExpansion
    set "s=!%~2!#"
    set "len=0"
    for %%P in (4096 2048 1024 512 256 128 64 32 16 8 4 2 1) do (
        if "!s:~%%P,1!" NEQ "" ( 
            set /a "len+=%%P"
            set "s=!s:~%%P!"
        )
    )
)
( 
    endlocal
    set "%~1=%len%"
    exit /b
)