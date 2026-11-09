@echo off
setlocal enabledelayedexpansion

title ForgeUI Build and Test Runner
cd /d "%~dp0.."

set "BUILD_DIR=build"
set "GENERATOR="

where cl.exe >nul 2>&1
if %ERRORLEVEL% EQU 0 goto :CONFIG_CMAKE

for %%P in (
    "D:\VisualStudio\Packages\Common7\Tools\VsDevCmd.bat"
    "D:\VisualStudio\Packages\VC\Auxiliary\Build\vcvars64.bat"
    "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"
    "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
    "%ProgramFiles%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
    "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
) do (
    if exist %%P (
        call %%P -no_logo -arch=x64 >nul 2>&1
        if !ERRORLEVEL! EQU 0 goto :CONFIG_CMAKE
        call %%P >nul 2>&1
        goto :CONFIG_CMAKE
    )
)

for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath 2^>nul`) do (
    if exist "%%i\Common7\Tools\VsDevCmd.bat" (
        call "%%i\Common7\Tools\VsDevCmd.bat" -no_logo -arch=x64
        goto :CONFIG_CMAKE
    )
    if exist "%%i\VC\Auxiliary\Build\vcvars64.bat" (
        call "%%i\VC\Auxiliary\Build\vcvars64.bat"
        goto :CONFIG_CMAKE
    )
)

where ninja.exe >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    set "GENERATOR=-G Ninja"
    goto :CONFIG_CMAKE
)

where g++.exe >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    where mingw32-make.exe >nul 2>&1
    if !ERRORLEVEL! EQU 0 (
        set "GENERATOR=-G MinGW Makefiles"
        goto :CONFIG_CMAKE
    )
)

:CONFIG_CMAKE
if exist "%BUILD_DIR%" rd /s /q "%BUILD_DIR%"

if defined GENERATOR (
    cmake -B "%BUILD_DIR%" -S . %GENERATOR% -DCMAKE_BUILD_TYPE=Release -DFORGE_BUILD_TESTS=ON -DFORGE_BUILD_WINDOWED_DEMO=ON
) else (
    cmake -B "%BUILD_DIR%" -S . -DCMAKE_BUILD_TYPE=Release -DFORGE_BUILD_TESTS=ON -DFORGE_BUILD_WINDOWED_DEMO=ON
)

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b %ERRORLEVEL%
)

cmake --build "%BUILD_DIR%" --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ================= Running Tests =================
if exist "%BUILD_DIR%\tests\Release\ForgeCoreTests.exe" (
    "%BUILD_DIR%\tests\Release\ForgeCoreTests.exe"
) else if exist "%BUILD_DIR%\tests\ForgeCoreTests.exe" (
    "%BUILD_DIR%\tests\ForgeCoreTests.exe"
) else (
    echo [ERROR] Test executable not found!
    pause
    exit /b 1
)

set TEST_ERR=%ERRORLEVEL%
echo =================================================

if "%~1"=="--no-pause" goto :END
pause

:END
exit /b %TEST_ERR%
