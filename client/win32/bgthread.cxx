#ifndef CSG_CLIENT_WIN32_BACKGROUND_THREAD_CXX
#define CSG_CLIENT_WIN32_BACKGROUND_THREAD_CXX

#include "bgthread.h"

static DWORD
threadLoop(void *arg) {
	P_BgThread pXData;
	size_t fChk;

	pXData = (P_BgThread)arg;
	fChk = 0;

	while (1) {
		DoBatchedEvents(pXData, pXData->pBgEvent, pXData->pBgMutex);
		if (HAS_FLG(fChk, BG_FLAG_QUIT))
			break;

//		DoInputEvents();
//		DoRenderEvents();

		Sleep(4);
	}
	return 0;
}

static int
DoEvent(P_BgThread pBgThread, BG_EVENTID evID) {
	(void)sizeof(pBgThread);
	switch(evID) {
		case BG_EVENT_NONE: return 0;
		case BG_EVENT_INPUT_RECORD_START: {

		} break;
		case BG_EVENT_INPUT_RECORD_STOP: {

		} break;
		case BG_EVENT_SURFACE_INIT: {
			Win32SurfaceInit(pBgThread->pSurf, pBgThread->pSysFn);
		} break;
	}
	return 0;
}

static void
DoBatchedEvents(P_BgThread pXData,
                P_BgEvent pBgEvent,
                P_BgMutex pBgMutex)
{
	char i;
	int hasThread;
	BG_EVENTID evID;

	for (i = 0; i < 16; ++i) {
		hasThread = Win32TryEnterCriticalSection(&(pBgMutex->csEvent),
		                                         &(pBgMutex->cvEvent),
												 1);
		if (!hasThread)
			break;

		if (pBgEvent->aEvent[0] == BG_EVENT_NONE) {
			Win32LeaveCriticalSection(&(pBgMutex->csEvent),
			                          &(pBgMutex->cvEvent));
			break;
		}

		evID = PopEvent(pBgEvent->aEvent);
		pBgEvent->ctEvent -= 1;
		Win32LeaveCriticalSection(&(pBgMutex->csEvent),
                                  &(pBgMutex->cvEvent));

		DoEvent(pXData, evID);
	}
}

static BG_EVENTID
PopEvent(char *aEvent) {
	BG_EVENTID evID;
	size_t *p_llPackedEvents;

	evID = aEvent[0];

	p_llPackedEvents = (size_t*)aEvent;
	*p_llPackedEvents >>= 8;

	// Flag: Unaligned Operation
	p_llPackedEvents = (size_t*)aEvent + 7;
	*p_llPackedEvents >>= 8;

	return evID;
}

static int
PushEvent(P_BgEvent pBgEvent, P_BgMutex pBgMutex, char value) {
	int hasThread;
	hasThread = Win32TryEnterCriticalSection(&(pBgMutex->csEvent),
	                                         &(pBgMutex->cvEvent),
	                                         2);
	if (!hasThread)
		return -1;

	pBgEvent->aEvent[(pBgEvent->ctEvent)++] = value;
	Win32LeaveCriticalSection(&(pBgMutex->csEvent), &(pBgMutex->cvEvent));
	return 0;
}

static int
Win32TryEnterCriticalSection(CRITICAL_SECTION *pCS,
                             CONDITION_VARIABLE *pCV,
                             size_t ctIterationPerMs)
{
	int i;
	BOOL eTimedCS;
	CRITICAL_SECTION tryCS;

	i = -1;
	InitializeCriticalSection(&tryCS);
	do {
		eTimedCS = TryEnterCriticalSection(pCS);
		if (eTimedCS)
			break;

		if (++i >= ctIterationPerMs)
			break;

		SleepConditionVariableCS(pCV, &tryCS, 1);
	} while(1);
	DeleteCriticalSection(&tryCS);

	return (eTimedCS > 0);
}

static void
Win32LeaveCriticalSection(CRITICAL_SECTION *pCS,
                          CONDITION_VARIABLE *pCV)
{
	LeaveCriticalSection(pCS);
	WakeConditionVariable(pCV);
}

static int
Win32SurfaceInit(P_SurfaceBuffer pSurf, P_SystemFn pSysFn) {
	int eSurf;
	InitSurface initSurf;

	if (pSurf->fnInit || pSurf->fnUpdate || pSurf->fnShutdown)
		return 1;

	pSurf->fnInit = (FnSurfaceInit)GetProcAddress(pSurf->hSurfaceDLL,
	                                              "expSurfaceInit");
	if (!pSurf->fnInit)
		return 2;

	pSurf->fnUpdate = (FnSurfaceUpdate)GetProcAddress(pSurf->hSurfaceDLL,
	                                                  "expSurfaceUpdate");
	if (!pSurf->fnUpdate)
		return 3;

	pSurf->fnShutdown = (FnSurfaceShutdown)GetProcAddress(
		pSurf->hSurfaceDLL,
		"expSurfaceShutdown");

	if (!pSurf->fnShutdown)
		return 4;

	// TODO: Get Surface Configuration
	initSurf.posX = 0;
	initSurf.posY = 0;
	initSurf.posW = 0;
	initSurf.posH = 0;
	eSurf = pSurf->fnInit(&initSurf, pSysFn);
	if (eSurf)
		return eSurf;

	pSurf->hProcess = initSurf.out_hProcess;
	pSurf->hWindow = initSurf.out_hWindow;
	return 0;
}

#endif // CSG_CLIENT_WIN32_BACKGROUND_THREAD_CXX

