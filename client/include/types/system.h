#ifndef CSG_CLIENT_INCLUDE_TYPES_SYSTEM_H
#define CSG_CLIENT_INCLUDE_TYPES_SYSTEM_H

typedef void   (*FnMemcpy)(void *dst, const void *src, size_t size);
typedef void   (*FnMemmove)(void *dst, const void *src, size_t size);
typedef void   (*FnMemset)(void *dst, int value, size_t size);
typedef void   (*FnMemzero)(void *dst, size_t size);
typedef size_t (*FnMemcmp)(const void *b1, const void *b2, size_t size);

typedef struct {
	FnMemcpy memcpy;
	FnMemset memset;
	FnMemcmp memcmp;
	FnMemzero memzero;
	FnMemmove memmove;
} SystemFn, *P_SystemFn;

#endif // CSG_CLIENT_INCLUDE_TYPES_SYSTEM_H

