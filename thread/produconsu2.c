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
// pthread_cond_t cond_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_unit_available = PTHREAD_COND_INITIALIZER;
// pthread_cond_t cond_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_space_available = PTHREAD_COND_INITIALIZER;

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
    while (!(numberOfUnits < MAX_BUFFER_SIZE))
    {
      printf("producer wait space available!\n");
      status = pthread_cond_wait(&cond_space_available, &mutex);
      if (status != 0)
      {
        printf("producer wait failed!\n");
        exit(1);
      }
    }
    numberOfUnits++;
    printf("producer increase to %d.\n", numberOfUnits);

    status = pthread_cond_signal(&cond_unit_available);
    if (status != 0)
    {
      printf("producer signal failed!\n");
      exit(1);
    }

    status = pthread_mutex_unlock(&mutex);
    if (status != 0)
    {
      printf("producer unlock failed!\n");
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
    while (!(numberOfUnits > 0))
    {
      printf("consumer wait unit available!\n");
      status = pthread_cond_wait(&cond_unit_available, &mutex);
      if (status != 0)
      {
        printf("consumer wait failed!\n");
        exit(1);
      }
    }
    numberOfUnits--;
    printf("consumer decrease to %d.\n", numberOfUnits);
    status = pthread_cond_signal(&cond_space_available);
    if (status != 0)
    {
      printf("consumer signal failed!\n");
      exit(1);
    }

    status = pthread_mutex_unlock(&mutex);
    if (status != 0)
    {
      printf("consumer unlock failed!\n");
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
    printf("producer create failed!\n");
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