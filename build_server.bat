@echo off
echo Building ChatServer...

:: Clean and create build directory
rmdir /s /q build 2>nul
mkdir build
cd build

:: Configure with CMake
cmake ..

:: Build the project
cmake --build . --config Debug

:: Create bin directory if it doesn't exist
mkdir bin 2>nul

:: Copy the executable from Debug to bin
copy /Y Debug\ChatServer.exe bin\
cd .. 