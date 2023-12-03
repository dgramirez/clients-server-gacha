#include "bgthread.cxx"
#include "xformfn.cxx"
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

	win32Memzero(&sysFn, sizeof(SystemFn));
	win32Memzero(&app, sizeof(AppBuffer));
	win32Memzero(&inet, sizeof(InetBuffer));
	win32Memzero(&surface, sizeof(SurfaceBuffer));
	win32Memzero(&bgThread, sizeof(BgThread));
	win32Memzero(&bgEvent, sizeof(BgEvent));
	win32Memzero(&bgMutex, sizeof(BgMutex));

	// System Functions
	win32InitFn(&sysFn);

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

	surface.memInput.cbAddr = 256;
	surface.memInput.vpAddr = (char*)(inet.bufMsgRecv) + inet.cbMsgRecv;

	surface.memSurface.cbAddr = KB(3);
	surface.memSurface.vpAddr = (char*)(surface.memInput.vpAddr) + 
	                            surface.memInput.cbAddr;

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

	PushEvent(&bgEvent, &bgMutex, &sysFn, BG_EVENT_SURFACE_INIT);

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

