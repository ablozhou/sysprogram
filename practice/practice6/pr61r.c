/*
1. Create a POSIX shared memory consisting of an array of 10 items where each item is a Person with a firstName (20 bytes) and a lastName (20 bytes). Implement a writer process that writes 10 items to this shared memory and a reader process that reads them from the shared memory and displays them to screen. Ensure that race conditions are avoided.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <errno.h>

#define TRUE 1
#define NAME_LEN 20
#define ARRAY_SIZE 10

typedef struct
{
    char firstName[NAME_LEN];
    char lastName[NAME_LEN];
} Person;

#define SEM "/mysem"
#define SHM "/mysharemm"
#define ITEM_SIZE sizeof(Person)

sem_t *sem = NULL;
int shmd = 0;
void cleanup(void)
{
    // close
    if (sem != NULL && sem != SEM_FAILED)
    {
        if (sem_close(sem) != 0)
        {
            printf("sem_close failed\n");
        }
        else
        {
            printf("sem_closed.\n");
        }
        if (sem_unlink(SEM) != 0)
        {
            printf("sem_unlink failed\n");
        }
        else
        {
            printf("sem_unlinked.\n");
        }
    }

    // unlink
    if (shmd > 0)
    {
        if (shm_unlink(SHM) != 0)
        {
            printf("Error unlink %s\n", SHM);
        }
        else
        {
            printf("unlink %s succeeded\n", SHM);
        }
    }
}
void main(int argc, char *argv[])
{
    int status;
    void *addr;
    Person person[ARRAY_SIZE];

    status = atexit(cleanup);
    if (status != 0)
    {
        printf("reg clean up failed: %d\n", status);
        exit(1);
    }

    sem = sem_open(SEM, 0);
    // Note: check ==, not =
    if (sem == SEM_FAILED)
    {
        printf("error open sem.\n");
        exit(1);
    }
    shmd = shm_open(SHM, O_RDWR, 0);
    if (shmd == -1)
    {
        printf("error open shm: %s\n", strerror(errno));
        exit(1);
    }

    addr = mmap(NULL, ARRAY_SIZE * ITEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmd, 0);
    if (addr == MAP_FAILED)
    {
        printf("Error mmap.\n");
        exit(1);
    }
    status = close(shmd);
    if (status != 0)
    {
        printf("error close shmd.\n");
        exit(1);
    }
    // write
    status = sem_wait(sem);
    if (status != 0)
    {
        printf("error sem_wait.\n");
        exit(1);
    }

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        memcpy((void *)&person[i], addr + i * ITEM_SIZE, ITEM_SIZE);
        printf("%d: %s %s\n", i, person[i].firstName, person[i].lastName);
    }
    status = sem_post(sem);
    if (status != 0)
    {
        printf("error sem_signal.\n");
        exit(1);
    }
    exit(0);
}