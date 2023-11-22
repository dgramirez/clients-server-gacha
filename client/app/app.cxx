#ifndef CSG_CLIENT_APP_CXX
#define CSG_CLIENT_APP_CXX

#include "app.h"

// Input Selection
static int
appInit(void* vpAppData) {
	P_AppData pApp;

	pApp = (P_AppData)vpAppData;
	RtlFillMemory(pApp->strField1, MAX_FIELD_INDEX, ' ');
	RtlFillMemory(pApp->strField2, MAX_FIELD_INDEX, ' ');
	pApp->ctField1 = 0;
	pApp->ctField2 = 0;

	pApp->bState = APP_STATE_START;
	pApp->bAction = 0;
	pApp->bMinSelect = 0;
	pApp->bMaxSelect = 0;
	pApp->bCurSelect = 0;
	pApp->bFieldSelect = APP_FIELD_FIRST;
	pApp->hasSelected = 0;
	pApp->hasEscaped = 0;

	return 0;
}

static int
appInputAscii(P_AppData pApp, P_UserBuffer pUserBuffer) {
	char *strField;
	size_t *p_cbField;

	if (pApp->bFieldSelect == APP_FIELD_FIRST) {
		strField = pApp->strField1;
		p_cbField = &(pApp->ctField1);
	}
	else if (pApp->bFieldSelect == APP_FIELD_SECOND) {
		strField = pApp->strField2;
		p_cbField = &(pApp->ctField2);
	}
	else
		return 0;

	for (int i = 0; i < MAX_USERINPUT; ++i) {
		if (pUserBuffer[i].eventType == USERINPUT_IGNORE)
			break;

		if (user_input_isvk(pUserBuffer + i)) {
			if (user_input_enter(pUserBuffer + i))
				pApp->hasSelected = 1;
			if (user_input_escape(pUserBuffer + i))
				pApp->hasEscaped = 1;
			else if (user_input_backspace(pUserBuffer + i)) {
				if (*(p_cbField) > 0)
					strField[--(*(p_cbField))] = ' ';
			}
			else if (user_input_delete(pUserBuffer + i)) {
				if (*(p_cbField) > 0)
					strField[--(*(p_cbField))] = ' ';
			}
			continue;
		}

		// Note: From this point, it MUST be an ascii character.
		if (!user_input_isascii(pUserBuffer + i))
			continue;

		if (*(p_cbField) < MAX_FIELD_INDEX)
			*(p_cbField) += user_input_type(pUserBuffer + i,
			                               strField + *(p_cbField));
	}

	return 0;
}

static int
appInputSelect(P_AppData pApp, P_UserBuffer pUserBuffer) {
	for (int i = 0; i < MAX_USERINPUT; ++i) {
		if (pUserBuffer[i].eventType == USERINPUT_IGNORE)
			break;

		if (!user_input_isvk(pUserBuffer + i))
			continue;

		if (user_input_escape(pUserBuffer + i)) 
			pApp->hasEscaped = 1;
		else if (user_input_backspace(pUserBuffer + i))
			pApp->hasEscaped = 1;
		else if (user_input_delete(pUserBuffer + i))
			pApp->hasEscaped = 1;
		else if (user_input_enter(pUserBuffer + i))
			pApp->hasSelected = 1;
		else if (user_input_up(pUserBuffer + i))
			pApp->bCurSelect--;
		else if (user_input_down(pUserBuffer + i))
			pApp->bCurSelect++;

		if (pApp->bCurSelect >= pApp->bMaxSelect)
			pApp->bCurSelect = pApp->bMinSelect;
		if (pApp->bCurSelect < pApp->bMinSelect)
			pApp->bCurSelect = pApp->bMaxSelect;
	}

	return 0;
}

