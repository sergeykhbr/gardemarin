@echo off
setlocal

set CONFIG=Debug

set BUILD_DIR=.build

echo ========================
echo === Configure Motor Driver ===
echo ========================
cmake -S . -B "%BUILD_DIR%" ^
    -DCMAKE_TOOLCHAIN_FILE="%~dp0gcc_arm.cmake" ^
    -DCMAKE_BUILD_TYPE=%CONFIG%

if errorlevel 1 exit /b 1

echo ========================
echo === Build Motor Driver ===
echo ========================
cmake --build "%BUILD_DIR%" --config %CONFIG% --parallel

if errorlevel 1 exit /b 1

echo ========================
echo === Build successful ===
echo ========================

pause
exit
