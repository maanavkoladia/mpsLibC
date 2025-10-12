#pragma once
/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include <pthread.h>
#include <stddef.h>
#include <string.h>

/* ================================================== */
/*                    ENUMS & TYPES                   */
/* ================================================== */

#define MAX_TAG_LEN (128)
#define THREAD_POOL_API

typedef enum {
    THREAD_POOL_SUCCESS = 0,
    THREAD_POOL_FAIL,
    THREAD_POOL_FAIL_INACTIVE,
    THREAD_POOL_FAIL_ACTIVE,
    THREAD_POOL_FAIL_SYSTEM,
} ThreadPool_err_t;

typedef struct ThreadPool_t ThreadPool_t;

/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */

THREAD_POOL_API ThreadPool_err_t ThreadPool_Init(ThreadPool_t** ppTP, size_t threadCount,
                                                 size_t workQueueDepth, char* TAG,
                                                 void* (*fnUserFunc)(void*));

THREAD_POOL_API ThreadPool_err_t ThreadPool_GiveData(ThreadPool_t* pTP, void* pvDataIn);

THREAD_POOL_API ThreadPool_err_t ThreadPool_Dtr(ThreadPool_t* pTP);
