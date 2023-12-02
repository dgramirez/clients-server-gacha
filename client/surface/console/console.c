#include "console.h"
#include "defines.h"

CLIENT_API int
expSurfaceInit(P_InitSurface pInit, P_SystemFn pSysFn) {
	BOOL eWin32;
	Win32Console win32Console;

	// Create Surface
	eWin32 = AllocConsole();
	if (eWin32 == 0)
		return 1;

	NO_REF(win32Console);
	NO_REF(pInit);
	NO_REF(pSysFn);
	return 0;
}


CLIENT_API int
expSurfaceUpdate() {
	return 0;
}

CLIENT_API int
expSurfaceShutdown() {
	FreeConsole();
	return 0;
}
