#ifndef CSG_CLIENT_INCLUDE_TYPES_RENDER_H
#define CSG_CLIENT_INCLUDE_TYPES_RENDER_H

typedef struct {
	char *bufBack;
	size_t cbBack;
	size_t cbBackMax;
	size_t colBack;
} RenderBuffer, *P_RenderBuffer;

#endif // CSG_CLIENT_INCLUDE_TYPES_RENDER_H

