/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "ThreadPool.h"
#include "../RingBuffer/PFifo/PFifo.h"
#include "../common/Assert_Common.h"
#include "../common/ForLoop.h"
#include "../common/LOG.h"
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

/* ================================================== */
/*                    ENUMS & TYPES                   */
/* ================================================== */

typedef uint ThreadID_t;

typedef struct ThreadPool_t {
    size_t threadCount;
    pthread_t* threads;
    pFifo_t* workQueue;
    char TAG[MAX_TAG_LEN];
} ThreadPool_t;

typedef struct {
    bool killPill;
    void* pvData;
} JobData_t;

typedef struct {
    ThreadID_t tid;
    pFifo_t* workQueue;
    void* (*fnUserFunc)(void*);
} JobRunner_Params_t;

static JobData_t killPill = {.killPill = true, .pvData = NULL};

/* ================================================== */
/*                 PRIVATE FUNCTION DEFINITIONS       */
/* ================================================== */

static void* Task_JobRunner(void* pvArgs) {
    JobRunner_Params_t* pParams = (JobRunner_Params_t*)pvArgs;
    while (1) {
        JobData_t qElem;
        pFifoPop(pParams->workQueue, &qElem);
        if (qElem.killPill) {
            break;
        }
        ASSERT_COMMON(qElem.pvData, "Got a NULL job data");
        pParams->fnUserFunc(qElem.pvData);
    }
    free(pParams);
    return NULL;
}

/* ================================================== */
/*                 PUBLIC FUNCTION DEFINITIONS        */
/* ================================================== */

THREAD_POOL_API ThreadPool_err_t ThreadPool_Init(ThreadPool_t** ppTP, size_t threadCount,
                                                 size_t workQueueDepth, char* TAG,
                                                 void* (*fnUserFunc)(void*)) {
    ASSERT_COMMON(ppTP, "Got a NULL TP output ptr");
    ASSERT_COMMON(threadCount >= 2, "Why are you even making this TP");
    ASSERT_COMMON(strnlen(TAG, MAX_TAG_LEN) < MAX_TAG_LEN, "TAG is too long");
    ASSERT_COMMON(workQueueDepth > 0, "Invalid work queue depth");
    ASSERT_COMMON(fnUserFunc, "Got a NULL user func, what do u even need this for then");

#ifndef NDEBUG
    ThreadPool_t* pTP_local = (ThreadPool_t*)malloc(sizeof(ThreadPool_t));
    ASSERT_COMMON(pTP_local, "Failed to alloc the main structure");

    pTP_local->threadCount = threadCount;
    strncpy(pTP_local->TAG, TAG, MAX_TAG_LEN);
    pTP_local->TAG[MAX_TAG_LEN - 1] = '\0';

    pTP_local->threads = (pthread_t*)malloc(sizeof(pthread_t) * pTP_local->threadCount);
    ASSERT_COMMON(pTP_local->threads, "Failed to alloc the array of pthread_ts");

    ASSERT_COMMON_POSIX(pFifoCreate(sizeof(JobData_t), workQueueDepth, &pTP_local->workQueue),
                        "Fialed to init the work queue");
    ASSERT_COMMON(pTP_local->workQueue, "Failed to create thread pool work queue");

#else
    ThreadPool_t* pTP_local = (ThreadPool_t*)malloc(sizeof(ThreadPool_t));
    if (!pTP_local) return THREAD_POOL_ERR_ALLOC;

    pTP_local->threadCount = threadCount;
    strncpy(pTP_local->TAG, TAG, MAX_TAG_LEN);
    pTP_local->TAG[MAX_TAG_LEN - 1] = '\0';

    pTP_local->threads = (pthread_t*)malloc(sizeof(pthread_t) * pTP_local->threadCount);
    if (!pTP_local->threads) {
        free(pTP_local);
        return THREAD_POOL_ERR_ALLOC;
    }

    pTP_local->workQueue = pFifoCreate(sizeof(JobData_t), workQueueDepth);
    if (!pTP_local->workQueue) {
        free(pTP_local->threads);
        free(pTP_local);
        return THREAD_POOL_ERR_ALLOC;
    }
#endif

    FOR_LOOP_COMMON(i, pTP_local->threadCount) {
        JobRunner_Params_t* params = malloc(sizeof(JobRunner_Params_t));
        *params = (JobRunner_Params_t){
            .fnUserFunc = fnUserFunc,
            .tid = i,
            .workQueue = pTP_local->workQueue,
        };
        ASSERT_COMMON_POSIX(pthread_create(&pTP_local->threads[i], NULL, Task_JobRunner, params),
                            "Failed to create a thread in the TP");
    }
    LOG("%s: TP Created", pTP_local->TAG);
    *ppTP = pTP_local;
    return THREAD_POOL_SUCCESS;
}

THREAD_POOL_API ThreadPool_err_t ThreadPool_GiveData(ThreadPool_t* pTP, void* pvDataIn) {
    ASSERT_COMMON(pTP, "Got a NULL TP");
    JobData_t work = {.pvData = pvDataIn, .killPill = false};
    pFifoPush(pTP->workQueue, &work);
    return THREAD_POOL_SUCCESS;
}

THREAD_POOL_API ThreadPool_err_t ThreadPool_Dtr(ThreadPool_t* pTP) {
    ASSERT_COMMON(pTP, "Got a NULL TP");
    FOR_LOOP_COMMON(i, pTP->threadCount) {
        pFifoPush(pTP->workQueue, &killPill);
    }

    FOR_LOOP_COMMON(i, pTP->threadCount) {
        pthread_join(pTP->threads[i], NULL);
    }

    pFifoFree(pTP->workQueue);
    free(pTP->threads);
    LOG("%s: ThreadPool Destroyed", pTP->TAG);
    free(pTP);
    return THREAD_POOL_SUCCESS;
}
