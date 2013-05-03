@ECHO OFF
SETLOCAL

SET PEER_DLL=%1
SET PEER_VERSION=%2

IF "%2"=="" (

ECHO This tool builds a patch application for peer.dll
ECHO Usage:
ECHO     build.cmd PathToPeerDllFile version
ECHO.
ECHO Sample:  
ECHO     build.cmd d:\peer.dll 2.1.3.5
ECHO.

GOTO :ERROR
)

IF NOT EXIST %1 (
    ECHO %1 does not exist.
    GOTO :ERROR
)

IF EXIST version.gen (
    del version.gen
)

copy /Y %PEER_DLL% res\peerdll2.bin>NUL

ECHO #define PEER_VERSION "%PEER_VERSION%" >> "version.gen"

ECHO Building the patch ...

"C:\Program Files\Microsoft Visual Studio 9.0\VC\vcpackages\vcbuild.exe" /rebuild appPatch.sln "Release|Win32">NUL

IF NOT EXIST %~dp0release\appPatch.exe (
    ECHO Failed to build the patch.
    GOTO :ERROR
)

SET PATCH_FILE=%~dp0release\peerPatch_%PEER_VERSION%.exe

copy %~dp0release\appPatch.exe %PATCH_FILE%>NUL

ECHO The patch has been built successfully: %PATCH_FILE%

EXIT /B 0

:ERROR

EXIT /B 1