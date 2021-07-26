#ifndef INCG_THREAD_H
#define INCG_THREAD_H
#include <stdbool.h>
#include <stdint.h>

#include "ring_buffer.h"

typedef struct ThreadOpaque Thread;

typedef int (
  *ThreadFunction)(RingBuffer* ringBuffer, int32_t sleepTimeSeconds, int id);

Thread* threadCreate(
  ThreadFunction function,
  RingBuffer*    ringBuffer,
  int32_t        sleepTimeSeconds,
  int            id);

bool threadFree(Thread* thread, int* threadExitStatus);
#endif /* INCG_THREAD_H */
