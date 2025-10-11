#include "LOG.h"
#include <stdbool.h>
#include <stdlib.h>

/* ================================================== */
/*                      DEFINES                       */
/* ================================================== */

#define ASSERT_OUT(fmt, ...)                                                   \
    do {                                                                       \
        fprintf(stderr, ANSI_COLOR_RED "ASSERT FAILED:\n" ANSI_COLOR_RESET);   \
        LOG(fmt, ##__VA_ARGS__);                                               \
    } while (0)

/* ---------------- Basic condition ---------------- */
#ifndef NDEBUG
#define ASSERT_COMMON(cond, fmt, ...)                                          \
    do {                                                                       \
        if (!(cond)) {                                                         \
            ASSERT_OUT(fmt, ##__VA_ARGS__);                                    \
            exit(1);                                                           \
        }                                                                      \
    } while (0)
#else
#define ASSERT_COMMON(cond, fmt, ...)                                          \
    do {                                                                       \
        (void)(cond);                                                          \
    } while (0)
#endif

/* ---------------- POSIX-style (funcRet != EXIT_SUCCESS) ---------------- */
#ifndef NDEBUG
#define ASSERT_COMMON_POSIX(funcRet, fmt, ...)                                 \
    do {                                                                       \
        if ((funcRet) != EXIT_SUCCESS) {                                       \
            ASSERT_OUT(fmt, ##__VA_ARGS__);                                    \
            exit(1);                                                           \
        }                                                                      \
    } while (0)
#else
#define ASSERT_COMMON_POSIX(funcRet, fmt, ...)                                 \
    do {                                                                       \
        (void)(funcRet);                                                       \
    } while (0)
#endif

/* ---------------- With callback ---------------- */
#ifndef NDEBUG
#define ASSERT_COMMON_CB(cond, cb, data, fmt, ...)                             \
    do {                                                                       \
        if (!(cond)) {                                                         \
            ASSERT_OUT(fmt, ##__VA_ARGS__);                                    \
            if ((cb) != NULL) {                                                \
                cb(data);                                                      \
            }                                                                  \
            exit(1);                                                           \
        }                                                                      \
    } while (0)
#else
#define ASSERT_COMMON_CB(cond, cb, data, fmt, ...)                             \
    do {                                                                       \
        (void)(cond);                                                          \
        (void)(cb);                                                            \
        (void)(data);                                                          \
    } while (0)
#endif

/* ---------------- Return on failure ---------------- */
#ifndef NDEBUG
#define ASSERT_COMMON_RET(cond, errRet, fmt, ...)                              \
    do {                                                                       \
        if (!(cond)) {                                                         \
            ASSERT_OUT(fmt, ##__VA_ARGS__);                                    \
            return (errRet);                                                   \
        }                                                                      \
    } while (0)
#else
#define ASSERT_COMMON_RET(cond, errRet, fmt, ...)                              \
    do {                                                                       \
        if (!(cond)) {                                                         \
            return (errRet);                                                   \
        }                                                                      \
    } while (0)

#endif
