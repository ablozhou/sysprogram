/*
consumer <proc_num>
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define TRUE 1
#define BUF_SIZE 10
#define MAX_SIZE 10
#define SEM_FULL "/semful"
#define SEM_EMPTY "/semempty"
#define SEM_MUTEX "/semmutex"
#define SHM_NAME "/myshem"
#define MEM_SIZE sizeof(Memory)
typedef struct
{
    int buffer[BUF_SIZE];
    int in;
    int out;
} Memory;

Memory *mem;
sem_t *sem_full = NULL;
sem_t *sem_empty = NULL;
sem_t *sem_mutex = NULL;
int shmd = 0;

void consumer(void *data)
{
    if (data == NULL || mem == NULL)
    {
        printf("error data is null.\n");
        exit(1);
    }
    int status;
    int id = *(int *)data;

    while (TRUE)
    {
        // wait full
        // printf("wait full\n");
        status = sem_wait(sem_full);
        if (status == -1)
        {
            printf("sem_wait sem_full failed.\n");
            exit(1);
        }
        // wait mutex
        // printf("wait mutex\n");
        status = sem_wait(sem_mutex);
        if (status == -1)
        {
            printf("sem_wait sem_mutex failed.\n");
            exit(1);
        }
        // produce
        int value = mem->buffer[mem->out];

        // increase produce
        mem->out = (mem->out + 1) % MAX_SIZE;
        // signal mutex
        printf("%d:%d consumed %d \n", id, mem->in, value);

        // printf("%d: post mutex\n", id);
        status = sem_post(sem_mutex);
        if (status == -1)
        {
            printf("sem_post sem_mutex failed.\n");
            exit(1);
        }
        // signal full
        // printf("%d: post sem_empty\n", id);
        status = sem_post(sem_empty);
        if (status == -1)
        {
            printf("sem_post sem_empty failed.\n");
            exit(1);
        }
        sleep(1);
    }
}

void main(int argc, char *argv[])
{
    int status;
    pid_t pid;
    if (argc < 2)
    {
        printf("Usage: %s consumer_num\n", argv[0]);
        exit(1);
    }

    int num = atoi(argv[1]);
    if (num < 0)
    {
        printf("consumer number must be positive.\n");
        exit(1);
    }
    // init semaphore
    // NOTICE: oflags:O_CREAT
    printf("sem open full.\n");
    sem_full = sem_open(SEM_FULL, 0);
    if (sem_full == SEM_FAILED)
    {
        printf("Error opening sem_full.\n");
        exit(1);
    }
    sem_empty = sem_open(SEM_EMPTY, 0);
    if (sem_empty == SEM_FAILED)
    {
        printf("Error opening sem_empty.\n");
        exit(1);
    }
    sem_mutex = sem_open(SEM_MUTEX, 0);
    if (sem_mutex == SEM_FAILED)
    {
        printf("Error opening sem_mutex.\n");
        exit(1);
    }
    // NOTICE: must be O_RDWR
    shmd = shm_open(SHM_NAME, O_RDWR, 0);
    if (shmd == -1)
    {
        printf("Error opening share memory.\n");
        exit(1);
    }
    // status = ftruncate(shmd, MEM_SIZE);
    // if (status != 0)
    // {
    //     printf("ftruncate failed.\n");
    //     exit(1);
    // }
    void *addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmd, 0);
    if (addr == MAP_FAILED)
    {
        printf("Error creating share memory.\n");
        exit(1);
    }
    status = close(shmd);
    if (status != 0)
    {
        printf("close share memory fd failed.\n");
        exit(1);
    }
    printf("share memory ok.\n");
    mem = (Memory *)addr;
    mem->in = 0;
    mem->out = 0;
    printf("begin fork.\n");
    for (int i = 0; i < num; i++)
    {
        pid = fork();
        if (pid == -1)
        {
            printf("Fork failed.\n");
            exit(1);
        }
        if (pid == 0)
        { // child
            printf("Fork child %d.\n", i);
            consumer(&i);
        }
    }
    // parent
    // wait
    for (int i = 0; i < num; i++)
    {
        pid = wait(NULL);
        if (pid == -1)
        {
            printf("Wait failed.\n");
            exit(1);
        }
    }
    // Note: close with pointer
    status = sem_close(sem_full);
    if (status == -1)
    {
        printf("sem_close sem_full failed.\n");
        exit(1);
    }
    status = sem_close(sem_empty);
    if (status == -1)
    {
        printf("sem_close sem_empty failed.\n");
        exit(1);
    }
    status = sem_close(sem_mutex);
    if (status == -1)
    {
        printf("sem_close sem_mutex failed.\n");
        exit(1);
    }

    // unlink
    status = shm_unlink(SHM_NAME);
    if (status == -1)
    {
        printf("shm_unlink failed.\n");
        exit(1);
    }
    status = sem_unlink(SEM_FULL);
    if (status == -1)
    {
        printf("sem_unlink sem_full failed.\n");
        exit(1);
    }
    status = sem_unlink(SEM_EMPTY);
    if (status == -1)
    {
        printf("sem_unlink sem_empty failed.\n");
        exit(1);
    }
    status = sem_unlink(SEM_MUTEX);
    if (status == -1)
    {
        printf("sem_unlink sem_mutex failed.\n");
        exit(1);
    }

    exit(0);
}