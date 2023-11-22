#ifndef CSG_CLIENT_RENDERER_CXX
#define CSG_CLIENT_RENDERER_CXX

#include "renderer.h"

#include <stdio.h>
#include <windows.h>

static ERR_RENDERER
rendererInit(P_RenderInit pInitData,
             P_RenderData pRenderData)
{
	BOOL hasWin32Error;
	HANDLE hStdOut;
	CONSOLE_CURSOR_INFO cci = {0};
	CONSOLE_SCREEN_BUFFER_INFOEX csbi = {0};

	// Parameter Check
	if (!pInitData)
		return E_REND_FAIL_NULL_RENDERER_INIT;
	if (!pRenderData)
		return E_REND_FAIL_NULL_RENDERER_DATA;
	if (!(pInitData->pp_bufConsoleText))
		return E_REND_FAIL_NULL_TEXT_BUFFERS;
	if (pInitData->szConsoleText != MAX_RENDERING_SIZE << 1)
		return E_REND_FAIL_TEXT_BUFFER_SIZE_NOT_2X_MAX_RENDERING_SIZE;

	// Renderer Setup (Double Buffering)
	pRenderData->szBack = pInitData->szConsoleText >> 1;
	pRenderData->szFront = pRenderData->szBack;

	pRenderData->p_bufBack = pInitData->pp_bufConsoleText;
	pRenderData->bufFront = *(pRenderData->p_bufBack) +
	                          pRenderData->szBack;

	RtlFillMemory(*(pRenderData->p_bufBack), pRenderData->szBack, ' ');
	RtlFillMemory(pRenderData->bufFront, pRenderData->szFront, ' ');

	// Console Setup
	hasWin32Error = AllocConsole();
	if (hasWin32Error == 0)
		return E_REND_FAIL_ALLOC_CONSOLE;

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE)
		return E_REND_FAIL_GET_STD_OUTPUT_HANDLE;

	hasWin32Error = GetConsoleCursorInfo(hStdOut, &cci);
	if (hasWin32Error == 0)
		return E_REND_FAIL_GET_CONSOLE_CURSOR_INFO;

	cci.bVisible = (BOOL)pInitData->isCursorVisible;

	hasWin32Error = SetConsoleCursorInfo(hStdOut, &cci);
	if (hasWin32Error == 0)
		return E_REND_FAIL_SET_CONSOLE_CURSOR_INFO;

	// Setup Buffer Size (64 x 16 = 1024)
	csbi.cbSize=sizeof(csbi);
	GetConsoleScreenBufferInfoEx(hStdOut, &csbi);
	csbi.dwSize.X = 64;
	csbi.dwSize.Y = 16;
	csbi.srWindow.Left = 0;
	csbi.srWindow.Right = csbi.dwSize.X;
	csbi.srWindow.Top = 0;
	csbi.srWindow.Bottom = csbi.dwSize.Y;
	SetConsoleScreenBufferInfoEx(hStdOut, &csbi);

	SetConsoleScreenBufferSize(hStdOut, csbi.dwSize);

	return E_REND_SUCCESS;
}

static ERR_RENDERER
render(P_RenderData pRenderData) {
	#define _MAX_SHORT 0x7FFF

	SHORT i;
	SHORT shLastIndex;
	BOOL hasWin32Error;
	COORD posDiff;
	DWORD szWrite;
	DWORD szWritten;
	HANDLE hStdOut;
	char *bufBack;
	char *bufFront;
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	bufBack = *(pRenderData->p_bufBack);
	bufFront = pRenderData->bufFront;

	posDiff.X = _MAX_SHORT;
	posDiff.Y = _MAX_SHORT;

	// Get starting and ending points for diff
	//      NOTE: This can be optimized via SIMD intrinsics.
	for (i = 0; i < pRenderData->szBack; ++i) {
		if (bufBack[i] != bufFront[i]) {
			if (posDiff.X == _MAX_SHORT)
				posDiff.X = i;

			posDiff.Y = i;
		}
	}

	if (posDiff.X < pRenderData->szBack) {
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hStdOut == INVALID_HANDLE_VALUE)
			return E_REND_FAIL_GET_STD_OUTPUT_HANDLE;

		hasWin32Error = GetConsoleScreenBufferInfo(hStdOut, &csbi);
		if (hasWin32Error == 0)
			return E_REND_FAIL_GET_CONSOLE_SCREEN_BUFFER_INFO;

		// Swap the buffers
		bufFront =(char*)((size_t)bufBack ^ (size_t)bufFront);
		bufBack  =(char*)((size_t)bufBack ^ (size_t)bufFront);
		bufFront =(char*)((size_t)bufBack ^ (size_t)bufFront);

		pRenderData->bufFront = bufFront;
		*(pRenderData->p_bufBack) = bufBack;
		RtlFillMemory(bufBack, pRenderData->szBack, ' ');

		// Write to Console
		szWrite = posDiff.Y - posDiff.X + 1;
		shLastIndex = posDiff.Y;
		posDiff.Y = posDiff.X / csbi.dwSize.X;
		posDiff.X = posDiff.X % csbi.dwSize.X;

		SetConsoleCursorPosition(hStdOut, posDiff);
		hasWin32Error = WriteConsoleA(hStdOut,
		                            bufFront + shLastIndex - szWrite+1,
		                            szWrite,
		                            &szWritten,
		                            0);
		if (hasWin32Error == 0)
			return E_REND_FAIL_WRITE_CONSOLE;

		// Clear the remainder of the console. CSBI for current cursor pos
		posDiff.X = 0;
		posDiff.Y = 0;

		hasWin32Error = GetConsoleScreenBufferInfo(hStdOut, &csbi);
		if (hasWin32Error == 0) {
			SetConsoleCursorPosition(hStdOut, posDiff);
			return E_REND_WARN_GET_CONSOLE_SCREEN_BUFFER_INFO_AFTER_WRITE;
		}

		szWrite = csbi.dwSize.Y * csbi.dwSize.X - shLastIndex;
		hasWin32Error = FillConsoleOutputCharacter(hStdOut,
		                                           (TCHAR)' ',
		                                           szWrite,
		                                           csbi.dwCursorPosition,
		                                           &szWritten);

		if (hasWin32Error == 0) {
			SetConsoleCursorPosition(hStdOut, posDiff);
			return E_REND_WARN_FILL_CONSOLE_OUTPUT_CHARACTER;
		}

		hasWin32Error = FillConsoleOutputAttribute(hStdOut,
		                                           csbi.wAttributes,
		                                           szWrite,
		                                           csbi.dwCursorPosition,
		                                           &szWritten);

		SetConsoleCursorPosition(hStdOut, posDiff);
		if (hasWin32Error == 0)
			return E_REND_WARN_FILL_CONSOLE_OUTPUT_ATTRIBUTE;
	}

	#undef _MAX_SHORT
	return E_REND_SUCCESS;
}

static ERR_RENDERER
rendererClose(P_RenderData pRenderData) {
	BOOL hasWin32Error;

	RtlFillMemory(*(pRenderData->p_bufBack), pRenderData->szBack, 0);
	RtlFillMemory(pRenderData->bufFront, pRenderData->szFront, 0);

	hasWin32Error = FreeConsole();
	if (hasWin32Error)
		return E_REND_FAIL_FREE_CONSOLE;

	return E_REND_SUCCESS;
}

#endif // CSG_CLIENT_RENDERER_CXX

