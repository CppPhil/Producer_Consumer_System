#include <stdio.h>
#include <stdlib.h>

#include "byte.h"
#include "consumer.h"
#include "ring_buffer.h"
#include "sleep_thread.h"

static int consumerThreadFunction(
  RingBuffer* ringBuffer,
  int32_t     sleepTimeSeconds,
  int         id,
  Thread*     self)
{
  for (;;) {
    bool       shouldShutdown;
    const bool ok = threadShouldShutdown(self, &shouldShutdown);

    if (!ok) { return EXIT_FAILURE; }

    if (shouldShutdown) { break; }

    byte                       byteJustRead;
    const RingBufferStatusCode statusCode
      = ringBufferRead(ringBuffer, &byteJustRead, id, self);

    if (statusCode == RB_THREAD_SHOULD_SHUTDOWN) { break; }

    if (RB_FAILURE(statusCode)) { return EXIT_FAILURE; }

    printf("Consumer (tid: %d) just read %c.\n", id, byteJustRead);

    sleepThread(sleepTimeSeconds);
  }

  return EXIT_SUCCESS;
}

Thread* consumerCreate(RingBuffer* ringBuffer, int32_t sleepTimeSeconds, int id)
{
  return threadCreate(
    &consumerThreadFunction, ringBuffer, sleepTimeSeconds, id);
}