@echo off
echo Organizing build directory...

:: Create necessary directories
mkdir build\bin 2>nul
mkdir build\lib 2>nul

:: Move executables to bin directory
move /Y build\Debug\*.exe build\bin\ 2>nul
move /Y build\*.exe build\bin\ 2>nul

:: Move DLLs to lib directory
move /Y build\*.dll build\lib\ 2>nul

:: Clean up unnecessary build artifacts
del /Q build\*.pdb 2>nul
del /Q build\*.ilk 2>nul
del /Q build\*.obj 2>nul
del /Q build\*.idb 2>nul

:: Remove empty directories
for /d %%x in (build\*.dir) do rd /s /q "%%x" 2>nul

echo Build directory organized!
echo Executables are now in build\bin
echo Libraries are now in build\lib 