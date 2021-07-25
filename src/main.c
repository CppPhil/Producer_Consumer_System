#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmd_args.h"
#include "ring_buffer.h"

int main(int argc, char** argv)
{
  const CmdArgs commandLineArguments = parseCmdArgs(argc, argv);

  if (!commandLineArguments.isOk) { return EXIT_FAILURE; }

  printf("producerCount: %" PRId32 "\n", commandLineArguments.producerCount);
  printf("consumerCount: %" PRId32 "\n", commandLineArguments.consumerCount);
  printf(
    "producerSleepTime: %" PRId32 "\n", commandLineArguments.producerSleepTime);
  printf(
    "consumerSleepTime: %" PRId32 "\n", commandLineArguments.consumerSleepTime);
  printf("\n");

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
