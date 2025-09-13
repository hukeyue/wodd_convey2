@echo off
setlocal

REM Script for building the odd_convey's WINDOWS PORT,
REM
REM Usage: build-xp.bat

REM Prerequisites:
REM
REM   Visual Studio 2026, CMake, Ninja,
REM   Visual Studio 2026 SDK.
REM

set VisualStudioInstallerFolder="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"
if %PROCESSOR_ARCHITECTURE%==x86 set VisualStudioInstallerFolder="%ProgramFiles%\Microsoft Visual Studio\Installer"

pushd %VisualStudioInstallerFolder%
for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath -prerelease`) do (
  set VisualStudioInstallDir=%%i
)
popd

set VCToolsVersion=14.16.27012
set "WindowsSDKVersion=10.0.10240.0\"
set "WindowsSdkDir=C:\Program Files (x86)\Windows Kits\10"
set "VCINSTALLDIR=%VisualStudioInstallDir%\VC"

cd /D "%~dp0"
cd ..

REM
REM Generate static x86 binary
REM
set Platform=x86
set VSCMD_ARG_TGT_ARCH=x86

call "%~dp0callxp-%Platform%.cmd"

del /s /q build-xp
md build-xp
cd build-xp
set CC=clang-cl
cmake -DCMAKE_C_COMPILER_TARGET=i686-pc-windows-msvc -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DCMAKE_BUILD_TYPE=Release -G Ninja ..
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%
ninja
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%
copy /b /y access.exe ..\access-xp.exe
copy /b /y knr_access.exe ..\knr_access-xp.exe
copy /b /y odd_convey.exe ..\odd_convey-xp.exe
