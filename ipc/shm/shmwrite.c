/*
Using share memory send a serial of numbers and get the sum of the numbers.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SEMAPHORE_NAME "/mysem"
#define SHARED_MEMORY "/sharemem"
// sender
void main(int argc, char *argv[])
{
    int status;
    if (argc < 3)
    {
        printf("Usage: %s shmem_name num1 [...]\n", argv[0]);
        exit(1);
    }
    int len = argc - 2;

    sem_t *sem = sem_open(SEMAPHORE_NAME, O_CREAT | O_RDWR, 0644, 0);
    if (sem == SEM_FAILED)
    {
        printf("Error open semaphore.\n");
        exit(1);
    }
    int shmd = shm_open(argv[1], O_CREAT | O_RDWR, 0644);
    if (shmd == -1)
    {
        printf("Error opening shmem.\n");
        exit(1);
    }

    status = ftruncate(shmd, len * sizeof(int));

    char *addr = mmap(NULL, len * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmd, 0);
    if (addr == MAP_FAILED)
    {
        printf("Error creating shmem.\n");
        exit(1);
    }

    status = close(shmd);
    if (status == -1)
    {
        printf("Error closing shm.\n");
        exit(1);
    }

    for (int i = 2; i < argc; i++)
    {
        int value = atoi(argv[i]);
        printf("add %d\n", value);
        memcpy(addr, &value, sizeof(int));
        addr += sizeof(int);
    }

    sem_close(sem);

    exit(0);
}