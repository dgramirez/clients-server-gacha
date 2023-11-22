#ifndef CSG_CLIENT_MAIN_H
#define CSG_CLIENT_MAIN_H

#include "app.h"
#include "user_input.h"
#include "platform_to_app.h"

#include <windows.h>
#include <stdint.h>

#define KB(x) ((x) << 10)
#define MB(x) ((x) << 20)
#define NO_REF(x) ((void)sizeof(x))

typedef struct _clientdata {
	// Buffer Data (2K) [All 512 bytes]
	void* pApp;
	void *pTmp;

	// Application Functions
	int (*appUpdate)(void*, P_PlatformData);
	PlatformData platformData;
} Win32Data, *P_Win32Data;

// Stubs
static int
appUpdateStub(void* vpAppData,
                P_PlatformData pPlatformData);

#endif // CSG_CLIENT_MAIN_H

