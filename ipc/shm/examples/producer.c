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

void *producer(void *param)
{
    int id = *((int *)param);
    for (int i = 0; i < 20; i++)
    { // 每个线程生产20个整数
        int item = rand() % 100;
        sem_wait(empty);
        sem_wait(mutex);

        shm_ptr->buffer[shm_ptr->in] = item;
        shm_ptr->in = (shm_ptr->in + 1) % 10;
        printf("Producer %d produced %d\n", id, item);

        sem_post(mutex);
        sem_post(full);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <number_of_producers>\n", argv[0]);
        return 1;
    }

    int num_producers = atoi(argv[1]);
    pthread_t producers[num_producers];
    int producer_ids[num_producers];

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SHM_SIZE);
    shm_ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    shm_ptr->in = 0;
    shm_ptr->out = 0;

    empty = sem_open("/sem_empty", O_CREAT, 0666, 10);
    full = sem_open("/sem_full", O_CREAT, 0666, 0);
    mutex = sem_open("/sem_mutex", O_CREAT, 0666, 1);

    for (int i = 0; i < num_producers; i++)
    {
        producer_ids[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    for (int i = 0; i < num_producers; i++)
    {
        pthread_join(producers[i], NULL);
    }

    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    sem_unlink("/sem_empty");
    sem_unlink("/sem_full");
    sem_unlink("/sem_mutex");
    munmap(shm_ptr, SHM_SIZE);
    close(shm_fd);
    shm_unlink(SHM_NAME);

    return 0;
}
