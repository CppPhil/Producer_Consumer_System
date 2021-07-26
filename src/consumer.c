#include <stdio.h>
#include <stdlib.h>

#include "byte.h"
#include "consumer.h"
#include "ring_buffer.h"
#include "sleep_thread.h"

/*!
 * \brief The thread function for the consumer threads.
 * \param ringBuffer The ring buffer to use.
 * \param sleepTimeSeconds The seconds to sleep for every iteration.
 * \param id The thread ID.
 * \param self A pointer to the thread itself.
 **/
static int consumerThreadFunction(
  RingBuffer* ringBuffer,
  int32_t     sleepTimeSeconds,
  int         id,
  Thread*     self)
{
  for (;;) {
    bool       shouldShutdown;
    const bool ok = threadShouldShutdown(self, &shouldShutdown);

    // If we couldn't determine the shutdown status -> exit with error.
    if (!ok) { return EXIT_FAILURE; }

    // Shut down if we should shut down.
    if (shouldShutdown) { break; }

    byte                       byteJustRead;
    const RingBufferStatusCode statusCode
      = ringBufferRead(ringBuffer, &byteJustRead, id, self);

    // If the thread sleeps in the condition variable but it is woken up
    // because we're shutting down RB_THREAD_SHOULD_SHUTDOWN is returned.
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