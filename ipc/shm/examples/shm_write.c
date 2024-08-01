/*
 * Create a POSIX shared memory object whose name is /MySharedMemory
 * It should contain a single integer whose value is 5
 * cc shmread.c -o shmread -Werror -lrt
 * ls /dev/shm
 * MyShareMeorry sem.MySemaphore
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>  /* For mode constants */
#include <fcntl.h>     /* For O_* constants */
#include <sys/mman.h>  /* For mmap */
#include <unistd.h>    /* For ftruncate and close */
#include <sys/types.h> /* For ftruncate */
#include <semaphore.h> /* For using semaphores */

int main(int argc, char *argv[])
{
    int shmDescriptor;
    int status;
    void *addr;
    int *pointerToSharedVariable;
    sem_t *mySemaphore;

    /* Create the semaphore if it doesn't already exist
    initial value has to be 1+, or else will be dead lock.
    sem_t *sem_open(const char *name, int oflag);
       sem_t *sem_open(const char *name, int oflag,
                       mode_t mode, unsigned int value);
    */
    mySemaphore = sem_open("/MySemaphore", O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (mySemaphore == SEM_FAILED)
    {
        printf("Failed to create semaphore.\n");
        exit(EXIT_FAILURE);
    }

    /* 1 Open the shared variable (create if it doesn't exist)
    readwrite or readonly flag, not write only.*/
    shmDescriptor = shm_open(
        "/MySharedMemory", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shmDescriptor == -1)
    {
        printf("Failed to create shared memory.\n");
        exit(EXIT_FAILURE);
    }

    /* 2 Set the size of the shared memory to the size of an integer */
    status = ftruncate(shmDescriptor, sizeof(int));
    if (status == -1)
    {
        printf("Failed to set the size of the shared memory.\n");
        exit(EXIT_FAILURE);
    }

    /*
    #include <sys/mman.h>

         void *
         mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
    The mmap() system call causes the pages starting at addr and continuing
     for at most len bytes to be mapped from the object described by fd,
     starting at byte offset offset.

    The protections (region accessibility) are specified in the prot argument
         by or'ing the following values:

         PROT_NONE   Pages may not be accessed.
         PROT_READ   Pages may be read.
         PROT_WRITE  Pages may be written.
         PROT_EXEC   Pages may be executed.
         Note that, due to hardware limitations, on some platforms PROT_WRITE may
         imply PROT_READ, and PROT_READ may imply PROT_EXEC.

        NOTE prot flag must be in same with the shm_open open flags.
    The flags argument specifies the type of the mapped object
    MAP_FILE,MAP_FIXED
    MAP_SHARED means other process also can access the memory.
    */
    /* 3 Map this shared memory into my address space */
    addr = mmap(
        NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmDescriptor, 0);
    if (addr == MAP_FAILED)
    {
        printf("Failed to map shared memory into address space.\n");
        exit(EXIT_FAILURE);
    }

    // close the descriptor as soon as I don't need it anymore
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
    *pointerToSharedVariable = 5; // CRITICAL SECTION
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

    exit(EXIT_SUCCESS);
}