#ifndef CSG_CLIENT_PLATFORM_TO_APP_H
#define CSG_CLIENT_PLATFORM_TO_APP_H

#include "types/input.h"
#include "types/system.h"
#include "types/render.h"
#include "types/inet.h"

#define MAX_INET_MSG_SEND 256
#define MAX_INET_MSG_RECV 256
#define MAX_INET_MSG MAX_INET_MSG_SEND + MAX_INET_MSG_RECV

typedef struct {
	UserBuffer userInput[MAX_USERINPUT];
	FnMemory memFn;
	RenderBuffer render;
	InetBuffer inet;

	size_t *szBack;
	char *bufBack;		// 1KB
	                    //    - 2KB for whole buffer

	size_t cbMsgSend;
	char *bufMsgSend;

	size_t cbMsgRecv;
	char *bufMsgRecv;

	// TODO: Thinking about this, This shouldn't be here.
	//       At some point, remove this (and ALL win32 functions)
	//       From the app.h and app.cxx files.
	//
	//       Supply a "platform-based" structure to the app's init
	//       Function to provide things like memcpy, memset,
	//       inet wakeup, etc.
	CRITICAL_SECTION inetMutex; // Note: Yes, i know this isn't a mutex
								//       For my purposes, it acts as such.
	CONDITION_VARIABLE inetCondVar;
} PlatformData, *P_PlatformData;

#endif // CSG_CLIENT_PLATFORM_TO_APP_H

