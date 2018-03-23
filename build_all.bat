@echo off
TITLE VCCOOP Build Script

SET "outputDir=.\vice-city-coop\bin"
SET "log=%outputDir%\build.log"

echo Cleaning directories before compilation..
RMDIR /S /Q %outputDir%
MKDIR %outputDir%
IF EXIST "%log%" DEL /F /Q "%log%"

echo Building project..
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x86 1>"%log%" 2>&1
devenv ".\vice-city-coop\vice-city-coop.sln" /Rebuild Release 1>"%log%" 2>&1

echo. 
IF ERRORLEVEL 1 (
   COLOR c
   echo end %date% - %time%
   echo Build failed! Check build.log in %outputDir%
)
echo. 

echo Copying binaries..
copy /Y .\vice-city-coop\server\Debug\server.exe %outputDir%
copy /Y .\vice-city-coop\vccoop_main\vccoopd.asi %outputDir%

echo Cleaning source files..
devenv ".\vice-city-coop\vice-city-coop.sln" /Clean 1>NUL 2>&1
pause