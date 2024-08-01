/*
 * Goal:
 * The main thread is going to create two child threads
 * There is a global variable named count initialized to 0
 * Each thread will update count n times (n > 0) and exit
 * where n is provided as a command line argument
 * The main thread will exit only after both child threads have completed
 * Compile using: cc thread.c -o thread -pthread
 * Run using: ./thread n
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

volatile int count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *incrementCount(void *input)
{
  int index = 0;
  int *pointerToInt = (int *)input;
  int numIncrements = *pointerToInt;
  int status;

  for (index = 0; index < numIncrements; index++)
  {
    status = pthread_mutex_lock(&mutex);
    if (status != 0)
    {
      printf("Failed to lock mutex.\n");
      exit(EXIT_FAILURE);
    }

    count++;

    status = pthread_mutex_unlock(&mutex);
    if (status != 0)
    {
      printf("Failed to unlock mutex.\n");
      exit(EXIT_FAILURE);
    }
  }

  return NULL;
}

int main(int argc, char *argv[])
{
  pthread_t childThread1, childThread2;
  void *result1;
  void *result2;
  int status;
  int numIncrements;

  /* Command line validation */
  if (argc != 2 ||
      strcmp(argv[1], "-h") == 0 ||
      strcmp(argv[1], "--help") == 0)
  {
    printf("Usage: %s numIncrements\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  numIncrements = atoi(argv[1]);
  if (numIncrements <= 0)
  {
    printf("numIncrements must be positive.\n");
    exit(EXIT_FAILURE);
  }

  /* Create the two child threads */
  status = pthread_create(
      &childThread1,
      NULL,
      incrementCount,
      (void *)&numIncrements);
  if (status != 0)
  {
    printf("Failed to create child thread.\n");
    exit(EXIT_FAILURE);
  }

  status = pthread_create(
      &childThread2,
      NULL,
      incrementCount,
      (void *)&numIncrements);
  if (status != 0)
  {
    printf("Failed to create child thread.\n");
    exit(EXIT_FAILURE);
  }

  /* Wait for the child threads to complete */
  status = pthread_join(
      childThread1,
      &result1);
  if (status != 0)
  {
    printf("Failed to wait for first child thread.\n");
    exit(EXIT_FAILURE);
  }

  status = pthread_join(
      childThread2,
      &result2);
  if (status != 0)
  {
    printf("Failed to wait for first child thread.\n");
    exit(EXIT_FAILURE);
  }

  printf("Final value of count = %d\n", count);

  exit(EXIT_SUCCESS);
}
