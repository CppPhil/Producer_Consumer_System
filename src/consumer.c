#include <stdio.h>
#include <stdlib.h>

#include "consumer.h"
#include "sleep_thread.h"

static int
consumerThreadFunction(RingBuffer* ringBuffer, int32_t sleepTimeSeconds, int id)
{
  for (;;) {
    byte                       byteJustRead;
    const RingBufferStatusCode statusCode
      = ringBufferRead(ringBuffer, &byteJustRead);

    printf("Consumer (tid: %d) just read %c.\n", id, (char)byteJustRead);

    if (RB_FAILURE(statusCode)) { return EXIT_FAILURE; }

    sleepThread(sleepTimeSeconds);
  }

  return EXIT_SUCCESS;
}

Thread* consumerCreate(RingBuffer* ringBuffer, int32_t sleepTimeSeconds, int id)
{
  return threadCreate(
    &consumerThreadFunction, ringBuffer, sleepTimeSeconds, id);
}