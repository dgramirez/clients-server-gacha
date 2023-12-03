#include "console.h"
#include "defines.h"

CLIENT_API int
expSurfaceInit(P_InitSurface pInit, P_SystemFn pSysFn) {
	BOOL eWin32;
	HANDLE hStdOut;
	CONSOLE_CURSOR_INFO cci;
	CONSOLE_SCREEN_BUFFER_INFOEX csbi;

	// Create Surface
	eWin32 = AllocConsole();
	if (eWin32 == 0)
		return E_SURFACE_FAIL_CREATE_WINDOW;
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// Remove Cursor
	pSysFn->memzero(&cci, sizeof(CONSOLE_CURSOR_INFO));
	eWin32 = GetConsoleCursorInfo(hStdOut, &cci);
	if (eWin32 == 0) {
		FreeConsole();
		return E_SURFACE_FAIL_GET_CONSOLE_CURSOR_INFO;
	}

	cci.bVisible = 0;
	eWin32 = SetConsoleCursorInfo(hStdOut, &cci);
	if (eWin32 == 0){
		FreeConsole();
		return E_SURFACE_FAIL_SET_CONSOLE_CURSOR_INFO;
	}

	// Setup Console Buffer
	pSysFn->memzero(&csbi, sizeof(CONSOLE_SCREEN_BUFFER_INFOEX));
	csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	eWin32 = GetConsoleScreenBufferInfoEx(hStdOut, &csbi);
	if (eWin32 == 0) {
		FreeConsole();
		return E_SURFACE_FAIL_GET_CONSOLE_SCREEN_BUFFER_INFO_EX;
	}

	csbi.dwSize.X = pInit->posW;
	csbi.dwSize.Y = pInit->posH;
	csbi.srWindow.Left = pInit->posX;
	csbi.srWindow.Right = csbi.srWindow.Left + csbi.dwSize.X;
	csbi.srWindow.Top = pInit->posY;
	csbi.srWindow.Bottom = csbi.srWindow.Top + csbi.dwSize.Y;
	eWin32 = SetConsoleScreenBufferInfoEx(hStdOut, &csbi);
	if (eWin32 == 0) {
		FreeConsole();
		return E_SURFACE_FAIL_SET_CONSOLE_SCREEN_BUFFER_INFO_EX;
	}

	pInit->out_hProcess = GetModuleHandle(0);
	pInit->out_hWindow = hStdOut;
	return E_SURFACE_SUCCESS;
}

