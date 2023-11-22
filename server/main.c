#include "rng_xoroshiro128plus.hxx"
#include <time.h>

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include "sqlite/sqlite3.c"

#define MSGLEN 512
#define DEFAULT_PORT "27015"

typedef struct {
	WSADATA WSAData;
	SOCKET ListenSocket, ClientSocket;
	struct addrinfo *BindInfo;
	char msg[MSGLEN];
} PlatformINet, *PPlatformINet;

static int
server_init(PPlatformINet pinet);

static int
server_listen(PPlatformINet pinet);

static int
server_authenticate(PPlatformINet pinet);

static int
server_shutdown(PPlatformINet pinet);

static void
server_print(const char *msg);

static void
server_echo(const char *msg);

int APIENTRY
WinMain(HINSTANCE hInst,
        HINSTANCE hInstPrev,
        PSTR cmdline,
        int cmdshow)
{
	PlatformINet inet = {0};
	AllocConsole();

	int run = 1;
	while (run) {
		if (server_init(&inet)) {
			run = 0;
			continue;
		}

		if (server_listen(&inet)) {
			run = 0;
			continue;
		}

		if (server_authenticate(&inet)) {
			server_shutdown(&inet);
			continue;
		}

		int communicate = 1;
		uint64_t offset = 0xBEEFDEAD;
		do {
			ZeroMemory(inet.msg, MSGLEN);
			wsprintfA(inet.msg, "Welcome to the Gacha System! Please Select 0, 1 or 2 to do your random pulls!");
			send(inet.ClientSocket, inet.msg, lstrlen(inet.msg), 0);

			ZeroMemory(inet.msg, MSGLEN);
			recv(inet.ClientSocket, inet.msg, MSGLEN, 0);

			if (inet.msg[0] == '0') {
				run = 0;
				communicate = 0;
			}
			else if (inet.msg[0] == '1') {
				uint64_t state64 = (uint64_t)time(0) + offset;
				uint64_t state128[2];
				state128[0] = splitmix64_next(&state64);
				state128[1] = splitmix64_next(&state64);

				int rnum = (xoroshiro128plus_next(state128) >> 4) % 256;

				ZeroMemory (inet.msg, MSGLEN);
				wsprintfA(inet.msg, "Your Random Pull: %d", rnum);

				send(inet.ClientSocket, inet.msg, (int)lstrlen(inet.msg), 0);
				offset += 1;
			}
			else if (inet.msg[0] == '2') {
				ZeroMemory(inet.msg, MSGLEN);
				int len = 0;
				unsigned long long rnum[10] = { 0 };

				uint64_t state64 = (uint64_t)time(0) + offset;
				uint64_t state128[2];
				state128[0] = splitmix64_next(&state64);
				state128[1] = splitmix64_next(&state64);

				for (int i = 0; i < 10; ++i) {
					rnum[i] = (xoroshiro128plus_next(state128) >> 4) % 256;
					wsprintfA(inet.msg + len, "Pull #%d: %lu\n", i, rnum[i]);
					len = lstrlen(inet.msg);
				}

				send(inet.ClientSocket, inet.msg, len, 0);
				offset += 1;
			}
			else {
				ZeroMemory (inet.msg, MSGLEN);
				wsprintfA(inet.msg, "Console Reset...");
				send(inet.ClientSocket, inet.msg, (int)strlen(inet.msg), 0);
			}

		} while (run && communicate);
	}

	sizeof(hInst);
	sizeof(hInstPrev);
	sizeof(cmdline);
	sizeof(cmdshow);

	FreeConsole();
	return 0;
}

