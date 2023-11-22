#ifndef CSG_CLIENT_APP_H
#define CSG_CLIENT_APP_H

#include "types/platform.h"

#define MAX_FIELD_INDEX 32

enum APP_STATE {
	// 0 - 15: App States
	APP_STATE_NULL = 0,
	APP_STATE_START,
	APP_STATE_CONNECT,
};

enum APP_FIELD {
	APP_FIELD_NULL = 0,
	APP_FIELD_FIRST,
	APP_FIELD_SECOND,
};

enum APP_ACTION {
	APP_ACTION_START_USERNAME = 0,
	APP_ACTION_START_PASSWORD = 1,
};

typedef struct {
	char strField1[MAX_FIELD_INDEX];
	char strField2[MAX_FIELD_INDEX];
	size_t ctField1;
	size_t ctField2;

	int8_t bState;
	int8_t bAction;
	int8_t bMinSelect;
	int8_t bMaxSelect;
	int8_t bCurSelect;
	int8_t bFieldSelect;
	int8_t hasSelected;
	int8_t hasEscaped;
} AppData, *P_AppData;

CLIENT_API int
appInit(void* vpAppData, P_FnMemory pFnMem);

CLIENT_API int
appUpdate(void* vpAppData,
          P_PlatformData pData);

// Input Selection
static int
appInputAscii(P_AppData pApp, P_UserBuffer pUserBuffer);

static int
appInputSelect(P_AppData pApp, P_UserBuffer pUserBuffer);

// Updates
static  int
appLogin(P_AppData pApp);

// Render
static int
appRenderLogin(P_AppData pApp,
		       P_RenderBuffer pRender,
               P_FnMemory pFnMem);

// Note: Win32 Console acts so wonky, especially when '\0' is involved.
//       This is created to avoid such an awful mess with it.
static int
appWriteLine(const char *p_szMsg,
             size_t cbMsg,
			 P_RenderBuffer pRender,
			 int iRenderOffset,
			 P_FnMemory pFnMem);

#endif // CSG_CLIENT_APP_H

