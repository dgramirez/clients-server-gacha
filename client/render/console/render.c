#include "render.h"

#include <windows.h>

CLIENT_API ERR_RENDER
renderInit(P_XFormRender pRender) {
	BOOL eWin32;
	ERR_RENDER eRender;
	HANDLE hStdOut;
	CONSOLE_CURSOR_INFO cci;
	CONSOLE_SCREEN_BUFFER_INFOEX csbi;

	eRender = renderParamCheck(pRender);
	if (eRender)
		return eRender;

	// TODO: AllocConsole [On the same thread]
	if (pRender->rendData.cbBackMax != 1024 &&
	    pRender->rendData.cbBackMax != 4096)
	{
		return E_REND_FAIL_INVALID_BACK_BUFFER_SIZE;
	}

	if (pRender->rendData.cbBackMax == 1024 &&
	    pRender->rendData.colBack != 64)
	{
		return E_REND_FAIL_INVALID_BACK_BUFFER_COL_64;
	}

	if (pRender->rendData.cbBackMax == 4096 &&
	    pRender->rendData.colBack != 128)
	{
		return E_REND_FAIL_INVALID_BACK_BUFFER_COL_128;
	}

	pRender->pFnMem->memset(pRender->rendData.bufBack,
	                        ' ',
	                        pRender->rendData.cbBack);

	pRender->pFnMem->memset(pRender->hApi,
	                        ' ',
	                        pRender->rendData.cbBack);

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE)
		return E_REND_FAIL_GET_STD_OUTPUT_HANDLE;

	pRender->pFnMem->memset(&cci, 0, sizeof(cci));
	eWin32 = GetConsoleCursorInfo(hStdOut, &cci);
	if (eWin32 == 0)
		return E_REND_FAIL_GET_CONSOLE_CURSOR_INFO;

	cci.bVisible = 0;
	eWin32 = SetConsoleCursorInfo(hStdOut, &cci);
	if (eWin32 == 0)
		return E_REND_FAIL_SET_CONSOLE_CURSOR_INFO;

	pRender->pFnMem->memset(&csbi, 0, sizeof(csbi));
	csbi.cbSize = sizeof(csbi);
	eWin32 = GetConsoleScreenBufferInfoEx(hStdOut, &csbi);
	if (eWin32 == 0)
		return E_REND_FAIL_GET_CONSOLE_SCREEN_BUFFER_INFOEX;

	csbi.dwSize.X = (short)pRender->rendData.colBack;
	csbi.dwSize.Y = csbi.dwSize.X == 64 ? 16 : 32;
	csbi.srWindow.Left = 0;
	csbi.srWindow.Top = 0;
	csbi.srWindow.Right = csbi.dwSize.X;
	csbi.srWindow.Bottom = csbi.dwSize.Y;
	eWin32 = SetConsoleScreenBufferInfoEx(hStdOut, &csbi);
	if (eWin32 == 0)
		return E_REND_FAIL_SET_CONSOLE_SCREEN_BUFFER_INFOEX;

	eWin32 = SetConsoleScreenBufferSize(hStdOut, csbi.dwSize);
	if (eWin32 == 0)
		return E_REND_FAIL_SET_CONSOLE_SCREEN_BUFFER_SIZE;
	
	return E_REND_SUCCESS;
}

