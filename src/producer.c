#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "byte.h"
#include "producer.h"
#include "ring_buffer.h"
#include "sleep_thread.h"

/*!
 * \brief Converts a character to its upper case variant.
 * \param character The character to get the upper case variant of.
 * \return The character in its upper case variant.
 * \note Assummes ASCII encoding.
 **/
static char toUpper(char character)
{
  unsigned char uc = (unsigned char)character;
  uc &= ~0x20; // Turn off the 3rd most significant bit.
  return (char)uc;
}

/*!
 * \brief The thread function for the producers.
 * \param ringBuffer The ring buffer to write to.
 * \param sleepTimeSeconds The count of seconds to sleep for every iteration.
 * \param id The thread ID.
 * \param self The thread itself.
 **/
static int producerThreadFunction(
  RingBuffer* ringBuffer,
  int32_t     sleepTimeSeconds,
  int         id,
  Thread*     self)
{
  // The (lower case) English alphabet.
  static const char   alphabet[]   = "abcdefghijklmnopqrstuvwxyz";
  static const size_t alphabetSize = sizeof(alphabet) - 1;

  size_t index = 0;

  for (;;) {
    bool       shouldShutdown;
    const bool ok = threadShouldShutdown(self, &shouldShutdown);

    // Exit with failure if we couldn't determine the thread shut down state.
    if (!ok) { return EXIT_FAILURE; }

    // Exit when we should shut down.
    if (shouldShutdown) { break; }

    // If the thread ID is an odd number use upper case letters.
    const byte byteToWrite
      = (id & 1) == 0 ? alphabet[index] : toUpper(alphabet[index]);

    const RingBufferStatusCode statusCode
      = ringBufferWrite(ringBuffer, byteToWrite, id, self);

    // If shutdown is requested and the thread was sleeping in the condition variable
    // of the ring buffer, then it will return with RB_THREAD_SHOULD_SHUTDOWN
    if (statusCode == RB_THREAD_SHOULD_SHUTDOWN) { break; }

    // On failure -> exit with failure.
    if (RB_FAILURE(statusCode)) { return EXIT_FAILURE; }

    printf("Producer (tid: %d) just wrote %c.\n", id, byteToWrite);

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
