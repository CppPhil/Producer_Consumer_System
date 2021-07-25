#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "main.h"

int main(void) { return f(); }

static void* threadFunction(void* arg)
{
  printf("Launched thread, arg: %d\n", (int)arg);
  return (void*)((int)arg + 5);
}

int f()
{
  printf("f called\n");

  pthread_t thread;
  const int statusCode
    = pthread_create(&thread, NULL, &threadFunction, (void*)1);

  if (statusCode != 0) {
    fprintf(stderr, "Could not create thread\n");
    return EXIT_FAILURE;
  }

  void* threadExitStatus = NULL;

  if (pthread_join(thread, &threadExitStatus) != 0) {
    fprintf(stderr, "Failure to join the thread.\n");
    return EXIT_FAILURE;
  }

  printf("Thread returned %d\n", (int)threadExitStatus);

  return EXIT_SUCCESS;
}