CLIENT_API int
render(P_XFormRender pRender) {
	SHORT i;
	SHORT shLastIndex;
	BOOL eWin32;
	ERR_RENDER eRender;
	COORD posDiff;
	DWORD cbWrite;
	DWORD cbWritten;
	HANDLE hStdOut;
	char *bufBack;
	char *bufFront;
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	eRender = renderParamCheck(pRender);
	if (eRender)
		return eRender;

	bufBack = pRender->rendData.bufBack;
	bufFront = pRender->hApi;

	posDiff.X = 0x7FFF;
	posDiff.Y = 0x7FFF;
	for (i = 0; i < pRender->rendData.cbBack; ++i) {
		if (bufBack[i] != bufFront[i]) {
			if (posDiff.X == 0x7FFF)
				posDiff.X = i;

			posDiff.Y = i;
		}
	}

	if (posDiff.X < pRender->rendData.cbBack) {
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hStdOut == INVALID_HANDLE_VALUE)
			return E_REND_FAIL_GET_STD_OUTPUT_HANDLE;

		eWin32 = GetConsoleScreenBufferInfo(hStdOut, &csbi);
		if (eWin32 == 0)
			return E_REND_FAIL_GET_CONSOLE_SCREEN_BUFFER_INFO_PRIOR_WRITE;

		pRender->pFnMem->memset(bufFront,
		                        ' ',
		                        pRender->rendData.cbBack);

		bufFront =(char*)((size_t)bufBack ^ (size_t)bufFront);
		bufBack  =(char*)((size_t)bufBack ^ (size_t)bufFront);
		bufFront =(char*)((size_t)bufBack ^ (size_t)bufFront);
		pRender->rendData.bufBack = bufBack;
		pRender->hApi = bufFront;

		cbWrite = posDiff.Y - posDiff.X + 1;
		shLastIndex = posDiff.Y;
		posDiff.Y = posDiff.X / csbi.dwSize.X;
		posDiff.X = posDiff.X % csbi.dwSize.X;

		eWin32 = SetConsoleCursorPosition(hStdOut, posDiff);
		if (eWin32 == 0)
			return E_REND_FAIL_SET_CONSOLE_CURSOR_POSITION;

		eWin32 = WriteConsoleA(hStdOut,
		                       bufFront + shLastIndex - cbWrite+1,
		                       cbWrite,
		                       &cbWritten,
		                       0);
		if (eWin32 == 0)
			return E_REND_FAIL_WRITE_CONSOLE;

		eWin32 = GetConsoleScreenBufferInfo(hStdOut, &csbi);
		if (eWin32 == 0)
			return E_REND_WARN_GET_CONSOLE_SCREEN_BUFFER_INFO_AFTER_WRITE;

		cbWrite = csbi.dwSize.Y * csbi.dwSize.X - shLastIndex;
		eWin32 = FillConsoleOutputCharacter(hStdOut,
		                                    (TCHAR)' ',
		                                    cbWrite,
		                                    csbi.dwCursorPosition,
		                                    &cbWritten);

		if (eWin32 == 0)
			return E_REND_WARN_FILL_CONSOLE_OUTPUT_CHARACTER;

		eWin32 = FillConsoleOutputAttribute(hStdOut,
		                                    csbi.wAttributes,
		                                    cbWrite,
		                                    csbi.dwCursorPosition,
		                                    &cbWritten);
		if (eWin32 == 0)
			return E_REND_WARN_FILL_CONSOLE_OUTPUT_ATTRIBUTE;
	}

	return E_REND_SUCCESS;
}

CLIENT_API int
renderCleanup(P_XFormRender pRender) {
	ERR_RENDER eRender;

	eRender = renderParamCheck(pRender);
	if (eRender)
		return eRender;

	pRender->pFnMem->memset(pRender->rendData.bufBack,
	                        0,
	                        pRender->rendData.cbBack);

	pRender->pFnMem->memset(pRender->hApi,
	                        0,
	                        pRender->rendData.cbBack);

	// TODO: FreeConsole [On the same thread]
	return E_REND_SUCCESS;
}

static ERR_RENDER
renderParamCheck(P_XFormRender pRender) {
	if (!pRender)
		return E_REND_FAIL_NULL_PARAM_RENDER;

	if (!(pRender->pFnMem))
		return E_REND_FAIL_NULL_RENDER_DATA_FN_MEM;

	if (!(pRender->pFnMem->memset))
		return E_REND_FAIL_NULL_FN_MEM_MEMSET;

	if (!(pRender->rendData.bufBack))
		return E_REND_FAIL_NULL_RENDER_DATA_BACK_BUFFER;

	if (!(pRender->hApi))
		return E_REND_FAIL_NULL_API_EXPECT_FRONT_BUFFER;

	if (pRender->cbApi != pRender->rendData.cbBack)
		return E_REND_FAIL_INVALID_API_SIZE_EXPECT_SIZE_OF_BACK_BUFFER;

	return E_REND_SUCCESS;
}

