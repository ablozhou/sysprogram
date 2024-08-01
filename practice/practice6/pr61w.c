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
    //
    if (sem != NULL)
    {
        if (sem_close(sem) != 0)
        {
            printf("sem_close failed\n");
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
    Person person[ARRAY_SIZE] = {

        {"Alice", "Johnson"},
        {"Bob", "Brown"},
        {"Charlie", "Davis"},
        {"David", "Wilson"},
        {"Eva", "Taylor"},
        {"Frank", "Anderson"},
        {"Grace", "Thomas"},
        {"Henry", "Moore"},
        {"Iane", "Smith"},
        {"John", "Doe"},

    };

    status = atexit(cleanup);
    if (status != 0)
    {
        printf("reg clean up failed: %d\n", status);
        exit(1);
    }

    sem = sem_open(SEM, O_CREAT, 0644, 1);
    // Note: ==, = no warning
    if (sem == SEM_FAILED)
    {
        printf("error open sem.\n");
        exit(1);
    }
    shmd = shm_open(SHM, O_CREAT | O_RDWR, 0644);
    if (shmd == -1)
    {
        printf("error open shm: %s.\n", strerror(errno));
        exit(1);
    }

    status = ftruncate(shmd, ARRAY_SIZE * ITEM_SIZE);
    if (status != 0)
    {
        printf("error truncate.\n");
        exit(1);
    }
    printf("mmap begin.\n");
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
    printf("sem wait.\n");
    status = sem_wait(sem);
    if (status != 0)
    {
        printf("error sem_wait.\n");
        exit(1);
    }

    printf("copy begin.\n");
    fflush(stdout);
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        printf("copy %d person: %s, %s.\n", i, person[i].firstName, person[i].lastName);

        memcpy(addr + i * ITEM_SIZE, (void *)&person[i], ITEM_SIZE);
    }
    status = sem_post(sem);
    if (status != 0)
    {
        printf("error sem_signal.\n");
        exit(1);
    }
    exit(0);
}