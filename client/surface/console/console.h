#ifndef CSG_CLIENT_SURFACE_CONSOLE_CONSOLE_H
#define CSG_CLIENT_SURFACE_CONSOLE_CONSOLE_H

#include "types/surface.h"
#include <windows.h>

CLIENT_API int
expSurfaceInit(P_InitSurface pSurf, P_SystemFn pSysFn);

CLIENT_API int
surfaceRender(void *bufBack, P_SurfaceBuffer pSurf, P_SystemFn pSysFn);

CLIENT_API int
surfaceCleanup(P_SurfaceBuffer pSurf, P_SystemFn pSysFn);

#endif // CSG_CLIENT_SURFACE_CONSOLE_CONSOLE_H

