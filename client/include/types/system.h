#ifndef CSG_CLIENT_INCLUDE_TYPES_SYSTEM_H
#define CSG_CLIENT_INCLUDE_TYPES_SYSTEM_H

typedef void* (*FnMemcpy)(void *dst, const void *src, size_t size);
typedef void* (*FnMemset)(void *dst, int value, size_t size);
typedef int   (*FnMemcmp)(void *b1, void *b2, size_t size);

typedef struct {
	FnMemcpy memcpy;
	FnMemset memset;
	FnMemcmp memcmp;
} FnMemory, *P_FnMemory;

#endif // CSG_CLIENT_INCLUDE_TYPES_SYSTEM_H

