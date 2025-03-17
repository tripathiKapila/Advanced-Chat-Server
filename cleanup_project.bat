@echo off
echo Cleaning up project...

:: Keep only the latest ChatServer.exe in the build directory
cd build

:: Remove other executables in build directory
if exist SimpleClient.exe del SimpleClient.exe
if exist SimpleServer.exe del SimpleServer.exe
if exist InteractiveClient.exe del InteractiveClient.exe
if exist AdvancedChatServer.exe del AdvancedChatServer.exe

:: Clean Debug directory
if exist Debug\SimpleClient.exe del Debug\SimpleClient.exe
if exist Debug\SimpleServer.exe del Debug\SimpleServer.exe
if exist Debug\InteractiveClient.exe del Debug\InteractiveClient.exe
if exist Debug\AdvancedChatServer.exe del Debug\AdvancedChatServer.exe

:: Clean src/Debug directory
if exist src\Debug\AdvancedChatServer.exe del src\Debug\AdvancedChatServer.exe
if exist src\Debug\main.exe del src\Debug\main.exe

:: Clean compiler ID executables
if exist CMakeFiles\3.26.4\CompilerIdC\CompilerIdC.exe del CMakeFiles\3.26.4\CompilerIdC\CompilerIdC.exe
if exist CMakeFiles\3.26.4\CompilerIdCXX\CompilerIdCXX.exe del CMakeFiles\3.26.4\CompilerIdCXX\CompilerIdCXX.exe

echo Cleanup complete! Only ChatServer.exe remains. 