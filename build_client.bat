@echo off
if not defined lib (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
)

pushd client

:: Setup all variables
set OUT_NAME=client
set OUT_ROOT=..\..\out
set OUT_EXE=/OUT:%OUT_ROOT%\%OUT_NAME%.exe
set OUT_DLL=/OUT:%OUT_ROOT%\%OUT_NAME%.dll
if not exist %OUT_ROOT% mkdir %OUT_ROOT%

set FLG_RELEASE=/O2
set FLG_DEBUG=/Od /Z7
set FLG_GENERATE_INTRINSICS=/Oi

set DEF_DLL=/D CLIENT_API=__declspec(dllexport)
set WIN32_LIBS=kernel32.lib User32.lib Ws2_32.lib

set CFLAGS=/nologo /fp:fast /fp:except- /Gm- /GR- /EHa- /GS- /Gs214785647 /WX /W4 /diagnostics:caret /FC /DWIN32_LEAN_AND_MEAN %FLG_DEBUG% /I..\include
set CLINK=/SUBSYSTEM:WINDOWS

echo.
echo Compiling Client App DLL...
echo.

pushd app
cl %CFLAGS% %DEF_DLL% app.c /link /DLL %OUT_DLL%
popd

if not "%ERRORLEVEL%" == "0" (
echo.
echo Compiling Client App DLL Failed!
echo.
) else (
echo.
echo Compiling Client App DLL Succeeded!
echo.
)

goto done

echo.
echo Compiling Client Executable...
echo.

pushd platform
cl %CFLAGS% main2.c /link %CLINK%
popd

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

:done
if not defined BLD_NOPAUSE (
	pause
	exit
)
