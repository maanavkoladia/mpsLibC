#ifndef ATOMIC_FLAG_H
#define ATOMIC_FLAG_H

#include "Assert_Common.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>

/* ================================================== */
/*                    ENUMS & TYPES                   */
/* ================================================== */
#define ATOMIC_FLAG_API static inline

#define MAX_TAG_LEN (128)

typedef enum {
    FLAG_CLEAR = 0,
    FLAG_SET = 1,
} FlagStatus_t;

typedef struct {
    atomic_uint_fast64_t atomicFlag;
    char TAG[MAX_TAG_LEN];
} AtomicFlag_t;

/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */

ATOMIC_FLAG_API void AtomicFlag_Init(AtomicFlag_t* flag, const char* tag_in,
                                     FlagStatus_t startingStatus) {
    ASSERT_COMMON(flag, "Got a NULL Flag");
    ASSERT_COMMON(sizeof(tag_in) < MAX_TAG_LEN, "Atomic Flag TAG too long");
    if (tag_in) {
        size_t len = strnlen(tag_in, MAX_TAG_LEN - 1);
        memcpy(flag->TAG, tag_in, len);
        flag->TAG[len] = '\0';
    } else {
        flag->TAG[0] = '\0';
    }

    atomic_store(&flag->atomicFlag, startingStatus);

    LOG("%s: %s", flag->TAG, startingStatus == FLAG_SET ? "SET" : "CLEAR");
}

ATOMIC_FLAG_API void AtomicFlag_Clear(AtomicFlag_t* flag) {
    atomic_store(&flag->atomicFlag, FLAG_CLEAR);
    LOG("%s: CLEAR", flag->TAG);
}

ATOMIC_FLAG_API void AtomicFlag_Set(AtomicFlag_t* flag) {
    atomic_store(&flag->atomicFlag, FLAG_SET);
    LOG("%s: SET", flag->TAG);
}

ATOMIC_FLAG_API void AtomicFlag_UpdateStatus(AtomicFlag_t* flag, FlagStatus_t newStatus) {
    atomic_store(&flag->atomicFlag, newStatus);
    LOG("%s: %s", flag->TAG, newStatus == FLAG_SET ? "SET" : "CLEAR");
}

ATOMIC_FLAG_API FlagStatus_t AtomicFlag_GetStatus(AtomicFlag_t* flag) {
    FlagStatus_t status = (FlagStatus_t)atomic_load(&flag->atomicFlag);
    // LOG("%s: %s", flag->TAG, status == FLAG_SET ? "SET" : "CLEAR");
    return status;
}

#endif // ATOMIC_FLAG_H
