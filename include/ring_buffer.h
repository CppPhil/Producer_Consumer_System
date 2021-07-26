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

#define RB_SUCCESS(enm) ((enm) == RB_OK)
#define RB_FAILURE(enm) ((enm) != RB_OK)

const char* ringBufferStatusCodeToString(RingBufferStatusCode statusCode);

typedef struct RingBufferOpaque RingBuffer;

RingBufferStatusCode ringBufferCreate(
  size_t       byteCount,
  RingBuffer** ringBuffer);

RingBufferStatusCode ringBufferFree(RingBuffer* ringBuffer);

RingBufferStatusCode
ringBufferWrite(RingBuffer* ringBuffer, byte toWrite, int threadId);

RingBufferStatusCode
ringBufferRead(RingBuffer* ringBuffer, byte* byteRead, int threadId);
#endif /* INCG_INCG_RING_BUFFER_H */
