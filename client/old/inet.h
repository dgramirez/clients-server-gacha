#ifndef CSG_CLIENT_INET_H
#define CSG_CLIENT_INET_H

#include <windows.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <process.h>

enum INET_MSG {
	INET_CLOSE = -1,
	INET_NULL = 0,
	INET_START = 1,
	INET_SEND = 2,
	INET_RECV = 3
};

typedef struct {
	char *bufMsgInet;
	size_t cbMsgInet;

	PCRITICAL_SECTION pMutex;
	PCONDITION_VARIABLE pCondVar;
} InetInit, *P_InetInit;

typedef struct {
	char *bufMsgSend;
	size_t cbMsgSend;

	char *bufMsgRecv;
	size_t cbMsgRecv;

	PCRITICAL_SECTION pMutex;
	PCONDITION_VARIABLE pCondVar;

	SHORT shPort;
	SHORT stayAwake;
	SOCKET SocketConnect;
	char cstrIP[16];
	struct addrinfo *BindInfo;
	WSADATA WSAData;
} InetData, *P_InetData;

static int
network_init(P_InetInit pInitInet);

#endif // CSG_CLIENT_INET_H

