@echo off
echo Downloading SQLite3 for Windows...
curl -L https://www.sqlite.org/2023/sqlite-dll-win64-x64-3420000.zip -o sqlite.zip
echo Extracting SQLite3...
powershell -command "Expand-Archive -Force sqlite.zip -DestinationPath ."
echo Copying SQLite3 DLL to build directory...
copy sqlite3.dll build\
echo Done! 