/*
Solve the dining philosopher’s problem using semaphores.
To prevent this, we can use a more sophisticated approach. One commonly used method is to enforce an ordering constraint such that a philosopher only picks up chopsticks if both are available. Another approach is to enforce a rule that a philosopher can only pick up the chopsticks if an odd philosopher picks up the left one first and an even philosopher picks up the right one first. Here’s how you can implement the second method in C using semaphores:
*/
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>  // for O_* constants
#include <string.h> // for strlen

#define NUM_PHILOSOPHERS 5

const char *SEM_NAMES[NUM_PHILOSOPHERS] = {"/chopstick0", "/chopstick1", "/chopstick2", "/chopstick3", "/chopstick4"};
sem_t *chopsticks[NUM_PHILOSOPHERS];
pthread_t philosophers[NUM_PHILOSOPHERS];

void *philosopher(void *num)
{
    int id = *(int *)num;
    while (1)
    {
// Thinking
printf("Philosopher %d is thinking\n", id);
sleep(rand() % 3 + 1);
// Hungry
printf("Philosopher %d is hungry\n", id);
if (id % 2 == 0)
{
// Even philosophers pick up the right chopstick first
sem_wait(chopsticks[(id + 1) % NUM_PHILOSOPHERS]);
sem_wait(chopsticks[id]);
}
else
{
// Odd philosophers pick up the left chopstick first
sem_wait(chopsticks[id]);
sem_wait(chopsticks[(id + 1) % NUM_PHILOSOPHERS]);
}
// Eating
printf("Philosopher %d is eating\n", id);
sleep(rand() % 3 + 1);
// Put down chopsticks
sem_post(chopsticks[id]);
sem_post(chopsticks[(id + 1) % NUM_PHILOSOPHERS]);
// Thinking again
    }
    return NULL;
}

int main()
{
    int i, ids[NUM_PHILOSOPHERS];

    for (i = 0; i < NUM_PHILOSOPHERS; i++)
    {
        chopsticks[i] = sem_open(SEM_NAMES[i], O_CREAT, 0644, 1);
        if (chopsticks[i] == SEM_FAILED)
        {
            perror("sem_open");
            exit(EXIT_FAILURE);
        }
        ids[i] = i;
    }

    for (i = 0; i < NUM_PHILOSOPHERS; i++)
    {
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }

    for (i = 0; i < NUM_PHILOSOPHERS; i++)
    {
        pthread_join(philosophers[i], NULL);
    }

    for (i = 0; i < NUM_PHILOSOPHERS; i++)
    {
        sem_close(chopsticks[i]);
        sem_unlink(SEM_NAMES[i]);
    }

    return 0;
}
