#ifndef CSG_CLIENT_INCLUDE_TYPES_APP_H
#define CSG_CLIENT_INCLUDE_TYPES_APP_H

typedef struct {
	void *hAppDLL;
	char szAppDLL[8];

	void *bufApp;
	size_t cbApp;
} AppBuffer, *P_AppBuffer;

#endif // CSG_CLIENT_INCLUDE_TYPES_APP_H

