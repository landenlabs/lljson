@echo off
setlocal enableextensions

rem
rem  Backup to Dropbox using 7z
rem

set DSTNAME=lljson-win
set DSTDIR=C:\Users\dlang_local\Dropbox\backups\cpp\_ll\
set Z7=D:\opt\art\FileOptimizer\Plugins64\7z.exe

set DT=%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.zip
for /F "tokens=* USEBACKQ" %%F IN (`pdate`) DO (SET DT=%%F)
set OUTZIP=%DSTNAME%_%DT%.zip
set DST=%DSTDIR%%OUTZIP%


rem - remove spaces in file name replace with 0 or _
rem lm "* *" "*1_*1"


echo %Z7% a "%DST%" .\ -mx0 -xr!bin -xr!obj -xr!Debug -xr!x64 -xr!.git -xr!.vs
%Z7% a "%DST%" .\ -mx0 -xr!bin -xr!obj -xr!Debug -xr!x64 -xr!.git -xr!.vs -xr!*.bak

echo ---- Backups ----
dir %DSTDIR%%DSTNAME%*
