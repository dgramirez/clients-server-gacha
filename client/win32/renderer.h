#ifndef CSG_CLIENT_RENDERER_H
#define CSG_CLIENT_RENDERER_H

/* Normally, you would have a renderer do this kind of thing.
 * But since this is designed to be an interactive text console
 * I decided to state that "Win32 Console" is my renderer.
 *
 * This is extracted to separate all the pieces
 */

#define MAX_RENDERING_SIZE 1024

typedef enum {
	E_REND_SUCCESS = 0,
	E_REND_FAIL_NULL_RENDERER_INIT,
	E_REND_FAIL_NULL_RENDERER_DATA,
	E_REND_FAIL_NULL_TEXT_BUFFERS,
	E_REND_FAIL_TEXT_BUFFER_SIZE_NOT_2X_MAX_RENDERING_SIZE,
	E_REND_FAIL_ALLOC_CONSOLE,
	E_REND_FAIL_GET_STD_OUTPUT_HANDLE,
	E_REND_FAIL_GET_CONSOLE_CURSOR_INFO,
	E_REND_FAIL_SET_CONSOLE_CURSOR_INFO,
	E_REND_FAIL_GET_CONSOLE_SCREEN_BUFFER_INFO,
	E_REND_FAIL_WRITE_CONSOLE,
	E_REND_WARN_GET_CONSOLE_SCREEN_BUFFER_INFO_AFTER_WRITE,
	E_REND_WARN_FILL_CONSOLE_OUTPUT_CHARACTER,
	E_REND_WARN_FILL_CONSOLE_OUTPUT_ATTRIBUTE,

	E_REND_FAIL_FREE_CONSOLE
} ERR_RENDERER;

typedef struct {
	char **pp_bufConsoleText;
	size_t szConsoleText;
	size_t isCursorVisible;
} RenderInit, *P_RenderInit;

typedef struct {
	char *bufFront;
	char **p_bufBack;
	size_t szFront;
	size_t szBack;
} RenderData, *P_RenderData;

static ERR_RENDERER
rendererInit(P_RenderInit pInitData,
             P_RenderData pRenderData);

static ERR_RENDERER
render(P_RenderData pRenderData);

static ERR_RENDERER
rendererClose(P_RenderData pRenderData);

#endif // CSG_CLIENT_RENDERER_H

