@echo off

set prog=lljson

set dstdir=%bindir%
if not exist "%dstdir%" (
 if exist c:\opt\bin  set dstdir=c:\opt\bin
 if exist d:\opt\bin  set dstdir=d:\opt\bin
)
echo "msbuild=%msbuild%"
if not exist "%msbuild%" (
echo Fall back msbuild not found at "%msbuild%"
set msbuild=F:\opt\VisualStudio\2022\Preview\MSBuild\Current\Bin\MSBuild.exe
)

cd %prog%-ms
@echo Clean %proj% 
rmdir /s x64 2> nul

@echo.
@echo Build release target
"%msbuild%" %prog%.sln -p:Configuration="Release";Platform=x64 -verbosity:minimal  -detailedSummary:True 
cd ..

@echo.
@echo ---- Build done 
if not exist "%prog%-ms\x64\Release\%prog%.exe" (
   echo Failed to build %prog%-ms\x64\Release\%prog%.exe
   goto _end
)

@echo.
@echo Copy Release to %dstdir%
:: dir %prog%-ms\x64\Release\%prog%.exe
copy %prog%-ms\x64\Release\%prog%.exe %dstdir%\%prog%.exe

@echo.
@echo Compare md5 hash
:: cmp -h %prog%-ms\x64\Release\%prog%.exe %dstdir%\%prog%.exe
ld -a -ph %prog%-ms\x64\Release\%prog%.exe %dstdir%\%prog%.exe

@rem play happy tone
rundll32.exe cmdext.dll,MessageBeepStub
rundll32 user32.dll,MessageBeep

:_end
