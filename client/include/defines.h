#ifndef CSG_CLIENT_INCLUDE_DEFINES_H
#define CSG_CLIENT_INCLUDE_DEFINES_H

#define KB(x) ((x) << 10)
#define MB(x) ((x) << 20)
#define NO_REF(x) ((void)sizeof(x))

#define SET_FLG(x, f) ((x) |= (f))
#define TOG_FLG(x, f) ((x) ^= (f))
#define REM_FLG(x, f) ((x) &= ~(f))
#define HAS_FLG(x, f) (((x) & (f)) > 0)
#define ALL_FLG(x, f) (((x) & (f)) == (f))

#endif // CSG_CLIENT_INCLUDE_DEFINES_H

