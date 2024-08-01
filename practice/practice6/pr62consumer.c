/*
2. Consider two processes: a producer and a consumer, and a shared variable numMessages. The shared variable starts with an initial value of 0. The producer increases the value of numMessages in steps of 1 in an infinite loop until it reaches a maximum value of 10. When the producer tries to increment the value of numMessages beyond 10, it must block until the value is less than 10. The consumer
decrements the value of numMessages in steps of 1 in an infinite loop until it reaches a minimum value of 0. When the consumer tries to decrement the value of numMessages below 0, it must block until the value becomes positive. When the producer and consumer are run simultaneously, the following conditions must be satisfied:
(a) There must be no race conditions.
(b) numMessages must not exceed 10.
(c) numMessages must not be negative.
Implement the producer and consumer using POSIX shared memory and POSIX semaphores.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <signal.h>
#include <errno.h>

#define TRUE 1
#define MSG_COUNT 10

#define SEM_FULL "/semfull"
#define SEM_EMPTY "/semempty"
#define SEM_MUTEX "/semmutex"
#define SHM "/myshm"

sem_t *sem_full = NULL;
sem_t *sem_empty = NULL;
sem_t *sem_mutex = NULL;

int shmd = 0;
int numMessages = 0;
void cleanup(void)
{
    if (sem_mutex != NULL)
    {
        if (sem_close(sem_mutex) != 0)
        {
            printf("sem_close failed.\n");
        }
    }
    if (sem_full != NULL)
    {
        if (sem_close(sem_full) != 0)
        {
            printf("sem_close failed.\n");
        }
    }
    if (sem_empty != NULL)
    {
        if (sem_close(sem_empty) != 0)
        {
            printf("sem_close failed.\n");
        }
        else
        {
            printf("sem_closed.\n");
        }
    }
}

void main(int argc, char *argv[])
{
    int status;
    void *addr;
    struct stat st;

    status = atexit(cleanup);
    if (status != 0)
    {
        printf("Error register clean up.\n");
        exit(1);
    }
    sem_full = sem_open(SEM_FULL, O_RDWR);
    if (sem_full == SEM_FAILED)
    {
        printf("Error opem sem_full: %s\n", strerror(errno));
        exit(1);
    }
    sem_empty = sem_open(SEM_EMPTY, O_RDWR);
    if (sem_empty == SEM_FAILED)
    {
        printf("Error opem sem_empty.\n");
        exit(1);
    }
    sem_mutex = sem_open(SEM_MUTEX, O_RDWR);
    if (sem_mutex == SEM_FAILED)
    {
        printf("Error opem sem_mutex.\n");
        exit(1);
    }

    shmd = shm_open(SHM, O_RDWR, 0);
    if (shmd == -1)
    {
        printf("Error shm_open %s.\n", SHM);
        exit(1);
    }
    // get the size of the shared memory
    status = fstat(shmd, &st);
    if (status != 0)
    {
        printf("Error fstat.\n");
        exit(1);
    }

    addr = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmd, 0);
    if (addr == MAP_FAILED)
    {
        printf("Error mmap.\n");
        exit(1);
    }
    status = close(shmd);
    if (status != 0)
    {
        printf("Error close shmd.\n");
        exit(1);
    }
    // consume
    while (TRUE)
    {
        // wait full
        status = sem_wait(sem_full);
        if (status != 0)
        {
            printf("Error sem_wait SEM_FULL.\n");
            exit(1);
        }
        status = sem_wait(sem_mutex);
        if (status != 0)
        {
            printf("Error sem_wait SEM_MUTEX.\n");
            exit(1);
        }

        numMessages = *(int *)addr;

        printf("numMessages: %d\n", numMessages);
        numMessages -= 1;
        // write to shared memory
        *(int *)addr = numMessages;
        printf("numMessages decreased: %d\n", numMessages);
        status = sem_post(sem_mutex);
        if (status != 0)
        {
            printf("Error sem_post SEM_MUTEX.\n");
            exit(1);
        }
        status = sem_post(sem_empty);
        if (status != 0)
        {
            printf("Error sem_post SEM_EMPTY.\n");
            exit(1);
        }
        sleep(1);
    }
}