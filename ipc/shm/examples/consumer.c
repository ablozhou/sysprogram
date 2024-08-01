#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define SHM_NAME "/shm_example"
#define SHM_SIZE sizeof(shared_memory)

typedef struct
{
    int buffer[10]; // 简单的缓冲区，大小为10
    int in;
    int out;
} shared_memory;

sem_t *empty, *full, *mutex;
shared_memory *shm_ptr;

void *consumer(void *param)
{
    int id = *((int *)param);
    while (1)
    {
        sem_wait(full);
        sem_wait(mutex);

        int item = shm_ptr->buffer[shm_ptr->out];
        shm_ptr->out = (shm_ptr->out + 1) % 10;
        printf("Consumer %d consumed %d\n", id, item);

        sem_post(mutex);
        sem_post(empty);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <number_of_consumers>\n", argv[0]);
        return 1;
    }

    int num_consumers = atoi(argv[1]);
    pthread_t consumers[num_consumers];
    int consumer_ids[num_consumers];

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    empty = sem_open("/sem_empty", O_CREAT, 0666, 10);
    full = sem_open("/sem_full", O_CREAT, 0666, 0);
    mutex = sem_open("/sem_mutex", O_CREAT, 0666, 1);

    for (int i = 0; i < num_consumers; i++)
    {
        consumer_ids[i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    for (int i = 0; i < num_consumers; i++)
    {
        pthread_join(consumers[i], NULL);
    }

    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    munmap(shm_ptr, SHM_SIZE);
    close(shm_fd);

    return 0;
}
