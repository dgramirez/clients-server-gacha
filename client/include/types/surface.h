#ifndef CSG_CLIENT_INCLUDE_TYPES_SURFACE_H
#define CSG_CLIENT_INCLUDE_TYPES_SURFACE_H

#include "types/system.h"
typedef enum {
	ERR_SURFACE_SUCCESS = 0,
	ERR_SURFACE_FAILURE_START = 0x100,
	ERR_SURFACE_FAILURE_END,
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

