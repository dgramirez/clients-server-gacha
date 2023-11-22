#include "app.cxx"
#include "inet.cxx"
#include "renderer.cxx"
#include "user_input.cxx"

#include <windows.h>

#include "main.h"
int APIENTRY
WinMain(HINSTANCE hInst,
        HINSTANCE hInstPrev,
        PSTR cmdline,
        int cmdshow)
{
	size_t szHeap;
	UserInputData inputData;
	RenderData renderData;
	RenderInit initRenderData;
	UserInputInit initUserInput;
	InetInit initInet;
	Win32Data win32Data;

	// Platform Data Setup
	szHeap = KB(4);
	win32Data.pApp = VirtualAlloc(0,
	                          szHeap,
	                          MEM_RESERVE | MEM_COMMIT,
	                          PAGE_READWRITE);

	initUserInput.bufRecord =
		(PINPUT_RECORD)((char*)(win32Data.pApp) + 512);

	win32Data.platformData.bufMsgSend =
		(char*)(initUserInput.bufRecord) + 512;

	// TODO: Let the INet Setup do this.
	win32Data.platformData.bufMsgRecv =
		win32Data.platformData.bufMsgSend + 256;

	win32Data.pTmp = win32Data.platformData.bufMsgSend + 512;
	win32Data.platformData.bufBack = (char*)(win32Data.pTmp) + 512;
	win32Data.appUpdate = appUpdateStub;

	// Setup Console
	initRenderData.pp_bufConsoleText = &(win32Data.platformData.bufBack);
	initRenderData.szConsoleText = KB(2);
	initRenderData.isCursorVisible = 0;
	rendererInit(&initRenderData, &renderData);

	win32Data.platformData.szBack = &(renderData.szBack);

	// Setup User Input
	initUserInput.bufUserInput = (void*)(win32Data.platformData.userInput);
	initUserInput.szUserInput = sizeof(UserBuffer) * MAX_USERINPUT;
	initUserInput.szRecord = MAX_RECORD_SIZE;
	userInputInit(&initUserInput, &inputData);

	// Setup Network
	initInet.bufMsgInet = win32Data.platformData.bufMsgSend;
	initInet.cbMsgInet = 512;
	networkInit(&initInet);

	// Note: Loading Application (Presume Successful)
	RtlFillMemory(win32Data.platformData.bufBack, KB(1), ' ');
	win32Data.appUpdate = appUpdate;
	appInit(win32Data.pApp);

	int run = 1;
	while (run) {
		// System Inputs
		userInputUpdate(&inputData);

		// Application Update
		win32Data.appUpdate(win32Data.pApp, &(win32Data.platformData));

		// System Render
		render(&renderData);

		// Sleep (To not overwork the cpu)
		Sleep(1);
	}
	FreeConsole();

	NO_REF(hInst);
	NO_REF(hInstPrev);
	NO_REF(cmdline);
	NO_REF(cmdshow);

	return 0;
}

// Stubs
static int
appUpdateStub(void* vpAppData,
                P_PlatformData pPlatformData)
{
	NO_REF(vpAppData);
	NO_REF(pPlatformData);
	return 0;
}

