#include <stdlib.h>

#include "consumer.h"
#include "sleep_thread.h"

static int consumerThreadFunction(RingBuffer* ringBuffer, int32_t sleepTimeSeconds)
{
  for (;;) {
    byte byteJustRead;
    const RingBufferStatusCode statusCode
      = ringBufferRead(ringBuffer, &byteJustRead);

    if (RB_FAILURE(statusCode)) {
      return EXIT_FAILURE;
    }

    sleepThread(sleepTimeSeconds);
  }

  return EXIT_SUCCESS;
}

Thread* consumerCreate(RingBuffer* ringBuffer, int32_t sleepTimeSeconds)
{
  return threadCreate(&consumerThreadFunction, ringBuffer, sleepTimeSeconds);
}