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
	size_t flags;

//	size_t *szBack;
//	char *bufBack;		// 1KB
//	                    //    - 2KB for whole buffer
//
//	size_t cbMsgSend;
//	char *bufMsgSend;
//
//	size_t cbMsgRecv;
//	char *bufMsgRecv;
} PlatformData, *P_PlatformData;

#endif // CSG_CLIENT_PLATFORM_TO_APP_H

