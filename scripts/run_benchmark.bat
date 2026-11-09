@echo off
setlocal enabledelayedexpansion

title ForgeUI vs Dear ImGui Benchmark Runner
cd /d "%~dp0.."

echo ====================================================
echo     ForgeUI vs Dear ImGui Benchmark Runner
echo ====================================================

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

:CONFIG_CMAKE
echo [1/3] Configuring CMake with -DFORGE_BUILD_BENCHMARK=ON ...
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DFORGE_BUILD_BENCHMARK=ON
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b %ERRORLEVEL%
)

echo [2/3] Building ForgeBenchmark ...
cmake --build build --target ForgeBenchmark --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed.
    pause
    exit /b %ERRORLEVEL%
)

echo [3/3] Running ForgeBenchmark ...
if exist "build\examples\Release\ForgeBenchmark.exe" (
    powershell -Command "Unblock-File -Path 'build\examples\Release\ForgeBenchmark.exe' -ErrorAction SilentlyContinue"
    "build\examples\Release\ForgeBenchmark.exe"
) else if exist "build\examples\ForgeBenchmark.exe" (
    powershell -Command "Unblock-File -Path 'build\examples\ForgeBenchmark.exe' -ErrorAction SilentlyContinue"
    "build\examples\ForgeBenchmark.exe"
) else if exist "build\Release\ForgeBenchmark.exe" (
    powershell -Command "Unblock-File -Path 'build\Release\ForgeBenchmark.exe' -ErrorAction SilentlyContinue"
    "build\Release\ForgeBenchmark.exe"
) else if exist "build\ForgeBenchmark.exe" (
    powershell -Command "Unblock-File -Path 'build\ForgeBenchmark.exe' -ErrorAction SilentlyContinue"
    "build\ForgeBenchmark.exe"
) else (
    echo [ERROR] Could not find ForgeBenchmark.exe
    pause
    exit /b 1
)

echo ====================================================
echo Benchmark complete.
echo ====================================================
if "%~1" neq "--no-pause" pause
