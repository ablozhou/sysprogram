#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <mqueue.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define SEMAPHORE_QUEUE_NAME "/semaphore_queue"
#define MAX_SIZE 10
#define PERMISSIONS 0644
void semaphore_init(int value)
{
    mqd_t mq;
    struct mq_attr attr;

    // Initialize the queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_SIZE;
    attr.mq_msgsize = sizeof(int);
    attr.mq_curmsgs = 0;

    // Create the message queue
    mq = mq_open(SEMAPHORE_QUEUE_NAME, O_CREAT | O_RDWR, PERMISSIONS, &attr);
    if (mq == -1)
    {
        perror("Semaphore init: mq_open");
        exit(EXIT_FAILURE);
    }

    // Initialize the semaphore with the given value
    for (int i = 0; i < value; i++)
    {
        if (mq_send(mq, (const char *)&i, sizeof(int), 0) == -1)
        {
            perror("Semaphore init: mq_send");
            exit(EXIT_FAILURE);
        }
    }

    // Close the message queue
    mq_close(mq);
}
void semaphore_wait()
{
    mqd_t mq;
    int dummy;

    // Open the message queue for reading
    mq = mq_open(SEMAPHORE_QUEUE_NAME, O_RDWR);
    if (mq == -1)
    {
        perror("Semaphore wait: mq_open");
        exit(EXIT_FAILURE);
    }

    // Wait (block) until a message is available
    if (mq_receive(mq, (char *)&dummy, sizeof(int), NULL) == -1)
    {
        perror("Semaphore wait: mq_receive");
        exit(EXIT_FAILURE);
    }

    // Close the message queue
    mq_close(mq);
}
void semaphore_signal()
{
    mqd_t mq;
    int dummy = 0;

    // Open the message queue for writing
    mq = mq_open(SEMAPHORE_QUEUE_NAME, O_RDWR);
    if (mq == -1)
    {
        perror("Semaphore signal: mq_open");
        exit(EXIT_FAILURE);
    }

    // Send a message to the queue
    if (mq_send(mq, (const char *)&dummy, sizeof(int), 0) == -1)
    {
        perror("Semaphore signal: mq_send");
        exit(EXIT_FAILURE);
    }

    // Close the message queue
    mq_close(mq);
}
void semaphore_destroy()
{
    // Unlink the message queue
    if (mq_unlink(SEMAPHORE_QUEUE_NAME) == -1)
    {
        perror("Semaphore destroy: mq_unlink");
        exit(EXIT_FAILURE);
    }
}
void *producer(void *arg)
{
    while (1)
    {
        semaphore_wait();
        printf("Producer produced an item\n");
        sleep(1); // Simulate production time
        semaphore_signal();
    }
    return NULL;
}

void *consumer(void *arg)
{
    while (1)
    {
        semaphore_wait();
        printf("Consumer consumed an item\n");
        sleep(2); // Simulate consumption time
        semaphore_signal();
    }
    return NULL;
}

int main()
{
    pthread_t producer_thread, consumer_thread;

    // Initialize semaphore with value 1
    semaphore_init(1);

    // Create producer and consumer threads
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // Wait for threads to finish
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // Destroy semaphore
    semaphore_destroy();

    return 0;
}
