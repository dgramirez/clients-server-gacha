#ifndef CSG_CLIENT_WIN32_XFORMFN_CXX
#define CSG_CLIENT_WIN32_XFORMFN_CXX

#include "types/system.h"
#include <windows.h>

static void
win32Memcpy(void *dst, const void *src, size_t size) {
	RtlCopyMemory(dst, src, size);
}

static void
win32Memset(void *dst, int value, size_t size) {
	RtlFillMemory(dst, size, value);
}

static void
win32Memzero(void *dst, size_t size) {
	ZeroMemory(dst, size);
}

static void
win32Memmove(void *dst, const void *src, size_t size) {
	RtlMoveMemory(dst, src, size);
}

static size_t
win32Memcmp(const void *m1, const void *m2, size_t size) {
	return RtlCompareMemory(m1, m2, size);
}

static int
win32TryEnterCriticalSection(void *vpCS,
                             void *vpCV,
                             size_t ctIterationPerMs)
{
	int i;
	BOOL eTimedCS;
	CRITICAL_SECTION *pCS;
	CONDITION_VARIABLE *pCV;
	CRITICAL_SECTION tryCS;

	pCS = (CRITICAL_SECTION*)vpCS;
	pCV = (CONDITION_VARIABLE*)vpCV;
	InitializeCriticalSection(&tryCS);

	i = 0;
	do {
		eTimedCS = TryEnterCriticalSection(pCS);
		if (eTimedCS)
			break;

		if (++i > ctIterationPerMs)
			break;

		SleepConditionVariableCS(pCV, &tryCS, 1);
	} while(1);
	DeleteCriticalSection(&tryCS);

	return (eTimedCS > 0);
}

static void
win32LeaveCriticalSection(void *vpCS,
                          void *vpCV)
{

	CRITICAL_SECTION *pCS;
	CONDITION_VARIABLE *pCV;

	pCS = (CRITICAL_SECTION *)vpCS;
	pCV = (CONDITION_VARIABLE *)vpCV;

	LeaveCriticalSection(pCS);
	WakeConditionVariable(pCV);
}

static void
win32WakeConditionVariable(void *vpCV) {
	CONDITION_VARIABLE *pCV;

	pCV = (CONDITION_VARIABLE *)vpCV;
	WakeConditionVariable(pCV);
}

static void
win32WakeAllConditionVariable(void *vpCV) {
	CONDITION_VARIABLE *pCV;

	pCV = (CONDITION_VARIABLE *)vpCV;
	WakeAllConditionVariable(pCV);
}

static void
win32InitFn(P_SystemFn pSysFn) {
	pSysFn->memcpy = win32Memcpy;
	pSysFn->memset = win32Memset;
	pSysFn->memcmp = win32Memcmp;
	pSysFn->memmove = win32Memmove;
	pSysFn->memzero = win32Memzero;

	pSysFn->mtx_lock = win32TryEnterCriticalSection;
	pSysFn->mtx_unlock = win32LeaveCriticalSection;
	pSysFn->cv_wake = win32WakeConditionVariable;
	pSysFn->cv_wakeall = win32WakeAllConditionVariable;
}

#endif // CSG_CLIENT_WIN32_XFORMFN_CXX

