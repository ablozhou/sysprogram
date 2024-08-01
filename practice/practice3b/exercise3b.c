/*
Consider the two implementations of the producer consumer problem with POSIX threads and mutex
discussed in class. Recall that the first version does not use POSIX condition variables whereas the
second version uses them. Modify both implementations such that the following parameters can be
specified by the user using command line arguments:
• the number of producer threads,
• the number of seconds each producer thread must sleep after producing a unit,
• the number of consumer threads,
• the number of seconds each consumer thread must sleep after consuming a unit,
• the minimum number of units that must be in the buffer at any time, and
• the maximum number of units that must be in the buffer at any time.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define TRUE 1
#define MAX_SIZE 10
// condition
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_unit_available = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_space_available = PTHREAD_COND_INITIALIZER;
volatile int product = 5;

void *producer(void *data)
{
  int status;
  int tid = gettid();
  int sleep_time = *(int *)data;
  printf("Producer thread %d created, sleep: %d\n", tid, sleep_time);
  // loop to produce,
  while (TRUE)
  {
    // lock
    // printf("%d locking \n", tid);
    status = pthread_mutex_lock(&mutex);
    if (status != 0)
    {
      printf("ERROR: producer pthread_mutex_lock failed: %d\n", status);
      exit(1);
    }
    // printf("%d locked \n", tid);
    // if full, wait condtion
    // if (product == MAX_SIZE)
    while (product >= MAX_SIZE)
    {
      printf("product is full: %d, waiting for space available.\n", product);
      status = pthread_cond_wait(&cond_space_available, &mutex);
      if (status != 0)
      {
        printf("ERROR: producer pthread_cond_wait failed: %d\n", status);
        exit(1);
      }
    }
    // increase product
    // printf("%d producing \n", tid);
    product++;

    // send signal to consumer if unit available
    if (product == 1)
    {
      printf("%d sending signal to consumer.\n", tid);
      status = pthread_cond_signal(&cond_unit_available);
      if (status != 0)
      {
        printf("ERROR: producer pthread_cond_signal failed: %d\n", status);
        exit(1);
      }
    }

    printf("%d Thread  produced to %d.\n", tid, product);
    // unlock
    status = pthread_mutex_unlock(&mutex);
    if (status != 0)
    {
      printf("ERROR: producer pthread_mutex_unlock failed: %d\n", status);
      exit(1);
    }
    // printf("%d unlocked \n", tid);
    sleep(sleep_time);
  }

  return NULL;
}

void *consumer(void *data)
{
  int status;
  int tid = gettid();
  int sleep_time = *(int *)data;
  printf("%d Consumer thread  created, sleep: %d\n", tid, sleep_time);

  // loop to produce,
  while (TRUE)
  {
    // lock
    // printf("%d locking \n", tid);
    status = pthread_mutex_lock(&mutex);
    if (status != 0)
    {
      printf("ERROR: consumer pthread_mutex_lock failed: %d\n", status);
      exit(1);
    }
    // printf("%d locked \n", tid);
    // if empty, wait produce condtion
    // if (product == 0)
    while (product <= 0)
    {
      printf("product is empty: %d, waiting for unit available.\n", product);
      status = pthread_cond_wait(&cond_unit_available, &mutex);
      if (status != 0)
      {
        printf("ERROR: consumer pthread_cond_wait failed: %d\n", status);
      }
    }
    // decrease product
    // printf("%d consuming \n", tid);
    product--;
    // if awailable, send signal to producer
    if (product == MAX_SIZE - 1) //?
    {
      printf("%d sending signal to producer.\n", tid);
      status = pthread_cond_signal(&cond_space_available);
      if (status != 0)
      {
        printf("ERROR: consumer pthread_cond_signal failed: %d\n", status);
      }
    }

    printf("%d Thread  consumer to %d.\n", tid, product);
    // unlock
    status = pthread_mutex_unlock(&mutex);
    if (status != 0)
    {
      printf("ERROR: consumer pthread_mutex_unlock failed: %d\n", status);
      exit(1);
    }
    // printf("%d unlocked \n", tid);
    sleep(sleep_time);
  }
  return NULL;
}

void main(int argc, char *argv[])
{
  int status;
  if (argc < 5 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
  {
    printf("Usage: %s producer_num producer_sleep_time consumer_num consumer_sleep_time \n", argv[0]);
    exit(1);
  }
  int producer_num = atoi(argv[1]);
  int producer_sleep_time = atoi(argv[2]);
  int consumer_num = atoi(argv[3]);
  int consumer_sleep_time = atoi(argv[4]);

  pthread_t *thread_producer = malloc(producer_num * sizeof(pthread_t));
  pthread_t *thread_consumer = malloc(consumer_num * sizeof(pthread_t));
  for (int i = 0; i < producer_num; i++)
  {
    status = pthread_create(&thread_producer[i], NULL, producer, (void *)&producer_sleep_time);
    if (status != 0)
    {
      printf("ERROR: producer pthread_create failed: %d\n", status);
      exit(1);
    }
  }
  for (int i = 0; i < consumer_num; i++)
  {
    status = pthread_create(&thread_consumer[i], NULL, consumer, (void *)&consumer_sleep_time);
    if (status != 0)
    {
      printf("ERROR: consumer pthread_create failed: %d\n", status);
      exit(1);
    }
  }
  for (int i = 0; i < producer_num; i++)
  {
    printf("Thread producer %d joinning.\n", i);
    status = pthread_join(thread_producer[i], NULL);
    if (status != 0)
    {
      printf("ERROR: producer pthread_join failed: %d\n", status);
      exit(1);
    }
  }
  for (int i = 0; i < consumer_num; i++)
  {
    printf("Thread consumer %d joinning.\n", i);
    status = pthread_join(thread_consumer[i], NULL);
    if (status != 0)
    {
      printf("ERROR: consumer pthread_join failed: %d\n", status);
      exit(1);
    }
  }
  exit(0);
}
