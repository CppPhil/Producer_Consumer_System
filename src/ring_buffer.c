#include <stdlib.h>

#include <pthread.h>

#include "ring_buffer.h"

// TODO: make this thread safe.

const char* ringBufferStatesCodeToString(RingBufferStatusCode statusCode)
{
  switch (statusCode) {
  case RB_OK: return "The operation completed successfully.";
  case RB_NOMEM: return "Could not allocate memory.";
  case RB_FAILURE_TO_INIT_MUTEX: return "Could not initialize mutex.";
  case RB_FAILURE_TO_DESTROY_MUTEX: return "Could not destroy mutex.";
  case RB_FAILURE_TO_LOCK_MUTEX: return "Could not lock mutex.";
  case RB_FAILURE_TO_UNLOCK_MUTEX: return "Could not unlock mutex.";
  case RB_FAILURE_TO_INIT_CONDVAR: return "Could not initialize condition variable.";
  case RB_FAILURE_TO_DESTROY_CONDVAR: return "Could not destroy condition variable.";
  case RB_FAILURE_TO_WAIT_ON_CONDVAR: return "Could not wait on condition variable.";
  case RB_FAILURE_TO_SIGNAL_CONDVAR: return "Could not signal condition variable.";
  default: break;
  }

  return "An unknown error occurred.";
}

typedef struct 
{
  byte* buffer; /*!< Buffer to hold the data written by the threads */
  size_t bufferSize; /*!< Size of the buffer in bytes */
  byte* in; /*!< The write pointer used to write to the buffer */
  byte* out; /*!< The read pointer used to read from the buffer */
  size_t count; /*!< Count representing the amount of bytes still to be read from the buffer */
  pthread_mutex_t mutex;
  pthread_cond_t conditionVariable;
} RingBufferImpl;

static RingBufferImpl* impl(RingBuffer* rb)
{
  return (RingBufferImpl*)rb;
}

static RingBuffer* opaque(RingBufferImpl* rb)
{
  return (RingBuffer*)rb;
}

RingBufferStatusCode ringBufferCreate(size_t byteCount, RingBuffer** ringBuffer)
{
  RingBufferImpl* rb = malloc(sizeof(RingBufferImpl));

  if (rb == NULL) {
    return RB_NOMEM;
  }

  rb->buffer = calloc(byteCount, 1);

  if (rb->buffer == NULL) {
    free(rb);
    return RB_NOMEM;
  }

  rb->bufferSize = byteCount;
  rb->in = rb->buffer;
  rb->out = rb->buffer;
  rb->count = 0;

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

  if (rb == NULL) {
    return;
  }

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
  if (*ptr == (rb->buffer + rb->bufferSize - 1)) {
    *ptr = rb->buffer;
  } else {
    ++*ptr;
  }
}

RingBufferStatusCode ringBufferWrite(RingBuffer* ringBuffer, byte toWrite)
{
  RingBufferImpl* rb = impl(ringBuffer);

  if (pthread_mutex_lock(&rb->mutex) != 0) {
    return RB_FAILURE_TO_LOCK_MUTEX;
  }

  while (rb->count >= rb->bufferSize) {
    if (pthread_cond_wait(&rb->conditionVariable, &rb->mutex) != 0) {
      return RB_FAILURE_TO_WAIT_ON_CONDVAR;
    }
  }

  rb->in = toWrite;
  ++rb->count;
  advancePointer(rb, &rb->in);

  if (pthread_mutex_unlock(&rb->mutex) != 0) {
    return RB_FAILURE_TO_UNLOCK_MUTEX;
  }

  if (pthread_cond_broadcast(&rb->conditionVariable) != 0) {
    return RB_FAILURE_TO_SIGNAL_CONDVAR;
  }

  return RB_OK;
}

RingBufferStatusCode ringBufferRead(RingBuffer* ringBuffer, byte* byteRead)
{
  RingBufferImpl* rb = impl(ringBuffer);

  if (pthread_mutex_lock(&rb->mutex) != 0) {
    return RB_FAILURE_TO_LOCK_MUTEX;
  }

  while (rb->count == 0) {
    if (pthread_cond_wait(&rb->conditionVariable, &rb->mutex) != 0) {
      return RB_FAILURE_TO_WAIT_ON_CONDVAR;
    }
  }

  const byte byteJustRead = *rb->out;
  --rb->count;
  advancePointer(rb, &rb->out);

  if (pthread_mutex_unlock(&rb->mutex) != 0) {
    return RB_FAILURE_TO_UNLOCK_MUTEX;
  }

  if (pthread_cond_broadcast(&rb->conditionVariable) != 0) {
    return RB_FAILURE_TO_SIGNAL_CONDVAR;
  }

  *byteRead = byteJustRead;
  return RB_OK; 
}
