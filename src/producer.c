#include <stddef.h>
#include <stdlib.h>

#include "producer.h"
#include "sleep_thread.h"

static int producerThreadFunction(RingBuffer* ringBuffer, int32_t sleepTimeSeconds)
{
  static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
  static const size_t alphabetSize = sizeof(alphabet) - 1;

  size_t index = 0;

  for (;;) {
    const RingBufferStatusCode statusCode
      = ringBufferWrite(ringBuffer, alphabet[index]);

    if (RB_FAILURE(statusCode)) {
      return EXIT_FAILURE;
    }

    sleepThread(sleepTimeSeconds);

    ++index;

    if (index == alphabetSize) {
      index = 0;
    }
  }

  return EXIT_SUCCESS;
}

Thread* producerCreate(RingBuffer* ringBuffer, int32_t sleepTimeSeconds)
{
  return threadCreate(&producerThreadFunction, ringBuffer, sleepTimeSeconds);
}
