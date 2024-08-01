/*
Let's suppose you are working with a new operating system and
programming language that do not provide support for Mutex and Semaphores and
only provide support for message passing. You are in dire need of a semaphore
implementation for a project that you are doing and are therefore made to
implement your own version of a semaphore. The only way you can do that in this
situation is to write an implementation for semaphores using message passing.
Provide an implementation for semaphores using j message passing. Make sure
you explain your solutions or any assumptions that you make.

Author: Haihan Zhou
Date: 2024-06-30

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <mqueue.h>

/*
struct mq_attr {
    long mq_flags;       // Flags (ignored for mq_open())
    long mq_maxmsg;      // Max. # of messages on queue
    long mq_msgsize;     // Max. message size (bytes)
    long mq_curmsgs;     // # of messages currently in queue
                            (ignored for mq_open())
    };
*/
#define TRUE 1
#define SEM_NAME_LENGTH 255
#define SEM_NAME "/mysem" // example semaphore name for test
#define PERMISSIONS 0644  // read write | read | read

// struct to hold the semaphore information
typedef struct
{
    long max_value;                 // max message count
    char sem_name[SEM_NAME_LENGTH]; // semaphore name
    mqd_t mq;                       // using message queue to implement.
} sem_t;

// open semaphore
sem_t *sem_open(char *sem_name, long init_value, long max_value, int oflag, mode_t mode)
{
    if (sem_name == NULL)
    {
        perror("Semaphore sem_name is NULL.\n");
        return NULL;
    }
    if (strlen(sem_name) >= SEM_NAME_LENGTH)
    {
        perror("Semaphore sem_name is too long.\n");
        return NULL;
    }
    if (init_value > max_value)
    {
        perror("Semaphore init_value > max_value.\n");
        return NULL;
    }

    sem_t *sem = malloc(sizeof(sem_t));
    if (sem == NULL)
    {
        perror("Semaphore new error.\n");
        return NULL;
    }
    /*  attr->mq_maxmsg  or attr->mq_msqsize was invalid.  Both of these
        fields must be greater than zero.  In a process that is unprivi‐
        leged  (does  not   have   the   CAP_SYS_RESOURCE   capability),
        attr->mq_maxmsg must be less than or equal to the msg_max limit,
        and  attr->mq_msgsize  must  be  less  than or equal to the msg‐
        size_max limit.  In addition,  even  in  a  privileged  process,
        attr->mq_maxmsg   cannot   exceed   the  HARD_MAX  limit.
        The default value for msg_max is 10.
        The  upper  limit  is  HARD_MSGMAX.
        The definition of HARD_MSGMAX has  changed  across  kernel  ver‐
        sions:
        •  Up to Linux 2.6.32: 131072 / sizeof(void *)
        •  Linux 2.6.33 to Linux 3.4: (32768 * sizeof(void *) / 4)
        •  Since Linux 3.5: 65,536
        /proc/sys/fs/mqueue/msg_max 10
        zhh@mysvr:~/sysprogram/os/assignment$ cat /dev/mqueue/mysem
        QSIZE:40         NOTIFY:0     SIGNO:0     NOTIFY_PID:0
    */
    // Initialize the queue attributes
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = max_value;
    attr.mq_msgsize = sizeof(int);
    attr.mq_curmsgs = 0;

    strcpy(sem->sem_name, sem_name);
    sem->max_value = max_value;

    // unlink the mqueue
    // if (oflag &= O_CREAT)
    // {
    //     mq_unlink(sem_name);
    // }

    // if (!oflag &= O_CREAT), attr will be ignored
    // sem->mq = mq_open(SEM_NAME, O_CREAT | O_RDWR, PERMISSIONS, &attr);
    sem->mq = mq_open(sem_name, oflag, mode, &attr);
    if (sem->mq == (mqd_t)-1)
    {
        perror("Semaphore open failed. \nPay attention that max value should less than /proc/sys/fs/mqueue/msg_max.\n");
        free(sem);
        return NULL;
    }

    if (oflag &= O_CREAT) // if not create ignore the init_value
    {
        for (int i = 0; i < init_value; i++)
        {
            if (mq_send(sem->mq, (const char *)&i, sizeof(int), 0) == -1)
            {
                perror("Semaphore set init value error.\n");
                mq_close(sem->mq);
                free(sem);
                return NULL;
            }
        }
    }
    printf("Semaphore open success.\n");
    return sem;
}

// release and clean
int sem_close(sem_t *sem)
{
    int status;
    if (sem == NULL)
    {
        perror("Semaphore parameter is NULL.\n");
        return 0;
    }
    status = mq_close(sem->mq);
    if (status != 0)
    {
        perror("Semaphore close failed.\n");
        free(sem);
        return status;
    }

    status = mq_unlink(sem->sem_name);
    if (status != 0)
    {
        perror("Semaphore unlink failed.\n");
        free(sem);
        return status;
    }
    free(sem);
}
// get semaphore number
int get_value(sem_t *sem)
{
    struct mq_attr attr;
    if (sem == NULL)
    {
        perror("Semaphore parameter is NULL.\n");
        return -1;
    }
    if (mq_getattr(sem->mq, &attr) == -1)
    {
        perror("Semaphore get value failed.\n");
        return -1;
    }
    return attr.mq_curmsgs;
}

// V operation
void wait(sem_t *sem)
{
    if (sem == NULL)
    {
        perror("Semaphore parameter is NULL.\n");
        exit(1);
    }
    int value = -1;
    int priority;
    if (mq_receive(sem->mq, (char *)&value, sizeof(int), &priority) == -1)
    {
        perror("Semaphore wait failed.\n");
        exit(1);
    }
}

// P operation
void signal(sem_t *sem)
{
    if (sem == NULL)
    {
        perror("Semaphore parameter is NULL.\n");
        exit(1);
    }
    int value = 0;
    if (mq_send(sem->mq, (char *)&value, sizeof(int), 0) == -1)
    {
        perror("Semaphore signal failed.\n");
        exit(1);
    };
}

// test thread, producer
void *producer(void *arg)
{
    sem_t *sem = (sem_t *)arg;
    while (TRUE)
    {
        signal(sem);
        printf("Producer produced an item. \n");
        printf("now size: %d\n", get_value(sem));
        sleep(1); // simulate production time
    }
    return NULL;
}

// test thread, consumer
void *consumer(void *arg)
{
    sem_t *sem = (sem_t *)arg;
    while (TRUE)
    {
        wait(sem);
        printf("Consumer consumed an item.\n");
        printf("left: %d\n", get_value(sem));
        sleep(2); // simulate consumption time
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t producer_thread, consumer_thread;

    // open semaphore
    sem_t *sem = sem_open(SEM_NAME, 5, 10, O_CREAT | O_RDWR, PERMISSIONS);
    if (sem == NULL)
    {
        perror("sem_open failed\n");
        exit(EXIT_FAILURE);
    }

    // create producer and consumer threads
    pthread_create(&producer_thread, NULL, producer, (void *)sem);
    pthread_create(&consumer_thread, NULL, consumer, (void *)sem);

    // wait for threads to finish
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // close semaphore, unlink
    sem_close(sem);

    return 0;
}