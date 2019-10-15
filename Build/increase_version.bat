@echo off 

set major=1
set minor=0

set /p build= <%~dp0\build.txt 
set /a build= %build%+1 

set version=%major%.%minor%.%build%

echo %build% >%~dp0\build.txt
echo %version% >%~dp0\version.txt

echo #define APPVERSION "%version%" >%~dp0\version.h

echo VERSION = %version%
