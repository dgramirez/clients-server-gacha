#ifndef CSG_CLIENT_WIN32_MAIN_H
#define CSG_CLIENT_WIN32_MAIN_H

#include "defines.h"
#include "types/app.h"
#include "types/inet.h"
#include "types/surface.h"
#include "types/system.h"

//#include "user_input.h"
//#include "platform_to_app.h"

#include <windows.h>
//#include <stdint.h>

typedef enum {
	DLL_IGNORE = 0,
	DLL_APPLICATION,
	DLL_WINSOCK,
	DLL_SURFACE,
	DLL_RENDER
} DLL_ID;

//typedef struct _clientdata {
//	// Buffer Data (2K) [All 512 bytes]
//	void *pApp;
//	void *pTmp;
//
//	// Application Functions
//	int (*appUpdate)(void*, P_PlatformData);
//	PlatformData platformData;
//} Win32Data, *P_Win32Data;

// Stubs
//static int
//appUpdateStub(void* vpAppData,
//              P_PlatformData pPlatformData);

static int
getDLLName(void **p_hOut, DLL_ID id);

static int
setupSurface(P_InetBuffer, P_SurfaceBuffer, P_SystemFn);

static void
win32Memcpy(void *dst, const void *src, size_t size);

static void
win32Memset(void *dst, int value, size_t size);

static void
win32Memzero(void *dst, size_t size);

static void
win32Memmove(void *dst, const void *src, size_t size);

static size_t
win32Memcmp(const void *m1, const void *m2, size_t size);

#endif // CSG_CLIENT_WIN32_MAIN_H

