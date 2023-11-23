#ifndef CSG_CLIENT_RENDER_CONSOLE_RENDER_H
#define CSG_CLIENT_RENDER_CONSOLE_RENDER_H

#include "render/xform_render.h"

CLIENT_API ERR_RENDER
renderInit(P_XFormRender pRender);

CLIENT_API ERR_RENDER
render(P_XFormRender pRender);

CLIENT_API ERR_RENDER
renderCleanup(P_XFormRender pRender);

static ERR_RENDER
renderParamCheck(P_XFormRender pRender);

#endif // CSG_CLIENT_RENDER_CONSOLE_RENDER_H

