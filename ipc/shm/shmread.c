#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <semaphore.h>
#include <errno.h>

void main(int argc, char *argv[])
{
    int status;
    struct stat st;
    if (argc < 2)
    {
        printf("Usage: %s shm_name\n", argv[0]);
        exit(1);
    }
    char *shm_name = argv[1];

    int shmd = shm_open(shm_name, O_RDONLY, 0);
    if (shmd < 0) // file descriptor 
    {
        printf("Error opening shm.\n");
        exit(1);
    }
    // Note: fstat to know the size of mmap file
    status = fstat(shmd, &st);
    if (status == -1)
    {
        printf("Error fstat: %s\n", strerror(errno));
        exit(1);
    }

    // Note: Protect type must consent with shm_open Oflags.
    // Note: st.st_size is byte size.
    char *addr = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, shmd, 0);
    if (addr == MAP_FAILED)
    {
        printf("Error mmapping shm.\n");
        exit(1);
    }
    // Note: there are no shm_close, just using close
    status = close(shmd);
    if (status == -1)
    {
        printf("Error closing shm.\n");
        exit(1);
    }

    int len = st.st_size / sizeof(int);
    for (int i = 0; i < len; i++)
    {
        printf("received %d\n", *(int *)addr);
        addr += sizeof(int);
    }

    status = shm_unlink(shm_name);
    if (status == -1)
    {
        printf("Error shm_unlink failed.\n");
        exit(1);
    }
    exit(0);
}