// Updates
static int
appUpdate(void* vpAppData,
          P_PlatformData pData)
{
	P_AppData pApp;
	const char *msg;

	pApp = (P_AppData)vpAppData;
	pApp->hasSelected = 0;
	pApp->hasEscaped = 0;

	switch(pApp->bState) {
		case APP_STATE_START: {
			appInputAscii(pApp, pData->userInput);
			appLogin(pApp);

			if (pApp->bState == APP_STATE_CONNECT) {
				msg = ";127.0.0.1;20715;";
				RtlCopyMemory(pData->bufMsgSend, msg, lstrlen(msg));
			}

			appRenderLogin(pApp,
			               pData->bufBack,
			               *pData->szBack);
		} break;
		case APP_STATE_CONNECT: {
			// No Status
			if (pData->bufMsgRecv[0] == 0) {
				// TODO: Make the dots loop 1 2 3 every second.
				msg = "Connecting . . .";
				appWriteLine(msg, 16, pData->bufBack);
			}

			// Connected!
			if (pData->bufMsgRecv[0] == 1) {
				// TODO: Make the dots loop 1 2 3 Every Second.
				msg = "Connected!";
				appWriteLine(msg, 12, pData->bufBack);

				// TODO: Lets not use sleep. Make a timer.
				Sleep (5);
				appInit(pApp);
			}

			// Not Connected D:
			if (pData->bufMsgRecv[0] == -1) {
				// TODO: Make the dots loop 1 2 3 Every Second.
				msg = "Failed to connect. . .";
				appWriteLine(msg, 22, pData->bufBack);

				// TODO: Lets not use sleep. Make a timer.
				Sleep (5);
				appInit(pApp);
			}
		} break;
	}

	return 0;
}

static int
appLogin(P_AppData pApp) {
	if (pApp->hasEscaped) {
		switch(pApp->bAction) {
			case APP_ACTION_START_USERNAME: {
				ExitProcess(0); // TODO: This should exit the program
			} break;

			case APP_ACTION_START_PASSWORD: {
					pApp->bAction = APP_ACTION_START_USERNAME;
					pApp->bFieldSelect = APP_FIELD_FIRST;
			}
		}
	}

	if (pApp->hasSelected && !pApp->hasEscaped) {
		switch(pApp->bAction) {
			case 0: {
				pApp->bAction = APP_ACTION_START_PASSWORD;
				pApp->bFieldSelect = APP_FIELD_SECOND;
			} break;
			case 1: {
				pApp->bState = APP_STATE_CONNECT;
				return 0;
			}
		}
	}

	return 0;
}

// Render
static int
appRenderLogin(P_AppData pApp, char *bufBack, size_t szBack) {
	const char *cstrMsg;

	if (pApp->hasEscaped) {
		RtlFillMemory(bufBack, szBack, ' ');
		RtlFillMemory(pApp->strField2, MAX_FIELD_INDEX, 0);
		pApp->ctField2 = 0;
	}

	cstrMsg = "Please Log In:";
	appWriteLine(cstrMsg, 14, bufBack);

	cstrMsg = "Username: ";
	appWriteLine(cstrMsg, 10, bufBack + 64);
	appWriteLine(pApp->strField1, pApp->ctField1, bufBack + 74);


	if (pApp->bAction == 1) {
		cstrMsg = "Password: ";
		appWriteLine(cstrMsg, 10, bufBack + 128);
		appWriteLine(pApp->strField2, pApp->ctField2, bufBack + 138);
	}

	return 0;
}

static int
appWriteLine(const char *cstrMsg, size_t ctMsg, char *bufBack) {
	size_t szLeft;

	if (ctMsg >= 64)
		return -1;

	// Wrap Detection (This is invalid)
	szLeft = 64 - ((size_t)bufBack & 63);
	if (ctMsg > szLeft)
		return -1;

	RtlCopyMemory(bufBack, cstrMsg, ctMsg);
	RtlFillMemory(bufBack + ctMsg, szLeft - ctMsg, ' ');
	return 0;
}

#endif // CSG_CLIENT_APP_CXX

