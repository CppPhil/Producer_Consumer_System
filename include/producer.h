#ifndef INCG_PRODUCER_H
#define INCG_PRODUCER_H
#include "thread.h"

Thread* producerCreate(RingBuffer* ringBuffer, int32_t sleepTimeSeconds);

#endif /* INCG_PRODUCER_H */
