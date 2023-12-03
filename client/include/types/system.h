#ifndef CSG_CLIENT_INCLUDE_TYPES_SYSTEM_H
#define CSG_CLIENT_INCLUDE_TYPES_SYSTEM_H

typedef void   (*FnMemcpy)(void *dst, const void *src, size_t size);
typedef void   (*FnMemmove)(void *dst, const void *src, size_t size);
typedef void   (*FnMemset)(void *dst, int value, size_t size);
typedef void   (*FnMemzero)(void *dst, size_t size);
typedef size_t (*FnMemcmp)(const void *b1, const void *b2, size_t size);

typedef int (*FnMtxLock)(void *, void *, size_t);
typedef void (*FnMtxUnlock)(void *, void *);
typedef void (*FnCVWake)(void*);
typedef void (*FnCVWakeAll)(void*);

typedef struct {
	FnMemcpy memcpy;
	FnMemset memset;
	FnMemcmp memcmp;
	FnMemzero memzero;
	FnMemmove memmove;

	FnMtxLock mtx_lock;
	FnMtxUnlock mtx_unlock;
	FnCVWake cv_wake;
	FnCVWakeAll cv_wakeall;
} SystemFn, *P_SystemFn;

typedef struct {
	void *vpAddr;
	size_t cbAddr;
} BufferObj, *P_BufferObj;

typedef struct {
	void *vpAddr;
	size_t cbAddr;
	size_t allocAddr;
	size_t padAddr;
} MemoryObj, *P_MemoryObj;

typedef struct {
	void *vpMtx;
	void *vpCV;
} MutexObj, *P_MutexObj;

#endif // CSG_CLIENT_INCLUDE_TYPES_SYSTEM_H

