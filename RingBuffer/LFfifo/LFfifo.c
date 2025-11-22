/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "LFfifo.h"
#include "../../common/Assert_Common.h"
#include "../../common/LOG.h"
#include <ck_ring.h>
#include <stdint.h>
#include <time.h>

/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */
typedef struct LF_Fifo_t {
    ck_ring_t ck_ring_fifo;
    ck_ring_buffer_t* ck_ring_fifo_buf;
    size_t capacity;
    size_t size;
} LF_Fifo_t;

struct timespec timeOutWakeUp = {.tv_nsec = 1000, .tv_sec = 0};
/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */

/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */

LF_FIFO_API err_LF_Fifo_t LF_Fifo_Init(LF_Fifo_t** ppFifoOut, size_t capacity) {
    ASSERT_COMMON(ppFifoOut, "Got a NULL fifo Ptr");
    ASSERT_COMMON(capacity > 0, "Empty Cpaacity Fifo, Are you sure moneky?");

    LF_Fifo_t* pFifoBuf = (LF_Fifo_t*)malloc(sizeof(LF_Fifo_t));
    if (!pFifoBuf) {
        return LF_FIFO_FAIL_INIT_SYSTEM;
    }

    pFifoBuf->capacity = capacity;
    pFifoBuf->size = capacity + 1;

    pFifoBuf->ck_ring_fifo_buf =
        (ck_ring_buffer_t*)malloc(sizeof(ck_ring_buffer_t*) * pFifoBuf->size);
    if (!pFifoBuf->ck_ring_fifo_buf) {
        free(pFifoBuf);
        return LF_FIFO_FAIL_INIT_SYSTEM;
    }

    // doing the ck ring init ritual
    ck_ring_init(&pFifoBuf->ck_ring_fifo, pFifoBuf->size);
    *ppFifoOut = pFifoBuf;
    return LF_FIFO_SUCCESS;
}

LF_FIFO_API err_LF_Fifo_t LF_Fifo_Dtr(LF_Fifo_t* pfifo) {
    ASSERT_COMMON(pfifo, "Got a NULL fifo ptr, stop fucking with me");
    free(pfifo->ck_ring_fifo_buf);
    free(pfifo);
    return LF_FIFO_SUCCESS;
}

LF_FIFO_API err_LF_Fifo_t LF_Fifo_SpinPush(LF_Fifo_t* pfifo, void* pvData) {
    ASSERT_COMMON(pfifo, "Got a NULL fifo");
    ASSERT_COMMON(pvData, "Got a NULL data ptr");

    while (!ck_ring_enqueue_spsc(&pfifo->ck_ring_fifo, pfifo->ck_ring_fifo_buf, pvData)) {
    }
    return LF_FIFO_SUCCESS;
}

LF_FIFO_API err_LF_Fifo_t LF_Fifo_TryPush(LF_Fifo_t* pfifo, void* pvData) {
    ASSERT_COMMON(pfifo, "Got a NULL fifo");
    ASSERT_COMMON(pvData, "Got a NULL data ptr");
    if (!ck_ring_enqueue_spsc(&pfifo->ck_ring_fifo, pfifo->ck_ring_fifo_buf, pvData)) {
        return LF_FIFO_FAIL_TRY_PUSH;
    }
    return LF_FIFO_SUCCESS;
}

LF_FIFO_API err_LF_Fifo_t LF_Fifo_TimedPush(LF_Fifo_t* pfifo, void* pvData,
                                            const struct timespec* Timeout) {
    ASSERT_COMMON(pfifo, "Got a NULL fifo");
    ASSERT_COMMON(pvData, "Got a NULL data ptr");
    ASSERT_COMMON(Timeout->tv_nsec >= timeOutWakeUp.tv_nsec, "TimeOut Too Short, Must be >= %lu ns",
                  timeOutWakeUp.tv_nsec);

    uint64_t finalCount =
        ((Timeout->tv_sec * 1000000000) + Timeout->tv_nsec) / timeOutWakeUp.tv_nsec;
    uint64_t wakeUpCounter = 0;
    while (!ck_ring_enqueue_spsc(&pfifo->ck_ring_fifo, pfifo->ck_ring_fifo_buf, pvData)) {
        if (wakeUpCounter++ == finalCount) {
            return LF_FIFO_FAIL_TIMED_PUSH;
        }
    }
    return LF_FIFO_SUCCESS;
}

LF_FIFO_API err_LF_Fifo_t LF_Fifo_SpinPop(LF_Fifo_t* pfifo, void** ppvDataOut) {
    ASSERT_COMMON(pfifo, "Got a NULL fifo");
    ASSERT_COMMON(ppvDataOut, "Got a NULL data ptr");
    while (!ck_ring_dequeue_spsc(&pfifo->ck_ring_fifo, pfifo->ck_ring_fifo_buf, ppvDataOut)) {
    }
    return LF_FIFO_SUCCESS;
}
LF_FIFO_API err_LF_Fifo_t LF_Fifo_TryPop(LF_Fifo_t* pfifo, void** ppvDataOut) {
    ASSERT_COMMON(pfifo, "Got a NULL fifo");
    ASSERT_COMMON(ppvDataOut, "Got a NULL data ptr");
    if (!ck_ring_dequeue_spsc(&pfifo->ck_ring_fifo, pfifo->ck_ring_fifo_buf, ppvDataOut)) {
        return LF_FIFO_FAIL_TRY_POP;
    }
    return LF_FIFO_SUCCESS;
}

LF_FIFO_API err_LF_Fifo_t LF_Fifo_TimedPop(LF_Fifo_t* pfifo, void** ppvDataOut,
                                           const struct timespec* Timeout) {
    ASSERT_COMMON(pfifo, "Got a NULL fifo");
    ASSERT_COMMON(ppvDataOut, "Got a NULL data ptr");
    ASSERT_COMMON(Timeout->tv_nsec >= timeOutWakeUp.tv_nsec, "TimeOut Too Short, Must be >= %lu ns",
                  timeOutWakeUp.tv_nsec);

    uint64_t finalCount =
        ((Timeout->tv_sec * 1000000000) + Timeout->tv_nsec) / timeOutWakeUp.tv_nsec;
    uint64_t wakeUpCounter = 0;
    while (!ck_ring_dequeue_spsc(&pfifo->ck_ring_fifo, pfifo->ck_ring_fifo_buf, ppvDataOut)) {
        if (wakeUpCounter++ == finalCount) {
            return LF_FIFO_FAIL_TIMED_PUSH;
        }
    }
    return LF_FIFO_SUCCESS;
}

LF_FIFO_API uint32_t LF_FifoCapacity(LF_Fifo_t* pfifo) {
    return pfifo->capacity;
}
