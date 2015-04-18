:: don't output commands
@echo off

:: make build directory
if not exist build mkdir build
pushd build

:: run CMake
cmake ..
if %errorlevel% neq 0 popd & exit /b %errorlevel%

:: build with CMake
cmake --build . --config Release
if %errorlevel% neq 0 popd & exit /b %errorlevel%

:: run tests
Release\mustache.exe
if %errorlevel% neq 0 popd & exit /b %errorlevel%

popd
