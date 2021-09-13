// Extract some macros from tclInt.h so that the C code
// can be pasted verbatim

#ifndef __TCL_GLOB_MATCH
#define __TCL_GLOB_MATCH

#ifndef JOIN
#  define JOIN(a,b) JOIN1(a,b)
#  define JOIN1(a,b) a##b
#endif

#if defined(__cplusplus)
#   define TCL_UNUSED(T) T
#elif defined(__GNUC__) && (__GNUC__ > 2)
#   define TCL_UNUSED(T) T JOIN(dummy, __LINE__) __attribute__((unused))
#else
#   define TCL_UNUSED(T) T JOIN(dummy, __LINE__)
#endif

int TclByteArrayMatch(
    const char *string,
    int strLen,
    const char *pattern,
    int ptnLen,
    TCL_UNUSED(int)
);

#endif
