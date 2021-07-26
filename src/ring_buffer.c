#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "ring_buffer.h"

#ifdef RB_IO
#define RB_PRINTLN(fmtStr, ...) printf("RingBuffer: " fmtStr "\n", __VA_ARGS__)
#else
#define RB_PRINTLN(...) (void)(__VA_ARGS__)
#endif

const char* ringBufferStatusCodeToString(RingBufferStatusCode statusCode)
{
  switch (statusCode) {
  case RB_OK: return "The operation completed successfully.";
  case RB_NOMEM: return "Could not allocate memory.";
  case RB_FAILURE_TO_INIT_MUTEX: return "Could not initialize mutex.";
  case RB_FAILURE_TO_DESTROY_MUTEX: return "Could not destroy mutex.";
  case RB_FAILURE_TO_LOCK_MUTEX: return "Could not lock mutex.";
  case RB_FAILURE_TO_UNLOCK_MUTEX: return "Could not unlock mutex.";
  case RB_FAILURE_TO_INIT_CONDVAR:
    return "Could not initialize condition variable.";
  case RB_FAILURE_TO_DESTROY_CONDVAR:
    return "Could not destroy condition variable.";
  case RB_FAILURE_TO_WAIT_ON_CONDVAR:
    return "Could not wait on condition variable.";
  case RB_FAILURE_TO_SIGNAL_CONDVAR:
    return "Could not signal condition variable.";
  case RB_THREAD_SHOULD_SHUTDOWN:
    return "The thread operating on this ring buffer should shut down.";
  case RB_FAILURE_TO_DETERMINE_SHUTDOWN_STATE:
    return "Could not determine shutdown state for the thread operating on "
           "this ring buffer.";
  default: break;
  }

  return "An unknown error occurred.";
}

typedef struct {
  byte*  buffer;     /*!< Buffer to hold the data written by the threads */
  size_t bufferSize; /*!< Size of the buffer in bytes */
  byte*  in;         /*!< The write pointer used to write to the buffer */
  byte*  out;        /*!< The read pointer used to read from the buffer */
  size_t count; /*!< Count representing the amount of bytes still to be read
                 *   from the buffer
                 **/
  pthread_mutex_t mutex;
  pthread_cond_t  conditionVariable;
} RingBufferImpl;

static RingBufferImpl* impl(RingBuffer* rb) { return (RingBufferImpl*)rb; }

static RingBuffer* opaque(RingBufferImpl* rb) { return (RingBuffer*)rb; }

RingBufferStatusCode ringBufferCreate(size_t byteCount, RingBuffer** ringBuffer)
{
  RingBufferImpl* rb = malloc(sizeof(RingBufferImpl));

  if (rb == NULL) { return RB_NOMEM; }

  rb->buffer = calloc(byteCount, 1);

  if (rb->buffer == NULL) {
    free(rb);
    return RB_NOMEM;
  }

  rb->bufferSize = byteCount;
  rb->in         = rb->buffer;
  rb->out        = rb->buffer;
  rb->count      = 0;

  if (pthread_mutex_init(&rb->mutex, NULL) != 0) {
    free(rb->buffer);
    free(rb);
    return RB_FAILURE_TO_INIT_MUTEX;
  }

  if (pthread_cond_init(&rb->conditionVariable, NULL) != 0) {
    free(rb->buffer);
    free(rb);
    return RB_FAILURE_TO_INIT_CONDVAR;
  }

  *ringBuffer = opaque(rb);
  return RB_OK;
}

RingBufferStatusCode ringBufferFree(RingBuffer* ringBuffer)
{
  RingBufferImpl* rb = impl(ringBuffer);

  if (rb == NULL) { return RB_OK; }

  if (pthread_mutex_destroy(&rb->mutex) != 0) {
    free(rb->buffer);
    free(rb);
    return RB_FAILURE_TO_DESTROY_MUTEX;
  }

  if (pthread_cond_destroy(&rb->conditionVariable) != 0) {
    free(rb->buffer);
    free(rb);
    return RB_FAILURE_TO_DESTROY_CONDVAR;
  }

  free(rb->buffer);
  free(rb);
  return RB_OK;
}

static void advancePointer(RingBufferImpl* rb, byte** ptr)
{
  if (*ptr == (rb->buffer + rb->bufferSize - 1)) { *ptr = rb->buffer; }
  else {
    ++*ptr;
  }
}

