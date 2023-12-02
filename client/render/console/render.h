#ifndef CSG_CLIENT_RENDER_CONSOLE_RENDER_H
#define CSG_CLIENT_RENDER_CONSOLE_RENDER_H

#include "xform/render.h"

CLIENT_API ERR_RENDER
renderInit(P_XFormRender pRender, P_SystemFn pMemFn);

CLIENT_API ERR_RENDER
render(P_XFormRender pRender, P_SystemFn pMemFn);

CLIENT_API ERR_RENDER
renderCleanup(P_XFormRender pRender, P_SystemFn pMemFn);

static ERR_RENDER
renderParamCheck(P_XFormRender pRender, P_SystemFn pMemFn);

#endif // CSG_CLIENT_RENDER_CONSOLE_RENDER_H

