#include "console.h"
#include "defines.h"

CLIENT_API int
expSurfaceInit(P_InitSurface pInit, P_SystemFn pSysFn) {
	BOOL eWin32;
	HANDLE hStdOut;
	CONSOLE_CURSOR_INFO cci;
	CONSOLE_SCREEN_BUFFER_INFOEX csbi;

	// Create Surface
	eWin32 = AllocConsole();
	if (eWin32 == 0)
		return E_SURFACE_FAIL_CREATE_WINDOW;
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// Remove Cursor
	pSysFn->memzero(&cci, sizeof(CONSOLE_CURSOR_INFO));
	eWin32 = GetConsoleCursorInfo(hStdOut, &cci);
	if (eWin32 == 0) {
		FreeConsole();
		return E_SURFACE_FAIL_GET_CONSOLE_CURSOR_INFO;
	}

	cci.bVisible = 0;
	eWin32 = SetConsoleCursorInfo(hStdOut, &cci);
	if (eWin32 == 0){
		FreeConsole();
		return E_SURFACE_FAIL_SET_CONSOLE_CURSOR_INFO;
	}

	// Setup Console Buffer
	pSysFn->memzero(&csbi, sizeof(CONSOLE_SCREEN_BUFFER_INFOEX));
	csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	eWin32 = GetConsoleScreenBufferInfoEx(hStdOut, &csbi);
	if (eWin32 == 0) {
		FreeConsole();
		return E_SURFACE_FAIL_GET_CONSOLE_SCREEN_BUFFER_INFO_EX;
	}

	csbi.dwSize.X = pInit->posW;
	csbi.dwSize.Y = pInit->posH;
	csbi.srWindow.Left = pInit->posX;
	csbi.srWindow.Right = csbi.srWindow.Left + csbi.dwSize.X;
	csbi.srWindow.Top = pInit->posY;
	csbi.srWindow.Bottom = csbi.srWindow.Top + csbi.dwSize.Y;
	eWin32 = SetConsoleScreenBufferInfoEx(hStdOut, &csbi);
	if (eWin32 == 0) {
		FreeConsole();
		return E_SURFACE_FAIL_SET_CONSOLE_SCREEN_BUFFER_INFO_EX;
	}

	pInit->out_hProcess = GetModuleHandle(0);
	pInit->out_hWindow = hStdOut;
	return E_SURFACE_SUCCESS;
}

CLIENT_API int
expSurfaceInput() {
	return 0;
}

CLIENT_API int
expSurfaceUpdate() {
	// No need to update the surface.
	return 0;
}

CLIENT_API int
expSurfaceShutdown() {
	FreeConsole();
	return 0;
}
