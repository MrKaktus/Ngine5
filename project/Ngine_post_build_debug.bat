REM Clear current latest build release
REM ###################################

del ".\..\public\bin\*.*" /Q /S
del ".\..\public\lib\*.*" /Q /S

REM Gather current latest build release
REM ###################################


mkdir ".\..\public\lib\win32\Debug"
copy .\..\bin\win32\Debug\Ngine5dbg.lib .\..\public\lib\win32\Debug\Ngine5dbg.lib

copy .\..\middleware\zlib-1.2.8\contrib\vstudio\vc11\x86\ZlibDllDebug\zlibwapi.dll .\..\public\bin\zlibwapi.dll  
copy .\..\middleware\openvr-0.9.11\bin\win32\openvr_api.dll .\..\public\bin\openvr_api.dll
copy "C:\Program Files\Autodesk\FBX\FBX SDK\2015.1\lib\vs2013\x86\debug\libfbxsdk.dll" .\..\public\bin\libfbxsdk.dll

