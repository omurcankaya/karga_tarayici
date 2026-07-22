@echo off
setlocal enabledelayedexpansion

set "SOURCE_DIR=%~dp0"
set "ARCH=x86"
set "BUILD_TYPE=Debug"
set "NON_INTERACTIVE=0"
set "CLEAN_BUILD=0"

:parse_args
if "%~1"=="" goto check_interactive
if /i "%~1"=="-arch" (
    set "ARCH=%~2"
    set "NON_INTERACTIVE=1"
    shift
    shift
    goto parse_args
)
if /i "%~1"=="-config" (
    set "BUILD_TYPE=%~2"
    set "NON_INTERACTIVE=1"
    shift
    shift
    goto parse_args
)
if /i "%~1"=="-clean" (
    set "CLEAN_BUILD=1"
    shift
    goto parse_args
)
shift
goto parse_args

:check_interactive
if "%NON_INTERACTIVE%"=="1" goto start_build

echo ===================================================
echo   Karga Tarayici Interactive Build Engine (CMake)
echo ===================================================
echo.
echo Select Target Architecture:
echo   [1] x86 (32-bit - Game Target Default)
echo   [2] x64 (64-bit)
set /p ARCH_CHOICE="Enter choice [1-2] (Default: 1): "

if "%ARCH_CHOICE%"=="2" (
    set "ARCH=x64"
) else (
    set "ARCH=x86"
)

echo.
echo Select Build Configuration:
echo   [1] Debug
echo   [2] Release
set /p CONFIG_CHOICE="Enter choice [1-2] (Default: 1): "

if "%CONFIG_CHOICE%"=="2" (
    set "BUILD_TYPE=Release"
) else (
    set "BUILD_TYPE=Debug"
)

:start_build
echo.
echo ===================================================
echo   Building Karga Tarayici Target: %ARCH% / %BUILD_TYPE%
echo ===================================================

set "BUILD_DIR=%SOURCE_DIR%build\%ARCH%_%BUILD_TYPE%"

if "%CLEAN_BUILD%"=="1" (
    if exist "%BUILD_DIR%" (
        echo Cleaning build directory %BUILD_DIR%...
        rmdir /s /q "%BUILD_DIR%"
    )
)

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

cd /d "%BUILD_DIR%"

if "%ARCH%"=="x86" (
    set "GEN_ARCH=Win32"
) else (
    set "GEN_ARCH=x64"
)

echo Running CMake configuration...
cmake -S "%SOURCE_DIR%." -B "%BUILD_DIR%" -A %GEN_ARCH% -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake Configuration Failed!
    exit /b %ERRORLEVEL%
)

echo.
echo Compiling binaries...
cmake --build "%BUILD_DIR%" --config %BUILD_TYPE%
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build Failed!
    exit /b %ERRORLEVEL%
)

echo.
echo ===================================================
echo   Build Successful!
echo   Outputs generated in: %SOURCE_DIR%bin\%ARCH%\%BUILD_TYPE%\
echo ===================================================
echo.

if "%NON_INTERACTIVE%"=="1" goto end

set "LAUNCH_APP=N"
set /p LAUNCH_APP="Do you want to launch KargaScannerApp.exe demo now? [Y/N] (Default: Y): "
if /i "%LAUNCH_APP%"=="N" goto end

set "EXE_PATH=%SOURCE_DIR%bin\%ARCH%\%BUILD_TYPE%\KargaScannerApp.exe"
if exist "%EXE_PATH%" (
    echo Launching %EXE_PATH%...
    start "" "%EXE_PATH%"
) else (
    echo [ERROR] Executable not found at %EXE_PATH%
)

:end
endlocal
