/*
1. Consider the two implementations of the producer consumer problem
with POSIX threads and mutex discussed in class. Recall that the first version
does not use POSIX condition variables whereas the second version uses them.
Modify both implementations such that the following parameters can be specified
by the user using command line arguments:

- the number of producer threads,
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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_unit = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_space = PTHREAD_COND_INITIALIZER;
typedef struct
{
    int id;
    int sleep_time;
    int min_units;
    int max_units;
} Context;

int count = 0;

void *producer(void *data)
{
    int status;
    if (data == NULL)
    {
        printf("Error: producer parameter is NULL\n");
        exit(1);
    }
    Context *param = (Context *)data;

    while (TRUE)
    {
        status = pthread_mutex_lock(&mutex);
        if (status != 0)
        {
            printf("Error: producer pthread_mutex_lock failed\n");
            exit(1);
        }
        while (!(count < param->max_units))
        {
            printf("%d Buffer is full,buffer is: %d, producer waiting...\n", param->id, count);
            status = pthread_cond_wait(&cond_space, &mutex);
            if (status != 0)
            {
                printf("Error: producer pthread_cond_wait failed\n");
                exit(1);
            }
        }
        count++;
        printf("%d produce 1, count is %d\n", param->id, count);

        if (count > param->min_units)
        {
            printf("%d send unit signal.\n", param->id);
            pthread_cond_signal(&cond_unit);
        }

        status = pthread_mutex_unlock(&mutex);
        if (status != 0)
        {
            printf("Error: producer pthread_mutex_unlock failed\n");
            exit(1);
        }
        sleep(param->sleep_time);
    }
}

void *consumer(void *data)
{
    int status;
    if (data == NULL)
    {
        printf("Error: consumer parameter is NULL\n");
        exit(1);
    }
    Context *param = (Context *)data;

    while (TRUE)
    {
        status = pthread_mutex_lock(&mutex);
        if (status != 0)
        {
            printf("Error: consumer pthread_mutex_lock failed\n");
            exit(1);
        }
        while (!(count > param->min_units))
        {
            printf("%d Buffer is insufficient, buffer is: %d, consumer waiting...\n", param->id, count);
            status = pthread_cond_wait(&cond_unit, &mutex);
            if (status != 0)
            {
                printf("Error: consumer pthread_cond_wait failed\n");
                exit(1);
            }
        }
        count--;
        printf("%d consumer 1, count is %d\n", param->id, count);
        if (count < param->max_units)
        {
            printf("%d send space signal.\n", param->id);
            pthread_cond_signal(&cond_space);
        }
        status = pthread_mutex_unlock(&mutex);
        if (status != 0)
        {
            printf("Error: consumer pthread_mutex_unlock failed\n");
            exit(1);
        }
        sleep(param->sleep_time);
    }
}
void main(int argc, char *argv[])
{
    int status = 0;
    int producer_thread_count = 0;
    int consumer_thread_count = 0;
    int producer_thread_sleep_time = 0;
    int consumer_thread_sleep_time = 0;
    int min_units = 0;
    int max_units = 0;

    if (argc < 7 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        printf("Usage: %s <producer threads count> <producer sleep seconds> \n\t <consumer threads count> <consumer sleep seconds> <minimum buffer> <maximum buffer>\n", argv[1]);
        exit(1);
    }
    producer_thread_count = atoi(argv[1]);
    producer_thread_sleep_time = atoi(argv[2]);
    consumer_thread_count = atoi(argv[3]);
    consumer_thread_sleep_time = atoi(argv[4]);
    min_units = atoi(argv[5]);
    max_units = atoi(argv[6]);

    if (producer_thread_count <= 0 || consumer_thread_count <= 0 || producer_thread_sleep_time <= 0 || consumer_thread_sleep_time <= 0 || min_units <= 0 || max_units <= 0)
    {
        printf("Error parameter error, each parameter must be positive.\n");
        exit(1);
    }

    pthread_t *producer_threads = malloc(producer_thread_count * sizeof(pthread_t));
    if (producer_threads == NULL)
    {
        printf("malloc producer_threads failed.\n");
        exit(1);
    }
    pthread_t *consumer_threads = malloc(consumer_thread_count * sizeof(pthread_t));
    if (consumer_threads == NULL)
    {
        printf("malloc consumer_threads failed.\n");
        exit(1);
    }
    Context *context1 = (Context *)malloc(producer_thread_count * sizeof(Context));
    if (context1 == NULL)
    {
        printf("malloc producer context failed.\n");
        exit(1);
    }
    Context *context2 = (Context *)malloc(consumer_thread_count * sizeof(Context));
    if (context2 == NULL)
    {
        printf("malloc consumer context failed.\n");
        exit(1);
    }
    for (int i = 0; i < producer_thread_count; i++)
    {
        context1[i].id = i;
        context1[i].sleep_time = producer_thread_sleep_time;
        context1[i].min_units = min_units;
        context1[i].max_units = max_units;

        status = pthread_create(&producer_threads[i], NULL, producer, &context1[i]);
        if (status != 0)
        {
            printf("Failed to create producer thread.\n");
            exit(1);
        }
    }
    for (int i = 0; i < consumer_thread_count; i++)
    {
        context2[i].id = i;
        context2[i].sleep_time = consumer_thread_sleep_time;
        context2[i].min_units = min_units;
        context2[i].max_units = max_units;

        status = pthread_create(&consumer_threads[i], NULL, consumer, &context2[i]);
        if (status != 0)
        {
            printf("Failed to create consumer thread.\n");
            exit(1);
        }
    }
    for (int i = 0; i < producer_thread_count; i++)
    {
        status = pthread_join(producer_threads[i], NULL);
        if (status != 0)
        {
            printf("Failed to join producer thread.\n");
            exit(1);
        }
    }
    for (int i = 0; i < consumer_thread_count; i++)
    {
        status = pthread_join(consumer_threads[i], NULL);
        if (status != 0)
        {
            printf("Failed to join consumer thread.\n");
            exit(1);
        }
    }
    free(producer_threads);
    free(consumer_threads);
    exit(0);
}