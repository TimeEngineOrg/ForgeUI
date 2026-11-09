@echo off
setlocal

title ForgeUI Windowed Demo Launcher
cd /d "%~dp0.."

echo ====================================================
echo     ForgeUI Windowed Demo Build and Runner
echo ====================================================

if "%~1"=="--build" goto :DO_BUILD
if "%~1"=="-b" goto :DO_BUILD

if exist "build\examples\ForgeWindowedDemo.exe" goto :RUN_APP
if exist "build\examples\Release\ForgeWindowedDemo.exe" goto :RUN_APP
if exist "build\ForgeWindowedDemo.exe" goto :RUN_APP
if exist "build\Release\ForgeWindowedDemo.exe" goto :RUN_APP

:DO_BUILD
echo [INFO] Build not found or rebuild requested. Invoking build pipeline...
call "%~dp0build_and_test.bat" --no-pause
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed.
    pause
    exit /b %ERRORLEVEL%
)

:RUN_APP
echo [INFO] Launching ForgeWindowedDemo...
echo [INFO] Controls: Left-click to interact, F12 to take screenshot, Close window to exit.
echo.

if exist "build\examples\ForgeWindowedDemo.exe" (
    "build\examples\ForgeWindowedDemo.exe" %*
) else if exist "build\examples\Release\ForgeWindowedDemo.exe" (
    "build\examples\Release\ForgeWindowedDemo.exe" %*
) else if exist "build\ForgeWindowedDemo.exe" (
    "build\ForgeWindowedDemo.exe" %*
) else if exist "build\Release\ForgeWindowedDemo.exe" (
    "build\Release\ForgeWindowedDemo.exe" %*
) else (
    echo [ERROR] Could not find ForgeWindowedDemo.exe in build\examples\ or build\
    pause
    exit /b 1
)

set DEMO_ERR=%ERRORLEVEL%
if %DEMO_ERR% NEQ 0 (
    echo.
    echo ====================================================
    echo [ERROR] ForgeWindowedDemo exited with error code: %DEMO_ERR%
    echo ====================================================
    pause
    exit /b %DEMO_ERR%
)

echo [INFO] Demo finished cleanly.
