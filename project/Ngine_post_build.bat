
REM Middleware
REM ##########

set PATH_FBX="C:\Program Files\Autodesk\FBX\FBX SDK\2019.0\lib\vs2015"
set PATH_OPENVR=.\..\middleware\openvr-1.1.3b\bin
set PATH_OPENXR=.\..\middleware\OpenXR\bin
set PATH_ZLIB=.\..\middleware\zlib-1.2.10\contrib\vstudio\vc14

REM Ngine Win32 Debug
REM #################

if exist ".\..\bin\win32\Debug\Ngine5dbg.lib" (
   del   ".\..\public\lib\win32\Debug\*.*" /s /q
   rmdir ".\..\public\lib\win32\Debug\*.*" /s /q
   mkdir ".\..\public\lib\win32\Debug"
   copy .\..\bin\win32\Debug\Ngine5dbg.lib .\..\public\lib\win32\Debug\Ngine5dbg.lib
   del   ".\..\public\bin\win32\Debug\*.*" /s /q
   rmdir ".\..\public\bin\win32\Debug\*.*" /s /q
   mkdir ".\..\public\bin\win32\Debug"
   copy %PATH_FBX%\x86\debug\libfbxsdk.dll .\..\public\bin\win32\Debug\libfbxsdk.dll
   copy %PATH_OPENVR%\win32\openvr_api.dll .\..\public\bin\win32\Debug\openvr_api.dll
   copy %PATH_OPENXR%\openxr_loader-1_0.dll .\..\public\bin\win32\Debug\openxr_loader-1_0.dll
   copy %PATH_ZLIB%\x86\ZlibDllDebug\zlibwapi.dll .\..\public\bin\win32\Debug\zlibwapi.dll  
   )

REM Ngine Win32 Release
REM ###################

if exist ".\..\bin\win32\Release\Ngine5.lib" (
   del   ".\..\public\lib\win32\Release\*.*" /s /q
   rmdir ".\..\public\lib\win32\Release\*.*" /s /q
   mkdir ".\..\public\lib\win32\Release"
   copy .\..\bin\win32\Release\Ngine5.lib .\..\public\lib\win32\Release\Ngine5.lib
   del   ".\..\public\bin\win32\Release\*.*" /s /q
   rmdir ".\..\public\bin\win32\Release\*.*" /s /q
   mkdir ".\..\public\bin\win32\Release"
   copy %PATH_FBX%\x86\release\libfbxsdk.dll .\..\public\bin\win32\Release\libfbxsdk.dll
   copy %PATH_OPENVR%\win32\openvr_api.dll .\..\public\bin\win32\Release\openvr_api.dll 
   copy %PATH_OPENXR%\openxr_loader-1_0.dll .\..\public\bin\win32\Release\openxr_loader-1_0.dll
   copy %PATH_ZLIB%\x86\ZlibDllRelease\zlibwapi.dll .\..\public\bin\win32\Release\zlibwapi.dll 
   )

REM Ngine Win64 Debug
REM #################

if exist ".\..\bin\win64\Debug\Ngine5dbg.lib" (
   del   ".\..\public\lib\win64\Debug\*.*" /s /q
   rmdir ".\..\public\lib\win64\Debug\*.*" /s /q
   mkdir ".\..\public\lib\win64\Debug"
   copy .\..\bin\win64\Debug\Ngine5dbg.lib .\..\public\lib\win64\Debug\Ngine5dbg.lib
   del   ".\..\public\bin\win64\Debug\*.*" /s /q
   rmdir ".\..\public\bin\win64\Debug\*.*" /s /q
   mkdir ".\..\public\bin\win64\Debug"
   copy %PATH_FBX%\x64\debug\libfbxsdk.dll .\..\public\bin\win64\Debug\libfbxsdk.dll
   copy %PATH_OPENVR%\win64\openvr_api.dll .\..\public\bin\win64\Debug\openvr_api.dll
   copy %PATH_OPENXR%\openxr_loader-1_0.dll .\..\public\bin\win64\Debug\openxr_loader-1_0.dll
   copy %PATH_ZLIB%\x64\ZlibDllDebug\zlibwapi.dll .\..\public\bin\win64\Debug\zlibwapi.dll  
   )

REM Ngine Win64 Release
REM ###################

if exist ".\..\bin\win64\Release\Ngine5.lib" (
   del   ".\..\public\lib\win64\Release\*.*" /s /q
   rmdir ".\..\public\lib\win64\Release\*.*" /s /q
   mkdir ".\..\public\lib\win64\Release"
   copy .\..\bin\win64\Release\Ngine5.lib .\..\public\lib\win64\Release\Ngine5.lib
   del   ".\..\public\bin\win64\Release\*.*" /s /q
   rmdir ".\..\public\bin\win64\Release\*.*" /s /q
   mkdir ".\..\public\bin\win64\Release"
   copy %PATH_FBX%\x64\release\libfbxsdk.dll .\..\public\bin\win64\Release\libfbxsdk.dll
   copy %PATH_OPENVR%\win64\openvr_api.dll .\..\public\bin\win64\Release\openvr_api.dll 
   copy %PATH_OPENXR%\openxr_loader-1_0.dll .\..\public\bin\win64\Release\openxr_loader-1_0.dll
   copy %PATH_ZLIB%\x64\ZlibDllRelease\zlibwapi.dll .\..\public\bin\win64\Release\zlibwapi.dll  
   )
