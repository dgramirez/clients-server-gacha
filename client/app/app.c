#ifndef CSG_CLIENT_APP_CXX
#define CSG_CLIENT_APP_CXX

#include "app.h"

// DLL Symbol Exports
CLIENT_API int
appInit(void* vpAppData, P_FnMemory pFnMem) {
	P_AppData pApp;

	pApp = (P_AppData)vpAppData;
	pFnMem->memset(pApp->strField1, ' ', MAX_FIELD_INDEX);
	pFnMem->memset(pApp->strField2, ' ', MAX_FIELD_INDEX);
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

CLIENT_API int
appUpdate(void* vpAppData,
          P_PlatformData pData)
{
	P_AppData pApp;
	const char *msg;
	size_t isBad = 0;

	pApp = (P_AppData)vpAppData;
	pApp->hasSelected = 0;
	pApp->hasEscaped = 0;

	switch(pApp->bState) {
		case APP_STATE_START: {
			appInputAscii(pApp, pData->userInput);
			isBad = appLogin(pApp);
			if (isBad) {
				pData->flags = 0;
				return -1;
			}

			if (pApp->bState == APP_STATE_CONNECT) {
				msg = ";127.0.0.1;20715;";
				pData->memFn.memcpy(pData->inet.bufMsgSend, msg, 17);
			}

			appRenderLogin(pApp,
			               &(pData->render),
			               &(pData->memFn));
		} break;
		case APP_STATE_CONNECT: {
			// No Status
			if (pData->inet.bufMsgRecv[0] == 0) {
				// TODO: Make the dots loop 1 2 3 every second.
				msg = "Connecting . . .";
				appWriteLine(msg,
				             16,
				             &(pData->render),
				             0,
				             &(pData->memFn));
			}

			// Connected!
			if (pData->inet.bufMsgRecv[0] == 1) {
				// TODO: Make the dots loop 1 2 3 Every Second.
				msg = "Connected!";
				appWriteLine(msg,
				             12,
				             &(pData->render),
				             0,
				             &(pData->memFn));

				// TODO: Make a timer.
				for (int i = 0; i < 10000000; ++i)
					i = i;

				appInit(pApp, &(pData->memFn));
			}

			// Not Connected D:
			if (pData->inet.bufMsgRecv[0] == -1) {
				// TODO: Make the dots loop 1 2 3 Every Second.
				msg = "Failed to connect. . .";
				appWriteLine(msg,
				             22,
				             &(pData->render),
				             0,
				             &(pData->memFn));

				// TODO: Make a timer.
				for (int i = 0; i < 10000000; ++i)
					i = i;

				appInit(pApp, &(pData->memFn));
			}
		} break;
	}

	return 0;
}

// Input Selection
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

		if (pUserBuffer[i].eventType == USERINPUT_EVENT_VK) {
			if (pUserBuffer[i].eventKey == USERINPUT_KEY_ENTER)
				pApp->hasSelected = 1;
			else if (pUserBuffer[i].eventKey == USERINPUT_KEY_ESCAPE)
				pApp->hasEscaped = 1;

			if (pUserBuffer[i].eventKey == USERINPUT_KEY_BACKSPACE) {
				if (*(p_cbField) > 0)
					strField[--(*(p_cbField))] = ' ';
			}
			else if (pUserBuffer[i].eventKey == USERINPUT_KEY_DELETE) {
				if (*(p_cbField) > 0)
					strField[--(*(p_cbField))] = ' ';
			}

			continue;
		}

		// Note: From this point, it MUST be an ascii character.
		if (pUserBuffer[i].eventType != USERINPUT_EVENT_ASCII)
			continue;

		if (*(p_cbField) < MAX_FIELD_INDEX)
			strField[(*(p_cbField))++] = pUserBuffer[i].eventKey;
	}

	return 0;
}

static int
appInputSelect(P_AppData pApp, P_UserBuffer pUserBuffer) {
	for (int i = 0; i < MAX_USERINPUT; ++i) {
		if (pUserBuffer[i].eventType == USERINPUT_IGNORE)
			break;

		if (pUserBuffer[i].eventType != USERINPUT_EVENT_VK)
			continue;

		if (pUserBuffer[i].eventKey == USERINPUT_KEY_ESCAPE)
			pApp->hasEscaped = 1;
		else if (pUserBuffer[i].eventKey == USERINPUT_KEY_BACKSPACE)
			pApp->hasEscaped = 1;
		else if (pUserBuffer[i].eventKey == USERINPUT_KEY_DELETE)
			pApp->hasEscaped = 1;
		else if (pUserBuffer[i].eventKey == USERINPUT_KEY_ENTER)
			pApp->hasSelected = 1;
		else if (pUserBuffer[i].eventKey == USERINPUT_KEY_UP)
			pApp->bCurSelect--;
		else if (pUserBuffer[i].eventKey == USERINPUT_KEY_DOWN)
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
appLogin(P_AppData pApp) {
	if (pApp->hasEscaped) {
		switch(pApp->bAction) {
			case APP_ACTION_START_USERNAME: {
				return -1;
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
appRenderLogin(P_AppData pApp,
		       P_RenderBuffer pRender,
               P_FnMemory pFnMem)
{
	const char *p_szMsg;

	if (pApp->hasEscaped) {
		pFnMem->memset(pRender->bufBack, ' ', pRender->cbBack);
		pFnMem->memset(pApp->strField2, 0, MAX_FIELD_INDEX);
		pApp->ctField2 = 0;
	}

	p_szMsg = "Please Log In:";
	appWriteLine(p_szMsg, 14, pRender, 0, pFnMem);

	p_szMsg = "Username: ";
	appWriteLine(p_szMsg, 10, pRender, 64, pFnMem);
	appWriteLine(pApp->strField1, pApp->ctField1, pRender, 74, pFnMem);

	if (pApp->bAction == 1) {
		p_szMsg = "Password: ";
		appWriteLine(p_szMsg, 10, pRender, 128, pFnMem);
		appWriteLine(pApp->strField2,
		             pApp->ctField2,
		             pRender,
		             138,
		             pFnMem);
	}

	return 0;
}

static int
appWriteLine(const char *p_szMsg,
             size_t cbMsg,
			 P_RenderBuffer pRender,
			 int iRenderOffset,
			 P_FnMemory pFnMem)
{
	size_t cbLeft;

	if (cbMsg >= pRender->colBack)
		return -1;

	// Wrap Detection (Cannot Wrap, only works with pow2)
	cbLeft = pRender->colBack -
		((size_t)pRender->bufBack & (pRender->colBack - 1));

	if (cbMsg > cbLeft)
		return -1;

	pFnMem->memcpy(pRender->bufBack + iRenderOffset, p_szMsg, cbMsg);
	pFnMem->memset(pRender->bufBack + cbMsg, ' ', cbLeft - cbMsg);
	return 0;
}

#endif // CSG_CLIENT_APP_CXX