static int
server_init(PPlatformINet pinet) {
	int iResult = WSAStartup(MAKEWORD(2,2),
	                         &(pinet->WSAData));

	if (iResult) {
		MessageBoxA(NULL, "WSA Startup Failed to Initialize","FAILURE", MB_OK);
		return 1;
	}

	struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL,
	                      DEFAULT_PORT,
	                      &hints,
	                      &(pinet->BindInfo));

	if (iResult) {
		MessageBoxA(NULL, "Failed to get the address information","FAILURE", MB_OK);
		WSACleanup();
		return 2;
	}

	pinet->ListenSocket = socket(pinet->BindInfo->ai_family,
	                             pinet->BindInfo->ai_socktype,
	                             pinet->BindInfo->ai_protocol);

	if (pinet->ListenSocket == INVALID_SOCKET) {
		MessageBoxA(NULL, "Failed to create listen socket","FAILURE", MB_OK);
		freeaddrinfo(pinet->BindInfo);
		WSACleanup();
		return 3;
	}

	iResult = bind(pinet->ListenSocket,
	               pinet->BindInfo->ai_addr,
	               (int)pinet->BindInfo->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		MessageBoxA(NULL, "Failed to bind the socket","FAILURE", MB_OK);
		freeaddrinfo(pinet->BindInfo);
		closesocket(pinet->ListenSocket);
		WSACleanup();
		return 4;
	}

	freeaddrinfo(pinet->BindInfo);
	return 0;
}

static int
server_listen(PPlatformINet pinet) {
	server_echo("Listening for a connection on port #: " DEFAULT_PORT);

	int iResult = listen(pinet->ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		ZeroMemory(pinet->msg, MSGLEN);
		wsprintfA(pinet->msg, "Failed to listen for a connection. Error Code: %d", WSAGetLastError());
		server_echo(pinet->msg);

		closesocket(pinet->ListenSocket);
		WSACleanup();
		system("pause");
		return 5;
	}

	pinet->ClientSocket = accept(pinet->ListenSocket, NULL, NULL);
	if (pinet->ClientSocket == INVALID_SOCKET) {
		ZeroMemory(pinet->msg, MSGLEN);
		wsprintfA(pinet->msg, "Failure to accept from the listen socket. Error Code: %d", WSAGetLastError());
		server_echo(pinet->msg);

		closesocket(pinet->ListenSocket);
		WSACleanup();
		system("pause");
		return 6;
	}

	closesocket(pinet->ListenSocket);
	return 0;
}

static int
server_authenticate(PPlatformINet pinet) {
	/* Client must send the authentication information upon connecting */

	int wrong = 0;
	while (wrong < 3) {
		int iResult = recv(pinet->ClientSocket, pinet->msg, MSGLEN, 0);
		if (!iResult) {
			server_echo("Client decided to close connection. Restarting...");
			return -1;
		}
		else if (iResult < 0) {
			ZeroMemory(pinet->msg, MSGLEN);
			wsprintfA(pinet->msg, "Failed to receive message! aborting!", wrong);
			server_echo(pinet->msg);
			return 7;
		}

		/* Major Note: This is NOT how you setup a password lol. */
		const char *auth_password = "abcde";
		if (memcmp(auth_password, pinet->msg, 5)) {
			wrong++;
			ZeroMemory(pinet->msg, MSGLEN);
			wsprintfA(pinet->msg, "Password Failure #%d", wrong);
			server_echo(pinet->msg);

			const char *msg;
			if (wrong == 3)
				msg = "2";
			else
				msg = "1";

			send(pinet->ClientSocket, msg, (int)lstrlen(msg), 0);
		}
		else
			break;
	}

	if (wrong >= 3) {
		const char *msg = "Too Many Failed Attempts! Disconnecting!";
		send(pinet->ClientSocket, msg, (int)lstrlen(msg), 0);
		server_echo(msg);
		return 8;
	}

	send(pinet->ClientSocket, "0", (int)lstrlen("0"), 0);

	server_echo("Authentication Completed! Continuing...");
	return 0;
}

static int
server_shutdown(PPlatformINet pinet) {
	shutdown(pinet->ClientSocket, SD_SEND);
	closesocket(pinet->ClientSocket);
	WSACleanup();
	return 0;
}

static void
server_print(const char *msg) {
	static DWORD written;
	HANDLE stdoutput = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsole(stdoutput, msg, lstrlen(msg), &written, 0);
}

static void
server_echo(const char *msg) {
	server_print(msg);
	server_print("\n");
}

