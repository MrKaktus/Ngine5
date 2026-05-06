REM Engine is expected to be located in the same parent directory as app
set PATH_ENGINE=".\..\..\..\Engine\"

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
