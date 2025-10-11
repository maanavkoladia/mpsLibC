/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */
#define PLOG_BUFSIZE (4096)

#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"

/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */

/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */
#define __FILENAME__                                                           \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifndef NDEBUG
#define LOG(fmt, ...)                                                          \
    do {                                                                       \
        char ___logBuf[PLOG_BUFSIZE];                                          \
        snprintf(___logBuf, sizeof(___logBuf),                                 \
                 ANSI_COLOR_CYAN                                               \
                 "[%s:%d:%s] \n" ANSI_COLOR_WHITE fmt ANSI_COLOR_RESET "\n",   \
                 __FILENAME__, __LINE__, __func__, ##__VA_ARGS__);             \
        fprintf(stderr, "%s", ___logBuf);                                      \
        fflush(stderr);                                                        \
    } while (0)
#else
#define LOG(...)                                                               \
    do {                                                                       \
    } while (0)
#endif
