#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "producer.h"
#include "sleep_thread.h"

static char toUpper(char character)
{
  unsigned char uc = (unsigned char)character;
  uc &= ~0x20;
  return (char)uc;
}

static int
producerThreadFunction(RingBuffer* ringBuffer, int32_t sleepTimeSeconds, int id)
{
  static const char   alphabet[]   = "abcdefghijklmnopqrstuvwxyz";
  static const size_t alphabetSize = sizeof(alphabet) - 1;

  size_t index = 0;

  for (;;) {
    const byte byteToWrite
      = (id & 1) == 0 ? alphabet[index] : toUpper(alphabet[index]);

    const RingBufferStatusCode statusCode
      = ringBufferWrite(ringBuffer, byteToWrite, id);

    printf("Producer (tid: %d) just wrote %c.\n", id, byteToWrite);

    if (RB_FAILURE(statusCode)) { return EXIT_FAILURE; }

    sleepThread(sleepTimeSeconds);

    ++index;

    if (index == alphabetSize) { index = 0; }
  }

  return EXIT_SUCCESS;
}

Thread* producerCreate(RingBuffer* ringBuffer, int32_t sleepTimeSeconds, int id)
{
  return threadCreate(
    &producerThreadFunction, ringBuffer, sleepTimeSeconds, id);
}
