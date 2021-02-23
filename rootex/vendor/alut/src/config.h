
/* Define to 1 if we are using a GCC with symbol visibility support. */
/* #undef HAVE_GCC_VISIBILITY */

/* Define to 1 if the system has the type `__int8'. */
#define HAVE___INT8 1

/* Define to 1 if your C compiler supports __attribute__. */
/* #undef HAVE___ATTRIBUTE__ */

/* Define to 1 if you have the `stat' function. */
/* #undef HAVE_STAT */

/* Define to 1 if you have the `_stat' function. */
#define HAVE__STAT 1

/* Define to 1 if you have the `nanosleep' function. */
/* #undef HAVE_NANOSLEEP */

/* Define to 1 if you have the `usleep' function. */
/* #undef HAVE_USLEEP */

/* Define to 1 if you have the `Sleep' function. */
#define HAVE_SLEEP 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if you have the <windows.h> header file. */
#define HAVE_WINDOWS_H 1

/* We could possibly need struct timespec and random(), which are not ANSI.
   Define to 500 if Single Unix conformance is wanted, 600 for sixth revision. */
#if HAVE_NANOSLEEP && HAVE_TIME_H
#define _XOPEN_SOURCE 600
/* We might need nanosleep, which is a POSIX IEEE Std 1003.1b-1993 feature.
   Define to the POSIX version that should be used. */
#define _POSIX_C_SOURCE 200112L
#endif

#define ALUT_BUILD_LIBRARY