CLIENT_API int
expSurfaceInput(P_MemoryObj pBufInput,
                P_BufferObj pBufTmp,
				P_MutexObj pMtxObj,
				P_SystemFn pSysFn)
{
	#define MAX_RECORDS (DWORD)(pBufTmp->cbAddr / sizeof(INPUT_RECORD))
	HANDLE hStdIn;
	P_UserBuffer pUB;
	int hasThread;
	BOOL eWin32;
	DWORD i;
	DWORD ctRecord;
	PINPUT_RECORD pIR;
	KEY_EVENT_RECORD *pKER;
	DWORD ctInput;

	pUB = (P_UserBuffer)pBufInput->vpAddr;
	hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	eWin32 = GetNumberOfConsoleInputEvents(hStdIn,
	                                       &ctRecord);

	if (ctRecord <= 0)
		return 0;

	pIR = (PINPUT_RECORD)pBufTmp->vpAddr;
	pSysFn->memzero(pIR, pBufTmp->cbAddr);
	eWin32 = ReadConsoleInput(hStdIn, pIR, MAX_RECORDS, &ctRecord);

	hasThread = pSysFn->mtx_lock(pMtxObj->vpMtx, pMtxObj->vpCV, 1);
	if (!hasThread)
		return -1;

	i = 0;
	ctInput = (DWORD)(pBufInput->allocAddr >> 3);
	for (; i < ctRecord; ++i) {
		if (pIR[i].EventType != KEY_EVENT)
			continue;

		pKER = &(pIR[i].Event.KeyEvent);
		if (pKER->wRepeatCount > 1)
			continue;

		if (!pKER->bKeyDown)
			continue;

		// Note: Key 0 to Key 9. Sorry for Magic Numbers, Blame msdn
		#define VK_KEY_ZERO 0x30
		#define VK_KEY_NINE 0x39
		if (pKER->wVirtualKeyCode >= VK_KEY_ZERO &&
			pKER->wVirtualKeyCode <= VK_KEY_NINE)
		{
			if ((ctInput << 3) >= pBufInput->cbAddr) {
				inputPop(pBufInput, pSysFn);
				ctInput--;
			}

			pUB[ctInput].eventType = USERINPUT_EVENT_ASCII;
			pUB[ctInput++].eventKey = (uint8_t)pKER->uChar.AsciiChar;
		}
		#undef VK_KEY_ZERO
		#undef VK_KEY_NINE

		// Note: Key A to Key Z. Sorry for Magic Numbers, Blame msdn
		#define VK_KEY_A 0x41
		#define VK_KEY_Z 0x5A
		else if (pKER->wVirtualKeyCode >= VK_KEY_A &&
			pKER->wVirtualKeyCode <= VK_KEY_Z)
		{
			if ((ctInput << 3) >= pBufInput->cbAddr) {
				inputPop(pBufInput, pSysFn);
				ctInput--;
			}

			pUB[ctInput].eventType = USERINPUT_EVENT_ASCII;
			pUB[ctInput++].eventKey = (uint8_t)pKER->uChar.AsciiChar;
		}
		#undef VK_KEY_A
		#undef VK_KEY_Z

		// Note: Remainder of VK
		else if (pKER->wVirtualKeyCode) {
			if (pKER->wVirtualKeyCode == VK_LEFT)
				pUB[ctInput].eventKey |= USERINPUT_KEY_LEFT;

			if (pKER->wVirtualKeyCode == VK_UP)
				pUB[ctInput].eventKey |= USERINPUT_KEY_UP;

			if (pKER->wVirtualKeyCode == VK_RIGHT)
				pUB[ctInput].eventKey |= USERINPUT_KEY_RIGHT;

			if (pKER->wVirtualKeyCode == VK_DOWN)
				pUB[ctInput].eventKey |= USERINPUT_KEY_DOWN;

			if (pKER->wVirtualKeyCode == VK_RETURN)
				pUB[ctInput].eventKey |= USERINPUT_KEY_ENTER;

			if (pKER->wVirtualKeyCode == VK_BACK)
				pUB[ctInput].eventKey |= USERINPUT_KEY_BACKSPACE;

			if (pKER->wVirtualKeyCode == VK_DELETE)
				pUB[ctInput].eventKey |= USERINPUT_KEY_DELETE;

			if (pKER->wVirtualKeyCode == VK_ESCAPE)
				pUB[ctInput].eventKey |= USERINPUT_KEY_ESCAPE;

			if (pUB[ctInput].eventKey > USERINPUT_KEY_NONE) {
				if ((ctInput << 3) >= pBufInput->cbAddr) {
					inputPop(pBufInput, pSysFn);
					ctInput--;
				}

				pUB[ctInput++].eventType = USERINPUT_EVENT_VK;
			}
		}


	}
	pBufInput->allocAddr = ctInput << 3;
	pSysFn->mtx_unlock(pMtxObj->vpMtx, pMtxObj->vpCV);

	#undef MAX_RECORDS
	return 0;
}

CLIENT_API int
expSurfaceUpdate() {
	// No need to update the surface.
	return 0;
}

CLIENT_API int
expSurfaceShutdown() {
	FreeConsole();
	return 0;
}

static int
inputPop(P_MemoryObj pMem, P_SystemFn pSysFn) {
	pSysFn->memcpy(pMem->vpAddr,
	               (uint64_t*)(pMem->vpAddr) + 1,
	               pMem->cbAddr - 8);
	return 0;
}

