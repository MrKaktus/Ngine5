
set PATH_ENGINE=".\..\..\..\..\Engine\"

REM Win32 Debug
REM ###########

if exist ".\..\..\bin\win32\Debug\ENGINE_PROJECT_NAME.exe" (
   copy %PATH_ENGINE%\public\bin\win32\Debug\*.* .\..\..\bin\win32\Debug\ 
   )

REM Win32 Release
REM #############

if exist ".\..\..\bin\win32\Release\ENGINE_PROJECT_NAME.exe" (
   copy %PATH_ENGINE%\public\bin\win32\Release\*.* .\..\..\bin\win32\Release\ 
   )

REM Win64 Debug
REM ###########

if exist ".\..\..\bin\win64\Debug\ENGINE_PROJECT_NAME.exe" (
   copy %PATH_ENGINE%\public\bin\win64\Debug\*.* .\..\..\bin\win64\Debug\ 
   )

REM Win64 Release
REM #############

if exist ".\..\..\bin\win64\Release\ENGINE_PROJECT_NAME.exe" (
   copy %PATH_ENGINE%\public\bin\win64\Release\*.* .\..\..\bin\win64\Release\ 
   )
