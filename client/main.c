#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>

#define MSGLEN 512
#define DEFAULT_PORT "27015"

typedef struct {
	WSADATA WSAData;
	SOCKET ConnectSocket;
	struct addrinfo *BindInfo;
	char msg[MSGLEN];
} PlatformINetClient, *PPlatformINetClient;

static int
client_init(PPlatformINetClient pinet);

static void
client_print(const char *msg);

static void
client_echo(const char *msg);

int APIENTRY
WinMain(HINSTANCE hInst,
        HINSTANCE hInstPrev,
        PSTR cmdline,
        int cmdshow)
{
	AllocConsole();

	DWORD written = 0;
	HANDLE stdinput = GetStdHandle(STD_INPUT_HANDLE);

	char pass[64] = {0};
	client_print("Password: ");
	ReadConsole(stdinput, &pass, 64, &written, 0);

	client_echo("Connecting...");

	PlatformINetClient inet = {0};
	if (client_init(&inet)) {
		ZeroMemory(inet.msg, MSGLEN);
		wsprintfA(inet.msg, "Failed to initialize. Error Code: %d", WSAGetLastError());
		client_echo(inet.msg);

		system("pause");
		return 5;
	}

	int iResult = send(inet.ConnectSocket, pass, (int)lstrlen(pass), 0);
	if (iResult == SOCKET_ERROR) {
		ZeroMemory(inet.msg, MSGLEN);
		wsprintfA(inet.msg, "Failed to send message. Error Code: %d", WSAGetLastError());
		client_echo(inet.msg);

		system("pause");
		return 6;
	}

	int try = 1;
	while (try) {
		recv(inet.ConnectSocket, inet.msg, MSGLEN, 0);
		switch(inet.msg[0]) {
			case '0': { 
				client_echo("Authenication Successful!");
				try = 0;
			} break;
			case '1':{
				client_print("Password: ");
				ReadConsole(stdinput, &pass, 64, &written, 0);
				send(inet.ConnectSocket, pass, (int)lstrlen(pass), 0);
			} break;
			case '2':{
				client_echo("Disconnected... Too many attempts. Aborting...");
				closesocket(inet.ConnectSocket);
				ReadConsole(stdinput, &pass, 64, &written, 0);
				return 0;
			} break;
		}
	}

	int run = 1;
	while (run) {
		ZeroMemory(inet.msg, MSGLEN);
		recv(inet.ConnectSocket, inet.msg, MSGLEN, 0);
		client_echo(inet.msg);

		char input[2];
		ReadConsole(stdinput, &input, 2, &written, 0);
		input[1] = '\0';
		send(inet.ConnectSocket, input, 1, 0);

		if (input[0] == '0') {
			run = 0;
			break;
		}

		ZeroMemory(inet.msg, MSGLEN);
		recv(inet.ConnectSocket, inet.msg, MSGLEN, 0);
		client_echo(inet.msg);
	}

	closesocket(inet.ConnectSocket);
	ReadConsole(stdinput, &pass, 64, &written, 0);

	sizeof(hInst);
	sizeof(hInstPrev);
	sizeof(cmdline);
	sizeof(cmdshow);
	return 0;
}

static int
client_init(PPlatformINetClient pinet) {
	int iResult = WSAStartup(MAKEWORD(2,2),
	                         &(pinet->WSAData));
	if (iResult) {
		MessageBoxA(NULL, "WSA Startup Failed to Initialize","FAILURE", MB_OK);
		system("pause");
		return 1;
	}

	struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
	iResult = getaddrinfo(NULL,
	                      DEFAULT_PORT,
	                      &hints,
	                      &(pinet->BindInfo));

	if (iResult) {
		MessageBoxA(NULL, "Failed to get the address information","FAILURE", MB_OK);
		WSACleanup();
		system("pause");
		return 2;
	}

	struct addrinfo *ptr = pinet->BindInfo;
	for (; ptr != 0; ptr = ptr->ai_next) {
		pinet->ConnectSocket = socket(ptr->ai_family,
		                              ptr->ai_socktype,
		                              ptr->ai_protocol);

		if(pinet->ConnectSocket == INVALID_SOCKET) {
			MessageBoxA(NULL, "Failed to create a socket","FAILURE", MB_OK);
			WSACleanup();
			system("pause");
			return 3;
		}

		iResult = connect(pinet->ConnectSocket,
		                  ptr->ai_addr,
		                  (int)ptr->ai_addrlen);

		if (iResult == SOCKET_ERROR) {
			closesocket(pinet->ConnectSocket);
			pinet->ConnectSocket = INVALID_SOCKET;
			continue;
		}

		break;
	}

	freeaddrinfo(pinet->BindInfo);

	if (pinet->ConnectSocket == INVALID_SOCKET) {
			MessageBoxA(NULL, "Failed to connect to server...","FAILURE", MB_OK);
			WSACleanup();
			system("pause");
			return 4;
	}

	return 0;
}

static void
client_print(const char *msg) {
	static DWORD written;
	HANDLE stdout = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsole(stdout, msg, lstrlen(msg), &written, 0);
}

static void
client_echo(const char *msg) {
	client_print(msg);
	client_print("\n");
}

