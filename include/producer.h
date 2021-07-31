#ifndef INCG_PRODUCER_H
#define INCG_PRODUCER_H
#include "thread.h"

/*!
 * \brief Creates a producer thread.
 * \param ringBuffer A pointer to the ring buffer that the producer should write
 *                   to.
 * \param sleepTimeSeconds The amount of seconds the producer should sleep
 *                         every iteration.
 * \param id The thread ID.
 * \return The thread created.
 * \warning The return value must be freed using `threadFree` when it is no
 *          longer needed.
 * \sa threadFree
 **/
Thread *
producerCreate(RingBuffer *ringBuffer, int32_t sleepTimeSeconds, int id);

#endif /* INCG_PRODUCER_H */
