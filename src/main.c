#include <stdio.h>
#include <stdlib.h>

#include "ring_buffer.h"

int main(void)
{
  RingBuffer*          ringBuffer     = NULL;
  const size_t         ringBufferSize = 20;
  RingBufferStatusCode statusCode
    = ringBufferCreate(ringBufferSize, &ringBuffer);

  if (RB_FAILURE(statusCode)) { goto error; }

  statusCode = ringBufferWrite(ringBuffer, 0xAB);

  if (RB_FAILURE(statusCode)) { goto error; }

  byte byteRead = 0;
  statusCode    = ringBufferRead(ringBuffer, &byteRead);

  if (RB_FAILURE(statusCode)) { goto error; }

  printf("Read 0x%02X from ring buffer.\n", byteRead);

  statusCode = ringBufferFree(ringBuffer);

  if (RB_FAILURE(statusCode)) {
    fprintf(
      stderr,
      "Could not free ring buffer: %s\n",
      ringBufferStatusCodeToString(statusCode));
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

error:
  fprintf(
    stderr,
    "An error occurred: %s\n",
    ringBufferStatusCodeToString(statusCode));
  statusCode = ringBufferFree(ringBuffer);

  if (RB_FAILURE(statusCode)) {
    fprintf(
      stderr,
      "Could not free ring buffer: %s\n",
      ringBufferStatusCodeToString(statusCode));
  }

  return EXIT_FAILURE;
}
