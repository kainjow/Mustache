:: don't output commands
@echo off

:: get version from input args
:: default value is 12
:: only 12 and 14 are supported
set vers=%1
if "%vers%" == "" set vers=12
if "%vers%" neq "12" (
  if "%vers%" neq "14" (
    echo Invalid version "%vers%" - expected 12 or 14.
    exit /b 1
  )
)

:: make build directory
if not exist build mkdir build
pushd build

:: run CMake
cmake -G "Visual Studio %vers%" ..
if %errorlevel% neq 0 popd & exit /b %errorlevel%

:: build with CMake
cmake --build . --config Release
if %errorlevel% neq 0 popd & exit /b %errorlevel%

:: run tests
Release\mustache.exe
if %errorlevel% neq 0 popd & exit /b %errorlevel%

popd

:: make build64 directory
if not exist build64 mkdir build64
pushd build64

:: run CMake
cmake -G "Visual Studio %vers% Win64" ..
if %errorlevel% neq 0 popd & exit /b %errorlevel%

:: build with CMake
cmake --build . --config Release
if %errorlevel% neq 0 popd & exit /b %errorlevel%

:: run tests
Release\mustache.exe
if %errorlevel% neq 0 popd & exit /b %errorlevel%

popd
