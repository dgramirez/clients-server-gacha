#ifndef CSG_CLIENT_INCLUDE_XFORM_APP_H
#define CSG_CLIENT_INCLUDE_XFORM_APP_H

#include "xform/surface.h"

CLIENT_API int
surfaceInit(P_XFormSurface, P_MemoryFn);

CLIENT_API int
surfaceCleanup(P_XFormSurface, P_MemoryFn);

#endif // CSG_CLIENT_INCLUDE_XFORM_APP_H