RingBufferStatusCode ringBufferWrite(
  RingBuffer* ringBuffer,
  byte        toWrite,
  int         threadId,
  Thread*     self)
{
  RingBufferImpl* rb = impl(ringBuffer);

  if (pthread_mutex_lock(&rb->mutex) != 0) { return RB_FAILURE_TO_LOCK_MUTEX; }

  RB_PRINTLN(
    "Producer (tid: %d) got the mutex and tries to write %c",
    threadId,
    toWrite);

  while (rb->count >= rb->bufferSize) {
    bool       shouldShutdown;
    const bool ok = threadShouldShutdown(self, &shouldShutdown);

    if (!ok) {
      if (pthread_mutex_unlock(&rb->mutex) != 0) {
        return RB_FAILURE_TO_UNLOCK_MUTEX;
      }

      return RB_FAILURE_TO_DETERMINE_SHUTDOWN_STATE;
    }

    if (shouldShutdown) {
      if (pthread_mutex_unlock(&rb->mutex) != 0) {
        return RB_FAILURE_TO_UNLOCK_MUTEX;
      }

      return RB_THREAD_SHOULD_SHUTDOWN;
    }

    RB_PRINTLN(
      "Producer (tid: %d) has to wait for space to become free, trying to "
      "write %c",
      threadId,
      toWrite);

    if (pthread_cond_wait(&rb->conditionVariable, &rb->mutex) != 0) {
      return RB_FAILURE_TO_WAIT_ON_CONDVAR;
    }
  }

  *rb->in = toWrite;
  ++rb->count;
  advancePointer(rb, &rb->in);

  RB_PRINTLN(
    "Producer (tid: %d) incremented count. There are now %zu bytes to read.",
    threadId,
    rb->count);

  if (pthread_mutex_unlock(&rb->mutex) != 0) {
    return RB_FAILURE_TO_UNLOCK_MUTEX;
  }

  if (pthread_cond_broadcast(&rb->conditionVariable) != 0) {
    return RB_FAILURE_TO_SIGNAL_CONDVAR;
  }

  RB_PRINTLN(
    "Producer (tid: %d): Write done. Broadcast condition variable", threadId);

  return RB_OK;
}

RingBufferStatusCode ringBufferRead(
  RingBuffer* ringBuffer,
  byte*       byteRead,
  int         threadId,
  Thread*     self)
{
  RingBufferImpl* rb = impl(ringBuffer);

  if (pthread_mutex_lock(&rb->mutex) != 0) { return RB_FAILURE_TO_LOCK_MUTEX; }

  RB_PRINTLN("Consumer (tid: %d) got the mutex and tries to read.", threadId);

  while (rb->count == 0) {
    bool       shouldShutdown;
    const bool ok = threadShouldShutdown(self, &shouldShutdown);

    if (!ok) {
      if (pthread_mutex_unlock(&rb->mutex) != 0) {
        return RB_FAILURE_TO_UNLOCK_MUTEX;
      }

      return RB_FAILURE_TO_DETERMINE_SHUTDOWN_STATE;
    }

    if (shouldShutdown) {
      if (pthread_mutex_unlock(&rb->mutex) != 0) {
        return RB_FAILURE_TO_UNLOCK_MUTEX;
      }

      return RB_THREAD_SHOULD_SHUTDOWN;
    }

    RB_PRINTLN(
      "Consumer (tid: %d) has to wait for data to be written while trying to "
      "read.",
      threadId);

    if (pthread_cond_wait(&rb->conditionVariable, &rb->mutex) != 0) {
      return RB_FAILURE_TO_WAIT_ON_CONDVAR;
    }
  }

  const byte byteJustRead = *rb->out;
  --rb->count;
  advancePointer(rb, &rb->out);

  RB_PRINTLN(
    "Consumer (tid: %d) decremented count. There are now %zu bytes to read.",
    threadId,
    rb->count);

  if (pthread_mutex_unlock(&rb->mutex) != 0) {
    return RB_FAILURE_TO_UNLOCK_MUTEX;
  }

  if (pthread_cond_broadcast(&rb->conditionVariable) != 0) {
    return RB_FAILURE_TO_SIGNAL_CONDVAR;
  }

  RB_PRINTLN(
    "Consumer (tid: %d): Read %c. Broadcast condition variable.",
    threadId,
    byteJustRead);

  *byteRead = byteJustRead;
  return RB_OK;
}

RingBufferStatusCode ringBufferShutdown(RingBuffer* ringBuffer)
{
  RingBufferImpl* rb = impl(ringBuffer);

  if (pthread_cond_broadcast(&rb->conditionVariable) != 0) {
    return RB_FAILURE_TO_SIGNAL_CONDVAR;
  }

  return RB_OK;
}
