#include "../common/Assert_Common.h"
#include "../common/LOG.h"
#include "ThreadPool.h"
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define THREAD_COUNT (2)
#define FIFO_DEPTH (100)
#define WORK_TO_DO (1000000)

static_assert(WORK_TO_DO % THREAD_COUNT == 0, "WORK_TO_DO must be divisible by THREAD_COUNT");

ThreadPool_t* mpTP = NULL;

typedef struct {
    uint idx;
} work_t;

uint calcs[WORK_TO_DO];

void* MyFunc(void* pvArgs) {
    work_t* pWork = (work_t*)pvArgs;

    // Artificial workload
    volatile uint x = 0;
    for (uint j = 0; j < 10000; j++) { // tweak iteration count to adjust runtime
        x += j * pWork->idx;
    }

    calcs[pWork->idx] = pWork->idx * 10;

    free(pWork);
    return NULL;
}

void* WorkPusher(void* pvArgs) {
    uint tid = *(uint*)pvArgs;
    free(pvArgs);

    uint start = tid * (WORK_TO_DO / THREAD_COUNT);
    uint end = (tid + 1) * (WORK_TO_DO / THREAD_COUNT);

    for (uint i = start; i < end; i++) {
        work_t* pWork = (work_t*)malloc(sizeof(work_t));
        pWork->idx = i;
        ThreadPool_GiveData(mpTP, pWork);
    }

    return NULL;
}

int main(void) {
    // long cores = sysconf(_SC_NPROCESSORS_ONLN);
    // LOG("Core Count: %lu", cores);

    pthread_t threads[THREAD_COUNT];
    ASSERT_COMMON_POSIX(ThreadPool_Init(&mpTP, THREAD_COUNT, FIFO_DEPTH, "mpTP", MyFunc),
                        "Failed to create ThreadPool");

    // Start work pusher threads
    for (uint i = 0; i < THREAD_COUNT; i++) {
        uint* tid = malloc(sizeof(uint));
        *tid = i;
        pthread_create(&threads[i], NULL, WorkPusher, tid);
    }

    // Wait for work pushers
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy thread pool (waits for all jobs to finish)
    ASSERT_COMMON_POSIX(ThreadPool_Dtr(mpTP), "Failed to destroy ThreadPool");

    // Verify results
    for (uint i = 0; i < WORK_TO_DO; i++) {
        ASSERT_COMMON(calcs[i] == i * 10, "Calculation mismatch at index %u", i);
    }

    LOG("All calculations verified successfully!");

    return EXIT_SUCCESS;
}
