#ifndef CSG_CLIENT_INCLUDE_TYPES_SURFACE_H
#define CSG_CLIENT_INCLUDE_TYPES_SURFACE_H

#include "types/system.h"
typedef enum {
	E_SURFACE_SUCCESS = 0,
	E_SURFACE_FAIL_CREATE_WINDOW = 0x81000000,
	E_SURFACE_FAIL_GET_CONSOLE_CURSOR_INFO,
	E_SURFACE_FAIL_SET_CONSOLE_CURSOR_INFO,
	E_SURFACE_FAIL_GET_CONSOLE_SCREEN_BUFFER_INFO_EX,
	E_SURFACE_FAIL_SET_CONSOLE_SCREEN_BUFFER_INFO_EX,
	E_SURFACE_FAIL_END,
	E_SURFACE_FAIL_START = E_SURFACE_FAIL_CREATE_WINDOW
} E_SURFACE;

typedef struct {
	short posX;
	short posY;
	short posW;
	short posH;

	void *out_hProcess;
	void *out_hWindow;
} InitSurface, *P_InitSurface;

// For Platform
typedef int (*FnSurfaceInit)(P_InitSurface, P_SystemFn);
typedef int (*FnSurfaceUpdate)();
typedef int (*FnSurfaceShutdown)();

typedef struct {
	void *hSurfaceDLL;
	void *hRenderDLL;

	void *hProcess;
	void *hWindow;

	void *bufInput;
	size_t cbInput;

	void *bufSurface;
	size_t cbSurface;

	FnSurfaceInit fnInit;
	FnSurfaceUpdate fnUpdate;
	FnSurfaceShutdown fnShutdown;
} SurfaceBuffer, *P_SurfaceBuffer;

#endif // CSG_CLIENT_INCLUDE_TYPES_SURFACE_H

