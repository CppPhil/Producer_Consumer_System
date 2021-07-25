#ifndef INCG_INCG_RING_BUFFER_H
#define INCG_INCG_RING_BUFFER_H
#include <stddef.h>

#include "byte.h"

typedef enum {
  RB_OK,
  RB_NOMEM,
  RB_FAILURE_TO_INIT_MUTEX,
  RB_FAILURE_TO_DESTROY_MUTEX,
  RB_FAILURE_TO_LOCK_MUTEX,
  RB_FAILURE_TO_UNLOCK_MUTEX,
  RB_FAILURE_TO_INIT_CONDVAR,
  RB_FAILURE_TO_DESTROY_CONDVAR,
  RB_FAILURE_TO_WAIT_ON_CONDVAR,
  RB_FAILURE_TO_SIGNAL_CONDVAR
} RingBufferStatusCode;

const char* ringBufferStatesCodeToString(RingBufferStatusCode statusCode);

typedef struct RingBufferOpaque RingBuffer;

RingBufferStatusCode ringBufferCreate(
  size_t       byteCount,
  RingBuffer** ringBuffer);

RingBufferStatusCode ringBufferFree(RingBuffer* ringBuffer);

RingBufferStatusCode ringBufferWrite(RingBuffer* ringBuffer, byte toWrite);

RingBufferStatusCode ringBufferRead(RingBuffer* ringBuffer, byte* byteRead);
#endif /* INCG_INCG_RING_BUFFER_H */
