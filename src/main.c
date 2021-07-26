#include <stdio.h>
#include <stdlib.h>

#include "cmd_args.h"
#include "consumer.h"
#include "producer.h"
#include "ring_buffer.h"

// TODO: Make a function for freeing the consumers
//       and one for freeing the producers
int main(int argc, char** argv)
{
  const CmdArgs commandLineArguments = parseCmdArgs(argc, argv);

  if (!commandLineArguments.isOk) { return EXIT_FAILURE; }

  RingBuffer*          ringBuffer     = NULL;
  const size_t         ringBufferSize = 10;
  RingBufferStatusCode statusCode
    = ringBufferCreate(ringBufferSize, &ringBuffer);

  if (RB_FAILURE(statusCode)) { goto error; }

  Thread** producers
    = calloc(commandLineArguments.producerCount, sizeof(Thread*));

  if (producers == NULL) { goto error; }

  int threadId = 1;

  for (int32_t prod = 0; prod < commandLineArguments.producerCount; ++prod) {
    producers[prod] = producerCreate(
      ringBuffer, commandLineArguments.producerSleepTime, threadId);

    if (producers[prod] == NULL) { goto error; }

    ++threadId;
  }

  Thread** consumers
    = calloc(commandLineArguments.consumerCount, sizeof(Thread*));

  if (consumers == NULL) { goto error; }

  for (int32_t cons = 0; cons < commandLineArguments.consumerCount; ++cons) {
    consumers[cons] = consumerCreate(
      ringBuffer, commandLineArguments.consumerSleepTime, threadId);

    if (consumers[cons] == NULL) { goto error; }

    ++threadId;
  }

  int programExitStatus = EXIT_SUCCESS;

  for (int32_t cons = 0; cons < commandLineArguments.consumerCount; ++cons) {
    int        consumerExitStatus;
    const bool couldFree = threadFree(consumers[cons], &consumerExitStatus);

    if (couldFree) {
      if (consumerExitStatus != EXIT_SUCCESS) {
        fprintf(stderr, "Consumer exited with %d.", consumerExitStatus);
        programExitStatus = EXIT_FAILURE;
      }
    }
    else {
      fprintf(stderr, "Could not free consumer thread\n");
      programExitStatus = EXIT_FAILURE;
    }
  }

  free(consumers);

  for (int32_t prod = 0; prod < commandLineArguments.producerCount; ++prod) {
    int        producerExitStatus;
    const bool couldFree = threadFree(producers[prod], &producerExitStatus);

    if (couldFree) {
      if (producerExitStatus != EXIT_SUCCESS) {
        fprintf(stderr, "Producer exited with %d.", producerExitStatus);
        programExitStatus = EXIT_FAILURE;
      }
    }
    else {
      fprintf(stderr, "Could not free producer thread\n");
      programExitStatus = EXIT_FAILURE;
    }
  }

  free(producers);
  statusCode = ringBufferFree(ringBuffer);

  if (RB_FAILURE(statusCode)) {
    fprintf(
      stderr,
      "Could not free ring buffer: %s\n",
      ringBufferStatusCodeToString(statusCode));
    return EXIT_FAILURE;
  }

  return programExitStatus;

error:
  if (consumers != NULL) {
    for (int32_t cons = 0; cons < commandLineArguments.consumerCount; ++cons) {
      int        consumerExitStatus;
      const bool couldFree = threadFree(consumers[cons], &consumerExitStatus);

      if (couldFree) {
        if (consumerExitStatus != EXIT_SUCCESS) {
          fprintf(stderr, "Consumer exited with %d.", consumerExitStatus);
        }
      }
      else {
        fprintf(stderr, "Could not free consumer thread\n");
      }
    }
  }

  free(consumers);

  if (producers != NULL) {
    for (int32_t prod = 0; prod < commandLineArguments.producerCount; ++prod) {
      int        producerExitStatus;
      const bool couldFree = threadFree(producers[prod], &producerExitStatus);

      if (couldFree) {
        if (producerExitStatus != EXIT_SUCCESS) {
          fprintf(stderr, "Producer exited with %d.", producerExitStatus);
        }
      }
      else {
        fprintf(stderr, "Could not free producer thread\n");
      }
    }
  }

  free(producers);

  if (RB_FAILURE(statusCode)) {
    fprintf(
      stderr,
      "An error occurred: %s\n",
      ringBufferStatusCodeToString(statusCode));
    statusCode = ringBufferFree(ringBuffer);
  }

  if (RB_FAILURE(statusCode)) {
    fprintf(
      stderr,
      "Could not free ring buffer: %s\n",
      ringBufferStatusCodeToString(statusCode));
  }

  return EXIT_FAILURE;
}
