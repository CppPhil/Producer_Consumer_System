#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmd_args.h"
#include "ring_buffer.h"

int main(int argc, char** argv)
{
  const CmdArgs commandLineArguments = parseCmdArgs(argc, argv);

  if (!commandLineArguments.isOk) { return EXIT_FAILURE; }

  RingBuffer*          ringBuffer     = NULL;
  const size_t         ringBufferSize = 20;
  RingBufferStatusCode statusCode
    = ringBufferCreate(ringBufferSize, &ringBuffer);

  if (RB_FAILURE(statusCode)) { goto error; }

  // TODO: HERE

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
