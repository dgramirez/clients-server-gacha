#ifndef CSG_CLIENT_USERINPUT_CXX
#define CSG_CLIENT_USERINPUT_CXX

#include "user_input.h"

static ERR_USERINPUT
userInputInit(P_UserInputInit pInitData,
              P_UserInputData pUserInputData)
{
	if (!pInitData)
		return E_USERINPUT_FAIL_NULL_INPUT_INIT;

	if (!pUserInputData)
		return E_USERINPUT_FAIL_NULL_OUTPUT_DATA;

	if (!(pInitData->bufUserInput))
		return E_USERINPUT_FAIL_NULL_USER_INPUT_BUFFER;

	if (!(pInitData->bufRecord))
		return E_USERINPUT_FAIL_NULL_TMP_RECORD_BUFFER;

	if (pInitData->szUserInput != sizeof(UserBuffer) * MAX_USERINPUT)
		return E_USERINPUT_FAIL_SIZE_USER_INPUT_BUFFER_NOT_CORRECT;

	if (pInitData->szRecord != MAX_RECORD_SIZE)
		return E_USERINPUT_FAIL_SIZE_TMP_RECORD_BUFFER_NOT_CORRECT;

	// Simple Setup
	pUserInputData->pUserInput = (P_UserBuffer)pInitData->bufUserInput;
	pUserInputData->pTmpRecord = (PINPUT_RECORD)pInitData->bufRecord;

	return E_USERINPUT_SUCCESS;
}

static ERR_USERINPUT
userInputUpdate(P_UserInputData pUserInputData) {
	// Compiler should optimize this to remove the division on runtime.
	#define MAX_RECORDS_IN_BYTES MAX_RECORD_SIZE / sizeof(INPUT_RECORD)

	BOOL hasWin32Error;
	DWORD szInput;
	DWORD ctRecord;
	DWORD i;
	HANDLE hStdIn;
	PINPUT_RECORD pIR;
	P_UserBuffer pUB;
	KEY_EVENT_RECORD *pKER;

	pUB = pUserInputData->pUserInput;
	ZeroMemory(pUB, MAX_USERINPUT * sizeof(UserBuffer));
	ZeroMemory(pUserInputData->pTmpRecord, MAX_RECORD_SIZE);

	hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	if (hStdIn == INVALID_HANDLE_VALUE)
		return E_USERINPUT_FAIL_GET_STD_HANDLE;

	hasWin32Error = GetNumberOfConsoleInputEvents(hStdIn,
	                                              &ctRecord);
	if (hasWin32Error == 0)
		return 45;

	if (ctRecord == 0)
		return E_USERINPUT_SUCCESS;

	pIR = pUserInputData->pTmpRecord;
	hasWin32Error = ReadConsoleInput(hStdIn,
	                                 pIR,
	                                 MAX_RECORDS_IN_BYTES,
	                                 &ctRecord);

	if (hasWin32Error == 0)
		return E_USERINPUT_FAIL_PEEK_CONSOLE_INPUT;

	i = 0;
	szInput = 0;
	for (; i < ctRecord && szInput < MAX_USERINPUT; ++i) {
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
				pUB[szInput].eventType = USERINPUT_EVENT_ASCII;
				pUB[szInput++].eventKey = (uint8_t)pKER->uChar.AsciiChar;
			}
			#undef VK_KEY_ZERO
			#undef VK_KEY_NINE

			// Note: Key A to Key Z. Sorry for Magic Numbers, Blame msdn
			#define VK_KEY_A 0x41
			#define VK_KEY_Z 0x5A
			else if (pKER->wVirtualKeyCode >= VK_KEY_A &&
				pKER->wVirtualKeyCode <= VK_KEY_Z)
			{
				pUB[szInput].eventType = USERINPUT_EVENT_ASCII;
				pUB[szInput++].eventKey = (uint8_t)pKER->uChar.AsciiChar;
			}
			#undef VK_KEY_A
			#undef VK_KEY_Z

			// Note: Remainder of VK
			else if (pKER->wVirtualKeyCode) {
				if (pKER->wVirtualKeyCode == VK_LEFT)
					pUB[szInput].eventKey |= USERINPUT_KEY_LEFT;

				if (pKER->wVirtualKeyCode == VK_UP)
					pUB[szInput].eventKey |= USERINPUT_KEY_UP;

				if (pKER->wVirtualKeyCode == VK_RIGHT)
					pUB[szInput].eventKey |= USERINPUT_KEY_RIGHT;

				if (pKER->wVirtualKeyCode == VK_DOWN)
					pUB[szInput].eventKey |= USERINPUT_KEY_DOWN;

				if (pKER->wVirtualKeyCode == VK_RETURN)
					pUB[szInput].eventKey |= USERINPUT_KEY_ENTER;

				if (pKER->wVirtualKeyCode == VK_BACK)
					pUB[szInput].eventKey |= USERINPUT_KEY_BACKSPACE;

				if (pKER->wVirtualKeyCode == VK_DELETE)
					pUB[szInput].eventKey |= USERINPUT_KEY_DELETE;

				if (pKER->wVirtualKeyCode == VK_ESCAPE)
					pUB[szInput].eventKey |= USERINPUT_KEY_ESCAPE;

				if (pUB[szInput].eventKey > USERINPUT_KEY_NONE)
					pUB[szInput++].eventType = USERINPUT_EVENT_VK;
			}
	}

	return E_USERINPUT_SUCCESS;
}

static int
user_input_isvk(P_UserBuffer pInput) {
	return pInput->eventType == USERINPUT_EVENT_VK;
}

static int
user_input_isascii(P_UserBuffer pInput) {
	return pInput->eventType == USERINPUT_EVENT_ASCII;
}

static int
user_input_up(P_UserBuffer pInput) {
	return pInput->eventKey & USERINPUT_KEY_UP;
}

static int
user_input_down(P_UserBuffer pInput) {
	return pInput->eventKey & USERINPUT_KEY_DOWN;
}

static int
user_input_left(P_UserBuffer pInput) {
	return pInput->eventKey & USERINPUT_KEY_LEFT;
}

static int
user_input_right(P_UserBuffer pInput) {
	return pInput->eventKey & USERINPUT_KEY_RIGHT;
}

static int
user_input_enter(P_UserBuffer pInput) {
	return pInput->eventKey & USERINPUT_KEY_ENTER;
}

static int
user_input_backspace(P_UserBuffer pInput) {
	return pInput->eventKey & USERINPUT_KEY_BACKSPACE;
}

static int
user_input_delete(P_UserBuffer pInput) {
	return pInput->eventKey & USERINPUT_KEY_DELETE;
}

static int
user_input_escape(P_UserBuffer pInput) {
	return pInput->eventKey & USERINPUT_KEY_ESCAPE;
}

static int
user_input_type(P_UserBuffer pInput, char *buffer) {
	if (pInput->eventType == USERINPUT_EVENT_VK)
		return 0;

	*buffer = pInput->eventKey;
	return 1;
}

#endif // CSG_CLIENT_USERINPUT_CXX

