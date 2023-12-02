#ifndef CSG_CLIENT_WIN32_BACKGROUND_THREAD_H
#define CSG_CLIENT_WIN32_BACKGROUND_THREAD_H

#include "types/inet.h"
#include "types/surface.h"
#include "types/system.h"

#include "defines.h"
#include <windows.h>

typedef enum {
	BG_EVENT_NONE = 0,

	BG_EVENT_INPUT_RECORD_START,
	BG_EVENT_INPUT_RECORD_STOP,

	BG_EVENT_SURFACE_INIT,
	BG_EVENT_SURFACE_SHUTDOWN,
	BG_EVENT_SURFACE_REBOOT,

	BG_EVENT_RENDER_INIT,
	BG_EVENT_RENDER_DRAW,
	BG_EVENT_RENDER_SHUTDOWN,
	BG_EVENT_RENDER_REBOOT,

	BG_EVENT_INET_INIT,
	BG_EVENT_INET_SEND,
	BG_EVENT_INET_RECORD_START,
	BG_EVENT_INET_RECORD_STOP,
	BG_EVENT_INET_SHUTDOWN,
	BG_EVENT_INET_REBOOT,
} BG_EVENTID;

typedef enum {
	BG_FLAG_NONE = 0,

	BG_FLAG_QUIT = 0x1,
	BG_FLAG_INPUT = 0x2,
	BG_FLAG_RENDER = 0x4,
} EV_CHECKFLAGS;

typedef struct {
	char aEvent[15];
	char ctEvent;
} BgEvent, *P_BgEvent;

typedef struct {
	CONDITION_VARIABLE cvEvent;
	CONDITION_VARIABLE cvInetSend;
	CONDITION_VARIABLE cvInetRecv;
	CONDITION_VARIABLE cvInput;
	CONDITION_VARIABLE cvRender;

	CRITICAL_SECTION csEvent;
	CRITICAL_SECTION csInetSend;
	CRITICAL_SECTION csInetRecv;
	CRITICAL_SECTION csInput;
	CRITICAL_SECTION csRender;
} BgMutex, *P_BgMutex;

typedef struct {
	P_InetBuffer pInet;
	P_SurfaceBuffer pSurf;
	P_SystemFn pSysFn;
	P_BgEvent pBgEvent;
	P_BgMutex pBgMutex;
} BgThread, *P_BgThread;

static DWORD
threadLoop(void *arg);

static void
DoBatchedEvents(P_BgThread pBgThread,
                P_BgEvent pBgEvent,
                P_BgMutex pBgMutex);

static BG_EVENTID
PopEvent(char* aEvent);

static int
PushEvent(P_BgEvent, P_BgMutex, char);

static int
DoEvent(P_BgThread pBgThread, BG_EVENTID evID);

// Surface Functions
static int
Win32SurfaceInit(P_SurfaceBuffer, P_SystemFn);

// Support Functions
static int
Win32TryEnterCriticalSection(CRITICAL_SECTION *pCS,
                             CONDITION_VARIABLE *pCV,
                             size_t ctIterationPerMs);

static void
Win32LeaveCriticalSection(CRITICAL_SECTION *pCS,
                          CONDITION_VARIABLE *pCV);

#endif // CSG_CLIENT_WIN32_BACKGROUND_THREAD_H

