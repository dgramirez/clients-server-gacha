@echo off
if not defined lib (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
)

pushd server

set OUT_NAME=server.exe
set OUT_ROOT=..\out
if not exist %OUT_ROOT% mkdir %OUT_ROOT%

set FLG_RELEASE=/O2
set FLG_DEBUG=/Od /Z7
set FLG_GENERATE_INTRINSICS=/Oi

set CFLAGS=/nologo /fp:fast /fp:except- /Gm- /GR- /EHa- /GS- /Gs214785647 /WX /W4 /diagnostics:caret /FC /DWIN32_LEAN_AND_MEAN /I..\dep
set CLINK=/SUBSYSTEM:WINDOWS /OUT:%OUT_ROOT%\%OUT_NAME% kernel32.lib User32.lib Ws2_32.lib

echo.
echo Compiling Server...
echo.

cl %CFLAGS% main.c /link %CLINK%

if exist *.obj (del /s /q /f *.obj)
popd

if not "%ERRORLEVEL%" == "0" (
echo.
echo Compiling Server has been Failure!
echo.
) else (
echo.
echo Compiling Server has been Successful!
echo.
)

if not defined BLD_NOPAUSE (
	pause
	exit
)