#ifndef CSG_CLIENT_INET_CXX
#define CSG_CLIENT_INET_CXX

#include "inet.h"

static DWORD
inetThreadLoop(LPVOID arg);

static int
networkInit(P_InetInit pInitInet) {
	InetData inetData;

	// Validation
	if (!pInitInet)
		return 1;

	if (!(pInitInet->bufMsgInet))
		return 2;

	if (pInitInet->cbMsgInet != 512)
		return 3;

	if (!(pInitInet->pMutex))
		return 4;

	if (!(pInitInet->pCondVar))
		return 5;

	// Setup Inet Data
	ZeroMemory(&inetData, sizeof(InetData));

	inetData.bufMsgSend = pInitInet->bufMsgInet;
	inetData.cbMsgSend = 0;

	inetData.bufMsgRecv = inetData.bufMsgSend + inetData.cbMsgSend;
	inetData.cbMsgRecv = 0;

	inetData.pMutex = pInitInet->pMutex;
	inetData.pCondVar = pInitInet->pCondVar;

	// Note: Critical Section, for my purpose, acts as a common mutex.
	InitializeCriticalSection(inetData.pMutex);
	InitializeConditionVariable(inetData.pCondVar);
	CreateThread(0, 0, inetThreadLoop, &inetData, 0, 0);

	SleepConditionVariableCS(inetData.pCondVar,
	                         inetData.pMutex,
	                         INFINITE);
	return 0;
}

static int
inetServerConnect(P_InetData pData) {
	SHORT i;
	SHORT last;
	int iResult;
	struct addrinfo *pBindSearch;
	struct addrinfo infoHints;

/* ------------------------------------------------- */
	// Send: ";" : "IP Address" :  ";" : "Port" : ";"
	// Big Note: This really should be part of a parsing system.
	//           Maybe this will be converted to a "todo".

	if (pData->bufMsgSend[0] != ';')
		return 1;

	last = 1;
	for (i = 1; i < pData->cbMsgSend; ++i) {
		if (pData->bufMsgSend[i] < '.' &&
		    pData->bufMsgSend[i] > ';')
		{
			WSACleanup();
			return 2;
		}

		if (pData->bufMsgSend[i] == ';' ||
			pData->bufMsgSend[i] == ':' ||
			pData->bufMsgSend[i] == '/')
		{
			break;
		}
	}

	if ((i - last - 1) >= 16)
		return 3;

	RtlCopyMemory(pData->cstrIP, pData->bufMsgSend + last, i - last - 1);

	if (pData->bufMsgSend[i+1] != ';')
		return 4;

	last = i + 1;
	for (i = last + 1; i < pData->cbMsgSend; ++i) {
		if (pData->bufMsgSend[i] < '.' &&
		    pData->bufMsgSend[i] > ';')
		{
			WSACleanup();
			return 5;
		}

		if (pData->bufMsgSend[i] == ';' ||
			pData->bufMsgSend[i] == ':' ||
			pData->bufMsgSend[i] == '/')
		{
			break;
		}
	}

	if ((i - last - 1) >= 5)
		return 6;

	RtlCopyMemory(pData->cstrIP, pData->bufMsgSend + last, i - last - 1);

/* ------------------------------------------------- */
	iResult = WSAStartup(MAKEWORD(2,2), &(pData->WSAData));
	if (iResult)
		return 7;

	ZeroMemory(&infoHints, sizeof(infoHints));
	infoHints.ai_family = AF_UNSPEC;
	infoHints.ai_socktype = SOCK_STREAM;
	infoHints.ai_protocol = IPPROTO_TCP;
	iResult = getaddrinfo(NULL,
	                      pData->shPort,
	                      &infoHints,
	                      &(pData->BindInfo));

	if (!iResult) {
		WSACleanup();
		return 8;
	}

	pBindSearch = pData->BindInfo;
	for (; pBindSearch != 0; pBindSearch = pBindSearch->ai_next)
	{
		pData->SocketConnect = socket(pBindSearch->ai_family,
		                              pBindSearch->ai_socktype,
		                              pBindSearch->ai_protocol);

		if (pData->SocketConnect == INVALID_SOCKET) {
			WSACleanup();
			return 9;
		}

		iResult = connect(pData->SocketConnect,
		                  pBindSearch->ai_addr,
		                  (int)pBindSearch->ai_addrlen);

		if (iResult == SOCKET_ERROR) {
			closesocket(pData->SocketConnect);
			pData->SocketConnect = INVALID_SOCKET;
			continue;
		}

		break;
	}

	freeaddrinfo(pData->BindInfo);

	if (pData->SocketConnect == INVALID_SOCKET) {
		WSACleanup();
		return 10;
	}

	return 0;
}

