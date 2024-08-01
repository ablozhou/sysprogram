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
#include <errno.h>
#include <signal.h>

#define TRUE 1
#define MSG_COUNT 10

#define SEM_FULL "/semfull"
#define SEM_EMPTY "/semempty"
#define SEM_MUTEX "/semmutex"
#define SHM "/myshm"

sem_t *sem_full = SEM_FAILED;
sem_t *sem_empty = SEM_FAILED;
sem_t *sem_mutex = SEM_FAILED;

int shmd = 0;

void cleanup(void)
{

    if (sem_full != SEM_FAILED)
    {
        if (sem_close(sem_full) != 0)
        {
            printf("sem_close failed.\n");
        }

        if (sem_unlink(SEM_FULL) != 0)
        {
            printf("sem_unlink failed.\n");
        }
        else
        {
            printf("sem_unlink succeeded.\n");
        }
    }
    if (sem_empty != SEM_FAILED)
    {
        if (sem_close(sem_empty) != 0)
        {
            printf("sem_close failed.\n");
        }

        if (sem_unlink(SEM_EMPTY) != 0)
        {
            printf("sem_unlink failed.\n");
        }
        else
        {
            printf("sem_unlink succeeded.\n");
        }
    }
    if (sem_mutex != SEM_FAILED)
    {
        if (sem_close(sem_mutex) != 0)
        {
            printf("sem_close failed.\n");
        }
        if (sem_unlink(SEM_MUTEX) != 0)
        {
            printf("sem_unlink failed.\n");
        }
        else
        {
            printf("sem_unlink succeeded.\n");
        }
    }
    if (shmd > 0)
    {
        if (shm_unlink(SHM) != 0)
        {
            printf("shm_unlink failed.\n");
        }
        else
        {
            printf("shm_unlink succeeded.\n");
        }
    }
}
void signal_handler(int signal)
{
    exit(0);
}
void main(int argc, char *argv[])
{
    int status;
    void *addr;
    int numMessages = 0;
    int *iAddr = NULL;
    status = atexit(cleanup);
    if (status != 0)
    {
        printf("Error register clean up.\n");
        exit(1);
    }
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("Error register sigint.\n");
        exit(1);
    }
    sem_full = sem_open(SEM_FULL, O_CREAT, 0644, 0);
    if (sem_full == SEM_FAILED)
    {
        printf("Error opem sem_full: %s\n", strerror(errno));
        exit(1);
    }
    sem_empty = sem_open(SEM_EMPTY, O_CREAT, 0644, MSG_COUNT);
    if (sem_empty == SEM_FAILED)
    {
        printf("Error opem sem_empty.\n");
        exit(1);
    }
    sem_mutex = sem_open(SEM_MUTEX, O_CREAT, 0644, 1);
    if (sem_mutex == SEM_FAILED)
    {
        printf("Error opem sem_mutex.\n");
        exit(1);
    }

    shmd = shm_open(SHM, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shmd == -1)
    {
        printf("Error shm_open %s.\n", SHM);
        exit(1);
    }
    status = ftruncate(shmd, sizeof(int));
    if (status != 0)
    {
        printf("Error ftruncate.\n");
        exit(1);
    }
    addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmd, 0);
    // Note: ==, not !=
    if (addr == MAP_FAILED)
    {
        printf("Error mmap: %s\n", strerror(errno));
        exit(1);
    }
    status = close(shmd);
    if (status != 0)
    {
        printf("Error close shmd.\n");
        exit(1);
    }
    iAddr = (int *)addr;
    // init
    *iAddr = numMessages;
    // produce
    while (TRUE)
    {
        // wait empty
        status = sem_wait(sem_empty);
        if (status != 0)
        {
            printf("Error sem_wait SEM_EMPTY.\n");
            exit(1);
        }
        status = sem_wait(sem_mutex);
        if (status != 0)
        {
            printf("Error sem_wait SEM_MUTEX.\n");
            exit(1);
        }
        // read
        numMessages = *iAddr;
        printf("numMessages: %d\n", numMessages);
        // increase and write to shared memory
        numMessages++;
        *iAddr = numMessages;
        printf("numMessages increased: %d\n", numMessages);
        status = sem_post(sem_mutex);
        if (status != 0)
        {
            printf("Error sem_post SEM_MUTEX.\n");
            exit(1);
        }
        status = sem_post(sem_full);
        if (status != 0)
        {
            printf("Error sem_post SEM_FULL.\n");
            exit(1);
        }
        sleep(2);
    }
}