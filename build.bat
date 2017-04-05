@echo off

set SRCFILES=DXHooker.cpp DXHooker_gw.cpp main.cpp
set LIBFILES=user32.lib d3d9.lib "%DXSDK_DIR%\lib\x86\d3dx9.lib"
set OUTFILE=gwtimer.dll
set OPT=/O2

if not defined DevEnvDir (
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86
)

cl /LD /EHsc /nologo %OPT%  /I"%DXSDK_DIR%\include" %SRCFILES% /link %LIBFILES% /out:%OUTFILE%
del *.obj