static DWORD
inetThreadLoop(LPVOID arg) {
	int result;
	int i;
	InetData inetData;

	RtlCopyMemory(&inetData, arg, sizeof(InetData));
	WakeConditionVariable(inetData.pCondVar);

	while (1) {
		// Loop for initialization
		while (1) {
			SleepConditionVariableCS(inetData.pCondVar,
			                         inetData.pMutex,
			                         INFINITE);

			if (inetData.bufMsgSend[0] == ';') {
				result = inetServerConnect(&inetData);
				if (!result) {
					inetData.bufMsgRecv[0] = 1;
					break;
				}

				// TODO: Ensure this is done correctly
				inetData.bufMsgRecv[0] = -1;
				inetData.shPort = 0;
				inetData.stayAwake = 0;
				inetData.SocketConnect = INVALID_SOCKET;

				RtlFillMemory(inetData.cstrIP, 16, 0);
				RtlFillMemory(&inetData.WSAData, sizeof(WSADATA), 0);
				RtlFillMemory(inetData.BindInfo,
				              sizeof(struct addrinfo),
				              0);
			}
		}

		// Loop for network
		while (1) {
			if (!inetData.stayAwake) {
				SleepConditionVariableCS(inetData.pCondVar,
										inetData.pMutex,
										16);
			}
			else
				inetData.stayAwake = 0;

			if (inetData.cbMsgSend) {
				if (inetData.cbMsgSend > 256)
					inetData.cbMsgSend = 256;

				result = send(inetData.SocketConnect,
				              inetData.bufMsgSend,
				              (int)inetData.cbMsgSend,
							  0);

				RtlFillMemory(inetData.bufMsgSend, inetData.cbMsgSend, 0);
				inetData.cbMsgSend = 0;
			}

			// Recieve
			if (inetData.cbMsgRecv > 256)
				continue;

			result = recv(inetData.SocketConnect,
			              inetData.bufMsgRecv + inetData.cbMsgRecv,
						  256 - (int)inetData.cbMsgRecv,
						  0);

			// strlen, maxed out at 256
			for (i = (int)inetData.cbMsgRecv; i < 256; ++i) {
				if (inetData.bufMsgRecv[i] == '\0') {
					inetData.cbMsgRecv = i + 1;
					break;
				}
			}
		}
	}

	return 0;
}

//static DWORD
//network_loop(void *args) {
//	InetData data;
//	RtlCopyMemory(&data, args, sizeof(InetData));
//	// TODO: Tell the platform thread to continue
//
//	while (1) {
//		if (data.psMsg[0] == -1) {
//			// TODO: Attempt to disconnect from SOCKET
//
//			data.xPort = 0;
//			ConnectSocket = INVALID_SOCKET;
//			RtlFillMemory(data.pszIP, 15, 0);
//			data.BindInfo = 0;
//			RtlFillMemory(&data.WSAData, sizeof(WSADATA), 0);
//
//			// TODO: Condition Lock this. Platform unlock upon app req.
//			(void)sizeof(data);
//			continue;
//		}
//
//		// Read Message
//		switch(data.psMsg[0]) {
//			case INET_CLOSE: continue;
//			case INET_NULL: continue;
//			case INET_START: {
//
//			} break;
//			case INET_SEND: {
//
//			} break;
//			case INET_RECV: {
//
//			} break;
//		}
//	}
//
//	return 0;
//}

#endif // CSG_CLIENT_INET_CXX

