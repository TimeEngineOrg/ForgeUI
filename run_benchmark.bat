@echo off
setlocal

echo ====================================================
echo     ForgeUI vs Dear ImGui Benchmark Runner
echo ====================================================

where cl.exe >nul 2>nul
if %errorlevel% neq 0 (
    if exist "D:\VisualStudio\Packages\VC\Auxiliary\Build\vcvars64.bat" (
        call "D:\VisualStudio\Packages\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    )
)

echo [1/3] Configuring CMake with -DFORGE_BUILD_BENCHMARK=ON ...
cmake -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DFORGE_BUILD_BENCHMARK=ON
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed.
    exit /b %errorlevel%
)

echo [2/3] Building ForgeBenchmark ...
cmake --build build --target ForgeBenchmark
if %errorlevel% neq 0 (
    echo [ERROR] Build failed.
    exit /b %errorlevel%
)

echo [3/3] Running ForgeBenchmark ...
powershell -Command "Unblock-File -Path 'build\ForgeBenchmark.exe' -ErrorAction SilentlyContinue"
build\ForgeBenchmark.exe

echo ====================================================
