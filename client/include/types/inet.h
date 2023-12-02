#ifndef CSG_CLIENT_INCLUDE_TYPES_INET_H
#define CSG_CLIENT_INCLUDE_TYPES_INET_H

typedef int (*FnInetWakeup)(void *hCondVar, void *hMutex);

typedef struct {
	void *hInetDLL;
	char szInetDLL[8];

	void *bufMsgSend;
	size_t cbMsgSend;
	
	void *bufMsgRecv;
	size_t cbMsgRecv;
} InetBuffer, *P_InetBuffer;

//typedef struct {
//	char *bufMsgSend;
//	size_t cbMsgSend;
//	size_t cbMsgSendMax;
//
//	char *bufMsgRecv;
//	size_t cbMsgRecv;
//	size_t cbMsgRecvMax;
//
//	void *hInetCondVar;
//	void *hInetMutex;
//
//	FnInetWakeup fnWakeup;
//} InetBuffer, *P_InetBuffer;

#endif // CSG_CLIENT_INCLUDE_TYPES_INET_H

