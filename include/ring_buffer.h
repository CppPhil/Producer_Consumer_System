#ifndef INCG_INCG_RING_BUFFER_H
#define INCG_INCG_RING_BUFFER_H
#include <stddef.h>

#include "byte.h"
#include "thread.h"

/*!
 * \brief Status codes returned by the ring buffer functions.
 **/
typedef enum {
    RB_OK, /*!< Indicates no failure */
    RB_NOMEM,
    RB_FAILURE_TO_INIT_MUTEX,
    RB_FAILURE_TO_DESTROY_MUTEX,
    RB_FAILURE_TO_LOCK_MUTEX,
    RB_FAILURE_TO_UNLOCK_MUTEX,
    RB_FAILURE_TO_INIT_CONDVAR,
    RB_FAILURE_TO_DESTROY_CONDVAR,
    RB_FAILURE_TO_WAIT_ON_CONDVAR,
    RB_FAILURE_TO_SIGNAL_CONDVAR,
    RB_THREAD_SHOULD_SHUTDOWN,
    RB_FAILURE_TO_DETERMINE_SHUTDOWN_STATE
} RingBufferStatusCode;

/*!
 * \def RB_SUCCESS
 * \brief Checks if a `RingBufferStatusCode` indicates success.
 **/
#define RB_SUCCESS(enm) ((enm) == RB_OK)

/*!
 * \def RB_FAILURE
 * \brief Checks if a `RingBufferStatusCode` indicates failure.
 **/
#define RB_FAILURE(enm) ((enm) != RB_OK)

/*!
 * \brief Converts a `RingBufferStatusCode` to a string.
 * \param statusCode The enumerator to convert.
 * \return The corresponding string.
 **/
const char *ringBufferStatusCodeToString(RingBufferStatusCode statusCode);

typedef struct RingBufferOpaque RingBuffer;

/*!
 * \brief Creates a ring buffer.
 * \param byteCount The size of the ring buffer in bytes.
 * \param ringBuffer Output parameter to write the ring buffer to.
 * \return The status code.
 * \warning The ring buffer must be freed using `ringBufferFree`
 * \sa ringBufferFree
 **/
RingBufferStatusCode ringBufferCreate(
    size_t       byteCount,
    RingBuffer **ringBuffer);

/*!
 * \brief Frees a ring buffer.
 * \param ringBuffer The ring buffer to free.
 * \return The status code.
 **/
RingBufferStatusCode ringBufferFree(RingBuffer *ringBuffer);

/*!
 * \brief Writes to the ring buffer.
 * \param ringBuffer The ring buffer to write to.
 * \param toWrite The byte to write.
 * \param threadId The thread ID of the thread that wants to write.
 * \param self Pointer to the thread trying to write.
 * \return The status code.
 **/
RingBufferStatusCode ringBufferWrite(
    RingBuffer *ringBuffer,
    byte        toWrite,
    int         threadId,
    Thread *    self);

/*!
 * \brief Reads from the ring buffer.
 * \param ringBuffer The ring buffer to read from.
 * \param byteRead Output parameter for the byte to read into.
 * \param threadId The thread ID of the thread trying to read.
 * \param self Pointer to the thread trying to read.
 * \return The status code.
 **/
RingBufferStatusCode ringBufferRead(
    RingBuffer *ringBuffer,
    byte *      byteRead,
    int         threadId,
    Thread *    self);

/*!
 * \brief Function used by the main thread to shut down the ring buffer.
 * \param ringBuffer The ring buffer to shut down.
 * \return The status code.
 *
 * The main thread will call this function to have the threads wake up
 * that are waiting on the condition variable of the ring buffer
 * so that they exit.
 **/
RingBufferStatusCode ringBufferShutdown(RingBuffer *ringBuffer);
#endif /* INCG_INCG_RING_BUFFER_H */
