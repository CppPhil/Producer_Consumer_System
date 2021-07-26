#ifndef INCG_CONSUMER_H
#define INCG_CONSUMER_H
#include "thread.h"

/*!
 * \brief Creates a consumer thread.
 * \param ringBuffer A pointer to the ring buffer that the consumer should use.
 * \param sleepTimeSeconds How many seconds the consumer should sleep every
 *                         iteration.
 * \param id The thread ID of the consumer thread to create.
 * \return The thread created.
 * \warning The return value must be freed using `threadFree`
 *          when it is no longer needed.
 * \sa threadFree
 **/
Thread*
consumerCreate(RingBuffer* ringBuffer, int32_t sleepTimeSeconds, int id);
#endif /* INCG_CONSUMER_H */
