#include <stdlib.h>

#include <pthread.h>

#include "thread.h"

typedef struct {
  pthread_t handle;
} ThreadImpl;

typedef struct {
  ThreadFunction function;
  RingBuffer*    ringBuffer;
  int32_t        sleepTimeSeconds;
} ThreadArgument;

static ThreadArgument* threadArgumentCreate(
  ThreadFunction function,
  RingBuffer*    ringBuffer,
  int32_t        sleepTimeSeconds)
{
  ThreadArgument* argument = malloc(sizeof(ThreadArgument));

  if (argument == NULL) { return NULL; }

  argument->function         = function;
  argument->ringBuffer       = ringBuffer;
  argument->sleepTimeSeconds = sleepTimeSeconds;

  return argument;
}

static void threadArgumentFree(ThreadArgument* argument) { free(argument); }

static Thread* opaque(ThreadImpl* thread) { return (Thread*)thread; }

static ThreadImpl* impl(Thread* thread) { return (ThreadImpl*)thread; }

static void* startRoutine(void* argument)
{
  ThreadArgument* arg = (ThreadArgument*)argument;
  const int       threadExitStatus
    = arg->function(arg->ringBuffer, arg->sleepTimeSeconds);
  threadArgumentFree(arg);
  return (void*)threadExitStatus;
}

Thread* threadCreate(
  ThreadFunction function,
  RingBuffer*    ringBuffer,
  int32_t        sleepTimeSeconds)
{
  ThreadImpl* thread = malloc(sizeof(ThreadImpl));

  if (thread == NULL) { return NULL; }

  ThreadArgument* argument
    = threadArgumentCreate(function, ringBuffer, sleepTimeSeconds);

  if (argument == NULL) {
    free(thread);
    return NULL;
  }

  if (pthread_create(&thread->handle, NULL, &startRoutine, argument) != 0) {
    threadArgumentFree(argument);
    free(thread);
    return NULL;
  }

  return opaque(thread);
}

bool threadFree(Thread* thread, int* threadExitStatus)
{
  ThreadImpl* thr = impl(thread);
  void*       exitStatus;

  if (pthread_join(thr->handle, &exitStatus) != 0) { return false; }

  free(thr);
  *threadExitStatus = (int)exitStatus;
  return true;
}
