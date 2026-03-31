@echo off
set SRC=%~dp0ForgedAlliance_exxt.exe
set DST=C:\ProgramData\FAForever\bin\ForgedAlliance_exxt.exe
set INIT=init_faf.lua
set LOG=%~dp0debug.log

echo Copying patched exe to FAForever bin...
copy /Y "%SRC%" "%DST%"

echo Starting from FAForever bin...
echo Log: %LOG%

cd /d C:\ProgramData\FAForever\bin
"%DST%"  /log "%LOG%" /nomovie
echo Exit code: %ERRORLEVEL%
pause
