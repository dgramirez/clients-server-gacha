@echo off
if not defined lib (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
)

set BLD_NOPAUSE=1
call build_server.bat
if not "%ERRORLEVEL%" == "0" ( goto :stop )

call build_client.bat
if not "%ERRORLEVEL%" == "0" ( goto :stop )

echo Success!
pause
exit

:stop
echo Failure!
pause
exit