#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmd_args.h"
#include "consumer.h"
#include "producer.h"
#include "ring_buffer.h"
#include "sleep_thread.h"

/*!
 * \brief Function to free threads (producers or consumers).
 * \param threads The array of threads to free.
 * \param elementCount The size of `threads` in elements.
 * \param exitStatusNonSuccessfulErrorMessage The error message to print when a thread indicates failure through its exit status code.
 * \param failureToFreeErrorMessage The error message to print when freeing a thread fails.
 * \return true on success; otherwise false.
 **/
static bool freeThreads(
  Thread**    threads,
  int32_t     elementCount,
  const char* exitStatusNonSuccessfulErrorMessage,
  const char* failureToFreeErrorMessage)
{
  // If the pointer is null -> do nothing (that's okay)
  if (threads == NULL) { return true; }

  bool success = true;

  for (int32_t i = 0; i < elementCount; ++i) {
    int        threadExitStatus;
    const bool couldFree = threadFree(threads[i], &threadExitStatus);

    if (couldFree) {
      if (threadExitStatus != EXIT_SUCCESS) {
        fprintf(
          stderr,
          "%s %d.\n",
          exitStatusNonSuccessfulErrorMessage,
          threadExitStatus);
        success = false;
      }
    }
    else {
      fprintf(stderr, "%s\n", failureToFreeErrorMessage);
      success = false;
    }
  }

  free(threads);

  return success;
}

/*!
 * \brief Global variable that will hold the last signal emitted.
 *        Should only ever be 0 (the default value) or SIGINT
 *        which is the only signal for which the signal handler
 *        shall be registered.
 **/
volatile sig_atomic_t gSignalStatus = 0;

/*!
 * \brief The signal handler for this application.
 * \param signal The signal that was emitted (should be SIGINT).
 **/
static void signalHandler(int signal) { gSignalStatus = signal; }

/*!
 * \brief The entry point of this application.
 * \param argc The count of command line arguments.
 * \param argv The command line arguments.
 * \return EXIT_SUCCESS on success; otherwise EXIT_FAILURE.
 **/
int main(int argc, char** argv)
{
  // Install the signal handler for SIGINT (e.g., when CTRL + C is pressed)
  signal(SIGINT, &signalHandler);

  const CmdArgs commandLineArguments = parseCmdArgs(argc, argv);

  if (!commandLineArguments.isOk) { return EXIT_FAILURE; }

  Thread**             producers      = NULL;
  Thread**             consumers      = NULL;
  RingBuffer*          ringBuffer     = NULL;
  const size_t         ringBufferSize = 10;
  RingBufferStatusCode statusCode
    = ringBufferCreate(ringBufferSize, &ringBuffer);

  if (RB_FAILURE(statusCode)) { goto error; }

  producers = calloc(commandLineArguments.producerCount, sizeof(Thread*));

  if (producers == NULL) { goto error; }

  int threadId = 1;

  for (int32_t prod = 0; prod < commandLineArguments.producerCount; ++prod) {
    producers[prod] = producerCreate(
      ringBuffer, commandLineArguments.producerSleepTime, threadId);

    if (producers[prod] == NULL) { goto error; }

    ++threadId;
  }

  consumers = calloc(commandLineArguments.consumerCount, sizeof(Thread*));

  if (consumers == NULL) { goto error; }

  for (int32_t cons = 0; cons < commandLineArguments.consumerCount; ++cons) {
    consumers[cons] = consumerCreate(
      ringBuffer, commandLineArguments.consumerSleepTime, threadId);

    if (consumers[cons] == NULL) { goto error; }

    ++threadId;
  }

  // Have the main thread wait for SIGINT to be emitted.
  while (gSignalStatus != SIGINT) { sleepThread(/* seconds */ 1); }

  // When the user has pressed CTRL + C -> shutdown the threads.
  printf("Shutdown of threads was requested.\n");

  bool couldShutdownThreads = true;

  // Request the producers to shut down.
  for (int32_t prod = 0; prod < commandLineArguments.producerCount; ++prod) {
    couldShutdownThreads = threadRequestShutdown(producers[prod]);
  }

  // Request the consumers to shut down.
  for (int32_t cons = 0; cons < commandLineArguments.consumerCount; ++cons) {
    couldShutdownThreads = threadRequestShutdown(consumers[cons]);
  }

  // Tell the ring buffer to shut down.
  // This will wake all threads sleeping on the ring buffer's condition variable
  // and tell them to shut down.
  statusCode = ringBufferShutdown(ringBuffer);

  if (RB_FAILURE(statusCode)) { goto error; }

  if (!couldShutdownThreads) { goto error; }

  int programExitStatus = EXIT_SUCCESS;

  if (!freeThreads(
        consumers,
        commandLineArguments.consumerCount,
        "Consumer exited with",
        "Could not free consumer thread")) {
    programExitStatus = EXIT_FAILURE;
  }

  if (!freeThreads(
        producers,
        commandLineArguments.producerCount,
        "Producer exited with",
        "Could not free producer thread")) {
    programExitStatus = EXIT_FAILURE;
  }

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
  freeThreads(
    consumers,
    commandLineArguments.consumerCount,
    "Consumer exited with",
    "Could not free consumer thread");
  freeThreads(
    producers,
    commandLineArguments.producerCount,
    "Producer exited with",
    "Could not free producer thread");

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
