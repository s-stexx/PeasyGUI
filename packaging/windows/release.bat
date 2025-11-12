@echo off
setlocal enabledelayedexpansion

REM ----------------------------------------------------------------------------------------
REM Config
REM ----------------------------------------------------------------------------------------
set TOP_LEVEL_DIR=%~dp0\..\..
pushd %TOP_LEVEL_DIR%

set QT_PATH=C:\Qt\6.8.3\mingw_64
set BUILD_DIR=build\MinSizeRel
set DIST_DIR=dist\win
set CMAKE_PREFIX_PATH=%QT_PATH%\lib\cmake
set CMAKE_EXE=C:\Qt\Tools\CMake_64\bin\cmake.exe

REM add to path for the duration of this Batch (mingw1310_64 is subject to change!)
set PATH=C:\Qt\Tools\mingw1310_64\bin;%PATH%

REM ----------------------------------------------------------------------------------------
echo ">>> Configuring and building PeasyGUI with CMake ..."
REM ----------------------------------------------------------------------------------------
if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
%CMAKE_EXE% -S . -B %BUILD_DIR% -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%
if errorlevel 1 exit /b 1

%CMAKE_EXE% --build %BUILD_DIR% --config MinSizeRel
if errorlevel 1 exit /b 1

REM ----------------------------------------------------------------------------------------
echo ">>> Deploying Qt dependencies with windeployqt ..."
REM ----------------------------------------------------------------------------------------
cd %BUILD_DIR%
%QT_PATH%\bin\windeployqt.exe --release PeasyGUI.exe
if errorlevel 1 exit /b 1
cd %TOP_LEVEL_DIR%

REM ----------------------------------------------------------------------------------------
echo ">>> Building example projects ..."
REM ----------------------------------------------------------------------------------------
cd packaging\examples\3_c_greet
if exist build rmdir /s /q build
%CMAKE_EXE% -S . -B %BUILD_DIR% -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%
if errorlevel 1 exit /b 1

%CMAKE_EXE% --build %BUILD_DIR% --config MinSizeRel
if errorlevel 1 exit /b 1
cd %TOP_LEVEL_DIR%
rmdir /s /q "packaging\examples\3_c_greet\build"

REM ----------------------------------------------------------------------------------------
echo ">>> Preparing dist folder ..."
REM ----------------------------------------------------------------------------------------
if exist %DIST_DIR% rmdir /s /q %DIST_DIR%
mkdir %DIST_DIR%
mkdir %DIST_DIR%\PeasyGUI
mkdir %DIST_DIR%\PeasyGUI\bin
mkdir %DIST_DIR%\PeasyGUI\bin\platforms
mkdir %DIST_DIR%\PeasyGUI\bin\imageformats
mkdir %DIST_DIR%\PeasyGUI\examples

copy packaging\windows\Installation_steps.txt %DIST_DIR% >nul

copy packaging\start_PeasyGUI.bat %DIST_DIR%\PeasyGUI >nul

copy README.md %DIST_DIR%\PeasyGUI\bin >nul 
copy LICENSE %DIST_DIR%\PeasyGUI\bin >nul
copy third_party_licenses\LICENSE.* %DIST_DIR%\PeasyGUI\bin >nul

copy %BUILD_DIR%\PeasyGUI.exe %DIST_DIR%\PeasyGUI\bin >nul

copy %BUILD_DIR%\libgcc_s_seh-1.dll %DIST_DIR%\PeasyGUI\bin >nul
copy %BUILD_DIR%\libstdc*-6.dll %DIST_DIR%\PeasyGUI\bin >nul
copy %BUILD_DIR%\libwinpthread-1.dll %DIST_DIR%\PeasyGUI\bin >nul
copy %BUILD_DIR%\Qt6Widgets.dll %DIST_DIR%\PeasyGUI\bin >nul
copy %BUILD_DIR%\Qt6Core.dll %DIST_DIR%\PeasyGUI\bin >nul
copy %BUILD_DIR%\Qt6Gui.dll %DIST_DIR%\PeasyGUI\bin >nul
copy %BUILD_DIR%\Qt6Network.dll %DIST_DIR%\PeasyGUI\bin >nul
copy %BUILD_DIR%\platforms\qwindows.dll %DIST_DIR%\PeasyGUI\bin\platforms >nul
copy %BUILD_DIR%\imageformats\*.dll %DIST_DIR%\PeasyGUI\bin\imageformats >nul

REM copy examples\ recursive
xcopy packaging\examples\* %DIST_DIR%\PeasyGUI\examples\ /E /I /Y

REM delete the unwanted shell/command files
del "%DIST_DIR%\PeasyGUI\examples\*.sh" /S /Q
del "%DIST_DIR%\PeasyGUI\examples\*.command" /S /Q

REM ----------------------------------------------------------------------------------------
echo ">>> Creating ZIP archive ..."
REM ----------------------------------------------------------------------------------------
set ZIP_NAME=%DIST_DIR%\PeasyGUI-0000000.zip
if exist %ZIP_NAME% del %ZIP_NAME%

powershell -command "Compress-Archive -Path '%DIST_DIR%\*' -DestinationPath '%ZIP_NAME%' -Force"

REM ----------------------------------------------------------------------------------------
echo ">>> Done. Created %ZIP_NAME%"
REM ----------------------------------------------------------------------------------------
popd
endlocal

pause