@echo off
if not defined lib (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
)

pushd client

:: Setup all variables
set OUT_ROOT=..\out
if not exist %OUT_ROOT% mkdir %OUT_ROOT%

set OUT_ROOT=..\%OUT_ROOT%
set OUT_EXE=/OUT:%OUT_ROOT%\client.exe
set OUT_SURFACE=/OUT:%OUT_ROOT%\wsc.dll
set OUT_RENDER=/OUT:%OUT_ROOT%\wrc.dll
set OUT_APP=/OUT:%OUT_ROOT%\app.dll
set OUT_CONSOLE=/OUT:..\%OUT_ROOT%\console.dll

set FLG_RELEASE=/O2
set FLG_DEBUG=/Od /Z7
set FLG_GENERATE_INTRINSICS=/Oi

set DEF_DLL=/D CLIENT_API=__declspec(dllexport)
set WIN32_LIBS=kernel32.lib User32.lib Ws2_32.lib

set CFLAGS=/nologo /fp:fast /fp:except- /Gm- /GR- /EHa- /GS- /Gs214785647 /WX /W4 /diagnostics:caret /FC /DWIN32_LEAN_AND_MEAN %FLG_DEBUG%
set CLINK=/SUBSYSTEM:WINDOWS

:::::::::::::::::::::::::::::::::::
::: Compile Platform Executable :::
:::::::::::::::::::::::::::::::::::
pushd win32
call :print_msg "Compiling Client App DLL"

cl %CFLAGS% /I..\include main.c /link %OUT_EXE%
if exist *.obj (del /s /q /f *.obj)

call :chk_error "Compiling Client App DLL"
popd

:::::::::::::::::::::::::::
::: Compile Surface DLL :::
:::::::::::::::::::::::::::
pushd surface
call :print_msg "Compiling Win32 Surface DLL"

cl %CFLAGS% %DEF_DLL% /I..\include console\console.c /link /DLL %OUT_SURFACE%
if exist *.obj (del /s /q /f *.obj)

call :chk_error "Compiling Win32 Surface DLL"
popd

::::::::::::::::::::::::::::
::: Compile Renderer DLL :::
::::::::::::::::::::::::::::
pushd render
call :print_msg "Compiling Win32 Surface DLL"

cl %CFLAGS% %DEF_DLL% /I..\include console\render.c /link /DLL %OUT_RENDER%
if exist *.obj (del /s /q /f *.obj)

call :chk_error "Compiling Win32 Surface DLL"
popd

goto done
:::::::::::::::::::::::::::
::: Compile Surface DLL :::
:::::::::::::::::::::::::::
pushd win32
call :print_msg Compiling Client App DLL...
cl %CFLAGS% /I..\include main.c /link %OUT_EXE%
popd

if not "%ERRORLEVEL%" == "0" (
	echo.
	echo Compiling Client App DLL Failed!
	echo.

	goto done
) else (
	echo.
	echo Compiling Client App DLL Succeeded!
	echo.
)

:::::::::::::::::::::::::::::::::::
::: Compile Platform Executable :::
:::::::::::::::::::::::::::::::::::
echo.
echo Compiling Client App DLL...
echo.

pushd win32
cl %CFLAGS% /I..\include main.c /link %OUT_EXE%
popd


goto done
:::::::::::::::::::::::
::: Compile App DLL :::
:::::::::::::::::::::::
echo.
echo Compiling Client App DLL...
echo.

pushd app
cl %CFLAGS% %DEF_DLL% /I..\include app.c /link /DLL %OUT_APP%
popd

if not "%ERRORLEVEL%" == "0" (
echo.
echo Compiling Client App DLL Failed!
echo.

goto done
) else (
echo.
echo Compiling Client App DLL Succeeded!
echo.
)

::::::::::::::::::::::::::::
::: Compile App Renderer :::
::::::::::::::::::::::::::::
echo.
echo Compiling Client Renderer (Console)...
echo.

pushd .\render\console
cl %CFLAGS% %DEF_DLL% /I..\..\include render.c /link /DLL %OUT_CONSOLE%
popd
pause
if not "%ERRORLEVEL%" == "0" (
echo.
echo Compiling Client Renderer (Console) Failed!
echo.

goto done
) else (
echo.
echo Compiling Client Renderer (Console) Succeeded!
echo.
)

goto done

echo.
echo Compiling Client Executable...
echo.

pushd platform
cl %CFLAGS% main2.c /link %CLINK%
popd


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

:print_msg
echo.
echo %*
echo.
exit /b

:chk_error
if not "%ERRORLEVEL%" == "0" (
	call :print_msg %1 Failed!
	goto done
) else (
	call :print_msg %1 Succeeded!
)