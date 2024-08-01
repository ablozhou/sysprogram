/*
 * Read the value of the shared variable /MySharedMemory
 cc shmread.c -o shmread -Werror -lrt
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>  /* For mode constants */
#include <fcntl.h>     /* For O_* constants */
#include <sys/mman.h>  /* For mmap */
#include <unistd.h>    /* For ftruncate and close */
#include <sys/types.h> /* For ftruncate */
#include <semaphore.h> /* For semaphores */

int main(int argc, char *argv[])
{
    int shmDescriptor;
    int status;
    void *addr;
    int *pointerToSharedVariable;
    int valueOfSharedVariable;
    struct stat shmMetadata;
    sem_t *mySemaphore;

    /* Open the existing semaphore
    sem_t *sem_open(const char *name, int oflag);
    if the semaphore does exist, the oflag just set to 0.
     */
    mySemaphore = sem_open("/MySemaphore", 0);
    if (mySemaphore == SEM_FAILED)
    {
        printf("Failed to create semaphore.\n");
        exit(EXIT_FAILURE);
    }

    /* Open the shared variable for reading, if shm has been created, mode_t just set 0. because shm_open has only one version, different from mq_open and open which have 2 versions. */
    shmDescriptor = shm_open("/MySharedMemory", O_RDONLY, 0);
    if (shmDescriptor == -1)
    {
        printf("Failed to open shared memory.\n");
        exit(EXIT_FAILURE);
    }

    /* Get metadata about the shared variable
        Reader has to figure it out what the size is.
        fstat is a wrap libary call of system call stat.
        to get meta data of the file:
        size, times, owner, permissions
            #include <sys/stat.h>

            int fstat(int fildes, struct stat *buf);


    */
    status = fstat(shmDescriptor, &shmMetadata);
    if (status == -1)
    {
        printf("Failed to get size of the shared memory.\n");
        exit(EXIT_FAILURE);
    }

    /* Map this shared memory into my address space */
    addr = mmap(
        NULL, shmMetadata.st_size, PROT_READ, MAP_SHARED, shmDescriptor, 0);
    if (addr == MAP_FAILED)
    {
        printf("Failed to map shared memory into address space.\n");
        exit(EXIT_FAILURE);
    }

    status = close(shmDescriptor);
    if (status == -1)
    {
        printf("Failed to close shared memory.\n");
        exit(EXIT_FAILURE);
    }

    pointerToSharedVariable = (int *)addr;

    status = sem_wait(mySemaphore);
    if (status != 0)
    {
        printf("Failed to lock semaphore.\n");
        exit(EXIT_FAILURE);
    }
    valueOfSharedVariable = *pointerToSharedVariable; // CRITICAL SECTION
    status = sem_post(mySemaphore);
    if (status != 0)
    {
        printf("Failed to unlock semaphore.\n");
        exit(EXIT_FAILURE);
    }

    status = sem_close(mySemaphore);
    if (status != 0)
    {
        printf("Failed to close semaphore.\n");
        exit(EXIT_FAILURE);
    }

    printf("The value of the shared memory = %d\n", valueOfSharedVariable);

    exit(EXIT_SUCCESS);
}