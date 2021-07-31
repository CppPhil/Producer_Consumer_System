#include <stdbool.h>
#include <stdlib.h>

#include <pthread.h>

#include "ring_buffer.h"
#include "thread.h"

/*!
 * \brief Thread implementation type.
 **/
typedef struct {
    pthread_t       handle;         /*!< The pthread handle */
    bool            shouldShutDown; /*!< The shutdown state */
    pthread_mutex_t mutex;          /*!< Mutex to protect `shouldShutDown` */
} ThreadImpl;

/*!
 * \brief Argument to the actual thread function.
 **/
typedef struct {
    ThreadFunction function;         /*!< The thread function to run */
    RingBuffer *   ringBuffer;       /*!< The ring buffer */
    int32_t        sleepTimeSeconds; /*!< Sleep time */
    int            id;               /*!< The thread ID */
    Thread *       self;             /*!< Pointer to the thread itself */
} ThreadArgument;

/*!
 * \brief Creates a thread argument.
 * \param function The thread function.
 * \param ringBuffer The ring buffer.
 * \param sleepTimeSeconds The sleep time.
 * \param id The thread ID.
 * \param self The thread itself.
 * \return The thread argument created on success; otherwise NULL.
 **/
static ThreadArgument *threadArgumentCreate(
    ThreadFunction function,
    RingBuffer *   ringBuffer,
    int32_t        sleepTimeSeconds,
    int            id,
    Thread *       self)
{
    ThreadArgument *argument = malloc(sizeof(ThreadArgument));

    if (argument == NULL) {
        return NULL;
    }

    argument->function         = function;
    argument->ringBuffer       = ringBuffer;
    argument->sleepTimeSeconds = sleepTimeSeconds;
    argument->id               = id;
    argument->self             = self;

    return argument;
}

/*!
 * \brief Frees a thread argument.
 * \param argument The thread argument to free.
 **/
static void threadArgumentFree(ThreadArgument *argument)
{
    free(argument);
}

static Thread *opaque(ThreadImpl *thread)
{
    return (Thread *) thread;
}

static ThreadImpl *impl(Thread *thread)
{
    return (ThreadImpl *) thread;
}

/*!
 * \brief The actual thread routine.
 * \param argument The void* argument.
 * \return The void* return value.
 **/
static void *startRoutine(void *argument)
{
    ThreadArgument *arg = (ThreadArgument *) argument;

    // Run the thread function.
    const int threadExitStatus = arg->function(
        arg->ringBuffer, arg->sleepTimeSeconds, arg->id, arg->self);

    threadArgumentFree(arg);
    return (void *) threadExitStatus;
}

Thread *threadCreate(
    ThreadFunction function,
    RingBuffer *   ringBuffer,
    int32_t        sleepTimeSeconds,
    int            id)
{
    ThreadImpl *thread = malloc(sizeof(ThreadImpl));

    if (thread == NULL) {
        return NULL;
    }

    ThreadArgument *argument = threadArgumentCreate(
        function, ringBuffer, sleepTimeSeconds, id, opaque(thread));

    if (argument == NULL) {
        free(thread);
        return NULL;
    }

    if (pthread_create(&thread->handle, NULL, &startRoutine, argument) != 0) {
        threadArgumentFree(argument);
        free(thread);
        return NULL;
    }

    thread->shouldShutDown = false;

    if (pthread_mutex_init(&thread->mutex, NULL) != 0) {
        threadArgumentFree(argument);
        free(thread);
        return NULL;
    }

    return opaque(thread);
}

bool threadFree(Thread *thread, int *threadExitStatus)
{
    ThreadImpl *thr = impl(thread);
    void *      exitStatus;

    if (pthread_join(thr->handle, &exitStatus) != 0) {
        pthread_mutex_destroy(&thr->mutex);
        free(thr);
        return false;
    }

    if (pthread_mutex_destroy(&thr->mutex) != 0) {
        free(thr);
        return false;
    }

    free(thr);
    *threadExitStatus = (int) exitStatus;
    return true;
}

bool threadRequestShutdown(Thread *thread)
{
    ThreadImpl *thr = impl(thread);

    if (pthread_mutex_lock(&thr->mutex) != 0) {
        return false;
    }

    thr->shouldShutDown = true;

    if (pthread_mutex_unlock(&thr->mutex) != 0) {
        return false;
    }

    return true;
}

bool threadShouldShutdown(Thread *thread, bool *shouldShutDown)
{
    ThreadImpl *thr = impl(thread);

    if (pthread_mutex_lock(&thr->mutex) != 0) {
        return false;
    }

    const bool currentShouldShutdownState = thr->shouldShutDown;

    if (pthread_mutex_unlock(&thr->mutex) != 0) {
        return false;
    }

    *shouldShutDown = currentShouldShutdownState;
    return true;
}
