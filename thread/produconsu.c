#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

#define MAX_BUFFER_SIZE 5
#define TRUE 1
#define FALSE 0

volatile int numberOfUnits = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer(void *input)
{
  int status;
  while (TRUE)
  {
    status = pthread_mutex_lock(&mutex);
    if (status != 0)
    {
      printf("producer lock failed!\n");
      exit(1);
    }
    if (numberOfUnits < MAX_BUFFER_SIZE)
    {
      numberOfUnits++;
      printf("produce increase to %d.\n", numberOfUnits);
    }
    else
    {
      printf("the buffer is full, producer waiting ...\n");
    }
    status = pthread_mutex_unlock(&mutex);
    if (status != 0)
    {
      printf("producer lock failed!\n");
      exit(1);
    }
    sleep(1);
  }
  return NULL;
}

void *consumer(void *input)
{
  int status;
  while (TRUE)
  {
    status = pthread_mutex_lock(&mutex);
    if (status != 0)
    {
      printf("consumer lock failed!\n");
      exit(1);
    }
    if (numberOfUnits > 0)
    {
      numberOfUnits--;
      printf("consumer decrease to %d.\n", numberOfUnits);
    }
    else
    {
      printf("the buffer is empty, consumer waiting ...\n");
    }
    status = pthread_mutex_unlock(&mutex);
    if (status != 0)
    {
      printf("consumer lock failed!\n");
      exit(1);
    }
    sleep(2);
  }
  return NULL;
}

void main(int argc, char *argv[])
{
  int status;

  void *result;
  pthread_t producer_thread, consumer_thread;
  status = pthread_create(&producer_thread, NULL, producer, NULL);
  if (status != 0)
  {
    printf("consumer create failed!\n");
    exit(1);
  }

  status = pthread_create(&consumer_thread, NULL, consumer, NULL);
  if (status != 0)
  {
    printf("consumer create failed!\n");
    exit(1);
  }
  status = pthread_join(producer_thread, &result);
  if (status != 0)
  {
    printf("join producer failed!\n");
    exit(1);
  }
  status = pthread_join(consumer_thread, &result);
  if (status != 0)
  {
    printf("join consumer  failed!\n");
    exit(1);
  }
  exit(0);
}