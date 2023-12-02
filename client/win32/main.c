#include "bgthread.cxx"
#include "main.h"
#include <windows.h>

int APIENTRY
WinMain(HINSTANCE hInst,
        HINSTANCE hInstPrev,
        PSTR cmdline,
        int cmdshow)
{
	SystemFn sysFn;
	AppBuffer app;
	InetBuffer inet;
	SurfaceBuffer surface;
	BgThread bgThread;
	BgEvent bgEvent;
	HANDLE hBgThread;
	size_t cbHeap;
	BgMutex bgMutex;

	// System Functions
	sysFn.memcpy = win32Memcpy;
	sysFn.memset = win32Memset;
	sysFn.memcmp = win32Memcmp;
	sysFn.memmove = win32Memmove;
	sysFn.memzero = win32Memzero;

	win32Memzero(&sysFn, sizeof(SystemFn));
	win32Memzero(&app, sizeof(AppBuffer));
	win32Memzero(&inet, sizeof(InetBuffer));
	win32Memzero(&surface, sizeof(SurfaceBuffer));
	win32Memzero(&bgThread, sizeof(BgThread));
	win32Memzero(&bgEvent, sizeof(BgEvent));
	win32Memzero(&bgMutex, sizeof(BgMutex));

	// Memory
	cbHeap = KB(4);
	app.cbApp = 256;
	app.bufApp = VirtualAlloc(0,
	                          cbHeap,
	                          MEM_RESERVE | MEM_COMMIT,
	                          PAGE_READWRITE);
	inet.cbMsgSend = 256;
	inet.bufMsgSend = (char*)(app.bufApp) + app.cbApp;

	inet.cbMsgRecv = 256;
	inet.bufMsgRecv = (char*)(inet.bufMsgSend) + inet.cbMsgSend;

	surface.cbInput = 256;
	surface.bufInput = (char*)(inet.bufMsgRecv) + inet.cbMsgRecv;

	surface.cbSurface = KB(3);
	surface.bufSurface = (char*)(surface.bufInput) + surface.cbInput;

	// DLL Setup
	getDLLName(&(surface.hSurfaceDLL), DLL_SURFACE);
	getDLLName(&(surface.hRenderDLL), DLL_RENDER);
//	getDLLName(&(inet.hInetDLL), DLL_WINSOCK);
//	getDLLName(&(app.hAppDLL), DLL_APPLICATION);

	// Background Thread
	InitializeConditionVariable(&(bgMutex.cvEvent));
	InitializeConditionVariable(&(bgMutex.cvInetSend));
	InitializeConditionVariable(&(bgMutex.cvInetRecv));
	InitializeConditionVariable(&(bgMutex.cvInput));
	InitializeConditionVariable(&(bgMutex.cvRender));

	InitializeCriticalSection(&(bgMutex.csEvent));
	InitializeCriticalSection(&(bgMutex.csInetSend));
	InitializeCriticalSection(&(bgMutex.csInetRecv));
	InitializeCriticalSection(&(bgMutex.csInput));
	InitializeCriticalSection(&(bgMutex.csRender));

	bgThread.pInet = &inet;
	bgThread.pSurf = &surface;
	bgThread.pSysFn = &sysFn;
	bgThread.pBgEvent = &bgEvent;
	bgThread.pBgMutex = &bgMutex;
	hBgThread = CreateThread(0, 0, threadLoop, &bgThread, 0, 0);

	PushEvent(&bgEvent, &bgMutex, BG_EVENT_SURFACE_INIT);

	int run = 1;
	while (run) {
		if (run == run+1)
			break;
	}

	NO_REF(hInst);
	NO_REF(hInstPrev);
	NO_REF(cmdline);
	NO_REF(cmdshow);

	ExitProcess(0);
}

static int
getDLLName(void **p_hOut, DLL_ID id) {
	switch(id) {
		case DLL_IGNORE: return -2;
		case DLL_APPLICATION: {
			*p_hOut = LoadLibraryA("wa0.dll");
		} break;
		case DLL_WINSOCK: {
			*p_hOut = LoadLibraryA("ww2.dll");
		} break;
		case DLL_SURFACE: {
			*p_hOut = LoadLibraryA("wsc.dll");
		} break;
		case DLL_RENDER: {
			*p_hOut = LoadLibraryA("wrc.dll");
		} break;
	}

	return (*p_hOut == 0) ? -1 : 0;
}

//static int
//setupSurface(P_InetBuffer pInet,
//             P_SurfaceBuffer pSurf,
//             P_SystemFn pSysFn)
//{
//	E_SURFACE eSurface;
//	FnSurfaceInit p_fnSurfaceInit;
//	// TODO: Setup Inet DLL
//
//	pSurf->hSurfaceDLL = LoadLibraryA(pSurf->szSurfaceDLL);
//	if (pSurf->hSurfaceDLL == NULL)
//		return 1;
//
//	p_fnSurfaceInit = (FnSurfaceInit)GetProcAddress(pSurf->hSurfaceDLL,
//	                                                "surfaceInit");
//	if (!p_fnSurfaceInit)
//		return 2;
//
//	eSurface = p_fnSurfaceInit(pSurf, pSysFn);
//	if (eSurface != ERR_SURFACE_SUCCESS)
//		return eSurface;
//
//	NO_REF(pInet);
//
//	return 0;
//}

static void
win32Memcpy(void *dst, const void *src, size_t size) {
	RtlCopyMemory(dst, src, size);
}

static void
win32Memset(void *dst, int value, size_t size) {
	RtlFillMemory(dst, size, value);
}

static void
win32Memzero(void *dst, size_t size) {
	ZeroMemory(dst, size);
}

static void
win32Memmove(void *dst, const void *src, size_t size) {
	RtlMoveMemory(dst, src, size);
}

static size_t
win32Memcmp(const void *m1, const void *m2, size_t size) {
	return RtlCompareMemory(m1, m2, size);
}

