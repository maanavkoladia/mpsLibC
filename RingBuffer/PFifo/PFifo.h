#ifndef PFIFO_C
#define PFIFO_C

/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include <semaphore.h>
#include <stddef.h>
#include <stdint.h>

/* ================================================== */
/*                      DEFINES                       */
/* ================================================== */
#define PFIFO_API

typedef enum {
    PFIFO_SUCCESS,
    PFIFO_FREE_FAIL,
    PFIFO_PUSH_FAIL,
    PFIFO_POP_FAIL,
    PFIFO_POP_FAIL_FIFO_EMPTY,
    PFIFO_POP_FAIL_FIFO_FULL,
} err_pFifo_t;

typedef struct pFifo_t pFifo_t;

/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */

PFIFO_API pFifo_t* pFifoCreate(size_t datasize, size_t fifoSize);

PFIFO_API err_pFifo_t pFifoFree(pFifo_t* fifo);

PFIFO_API err_pFifo_t pFifoPush(pFifo_t* pfifo, void* data);

PFIFO_API err_pFifo_t pFifoPop(pFifo_t* pfifo, void* data);

PFIFO_API err_pFifo_t pFifoTryPop(pFifo_t* pfifo, void* data);

PFIFO_API uint32_t pFifoSize(pFifo_t* fifo);

PFIFO_API size_t pFifoElemCount(pFifo_t* fifo);

PFIFO_API void pFifoFlush(pFifo_t* fifo);

#endif // !PFIFO_C
