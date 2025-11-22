#ifndef LF_FIFO
#define LF_FIFO
/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include <stddef.h>
#include <stdint.h>

/* ================================================== */
/*                      DEFINES                       */
/* ================================================== */
#define LF_FIFO_API

typedef enum {
    LF_FIFO_SUCCESS = 0,
    LF_FIFO_FAIL_INIT_SYSTEM,
    LF_FIFO_FAIL_TRY_PUSH,
    LF_FIFO_FAIL_TIMED_PUSH,
    LF_FIFO_FAIL_TRY_POP,
    LF_FIFO_FAIL_TIMED_POP,
} err_LF_Fifo_t;

typedef struct LF_Fifo_t LF_Fifo_t;

/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */

LF_FIFO_API err_LF_Fifo_t LF_Fifo_Init(LF_Fifo_t** ppFifoOut, size_t capacity);

LF_FIFO_API err_LF_Fifo_t LF_Fifo_Dtr(LF_Fifo_t* pfifo);

LF_FIFO_API err_LF_Fifo_t LF_Fifo_SpinPush(LF_Fifo_t* pfifo, void* pvData);
LF_FIFO_API err_LF_Fifo_t LF_Fifo_TryPush(LF_Fifo_t* pfifo, void* pvData);
LF_FIFO_API err_LF_Fifo_t LF_Fifo_TimedPush(LF_Fifo_t* pfifo, void* pvData,
                                            const struct timespec* Timeout);

LF_FIFO_API err_LF_Fifo_t LF_Fifo_SpinPop(LF_Fifo_t* pfifo, void** ppvDataOut);
LF_FIFO_API err_LF_Fifo_t LF_Fifo_TryPop(LF_Fifo_t* pfifo, void** ppvDataOut);
LF_FIFO_API err_LF_Fifo_t LF_Fifo_TimedPop(LF_Fifo_t* pfifo, void** ppvDataOut,
                                           const struct timespec* Timeout);

LF_FIFO_API uint32_t LF_FifoCapacity(LF_Fifo_t* pfifo);

#endif
