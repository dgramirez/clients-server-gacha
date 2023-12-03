#ifndef CSG_CLIENT_INCLUDE_TYPES_SURFACE_H
#define CSG_CLIENT_INCLUDE_TYPES_SURFACE_H

#include "types/system.h"
#include "types/input.h"

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
typedef int (*FnSurfaceInput)(P_MemoryObj bufInput,
                              P_BufferObj bufTmp,
                              P_MutexObj pMtxObj,
                              P_SystemFn pSysFn);

typedef struct {
	void *hSurfaceDLL;
	void *hRenderDLL;

	MemoryObj memInput;
	MutexObj moInput;

	MemoryObj memSurface;
	MutexObj moSurface;

	void *hProcess;
	void *hWindow;

	FnSurfaceInit fnInit;
	FnSurfaceInput fnInput;
	FnSurfaceUpdate fnUpdate;
	FnSurfaceShutdown fnShutdown;
} SurfaceBuffer, *P_SurfaceBuffer;

#endif // CSG_CLIENT_INCLUDE_TYPES_SURFACE_H

