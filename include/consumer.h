#ifndef INCG_CONSUMER_H
#define INCG_CONSUMER_H
#include "thread.h"

Thread*
consumerCreate(RingBuffer* ringBuffer, int32_t sleepTimeSeconds, int id);
#endif /* INCG_CONSUMER_H */