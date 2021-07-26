#ifndef INCG_THREAD_H
#define INCG_THREAD_H
#include <stdbool.h>
#include <stdint.h>

typedef struct RingBufferOpaque RingBuffer;

typedef struct ThreadOpaque Thread;

typedef int (*ThreadFunction)(
  RingBuffer* ringBuffer,
  int32_t     sleepTimeSeconds,
  int         id,
  Thread*     self);

/*!
 * \brief Creates a thread.
 * \param function The function that the thread will run.
 * \param ringBuffer The ring buffer.
 * \param sleepTimeSeconds The sleep time.
 * \param id The thread ID.
 **/
Thread* threadCreate(
  ThreadFunction function,
  RingBuffer*    ringBuffer,
  int32_t        sleepTimeSeconds,
  int            id);

/*!
 * \brief Frees the given thread.
 * \param thread The thread to free.
 * \param threadExitStatus Output parameter to write the thread's exit status code to.
 * \return true on success; false otherwise.
 * 
 * Joins and then frees the given thread.
 **/
bool threadFree(Thread* thread, int* threadExitStatus);

/*!
 * \brief Request shutdown of a thread.
 * \param thread The thread that should shut down.
 * \return true on success; otherwise false.
 **/
bool threadRequestShutdown(Thread* thread);

/*!
 * \brief Query a thread's shutdown state.
 * \param thread The thread to get the shutdown state of.
 * \param shouldShutDown will be true if the thread should shut down; otherwise false. Will only be valid if true is returned.
 * \return true on success; otherwise false.
 **/
bool threadShouldShutdown(Thread* thread, bool* shouldShutDown);
#endif /* INCG_THREAD_H */
